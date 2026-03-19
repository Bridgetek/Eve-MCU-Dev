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
 * https://brtchip.com/wp-content/uploads/2021/11/BRT_Software_License_Agreement.pdf ("the Licence Terms").
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

/* EVE MCU HEADER */

#if defined(__linux__) || defined(__CYGWIN__)
// Linux endianness (not BSD variants)
#include <endian.h>
#include <unistd.h>
#elif defined(_WIN32)
// Windows endianness is little endian
#else
// Other endianness (check naming conventions)
#include <sys/endian.h>
#endif // _WIN32

// From issue #25
#if defined(BYTE_ORDER) && BYTE_ORDER == ORDER_LITTLE_ENDIAN
#define HOST_IS_LITTLE_ENDIAN 1
#elif defined(BYTE_ORDER) && BYTE_ORDER == ORDER_BIG_ENDIAN
#define HOST_IS_LITTLE_ENDIAN 0
#endif

#if defined(QUADSPI_ENABLE)
#if IS_EVE_API(1)
#error Quad SPI is not supported on EVE API 1 (FT80x)
#endif
#pragma message ("libFT4222 Quad SPI enabled")
#else
#pragma message ("libFT4222 Single SPI enabled")
#endif

#include "ftd2xx.h"
#include "libft4222.h"

/* EVE MCU HEADER END */

#include <EVE.h>
#include <HAL.h>
#include <MCU.h>

/* EVE MCU */

// This platform specific section contains the functions which
// enable the GPIO and SPI interfaces.

// GPIO is not utilized in Lib4222 as it is directly managed by firmware.
#define FT8XX_CS_N_PIN   1    
#define FT8XX_PD_N_PIN   GPIO_PORT0
// GPIO0         , GPIO1      , GPIO2       , GPIO3         }
static GPIO_Dir gpio_dir[4] = { GPIO_OUTPUT , GPIO_OUTPUT, GPIO_INPUT, GPIO_OUTPUT };

// ----------------------- MCU Transmit Buffering  -----------------------------

#define MCU_BUFFER_SIZE (256 + 16)
static uint8_t *MCU_buffer;
static uint16_t MCU_bufferLen;

// ------------------ Platform specific initialisation  ------------------------

static FT_HANDLE ftHandleSPI;
static FT_HANDLE ftHandleGPIO;

// QuadSPI enabled
static int ftIsQuad = FALSE;

