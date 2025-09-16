/**
 @file EVE_libmpsse.c
 */
/*
 * ============================================================================
 * History
 * =======
 * Nov 2019		Initial version
 *
 *
 *
 *
 *
 *
 *
 * (C) Copyright Bridgetek Pte Ltd
 * ============================================================================
 *
 * This source code ("the Software") is provided by Bridgetek Pte Ltd
 * ("Bridgetek") subject to the licence terms set out
 * http://www.ftdichip.com/FTSourceCodeLicenceTerms.htm ("the Licence Terms").
 * You must read the Licence Terms before downloading or using the Software.
 * By installing or using the Software you agree to the Licence Terms. If you
 * do not agree to the Licence Terms then do not download or use the Software.
 *
 * Without prejudice to the Licence Terms, here is a summary of some of the key
 * terms of the Licence Terms (and in the event of any conflict between this
 * summary and the Licence Terms then the text of the Licence Terms will
 * prevail).
 *
 * The Software is provided "as is".
 * There are no warranties (or similar) in relation to the quality of the
 * Software. You use it at your own risk.
 * The Software should not be used in, or for, any medical device, system or
 * appliance. There are exclusions of Bridgetek liability for certain types of loss
 * such as: special loss or damage; incidental loss or damage; indirect or
 * consequential loss or damage; loss of income; loss of business; loss of
 * profits; loss of revenue; loss of contracts; business interruption; loss of
 * the use of money or anticipated savings; loss of information; loss of
 * opportunity; loss of goodwill or reputation; and/or loss of, damage to or
 * corruption of data.
 * There is a monetary cap on Bridgetek's liability.
 * The Software may have subsequently been amended by another user and then
 * distributed by that other user ("Adapted Software").  If so that user may
 * have additional licence terms that apply to those amendments. However, Bridgetek
 * has no liability in relation to those amendments.
 * ============================================================================
 */

// Guard against being used for incorrect platform or architecture.
// USE_MPSSE holds the MPSSE channel to open.
// In gcc compilers this is in the Makefile. -DUSE_MPSSE=0
// In VisualStudio this is in Project Properties -> Configuration Properties -> 
//     C/C++ -> Preprocessor -> Preprocessor Definitions.
#if defined(USE_MPSSE)

#pragma message ("Compiling " __FILE__ " for libMPSSE")

#include <string.h>
#include <stdio.h>
#include <stdint.h> // for Uint8/16/32 and Int8/16/32 data types
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifndef _WIN32 // Windows is always little-endian (for now)
#include <endian.h>
#include <unistd.h>
#endif // _WIN32

#include "ftd2xx.h"
#include "libmpsse_spi.h"


// ----------------------- MCU Transmit Buffering  -----------------------------

#define MCU_BUFFER_SIZE 512
uint8_t *MCU_buffer;
uint16_t MCU_bufferLen;

// This is the Windows Platform specific section and contains the functions which
// enable the GPIO and SPI interfaces.

/*
    MPSSE pin connections:

    BD0 - SCK
    BD1 - MOSI
    BD2 - MISO
    BD3 - SS#	(SPI_CONFIG_OPTION_CS_DBUS3)
    BD7 - PD#	(SPI_CONFIG_OPTION_CS_DBUS7)

    Direction (1 =Output, 0 =Inputs) 1xxx 1011
*/
// ------------------ Platform specific initialisation  ------------------------

FT_HANDLE ftHandle;

void MCU_Init(void)
{
    FT_DEVICE_LIST_INFO_NODE devList;
    ChannelConfig channelConf;
    DWORD channel;
    DWORD channels;
    FT_STATUS status;

    /* Set SPI clock speed to 15 MHz - See the notes for MCU_SPI_TIMEOUT in the MCU.h file. */
    memset(&channelConf, 0, sizeof(ChannelConfig));
    channelConf.ClockRate = 15000000;
    channelConf.LatencyTimer = 10;
    channelConf.configOptions = SPI_CONFIG_OPTION_MODE0 | SPI_CONFIG_OPTION_CS_DBUS3 | SPI_CONFIG_OPTION_CS_ACTIVELOW;

    Init_libMPSSE();

    status = SPI_GetNumChannels(&channels);
    for (channel = 0; channel < channels; channel++)
    {
        status = SPI_GetChannelInfo(channel, &devList);
        if (status != FT_OK)
        {
            printf("SPI_GetChannelInfo returned %d for channel %d\n", status, channel);
            continue;
        }

        printf("SPI channel % d: ", channel);
        if (channel == USE_MPSSE)
        {
            printf("selected\n");
            /*print the dev info*/
            printf("\t\tVID/PID: 0x%04x/0x%04x\n", devList.ID >> 16, devList.ID & 0xffff);
            printf("\t\tSerialNumber: %s\n", devList.SerialNumber);
            printf("\t\tDescription: %s\n", devList.Description);
        }
        else
        {
            printf("ignored\n");
        }
    }

    if (channels > USE_MPSSE)
    {
        // Open the the channel specified by the USE_MPSSE macro.
        // This must be defined to get this far.
        channel = USE_MPSSE;

        status = SPI_OpenChannel(channel, &ftHandle);
        if (status != FT_OK)
        {
            fprintf(stderr, "Channel %d failed to open status %d\n", channel, status);
            exit (-2);
        }
        status = SPI_InitChannel(ftHandle, &channelConf);
        if (status != FT_OK)
        {
            fprintf(stderr, "Channel %d failed to initialise SPI status %d\n", channel, status);
            exit (-3);
        }
    }
    else
    {
        fprintf(stderr, "No SPI channels found\n");
        exit (-1);
    }

    MCU_buffer = malloc(MCU_BUFFER_SIZE);
    if (MCU_buffer == NULL)
    {
        fprintf(stderr, "Setup malloc failed\n");
        exit(-99);
    }
    MCU_bufferLen = 0;
}

