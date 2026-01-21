/**
 @file EVE_tester.c
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
#if defined(SOFTWARE_SIMULATOR)

#pragma message ("Compiling " __FILE__ " for software tester")

#include <string.h>
#include <stdio.h>
#include <stdint.h> // for Uint8/16/32 and Int8/16/32 data types
#include <stdlib.h>
#ifndef _WIN32 // Windows is always little-endian (for now)
#include <endian.h>
#include <unistd.h>
#endif // _WIN32

#include <EVE.h>
#include <MCU.h>

extern uint16_t HAL_GetCmdPointer(void);

#if IS_EVE_API(1, 2, 3, 4)
#define HOSTCMDLEN 3
#else
#define HOSTCMDLEN 5
#endif

enum eve_state {
    state_reset,
    state_booting,
    state_idle,
    state_address,
    state_write,
    state_write_cont,
    state_read,
    state_read_cont,
    state_end
} state = state_end;

uint32_t address;
int hostptr = 0;
uint8_t hostcmd[16];

char const *get_state()
{
    if (state == state_reset) return "reset";
    else if (state == state_booting) return "booting";
    else if (state == state_idle) return "idle";
    else if (state == state_address) return "address";
    else if (state == state_write) return "write";
    else if (state == state_read) return "read";
    else if (state == state_write_cont) return "write continue";
    else if (state == state_read_cont) return "read continue";
    else if (state == state_end) return "end";
    return "unknown";
}

char const *get_register()
{
    if (address == EVE_REG_ID) return "REG_ID";
    else if (address == EVE_REG_CPURESET) return "REG_CPURESET";
    else if (address == EVE_REG_CMD_READ) return "REG_CMD_READ";
    else if (address == EVE_REG_CMD_WRITE) return "REG_CMD_WRITE";
#ifdef EVE_USE_CMDB_METHOD
    else if (address == EVE_REG_CMDB_SPACE) return "REG_CMDB_SPACE";
    else if (address == EVE_REG_CMDB_WRITE) return "REG_CMDB_WRITE";
#endif
    else if (address == EVE_REG_PCLK_POL) return "REG_PCLK_POL";
    else if (address == EVE_REG_FREQUENCY) return "REG_FREQUENCY";
#if IS_EVE_API(1,2,3,4)
    else if (address == EVE_REG_PCLK) return "REG_PCLK";
    else if (address == EVE_REG_CSPREAD) return "REG_CSPREAD";
    else if (address == EVE_REG_SWIZZLE) return "REG_SWIZZLE";
    else if (address == EVE_REG_DITHER) return "REG_DITHER";
    else if (address == EVE_REG_OUTBITS) return "REG_OUTBITS";
    else if (address == EVE_REG_ROTATE) return "REG_ROTATE";
    else if (address == EVE_REG_TOUCH_RZTHRESH) return "REG_TOUCH_RZTHRESH";
    else if (address == EVE_REG_VOL_PB) return "REG_VOL_PB";
#endif
    else if (address == EVE_REG_VSYNC1) return "REG_VSYNC1";
    else if (address == EVE_REG_VSYNC0) return "REG_VSYNC0";
    else if (address == EVE_REG_VSIZE) return "REG_VSIZE";
    else if (address == EVE_REG_VOFFSET) return "REG_VOFFSET";
    else if (address == EVE_REG_VCYCLE) return "REG_VCYCLE";
    else if (address == EVE_REG_HSYNC1) return "REG_HSYNC1";
    else if (address == EVE_REG_HSYNC0) return "REG_HSYNC0";
    else if (address == EVE_REG_HSIZE) return "REG_HSIZE";
    else if (address == EVE_REG_HOFFSET) return "REG_HOFFSET";
    else if (address == EVE_REG_HCYCLE) return "REG_HCYCLE";
    else if (address == EVE_REG_DLSWAP) return "REG_DLSWAP";
    else if (address == EVE_REG_GPIO) return "REG_GPIO";
    else if (address == EVE_REG_GPIO_DIR) return "REG_GPIO_DIR";
    else if (address == EVE_REG_PWM_DUTY) return "REG_PWM_DUTY";
    else if (address == EVE_REG_VOL_SOUND) return "REG_VOL_SOUND";
    else if (address == EVE_REG_SOUND) return "REG_SOUND";
    else if (address == EVE_REG_GPIO_DIR) return "REG_GPIO_DIR";
    else if ((address >= EVE_RAM_DL) && (address < (EVE_RAM_DL + EVE_RAM_DL_SIZE)))
        return "RAM_DL";
    else if ((address >= EVE_RAM_CMD) && (address < (EVE_RAM_CMD + EVE_RAM_CMD_SIZE)))
        return "RAM_CMD";
    else if ((address >= EVE_RAM_G) && (address < (EVE_RAM_G + EVE_RAM_G_SIZE)))
        return "RAM_G";
#if IS_EVE_API(5)
    else if (address == EVE_REG_BOOT_STATUS) return "REG_BOOT_STATUS";
    else if ((address >= EVE_COPROC_REPORT) && (address < (EVE_COPROC_REPORT + 256)))
        return "COPROC_REPORT";
#endif
    return "Unknown Register";
}

uint32_t get_response()
{
    uint32_t response = 0;

    if (address == EVE_REG_ID)
    {
        response = 0x7c;
    }
    else if (address == EVE_REG_CMD_READ)
    {
        response = HAL_GetCmdPointer();
    }
    else if (address == EVE_REG_CPURESET)
    {
        response = 0x00;
    }
#ifdef EVE_USE_CMDB_METHOD
    else if (address == EVE_REG_CMDB_SPACE)
    {
        response = EVE_RAM_CMD_SIZE - 4;
    }
#endif
#if IS_EVE_API(5)
    else if (address == EVE_REG_BOOT_STATUS)
    {
        response = 0x522e2e2e;
    }
    else if (address == EVE_REG_FREQUENCY)
    {
        response = 72000000;
    }
    else if ((address >= EVE_COPROC_REPORT) && (address < (EVE_COPROC_REPORT + 256)))
    {
        const char msg[] = "This is a test\0\0\0";
        if (address - EVE_COPROC_REPORT < strlen(msg))
        {
            memcpy(&response, &msg[address - EVE_COPROC_REPORT], 4);
        }
        else
        {
            response = 0;
        }
    }
#endif
    return response;
}
// ------------------ Platform specific initialisation  ------------------------

void MCU_Init(void)
{
    printf("EVE device: %s\n", 
#if (FT8XX_TYPE == FT800)
        "FT800"
#elif (FT8XX_TYPE == FT801)
        "FT801"
#elif (FT8XX_TYPE == FT810)
        "FT810"
#elif (FT8XX_TYPE == FT811)
        "FT811"
#elif (FT8XX_TYPE == FT812)
        "FT812"
#elif (FT8XX_TYPE == FT813)
        "FT813"
#elif (FT8XX_TYPE == BT880)
        "BT880"
#elif (FT8XX_TYPE == BT881)
        "BT881"
#elif (FT8XX_TYPE == BT882)
        "BT882"
#elif (FT8XX_TYPE == BT883)
        "BT883"
#elif (FT8XX_TYPE == BT815)
        "BT815"
#elif (FT8XX_TYPE == BT816)
        "BT816"
#elif (FT8XX_TYPE == BT817)
        "BT817"
#elif (FT8XX_TYPE == BT818)
        "BT818"
#elif (FT8XX_TYPE == BT820)
        "BT820"
#else
        "FT8XX_TYPE definition not recognised"
#endif // FT8XX_TYPE
    );

    printf("EVE_API: %d\n", EVE_API);
#if IS_EVE_API(2)
    printf("EVE_SUB_API: %d\n" EVE_SUB_API);
#endif

#ifdef MODULE_TYPE
#if MODULE_TYPE != NO_MODULE
    printf("EVE Module Name: %s\n", 
#if MODULE_TYPE == VM800B
        "VM800B35A-BK"
#elif MODULE_TYPE == VM800C35A
        "VM800C35A-D"
#elif MODULE_TYPE == VM800C43A
        "VM800C43A-D"
#elif MODULE_TYPE == VM800C50A
        "VM800C50A-D"
#elif MODULE_TYPE == VM810C
        "VM810C50A-D"
#elif MODULE_TYPE == ME812A
        "ME812A-WH50R, ME812AU-WH50R"
#elif MODULE_TYPE == ME813A
        "ME813A-WH50C"
#elif MODULE_TYPE == VM816C
        "VM816C50A-D, VM816CU50A-D"
#elif MODULE_TYPE == IDM204021R
        "IDM2040-21R"
#elif MODULE_TYPE == IDM204043A
        "IDM2040-43A"
#elif MODULE_TYPE == IDM20407A
        "IDM2040-7A"
#endif // MODULE_TYPE
    );
#endif // MODULE_TYPE != NO_MODULE
#endif // defined(MODULE_TYPE)

#ifdef PANEL_TYPE
#if PANEL_TYPE != NO_PANEL
    printf("EVE Panel Type: %s\n", 
#if PANEL_TYPE == DP_0351_11A
        "DP-0351-11A QVGA (Resistive)"
#elif PANEL_TYPE == DP_0431_11A
        "DP-0431-11A WQVGA (Resistive)"
#elif PANEL_TYPE == DP_0501_01A
        "DP-0501-01A WVGA (Capacitive)"
#elif PANEL_TYPE == DP_0501_11A
        "DP-0351-11A WVGA (Resistive)"
#elif PANEL_TYPE == DP_0502_11A
        "DP-0502-11A WQVGA (Resistive)"
#elif PANEL_TYPE == DP_0701_11A
        "DP-0701-11A WVGA (Capacitive)"
#elif PANEL_TYPE == DP_1012_01A
        "DP-1012-01A WUXGA (Capacitive)"
#elif PANEL_TYPE == DP_1561_01A
        "DP-1561-01A FullHD (Capacitive)"
#elif PANEL_TYPE == DP_IDM43A
        "IDM204043A (Capacitive)"
#elif PANEL_TYPE == DP_IDM21R
        "IDM204021R (Capacitive)"
#endif // PANEL_TYPE
    );
#endif // PANEL_TYPE != NO_PANEL
#endif // defined(PANEL_TYPE)

    printf("EVE display panel resolution: %s\n", 
#if (DISPLAY_RES == QVGA)
        "QVGA 320x240"
#elif (DISPLAY_RES == WQVGA)
        "WQVGA 480x272"
#elif (DISPLAY_RES == WQVGAR)
        "WQVGAR 480x480"
#elif (DISPLAY_RES == WVGA)
        "WVGA 800x480"
#elif (DISPLAY_RES == WSVGA)
        "WSVGA 1024x600"
#elif (DISPLAY_RES == WXGA)
        "WXGA 1280x800"
#elif (DISPLAY_RES == FULLHD)
        "FULLHD 1920x1080"
#elif (DISPLAY_RES == WUXGA)
        "WUXGA 1920x1200"
#else
        "DISPLAY_RES definition not recognised"
#endif // DISPLAY_RES
    );

    state = state_end;
}

void MCU_Deinit(void)
{
    state = state_end;
}

void MCU_Setup(void)
{
}

// --------------------- Chip Select line low ----------------------------------
void MCU_CSlow(void)
{
    if (state == state_idle)
    {
        state = state_address;
        printf("addr: ");
    }
    else if (state == state_booting)
    {
        printf("host command: ");
        hostptr = 0;
    }
}

// --------------------- Chip Select line high ---------------------------------
void MCU_CShigh(void)
{
    if (state == state_booting)
    {
        if (hostptr == HOSTCMDLEN)
        {
            int i;
            int flag = 0;
            for (i = 0; i < HOSTCMDLEN; i++)
            {
                flag += hostcmd[i];
            }
            if (flag == 0)
            {
                // Leave host command state
                state = state_idle;
                printf("\nidle");
            }
            printf("\n");
        }
    }
    else if (state == state_idle)
    {
        // No action
    }
    else
    {
        state = state_idle;
        printf("\n");
    }
}

// -------------------------- PD line low --------------------------------------
void MCU_PDlow(void)
{
    state = state_reset;
    printf("%s\n", get_state());
}

// ------------------------- PD line high --------------------------------------
void MCU_PDhigh(void)
{
    state = state_booting;
    printf("%s\n", get_state());
}

// ------------------------- Delay functions -----------------------------------

void MCU_Delay_20ms(void)
{
    printf("delay 20 ms\n");
}

void MCU_Delay_500ms(void)
{
    printf("delay 500 ms\n");
}

// --------------------- SPI Send and Receive ----------------------------------

uint8_t MCU_SPIRead8(void)
{
    uint8_t DataRead = 0;

    DataRead = (uint8_t)get_response();
    
    if (state == state_read)
    {
        printf("%s %02x ", get_register(), DataRead);
    }
    else if (state == state_read_cont)
    {
        printf("%02x ", DataRead);
    }
    else
    {
        printf("[%s] state error %s\n", __FUNCTION__, get_state());
        exit(-1);
    }

    return DataRead;
}

void MCU_SPIWrite8(uint8_t DataToWrite)
{
    if (state == state_write)
    {
        printf("%s %02x ", get_register(), DataToWrite);
        state = state_write_cont;
    }
    else if (state == state_write_cont)
    {
        printf("%02x ", DataToWrite);
    }
#if IS_EVE_API(1,2,3,4)
    // Host commands
    else if (state == state_booting)
    {
        printf("%02x ", DataToWrite);
        hostcmd[hostptr++] = DataToWrite;
        if (hostptr > HOSTCMDLEN)
        {
            printf("[%s] host command length error %d\n", __FUNCTION__, hostptr);
            exit(-1);
        }
    }
#endif
    else
    {
        printf("[%s] state error %s\n", __FUNCTION__, get_state());
        exit(-1);
    }
}

uint16_t MCU_SPIRead16(void)
{
    uint16_t DataRead = 0;

    DataRead = (uint16_t)get_response();
    
    if (state == state_read)
    {
        printf("%s %04x ", get_register(), DataRead);
    }
    else if (state == state_read_cont)
    {
        printf("%04x ", DataRead);
    }
    else
    {
        printf("[%s] state error %s\n", __FUNCTION__, get_state());
        exit(-1);
    }

    return DataRead;
}

void MCU_SPIWrite16(uint16_t DataToWrite)
{
    if (state == state_write)
    {
        printf("%s %04x ", get_register(), DataToWrite);
        state = state_write_cont;
    }
    else if (state == state_write_cont)
    {
        printf("%04x ", DataToWrite);
    }
    else
    {
        printf("[%s] state error %s\n", __FUNCTION__, get_state());
        exit(-1);
    }
}

uint32_t MCU_SPIRead24(void)
{
    uint32_t DataRead = 0;

    DataRead = (uint32_t)get_response() & 0x00ffffff;

    if (state == state_read)
    {
        printf("%s %06x ", get_register(), DataRead);
    }
    else if (state == state_read_cont)
    {
        printf("%06x ", DataRead);
    }
    else
    {
        printf("[%s] state error %s\n", __FUNCTION__, get_state());
        exit(-1);
    }
    
    return DataRead;
}

void MCU_SPIWrite24(uint32_t DataToWrite)
{
    if (state == state_address)
    {
        address = MCU_be32toh(DataToWrite);
        if (address >> 31)
        {
            state = state_write;
            address = (address & 0x7fffff00) >> 8;
            printf("write %x ", address);
        }
        else
        {
            // Read address cannot be send in 24 bits
            printf("[%s] state error %s cannot set read address in 24 bits\n", __FUNCTION__, get_state());
            exit(-1);
        }
    }
    else
    {
        printf("[%s] state error %s\n", __FUNCTION__, get_state());
        exit(-1);
    }
}

uint32_t MCU_SPIRead32(void)
{
    uint32_t DataRead = 0;

    DataRead = get_response();
    
    if (state == state_read)
    {
        printf("%s %08x ", get_register(), DataRead);
    }
    else if (state == state_read_cont)
    {
        printf("%08x ", DataRead);
    }
    else
    {
        printf("[%s] state error %s\n", __FUNCTION__, get_state());
        exit(-1);
    }
    
    return DataRead;
}

void MCU_SPIWrite32(uint32_t DataToWrite)
{
    if (state == state_address)
    {
        address = MCU_be32toh(DataToWrite);
        if (address >> 31)
        {
            state = state_write;
            printf("write ");
        }
        else
        {
            state = state_read;
            printf("read ");
        }
        address = MCU_be32toh(DataToWrite) & 0x7fffffff;
#if IS_EVE_API(1, 2, 3, 4)
        address = address >> 8;
#endif
        printf("%x ", address);
    }
    else if (state == state_write)
    {
        printf("%s %08x ", get_register(), DataToWrite);
        state = state_write_cont;
    }
    else if (state == state_write_cont)
    {
        printf("%08x ", DataToWrite);
    }
    else
    {
        printf("[%s] state error %s\n", __FUNCTION__, get_state());
        exit(-1);
    }
}

void MCU_SPIRead(uint8_t *DataToRead, uint32_t length)
{
    uint32_t DataRead = 0;
    uint16_t DataPtr = 0;
    uint32_t i;

    DataRead = get_response();

    if (state == state_read)
    {
#if IS_EVE_API(5)
        // Preamble
        DataToRead[0] = 0;
        DataToRead[1] = 0;
        DataToRead[2] = 0;
        DataToRead[3] = 1;
        DataPtr = 4;
#endif
        printf("%s (LE) ", get_register());

        for (i = DataPtr; i < length; i++)
        {
            DataToRead[DataPtr++] = DataRead & 0xff;
            printf("%02x", DataRead & 0xff);
            DataRead = DataRead >> 8;
        }
    }
    else if (state == state_read_cont)
    {
        for (i = DataPtr; i < length; i++)
        {
            DataToRead[DataPtr++] = DataRead & 0xff;
            printf("%02x", DataRead & 0xff);
            DataRead = DataRead >> 8;
        }
    }
    else
    {
        printf("[%s] state error %s\n", __FUNCTION__, get_state());
        exit(-1);
    }
}

void MCU_SPIWrite(const uint8_t *DataToWrite, uint32_t length)
{
    uint32_t i;

    if (state == state_write)
    {
        printf("%s ", get_register());

        for (i = 0; i < length; i++)
        {
            printf("%02x", DataToWrite[i]);
        }
        printf(" ");
    }
    else if (state == state_write_cont)
    {
        for (i = 0; i < length; i++)
        {
            printf("%02x", DataToWrite[i]);
        }
        printf(" ");
    }
#if IS_EVE_API(5)
    else if (state == state_booting)
    {
        uint32_t i;
        for (i = 0; i < length; i++)
        {
            hostcmd[hostptr++] = DataToWrite[i];
            printf("%02x", DataToWrite[i]);
            if (hostptr > HOSTCMDLEN)
            {
                printf("[%s] host command length error %d\n", __FUNCTION__, hostptr);
                exit(-1);
            }
        }
        printf(" ");
    }
#endif
    else
    {
        printf("[%s] state error %s\n", __FUNCTION__, get_state());
        exit(-1);
    }
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

#endif /* defined(SOFTWARE_SIMULATOR) */