static void mcu_setup_spi(FT4222_SPIClock div, FT4222_SPIMode mode)
{
    FT_STATUS ftStatus;

    //Set default Read timeout 5s and Write timeout 5sec
    ftStatus = FT_SetTimeouts(ftHandleSPI, 5000, 5000);
    if (FT_OK != ftStatus)
    {
        DEBUG_ERROR("FT4222 Setup FT_SetTimeouts failed: %d\n", ftStatus);
        exit(ftStatus);
    }

    // no latency to usb
    ftStatus = FT_SetLatencyTimer(ftHandleSPI, 2);
    if (FT_OK != ftStatus)
    {
        DEBUG_ERROR("FT4222 Setup FT_SetLatencyTimer failed: %d\n", ftStatus);
        exit(ftStatus);
    }

    /* Set SPI clock speed to 20 MHz - See the notes for MCU_SPI_TIMEOUT in the MCU.h file. */
    ftStatus = FT4222_SPIMaster_Init(ftHandleSPI, SPI_IO_SINGLE, div, CLK_IDLE_LOW, CLK_LEADING, FT8XX_CS_N_PIN);
    if (FT_OK != ftStatus)
    {
        DEBUG_ERROR("FT4222 Setup SPIMaster Init failed: %d\n", ftStatus);
        exit(ftStatus);
    }

    ftStatus = FT4222_SPIMaster_SetCS(ftHandleSPI, CS_ACTIVE_LOW);
    if (FT_OK != ftStatus)
    {
        DEBUG_ERROR("FT4222 Setup SPIMaster SetCS set failed: %d\n", ftStatus);
        exit(ftStatus);
    }

    if (mode != SPI_IO_NONE)
    {
        ftStatus = FT4222_SPIMaster_SetLines(ftHandleSPI, mode);
        if (FT_OK != ftStatus)
        {
            DEBUG_ERROR("FT4222 Setup SPIMaster SetLines failed: %d\n", ftStatus);
            exit(ftStatus);
        }
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
            DEBUG_PRINTF("FT4222 Init FT_GetDeviceInfoDetail returned %d for interface %d\n", ftStatus, iDev);
            continue;
        }

        // Proceed only with FT4222H devices configured in mode 0 (single SPI + GPIO)
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
            DEBUG_ERROR("FT4222 Init Open FT4222 SPI device failed: %d\n", ftStatus);
            exit(ftStatus);
        }

        ftStatus = FT_OpenEx((PVOID)(uintptr_t)devNumGPIO, FT_OPEN_BY_LOCATION, &ftHandleGPIO);
        if (FT_OK != ftStatus)
        {
            DEBUG_ERROR("FT4222 Init Open FT4222 GPIO device failed: %d\n", ftStatus);
            exit(ftStatus);
        }

        // Set SPI clock speed to 1.25 MHz
        // 1.25 MHz allows all EVE devices to initialise correctly
        // After initialisation the SPI speed can be increased in the MCU_Setup()
        // Clock is 80 MHz / 64 = 1.25 MHz
        mcu_setup_spi(CLK_DIV_64, SPI_IO_SINGLE);

        ftStatus = FT4222_SetClock(ftHandleGPIO, SYS_CLK_80);
        if (FT_OK != ftStatus)
        {
            DEBUG_ERROR("FT4222 Init SetClock failed: %d\n", ftStatus);
            exit(ftStatus);
        }

        ftStatus = FT4222_SetSuspendOut(ftHandleGPIO, FALSE);
        if (FT_OK != ftStatus)
        {
            DEBUG_ERROR("FT4222 Init Disable Suspend Out function on GPIO2 failed: %d\n", ftStatus);
            exit(ftStatus);
        }

        ftStatus = FT4222_SetWakeUpInterrupt(ftHandleGPIO, FALSE);
        if (FT_OK != ftStatus)
        {
            DEBUG_ERROR("FT4222 Init Disable Wakeup/Interrupt feature on GPIO3 failed: %d\n", ftStatus);
            exit(ftStatus);
        }

        /* Interface 2 is GPIO */
        ftStatus = FT4222_GPIO_Init(ftHandleGPIO, gpio_dir);
        if (FT_OK != ftStatus)
        {
            DEBUG_ERROR("FT4222 Init FT4222 as GPIO interface failed: %d\n", ftStatus);
            exit(ftStatus);
        }
    }
    else
    {
        DEBUG_ERROR("No FT4222 channels found\n");
        exit(-1);
    }

    MCU_buffer = malloc(MCU_BUFFER_SIZE);
    if (MCU_buffer == NULL)
    {
        DEBUG_ERROR("Setup malloc failed\n");
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
#if defined QUADSPI_ENABLE
    HAL_SetSPIMode(2);
    mcu_setup_spi(CLK_DIV_4, SPI_IO_QUAD);
    ftIsQuad = TRUE;
#else // QUADSPI_ENABLE
    mcu_setup_spi(CLK_DIV_4, SPI_IO_SINGLE);
#endif // QUADSPI_ENABLE
}

// ------------------------- Output buffering ----------------------------------

static int MCU_multi_transfer(uint8_t *DataToRead, uint32_t len)
{
    FT_STATUS status = FT_OK;
    uint32_t transferred;

    /* Read/write address must always be in the MCU_buffer at this point. */
    if (MCU_bufferLen)
    {
        /* Send data in the write buffer (including the address).
         * Followed by any further write data. 
         * Finally, if there is data to read then read this. 
         * There will be only read OR write operations to perform. */
        status = FT4222_SPIMaster_MultiReadWrite(ftHandleSPI, (uint8_t *)DataToRead, (uint8_t *)MCU_buffer, 0, MCU_bufferLen, len, &transferred);

        // Data is now sent.
        MCU_bufferLen = 0;
    }

    return status;
}

static int MCU_transmit_buffer(int end)
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
            DEBUG_ERROR("FT4222 SPIMaster Write failed %d\n", status);
            exit(status);
        }
        else
        {
            toWrite = 0;
        }
    }
    MCU_bufferLen = 0;

    return toWrite;
}