void MCU_Deinit(void)
{
    SPI_CloseChannel(ftHandle);
    Cleanup_libMPSSE();
    
    ftHandle = NULL;
}

void MCU_Setup(void)
{
}

// ------------------------- Output buffering ----------------------------------

void MCU_transmit_buffer(void)
{
    FT_STATUS status;
    DWORD transferred;

    status = SPI_Write(ftHandle, (uint8_t *)MCU_buffer, MCU_bufferLen, &transferred, 0);
     if (FT_OK != status)
     {
         // spi master write failed
        fprintf(stderr, "MCU_transmit_buffer failed %d\n", status);
    }
    MCU_bufferLen = 0;
}

int MCU_append_buffer(const uint8_t *buffer, uint16_t length)
{
    int i = MCU_bufferLen;
    int j = 0;
    int plength;

    while (j < length)
    {
        plength = length - j;
        
        if (plength + MCU_bufferLen >= MCU_BUFFER_SIZE)
            plength = MCU_BUFFER_SIZE - MCU_bufferLen;
        
        /* NOTE: memcpy is used here as the libft4222 is not generally 
         * used for embedded systems.
         */
        memcpy(&MCU_buffer[i], &buffer[j], plength);
        j += plength;
        i += plength;
        MCU_bufferLen += plength;
        if (MCU_bufferLen >= MCU_BUFFER_SIZE)
        {
            MCU_transmit_buffer();
            i = 0;
        }
    }

    return i;
}

// --------------------- Chip Select line low ----------------------------------
void MCU_CSlow(void)
{
    SPI_ToggleCS(ftHandle, TRUE);
}

// --------------------- Chip Select line high ---------------------------------
void MCU_CShigh(void)
{
    MCU_transmit_buffer();
    SPI_ToggleCS(ftHandle, FALSE);
}

// -------------------------- PD line low --------------------------------------
void MCU_PDlow(void)
{
    // PD# set to 0, connect BLUE wire of MPSSE to PD# of FT8xx board
    SPI_ToggleCS(ftHandle, FALSE);
    SPI_ChangeCS(ftHandle, SPI_CONFIG_OPTION_MODE0 | SPI_CONFIG_OPTION_CS_DBUS7 | SPI_CONFIG_OPTION_CS_ACTIVELOW);
    SPI_ToggleCS(ftHandle, TRUE);
}

// ------------------------- PD line high --------------------------------------
void MCU_PDhigh(void)
{
    // PD# set to 1, connect BLUE wire of MPSSE to PD# of FT8xx board
    SPI_ChangeCS(ftHandle, SPI_CONFIG_OPTION_MODE0 | SPI_CONFIG_OPTION_CS_DBUS7 | SPI_CONFIG_OPTION_CS_ACTIVELOW);
    SPI_ToggleCS(ftHandle, FALSE);
    SPI_ChangeCS(ftHandle, SPI_CONFIG_OPTION_MODE0 | SPI_CONFIG_OPTION_CS_DBUS3 | SPI_CONFIG_OPTION_CS_ACTIVELOW);
    SPI_ToggleCS(ftHandle, FALSE);
}

// ------------------------- Delay functions -----------------------------------

void MCU_Delay_20ms(void)
{
#ifdef _WIN32
    Sleep(20);
#else
    usleep(20 * 1000);
#endif
}

void MCU_Delay_500ms(void)
{
#ifdef _WIN32
    Sleep(500);
#else
    usleep(500 * 1000);
#endif
}

