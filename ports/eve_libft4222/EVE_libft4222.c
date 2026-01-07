/**
 @file EVE_libft4222.c
 */
/*
 * ============================================================================
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
// USE_FT4222 holds the FT4222H channel to open.
// In gcc compilers this is in the Makefile. -DUSE_FT4222=0
// In VisualStudio this is in Project Properties -> Configuration Properties -> 
//     C/C++ -> Preprocessor -> Preprocessor Definitions.
#if defined(USE_FT4222)

#pragma message ("Compiling " __FILE__ " for libFT4222")

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
#include "libft4222.h"


// This is the Windows Platform specific section and contains the functions which
// enable the GPIO and SPI interfaces.

#define FT8XX_CS_N_PIN   1    /* GPIO is not utilized in Lib4222 as it is directly managed by firmware */
#define FT8XX_PD_N_PIN   GPIO_PORT0
/* GPIO0         , GPIO1      , GPIO2       , GPIO3         } */
GPIO_Dir gpio_dir[4] = { GPIO_OUTPUT , GPIO_OUTPUT, GPIO_OUTPUT, GPIO_OUTPUT };

// ----------------------- MCU Transmit Buffering  -----------------------------

#define MCU_BUFFER_SIZE 512
uint8_t *MCU_buffer;
uint16_t MCU_bufferLen;

// ------------------ Platform specific initialisation  ------------------------

FT_HANDLE ftHandleSPI;
FT_HANDLE ftHandleGPIO;

static void mcu_setup_spi(FT4222_SPIClock div)
{
    FT_STATUS ftStatus;

    //Set default Read timeout 5s and Write timeout 5sec
    ftStatus = FT_SetTimeouts(ftHandleSPI, 5000, 5000);
    if (FT_OK != ftStatus)
    {
        fprintf(stderr, "FT4222 SPI FT_SetTimeouts failed!\n");
        exit(-3);
    }

    // no latency to usb
    ftStatus = FT_SetLatencyTimer(ftHandleSPI, 2);
    if (FT_OK != ftStatus)
    {
        fprintf(stderr, "FT4222 SPI FT_SetLatencyTimer failed!\n");
        exit(-4);
    }

    /* Set SPI clock speed to 20 MHz - See the notes for MCU_SPI_TIMEOUT in the MCU.h file. */
    ftStatus = FT4222_SPIMaster_Init(ftHandleSPI, SPI_IO_SINGLE, div, CLK_IDLE_LOW, CLK_LEADING, FT8XX_CS_N_PIN);
    if (FT_OK != ftStatus)
    {
        fprintf(stderr, "Init FT4222 as SPI master device failed!\n");
        exit(-5);
    }

    ftStatus = FT4222_SPIMaster_SetCS(ftHandleSPI, CS_ACTIVE_LOW);
    if (FT_OK != ftStatus)
    {
        fprintf(stderr, "Init FT4222 as CS set failed!\n");
        exit(-8);
    }
}