static int MCU_receive_buffer(uint8_t *DataToRead, uint32_t len, int end)
{
    FT_STATUS status = FT_OK;

    if (ftIsQuad)
    {
        MCU_multi_transfer(DataToRead, len);
    }
    else
    {
        uint16_t transferred;

        MCU_transmit_buffer(0);
        status = FT4222_SPIMaster_SingleRead(ftHandleSPI, (uint8_t *)DataToRead, len, &transferred, end);
    }

    return status;
}

static int MCU_append_buffer(const uint8_t *buffer, uint16_t length, int end)
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
            if (ftIsQuad)
            {
                MCU_multi_transfer(NULL, 0);
                /* Keep the previous write address in the transmit buffer if we are not
                 * ending this transfer. */
                if (end == 0)
                {
                    /* Refill buffer after address. */
#if IS_EVE_API(1, 2, 3, 4) /* Different write addressing method on BT82x */
                    /* 24 bit write address followed by data */
                    i = 3;
                    MCU_bufferLen = 3;
#else
                    /* 32 bit write address followed by data */
                    i = 4;
                    MCU_bufferLen = 4;
#endif
                }
            }
            else
            {
                if (j < length)
                    MCU_transmit_buffer(0);
                else
                    MCU_transmit_buffer(end);
                /* Refill buffer from start. No need to resend address. */
                i = 0;
            }
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
    if (ftIsQuad)
    {
        MCU_multi_transfer(NULL, 0);
    }
    else
    {
        if (MCU_transmit_buffer(1) == 0)
        {
            // Pull CS high with a dummy read to address zero.
            // This is only required after an unaddressed read.
            FT_STATUS status;
            uint32_t DataToWrite = 0;
            uint32_t DataToRead = 0;

            MCU_bufferLen = 4;
            memset(MCU_buffer, 0, 4);
            status = MCU_transmit_buffer(1);
            if (FT4222_OK != status)
            {
                // spi master read failed
                DEBUG_ERROR("FT4222 MCU_CShigh failed %d\n", status);
                exit(status);
            }
        }
    }
}

// -------------------------- PD line low --------------------------------------
void MCU_PDlow(void)
{
    // PD# set to 0, connect BLUE wire of MPSSE to PD# of FT8xx board
    if (FT4222_OK != (FT4222_GPIO_Write(ftHandleGPIO, FT8XX_PD_N_PIN, 0)))
    {
        DEBUG_ERROR("FT4222 MCU_PDlow change failed!\n");
        exit(-100);
    }
}

// ------------------------- PD line high --------------------------------------
void MCU_PDhigh(void)
{
    // PD# set to 1, connect BLUE wire of MPSSE to PD# of FT8xx board
    if (FT4222_OK != (FT4222_GPIO_Write(ftHandleGPIO, FT8XX_PD_N_PIN, 1)))
    {
        DEBUG_ERROR("FT4222 MCU_PDhigh change failed!\n");
        exit(-100);
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

    status = MCU_receive_buffer((uint8_t *)&DataRead, 1, 0);
    if (FT4222_OK != status)
    {
         // spi master read failed
        DEBUG_ERROR("FT4222 MCU_SPIRead8 failed %d\n", status);
        exit(status);
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

    status = MCU_receive_buffer((uint8_t *)&DataRead, 2, 0);
    if (FT4222_OK != status)
    {
         // spi master read failed
        DEBUG_ERROR("MCU_SPIRead16 failed %d\n", status);
        exit(status);
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

    status = MCU_receive_buffer((uint8_t *)&DataRead, 3, 0);
    if (FT4222_OK != status)
    {
         // spi master read failed
        DEBUG_ERROR("MCU_SPIRead24 failed %d\n", status);
        exit(status);
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

    status = MCU_receive_buffer((uint8_t *)&DataRead, 4, 0);
    if (FT4222_OK != status)
    {
         // spi master read failed
        DEBUG_ERROR("MCU_SPIRead32 failed %d\n", status);
        exit(status);
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

    status = MCU_receive_buffer((uint8_t *)DataToRead, length, 0);
    if (FT4222_OK != status)
    {
         // spi master read failed
        DEBUG_ERROR("MCU_SPIRead failed %d\n", status);
        exit(status);
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

/* EVE MCU END */

#endif /* defined(USE_FT4222) */