// --------------------- SPI Send and Receive ----------------------------------

uint8_t MCU_SPIRead8(void)
{
    FT_STATUS status;
    uint8_t DataRead = 0;
    DWORD transferred;

    MCU_transmit_buffer();
    status = SPI_Read(ftHandle, &DataRead, 1, &transferred, 0);
     if (FT_OK != status)
     {
         // spi master read failed
        fprintf(stderr, "MCU_SPIRead8 failed %d\n", status);
    }

    return DataRead;
}

void MCU_SPIWrite8(uint8_t DataToWrite)
{
    MCU_append_buffer(&DataToWrite, 1);
}

uint16_t MCU_SPIRead16(void)
{
    FT_STATUS status;
    uint16_t DataRead = 0;
    DWORD transferred;

    MCU_transmit_buffer();
    status = SPI_Read(ftHandle, (UCHAR *)&DataRead, 2, &transferred, 0);
     if (FT_OK != status)
     {
         // spi master read failed
        fprintf(stderr, "MCU_SPIRead16 failed %d\n", status);
    }

    return DataRead;
}

void MCU_SPIWrite16(uint16_t DataToWrite)
{
    MCU_append_buffer((const uint8_t *)&DataToWrite, 2);
}

uint32_t MCU_SPIRead24(void)
{
    FT_STATUS status;
    uint32_t DataRead = 0;
    DWORD transferred;

    MCU_transmit_buffer();
    status = SPI_Read(ftHandle, (UCHAR *)&DataRead, 3, &transferred, 0);
     if (FT_OK != status)
     {
         // spi master read failed
        fprintf(stderr, "MCU_SPIRead24 failed %d\n", status);
    }

    return DataRead;
}

void MCU_SPIWrite24(uint32_t DataToWrite)
{
    MCU_append_buffer((const uint8_t *)&DataToWrite, 3);
}

uint32_t MCU_SPIRead32(void)
{
    FT_STATUS status;
    uint32_t DataRead = 0;
    DWORD transferred;

    MCU_transmit_buffer();
    status = SPI_Read(ftHandle, (UCHAR *)&DataRead, 4, &transferred, 0);
     if (FT_OK != status)
     {
         // spi master read failed
        fprintf(stderr, "MCU_SPIRead32 failed %d\n", status);
    }

    return DataRead;
}

void MCU_SPIWrite32(uint32_t DataToWrite)
{
    MCU_append_buffer((const uint8_t *)&DataToWrite, 4);
}

void MCU_SPIRead(uint8_t *DataToRead, uint32_t length)
{
    FT_STATUS status;
    DWORD transferred;

    MCU_transmit_buffer();
    status = SPI_Read(ftHandle, (UCHAR *)DataToRead, length, &transferred, 0);
     if (FT_OK != status)
     {
         // spi master read failed
        fprintf(stderr, "MCU_SPIRead failed %d\n", status);
    }
}

void MCU_SPIWrite(const uint8_t *DataToWrite, uint32_t length)
{
    MCU_append_buffer(DataToWrite, length);
}

uint16_t MCU_htobe16(uint16_t h)
{
#ifdef _WIN32
    return _byteswap_ushort(h);
#else // _WIN32
    return htobe16(h);
#endif // _WIN32
    }

uint32_t MCU_htobe32(uint32_t h)
{
#ifdef _WIN32
    return _byteswap_ulong(h);
#else // _WIN32
    return htobe32(h);
#endif // _WIN32
}

uint16_t MCU_htole16(uint16_t h)
{
#ifdef _WIN32
    return (h);
#else // _WIN32
    return htole16(h);
#endif // _WIN32
}

uint32_t MCU_htole32(uint32_t h)
{
#ifdef _WIN32
    return (h);
#else // _WIN32
    return htole32(h);
#endif // _WIN32
}

uint16_t MCU_be16toh(uint16_t h)
{
#ifdef _WIN32
    return _byteswap_ushort(h);
#else // _WIN32
    return be16toh(h);
#endif // _WIN32
}

uint32_t MCU_be32toh(uint32_t h)
{
#ifdef _WIN32
    return _byteswap_ulong(h);
#else // _WIN32
    return be32toh(h);
#endif // _WIN32
}

uint16_t MCU_le16toh(uint16_t h)
{
#ifdef _WIN32
    return (h);
#else // _WIN32
    return le16toh(h);
#endif // _WIN32
}

uint32_t MCU_le32toh(uint32_t h)
{
#ifdef _WIN32
    return (h);
#else // _WIN32
    return le32toh(h);
#endif // _WIN32
}

#endif /* defined(USE_MPSSE) */