void MCU_Init(void)
{
    FT_STATUS ftStatus;

    DWORD numOfDevices = 0;
    DWORD countSPI = USE_FT4222;
    DWORD countGPIO = USE_FT4222;

    ftStatus = FT_CreateDeviceInfoList(&numOfDevices);

    DWORD devNumSPI = -1;
    DWORD devNumGPIO = -1;

    for (DWORD iDev = 0; iDev < numOfDevices; ++iDev)
    {
        FT_DEVICE_LIST_INFO_NODE devInfo;
        memset(&devInfo, 0, sizeof(devInfo));

        ftStatus = FT_GetDeviceInfoDetail(iDev, &devInfo.Flags, &devInfo.Type, &devInfo.ID, &devInfo.LocId,
                                        devInfo.SerialNumber,
                                        devInfo.Description,
                                        &devInfo.ftHandle);
        if (ftStatus != FT_OK)
        {
            printf("FT_GetDeviceInfoDetail returned %d for interface %d\n", ftStatus, iDev);
            continue;
        }

        // Proceed with FT4222H devices configured in mode 0 (single SPI + GPIO)
        if (devInfo.Type != FT_DEVICE_4222H_0)
        {
            continue;
        }

        printf("FT4222 device % d: ", iDev);

        if( ! strcmp( devInfo.Description, "FT4222 A"))
        {
            if (countSPI == 0)
            {
                devNumSPI = devInfo.LocId;
                printf("selected for SPI\n");
                printf("\t\tVID/PID: 0x%04x/0x%04x\n", devInfo.ID >> 16, devInfo.ID & 0xffff);
                printf("\t\tSerialNumber: %s\n", devInfo.SerialNumber);
                printf("\t\tDescription: %s\n", devInfo.Description);
            }
            else
            {
                printf("ignored\n");
            }
            countSPI--;
        }

        if( ! strcmp( devInfo.Description, "FT4222 B"))
        {
            if (countGPIO == 0)
            {
                devNumGPIO = devInfo.LocId;
                printf("selected for GPIO\n");
                printf("\t\tVID/PID: 0x%04x/0x%04x\n", devInfo.ID >> 16, devInfo.ID & 0xffff);
                printf("\t\tSerialNumber: %s\n", devInfo.SerialNumber);
                printf("\t\tDescription: %s\n", devInfo.Description);
            }
            else
            {
                printf("ignored\n");
            }
            countGPIO--;
        }
    }

    if ((devNumSPI != -1) && (devNumGPIO != -1))
    {
        ftStatus = FT_OpenEx((PVOID)(uintptr_t)devNumSPI, FT_OPEN_BY_LOCATION, &ftHandleSPI);
        if (FT_OK != ftStatus)
        {
            fprintf(stderr, "Open a FT4222 SPI device failed!\n");
            exit(-2);
        }

        ftStatus = FT_OpenEx((PVOID)(uintptr_t)devNumGPIO, FT_OPEN_BY_LOCATION, &ftHandleGPIO);
        if (FT_OK != ftStatus)
        {
            fprintf(stderr, "Open a FT4222 GPIO device failed!\n");
            exit(-2);
        }

        // Set SPI clock speed to 1.25 MHz
        // 1.25 MHz allows all EVE devices to initialise correctly
        // After initialisation the SPI speed can be increased in the MCU_Setup()
        // Clock is 80 MHz / 64 = 1.25 MHz
        mcu_setup_spi(CLK_DIV_64);

        ftStatus = FT4222_SetClock(ftHandleGPIO, SYS_CLK_80);
        if (FT_OK != ftStatus)
        {
            fprintf(stderr, "Set clock failed!\n");
            exit(-6);
        }

        ftStatus = FT4222_SetSuspendOut(ftHandleGPIO, FALSE);
        if (FT_OK != ftStatus)
        {
            fprintf(stderr, "Disable suspend out function on GPIO2 failed!\n");
            exit(-6);
        }

        ftStatus = FT4222_SetWakeUpInterrupt(ftHandleGPIO, FALSE);
        if (FT_OK != ftStatus)
        {
            fprintf(stderr, "Disable wakeup/interrupt feature on GPIO3 failed!\n");
            exit(-7);
        }

        /* Interface 2 is GPIO */
        ftStatus = FT4222_GPIO_Init(ftHandleGPIO, gpio_dir);
        if (FT_OK != ftStatus)
        {
            fprintf(stderr, "Init FT4222 as GPIO interface failed!\n");
            exit(-9);
        }
    }
    else
    {
        fprintf(stderr, "No FT4222 channels found\n");
        exit(-1);
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
    FT_Close(ftHandleSPI);
    FT_Close(ftHandleGPIO);
    
    ftHandleSPI = ftHandleGPIO = NULL;
}

void MCU_Setup(void)
{
    // Increase SPI speed to 20 MHz after initialisation is complete
    // See the notes for MCU_SPI_TIMEOUT in the MCU.h file.
    // Clock is 80 MHz / 4 = 20 MHz
    mcu_setup_spi(CLK_DIV_4);
}

// ------------------------- Output buffering ----------------------------------

int MCU_transmit_buffer(int end)
{
    FT_STATUS status;
    uint16_t transferred;
    int toWrite = MCU_bufferLen;

    if (toWrite)
    {
        status = FT4222_SPIMaster_SingleWrite(ftHandleSPI, (uint8_t *)MCU_buffer, MCU_bufferLen, &transferred, end);
        if (FT4222_OK != status)
        {
            // spi master read failed
            fprintf(stderr, "FT4222_SPIMaster_SingleWrite failed %d\n", status);
        }
        else
        {
            toWrite = 0;
        }
    }
    MCU_bufferLen = 0;

    return toWrite;
}

int MCU_append_buffer(const uint8_t *buffer, uint16_t length, int end)
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
            if (j < length)
                MCU_transmit_buffer(0);
            else
                MCU_transmit_buffer(end);
            i = 0;
        }
    }

    return i;
}

// --------------------- Chip Select line low ----------------------------------
void MCU_CSlow(void)
{
    // No action. CS is under control of the FT4222H.
}

// --------------------- Chip Select line high ---------------------------------
void MCU_CShigh(void)
{
    if (MCU_transmit_buffer(1) == 0)
    {
        // Pull CS high with a dummy read to address zero.
        // This is only required after an unaddressed read.
        FT_STATUS status;
        uint32_t DataToWrite = 0;
        uint32_t DataToRead = 0;
        uint16_t transferred;

        status = FT4222_SPIMaster_SingleReadWrite(ftHandleSPI, (uint8_t *)&DataToRead, (uint8_t *)&DataToWrite, 4, &transferred, 1);
        if (FT4222_OK != status)
        {
            // spi master read failed
            fprintf(stderr, "MCU_CShigh failed %d\n", status);
        }
    }
}

// -------------------------- PD line low --------------------------------------
void MCU_PDlow(void)
{
    // PD# set to 0, connect BLUE wire of MPSSE to PD# of FT8xx board
    if (FT4222_OK != (FT4222_GPIO_Write(ftHandleGPIO, FT8XX_PD_N_PIN, 0)))
    {
        fprintf(stderr, "FT4222 GPIO change failed!\n");
    }
}

// ------------------------- PD line high --------------------------------------
void MCU_PDhigh(void)
{
    // PD# set to 1, connect BLUE wire of MPSSE to PD# of FT8xx board
    if (FT4222_OK != (FT4222_GPIO_Write(ftHandleGPIO, FT8XX_PD_N_PIN, 1)))
    {
        fprintf(stderr, "FT4222 GPIO change failed!\n");
    }
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
    uint16_t transferred;

    MCU_transmit_buffer(0);
    status = FT4222_SPIMaster_SingleRead(ftHandleSPI, (uint8_t *)&DataRead, 1, &transferred, 0);
     if (FT4222_OK != status)
     {
         // spi master read failed
        fprintf(stderr, "MCU_SPIRead8 failed %d\n", status);
    }
 
    return DataRead;
}

void MCU_SPIWrite8(uint8_t DataToWrite)
{
    MCU_append_buffer((uint8_t *)&DataToWrite, 1, 0);
}

uint16_t MCU_SPIRead16(void)
{
    FT_STATUS status;
    uint16_t DataRead;
    uint16_t transferred;

    MCU_transmit_buffer(0);
    status = FT4222_SPIMaster_SingleRead(ftHandleSPI, (uint8_t *)&DataRead, 2, &transferred, 0);
     if (FT4222_OK != status)
     {
         // spi master read failed
        fprintf(stderr, "MCU_SPIRead16 failed %d\n", status);
    }

    return DataRead;
}

void MCU_SPIWrite16(uint16_t DataToWrite)
{
    MCU_append_buffer((uint8_t *)&DataToWrite, 2, 0);
}

uint32_t MCU_SPIRead24(void)
{
    FT_STATUS status;
    uint32_t DataRead;
    uint16_t transferred;

    MCU_transmit_buffer(0);
    status = FT4222_SPIMaster_SingleRead(ftHandleSPI, (uint8_t *)&DataRead, 3, &transferred, 0);
     if (FT4222_OK != status)
     {
         // spi master read failed
        fprintf(stderr, "MCU_SPIRead24 failed %d\n", status);
    }

    return DataRead;
}

void MCU_SPIWrite24(uint32_t DataToWrite)
{
    MCU_append_buffer((uint8_t *)&DataToWrite, 3, 0);
}

uint32_t MCU_SPIRead32(void)
{
    FT_STATUS status;
    uint32_t DataRead = 0;
    uint16_t transferred;

    MCU_transmit_buffer(0);
    status = FT4222_SPIMaster_SingleRead(ftHandleSPI, (uint8_t *)&DataRead, 4, &transferred, 0);
     if (FT4222_OK != status)
     {
         // spi master read failed
        fprintf(stderr, "MCU_SPIRead32 failed %d\n", status);
    }

    return DataRead;
}

void MCU_SPIWrite32(uint32_t DataToWrite)
{
    MCU_append_buffer((uint8_t *)&DataToWrite, 4, 0);
}

void MCU_SPIRead(uint8_t *DataToRead, uint32_t length)
{
    FT_STATUS status;
    uint16_t transferred;

    MCU_transmit_buffer(0);
    status = FT4222_SPIMaster_SingleRead(ftHandleSPI, (uint8_t *)DataToRead, length, &transferred, 0);
     if (FT4222_OK != status)
     {
         // spi master read failed
        fprintf(stderr, "MCU_SPIRead failed %d\n", status);
    }
}

void MCU_SPIWrite(const uint8_t *DataToWrite, uint32_t length)
{
    MCU_append_buffer(DataToWrite, length, 0);
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

#endif /* defined(USE_FT4222) */
