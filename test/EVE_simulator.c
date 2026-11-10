/**
 * @file EVE_tester.c
 */
/*
 * ============================================================================
 * (C) Copyright,  Bridgetek Pte. Ltd.
 * ============================================================================
 *
 * This source code ("the Software") is provided by Bridgetek Pte Ltd
 * ("Bridgetek") subject to the licence terms set out
 * http://brtchip.com/BRTSourceCodeLicenseAgreement/ ("the Licence Terms").
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
#include <time.h>
#else
#include <windows.h>
#endif // _WIN32

/* Include EVE-MCU-Dev library API layer */
#include <EVE.h>
/* Include functions for EVE-MCU-Dev library MCU layer */
#include <MCU.h>

extern uint16_t HAL_GetCmdPointer(void);

#if IS_EVE_API(1, 2, 3, 4)
#define HOSTCMDLEN 3
#else
#define HOSTCMDLEN 5
#endif

#define SIMULATOR_ERROR(...) fprintf(stderr, __VA_ARGS__)

#if SIMULATOR_BOOT
#define SIMULATOR_PRINT_BOOT(...) printf(__VA_ARGS__)
#else
#define SIMULATOR_PRINT_BOOT(...) ((void)0)
#endif

#if SIMULATOR_SPI_PROTOCOL
#define SIMULATOR_PRINT_SPI(...) printf(__VA_ARGS__)
#else
#define SIMULATOR_PRINT_SPI(...) ((void)0)
#endif

#if SIMULATOR_OUTPUT==1
#define SIMULATOR_PRINT_REGISTERS(...) do { printf(__VA_ARGS__); doeol = 1;} while (0);
#else
#define SIMULATOR_PRINT_REGISTERS(...) ((void)0)
#endif

#if SIMULATOR_OUTPUT==2
#define SIMULATOR_PRINT_CMDS(...) do { printf(__VA_ARGS__); doeol = 1;} while (0);
#else
#define SIMULATOR_PRINT_CMDS(...) ((void)0)
#endif

#if SIMULATOR_OUTPUT==3
#define SIMULATOR_PRINT_RAM_G(...) do { printf(__VA_ARGS__); doeol = 1;} while (0);
#else
#define SIMULATOR_PRINT_RAM_G(...) ((void)0)
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
int doeol = 0;

uint16_t write_pointer = 0;

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

void inc_address()
{
#if defined(EVE_USE_CMDB_METHOD)
    if (!((address == EVE_REG_CMDB_SPACE) || (address == EVE_REG_CMDB_WRITE)))
    {
        address += 4;
    }
#endif // defined(EVE_USE_CMDB_METHOD)
}

char const *get_register()
{
    if (address == EVE_REG_ID) return "REG_ID";
    else if (address == EVE_REG_CPURESET) return "REG_CPURESET";
    else if (address == EVE_REG_CMD_READ) return "REG_CMD_READ";
    else if (address == EVE_REG_CMD_WRITE) return "REG_CMD_WRITE";
#if defined(EVE_USE_CMDB_METHOD)
    else if (address == EVE_REG_CMDB_SPACE) return "REG_CMDB_SPACE";
    else if (address == EVE_REG_CMDB_WRITE) return "REG_CMDB_WRITE";
#endif // defined(EVE_USE_CMDB_METHOD)
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
    else if (address == EVE_REG_TOUCH_SCREEN_XY) return "REG_TOUCH_SCREEN_XY";
    else if (address == EVE_REG_TOUCH_RAW_XY) return "REG_TOUCH_RAW_XY";
    else if (address == EVE_REG_TOUCH_TAG) return "REG_TOUCH_TAG";
    else if (address == EVE_REG_TOUCH_TAG_XY) return "REG_TOUCH_TAG_XY";
    else if (address == EVE_REG_TOUCH_TRANSFORM_A) return "REG_TOUCH_TRANSFORM_A";
    else if (address == EVE_REG_TOUCH_TRANSFORM_B) return "REG_TOUCH_TRANSFORM_B";
    else if (address == EVE_REG_TOUCH_TRANSFORM_C) return "REG_TOUCH_TRANSFORM_C";
    else if (address == EVE_REG_TOUCH_TRANSFORM_D) return "REG_TOUCH_TRANSFORM_D";
    else if (address == EVE_REG_TOUCH_TRANSFORM_E) return "REG_TOUCH_TRANSFORM_E";
    else if (address == EVE_REG_TOUCH_TRANSFORM_F) return "REG_TOUCH_TRANSFORM_F";
    else if (address == EVE_REG_FRAMES) return "REG_FRAMES";
    else if (address == EVE_ROMFONT_TABLEADDRESS) return "ROMFONT_TABLEADDRESS";
    else if ((address >= EVE_RAM_DL) && (address < (EVE_RAM_DL + EVE_RAM_DL_SIZE)))
        return "RAM_DL";
    else if ((address >= EVE_RAM_CMD) && (address < (EVE_RAM_CMD + EVE_RAM_CMD_SIZE)))
        return "RAM_CMD";
    else if ((address >= EVE_RAM_G) && (address < (EVE_RAM_G + EVE_RAM_G_SIZE)))
        return "RAM_G";
#if IS_EVE_API(5)
    else if (address == EVE_REG_BOOT_STATUS) return "REG_BOOT_STATUS";
    else if ((address >= EVE_RAM_REPORT) && (address < (EVE_RAM_REPORT + 256)))
        return "COPROC_REPORT";
#endif
    return "Unknown Register";
}

uint32_t get_response_32()
{
    uint32_t response = 0;

    if (address == EVE_REG_ID)
    {
        response = 0x7c;
    }
    else if (address == EVE_REG_CMD_READ)
    {
        response = write_pointer;
    }
    else if (address == EVE_REG_CMD_WRITE)
    {
        response = write_pointer;
    }
    else if (address == EVE_REG_CPURESET)
    {
        response = 0x00;
    }
#if defined(EVE_USE_CMDB_METHOD)
    else if (address == EVE_REG_CMDB_SPACE)
    {
        response = EVE_RAM_CMD_SIZE - 4;
    }
#endif // defined(EVE_USE_CMDB_METHOD)
#if IS_EVE_API(5)
    else if (address == EVE_REG_BOOT_STATUS)
    {
        response = 0x522e2e2e;
    }
    else if (address == EVE_REG_FREQUENCY)
    {
        response = 72000000;
    }
    else if ((address >= EVE_RAM_REPORT) && (address < (EVE_RAM_REPORT + 256)))
    {
        // Keep up-to-date with the expected value in `bt82x_patch.c` function eve_loadpatch();
        const char msg[] = "patch_base;1.5;touch-1.1;\0\0\0";
        
        if (address - EVE_RAM_REPORT < strlen(msg))
        {
            memcpy(&response, &msg[address - EVE_RAM_REPORT], 4);
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

int MCU_Init(void)
{
    printf("EVE device: %s\n", 
#if (EVE_DEVICE == EVE_FT800)
        "FT800"
#elif (EVE_DEVICE == EVE_FT801)
        "FT801"
#elif (EVE_DEVICE == EVE_FT810)
        "FT810"
#elif (EVE_DEVICE == EVE_FT811)
        "FT811"
#elif (EVE_DEVICE == EVE_FT812)
        "FT812"
#elif (EVE_DEVICE == EVE_FT813)
        "FT813"
#elif (EVE_DEVICE == EVE_BT880)
        "BT880"
#elif (EVE_DEVICE == EVE_BT881)
        "BT881"
#elif (EVE_DEVICE == EVE_BT882)
        "BT882"
#elif (EVE_DEVICE == EVE_BT883)
        "BT883"
#elif (EVE_DEVICE == EVE_BT815)
        "BT815"
#elif (EVE_DEVICE == EVE_BT816)
        "BT816"
#elif (EVE_DEVICE == EVE_BT817)
        "BT817"
#elif (EVE_DEVICE == EVE_BT818)
        "BT818"
#elif (EVE_DEVICE == EVE_BT820)
        "BT820"
#else
        "EVE_DEVICE definition not recognised"
#endif // EVE_DEVICE
    );

    printf("EVE_API: %d\n", EVE_API);
#if IS_EVE_API(2)
    printf("EVE_SUB_API: %d\n", EVE_SUB_API);
#endif

#ifdef EVE_MODULE
#if EVE_MODULE != EVE_NO_MODULE
    printf("EVE Module Name: %s\n", 
#if EVE_MODULE == EVE_VM800B
        "VM800B35A-BK"
#elif EVE_MODULE == EVE_VM800C35A
        "VM800C35A-D"
#elif EVE_MODULE == EVE_VM800C43A
        "VM800C43A-D"
#elif EVE_MODULE == EVE_VM800C50A
        "VM800C50A-D"
#elif EVE_MODULE == EVE_VM810C
        "VM810C50A-D"
#elif EVE_MODULE == EVE_ME812A
        "ME812A-WH50R, ME812AU-WH50R"
#elif EVE_MODULE == EVE_ME813A
        "ME813A-WH50C"
#elif EVE_MODULE == EVE_VM816C
        "VM816C50A-D, VM816CU50A-D"
#elif EVE_MODULE == EVE_IDM204021R
        "IDM2040-21R"
#elif EVE_MODULE == EVE_IDM204043A
        "IDM2040-43A"
#elif EVE_MODULE == EVE_IDM20407A
        "IDM2040-7A"
#elif EVE_MODULE == EVE_VM820B10A       
        "VM820B10A"
#elif EVE_MODULE == EVE_VM820B15A      
        "VM820B15A"
#elif EVE_MODULE == EVE_IDK_FT810_43A 
        "IDK-FT810-43A"
#elif EVE_MODULE == EVE_IDK_BT816_50A
        "IDK-BT816-50A"
#elif EVE_MODULE == EVE_IDK_BT817_70A
        "IDK-BT817-70A"
#elif EVE_MODULE == EVE_IDK_BT817_101A  
        "IDK-BT817-101A"
#elif EVE_MODULE ==  EVE_IDK_BT820_101A  
        "IDK-BT820-101A"
#else
        "EVE_MODULE definition not recognised"
#endif // EVE_MODULE
    );
#endif // EVE_MODULE != EVE_NO_MODULE
#endif // defined(EVE_MODULE)

#ifdef EVE_PANEL
#if EVE_PANEL != EVE_NO_PANEL
    printf("EVE Panel Type: %s\n", 
#if EVE_PANEL == EVE_DP_0351_11A
        "DP-0351-11A QVGA (Resistive)"
#elif EVE_PANEL == EVE_DP_0431_11A
        "DP-0431-11A WQVGA (Resistive)"
#elif EVE_PANEL == EVE_DP_0501_01A
        "DP-0501-01A WVGA (Capacitive)"
#elif EVE_PANEL == EVE_DP_0501_11A
        "DP-0351-11A WVGA (Resistive)"
#elif EVE_PANEL == EVE_DP_0502_11A
        "DP-0502-11A WQVGA (Resistive)"
#elif EVE_PANEL == EVE_DP_0701_01A
        "DP-0701-01A WVGA (Capacitive)"
#elif EVE_PANEL == EVE_DP_1012_01A
        "DP-1012-01A WUXGA (Capacitive)"
#elif EVE_PANEL == EVE_DP_1561_01A
        "DP-1561-01A FullHD (Capacitive)"
#elif EVE_PANEL == EVE_DP_IDM21R
        "IDM204021R (Capacitive)"
#else
        "EVE_PANEL not recognised"
#endif // EVE_PANEL
    );
#endif // EVE_PANEL != EVE_NO_PANEL
#endif // defined(EVE_PANEL)

    printf("EVE display panel resolution: %s\n", 
#if (EVE_DISPLAY_RES == EVE_QVGA)
        "QVGA 320x240"
#elif (EVE_DISPLAY_RES == EVE_WQVGA)
        "WQVGA 480x272"
#elif (EVE_DISPLAY_RES == EVE_WQVGAR)
        "WQVGAR 480x480"
#elif (EVE_DISPLAY_RES == EVE_WVGA)
        "WVGA 800x480"
#elif (EVE_DISPLAY_RES == EVE_WSVGA)
        "WSVGA 1024x600"
#elif (EVE_DISPLAY_RES == EVE_WXGA)
        "WXGA 1280x800"
#elif (EVE_DISPLAY_RES == EVE_FULLHD)
        "FULLHD 1920x1080"
#elif (EVE_DISPLAY_RES == EVE_WUXGA)
        "WUXGA 1920x1200"
#else
        "EVE_DISPLAY_RES definition not recognised"
#endif // EVE_DISPLAY_RES
    );

    state = state_end;
    return 0;
}

int MCU_Deinit(void)
{
    state = state_end;
    return 0;
}

int MCU_Setup(void)
{
    return 0;
}

// --------------------- Chip Select line low ----------------------------------
void MCU_CSlow(void)
{
    if (state == state_idle)
    {
        state = state_address;
        SIMULATOR_PRINT_SPI("addr: ");
    }
    else if (state == state_booting)
    {
        SIMULATOR_PRINT_BOOT("host command: ");
        hostptr = 0;
    }
    doeol = 0;
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
            }
            SIMULATOR_PRINT_BOOT("\n");
        }
    }
    else if (state == state_idle)
    {
        // No action
    }
    else
    {
        if (doeol)
        {
            if (state == state_read_cont)
            {
                SIMULATOR_PRINT_CMDS("\n");
                SIMULATOR_PRINT_REGISTERS("\n");
                SIMULATOR_PRINT_RAM_G("\n");
            }
            else if (state == state_write_cont)
            {
                SIMULATOR_PRINT_CMDS("\n");
                SIMULATOR_PRINT_REGISTERS("\n");
                SIMULATOR_PRINT_RAM_G("\n");
            }
            doeol = 0;
        }
        state = state_idle;
    }
}

// -------------------------- PD line low --------------------------------------
void MCU_PDlow(void)
{
    state = state_reset;
    SIMULATOR_PRINT_BOOT("%s\n", get_state());
}

// ------------------------- PD line high --------------------------------------
void MCU_PDhigh(void)
{
    state = state_booting;
    SIMULATOR_PRINT_BOOT("%s\n", get_state());
}

// ------------------------ interrupt input ------------------------------------
int MCU_Int(void) 
{
    return 1;
}

// ------------------------- Delay functions -----------------------------------

void MCU_Delay_20ms(void)
{
    SIMULATOR_PRINT_BOOT("delay 20 ms\n");
}

void MCU_Delay_500ms(void)
{
    SIMULATOR_PRINT_BOOT("delay 500 ms\n");
}

uint32_t MCU_Time_ms(void)
{
#ifdef _WIN32
    // The resolution of this Windows API call may be as large as 15 to 16 ms.
    return GetTickCount();
#else
    // For MinGW synthesize a tick counter from clock_gettime. Resolution 1 ms.
    struct timespec spec;

    clock_gettime(CLOCK_MONOTONIC, &spec);
    return (uint32_t)((spec.tv_sec * 1000) + (spec.tv_nsec / 1000));
#endif
}

// --------------------- SPI Send and Receive ----------------------------------

uint8_t MCU_SPIRead8(void)
{
    uint8_t DataRead = 0;

    DataRead = (uint8_t)get_response_32();
    
    if (state == state_read)
    {
        if ((address >= EVE_RAM_G) && (address < (EVE_RAM_G + EVE_RAM_G_SIZE)))
        {
            SIMULATOR_PRINT_RAM_G("<-- %02x ", DataRead);
        }
        SIMULATOR_PRINT_REGISTERS("%s Read %02x ", get_register(), DataRead);
        state = state_read_cont;
    }
    else if (state == state_read_cont)
    {
        if ((address >= EVE_RAM_G) && (address < (EVE_RAM_G + EVE_RAM_G_SIZE)))
        {
            SIMULATOR_PRINT_RAM_G("%02x ", DataRead);
        }
        SIMULATOR_PRINT_REGISTERS("%02x ", DataRead);
    }
    else
    {
        SIMULATOR_ERROR("[%s] state error %s\n", __FUNCTION__, get_state());
        exit(-1);
    }
    inc_address(1);

    return DataRead;
}

void MCU_SPIWrite8(uint8_t DataToWrite)
{
    if (state == state_write)
    {
        SIMULATOR_PRINT_REGISTERS("%s Write %02x ", get_register(), DataToWrite);
        state = state_write_cont;
    }
    else if (state == state_write_cont)
    {
        SIMULATOR_PRINT_REGISTERS("%02x ", DataToWrite);
    }
#if IS_EVE_API(1,2,3,4)
    // Host commands
    else if (state == state_booting)
    {
        SIMULATOR_PRINT_BOOT("%02x ", DataToWrite);
        hostcmd[hostptr++] = DataToWrite;
        if (hostptr > HOSTCMDLEN)
        {
            SIMULATOR_ERROR("[%s] host command length error %d\n", __FUNCTION__, hostptr);
            exit(-1);
        }
    }
#endif
    else
    {
        SIMULATOR_ERROR("[%s] state error %s\n", __FUNCTION__, get_state());
        exit(-1);
    }
    inc_address(1);
}

uint16_t MCU_SPIRead16(void)
{
    uint16_t DataRead = 0;

    DataRead = (uint16_t)get_response_32();
    
    if (state == state_read)
    {
        if ((address >= EVE_RAM_G) && (address < (EVE_RAM_G + EVE_RAM_G_SIZE)))
        {
            SIMULATOR_PRINT_RAM_G("<-- %04x ", DataRead);
        }
        SIMULATOR_PRINT_REGISTERS("%s Read %04x ", get_register(), DataRead);
        state = state_read_cont;
    }
    else if (state == state_read_cont)
    {
        if ((address >= EVE_RAM_G) && (address < (EVE_RAM_G + EVE_RAM_G_SIZE)))
        {
            SIMULATOR_PRINT_RAM_G("%04x ", DataRead);
        }
        SIMULATOR_PRINT_REGISTERS("%04x ", DataRead);
    }
    else
    {
        SIMULATOR_ERROR("[%s] state error %s\n", __FUNCTION__, get_state());
        exit(-1);
    }

    inc_address(2);
    return DataRead;
}

void MCU_SPIWrite16(uint16_t DataToWrite)
{
    if (state == state_write)
    {
        SIMULATOR_PRINT_REGISTERS("%s Write %04x ", get_register(), DataToWrite);
        state = state_write_cont;
    }
    else if (state == state_write_cont)
    {
        SIMULATOR_PRINT_REGISTERS("%04x ", DataToWrite);
        if (address == EVE_REG_CMD_WRITE)
        {
            write_pointer = DataToWrite;
        }
    }
    else
    {
        SIMULATOR_ERROR("[%s] state error %s\n", __FUNCTION__, get_state());
        exit(-1);
    }
    inc_address(2);
}

void MCU_SPIWrite24(uint32_t DataToWrite)
{
    if (state == state_address)
    {
        address = MCU_be32toh(DataToWrite);
        SIMULATOR_PRINT_SPI("0x%x ", address);
        if (address >> 31)
        {
            state = state_write;
            address = (address & 0x7fffff00) >> 8;
        }
        else
        {
            // Read address cannot be send in 24 bits
            SIMULATOR_ERROR("[%s] state error %s cannot set read address in 24 bits\n", __FUNCTION__, get_state());
            exit(-1);
        }
    }
    else
    {
        SIMULATOR_ERROR("[%s] state error %s\n", __FUNCTION__, get_state());
        exit(-1);
    }
}

uint32_t MCU_SPIRead32(void)
{
    uint32_t DataRead = 0;

    DataRead = get_response_32();
    
    if (state == state_read)
    {
        if ((address >= EVE_RAM_G) && (address < (EVE_RAM_G + EVE_RAM_G_SIZE)))
        {
            SIMULATOR_PRINT_RAM_G("<-- %08x ", DataRead);
        }
        SIMULATOR_PRINT_REGISTERS("%s Read %08x ", get_register(), DataRead);
        state = state_read_cont;
    }
    else if (state == state_read_cont)
    {
        if ((address >= EVE_RAM_G) && (address < (EVE_RAM_G + EVE_RAM_G_SIZE)))
        {
            SIMULATOR_PRINT_RAM_G("%08x ", DataRead);
        }
        SIMULATOR_PRINT_REGISTERS("%08x ", DataRead);
    }
    else
    {
        SIMULATOR_ERROR("[%s] state error %s\n", __FUNCTION__, get_state());
        exit(-1);
    }
    
    inc_address(4);
    return DataRead;
}

void MCU_SPIWrite32(uint32_t DataToWrite)
{
    if (state == state_address)
    {
#if IS_EVE_API(1, 2, 3, 4)
        DataToWrite = DataToWrite << 8;
#endif
        address = MCU_be32toh(DataToWrite);
        SIMULATOR_PRINT_SPI("0x%x ", address);
#if IS_EVE_API(1, 2, 3, 4)
        if (address >> 23)
        {
            state = state_write;
        }
        else
        {
            state = state_read;
        }
        address = address & 0x7fffff;
#else
        if (address >> 31)
        {
            state = state_write;
        }
        else
        {
            state = state_read;
        }
        address = address & 0x7fffffff;
#endif
    }
    else if (state == state_write)
    {
        if ((address >= EVE_RAM_G) && (address < (EVE_RAM_G + EVE_RAM_G_SIZE)))
        {
            SIMULATOR_PRINT_RAM_G("--> %08x ", DataToWrite);
        }
        else if (address == EVE_REG_CMDB_WRITE)
        {
            SIMULATOR_PRINT_RAM_G("--> %08x ", DataToWrite);
            SIMULATOR_PRINT_CMDS("--> %08x ", DataToWrite);
        }
        SIMULATOR_PRINT_REGISTERS("%s Write %08x ", get_register(), DataToWrite);

        state = state_write_cont;
        if (address == EVE_REG_CMD_WRITE)
        {
            write_pointer = DataToWrite;
        }
        inc_address(4);
    }
    else if (state == state_write_cont)
    {
        if ((address >= EVE_RAM_G) && (address < (EVE_RAM_G + EVE_RAM_G_SIZE)))
        {
            SIMULATOR_PRINT_RAM_G("%08x ", DataToWrite);
        }
        else if (address == EVE_REG_CMDB_WRITE)
        {
            SIMULATOR_PRINT_RAM_G("%08x ", DataToWrite);
            SIMULATOR_PRINT_CMDS("%08x ", DataToWrite);
        }
        SIMULATOR_PRINT_REGISTERS("%08x ", DataToWrite);
        inc_address(4);
    }
    else
    {
        SIMULATOR_ERROR("[%s] state error %s\n", __FUNCTION__, get_state());
        exit(-1);
    }
}

void MCU_SPIRead(uint8_t *DataToRead, uint32_t length)
{
    uint32_t DataRead = 0;
    uint16_t DataPtr = 0;
    uint32_t val;
    uint32_t i;
    uint32_t j;

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
        SIMULATOR_PRINT_REGISTERS("%s Read ", get_register());
        if ((address >= EVE_RAM_G) && (address < (EVE_RAM_G + EVE_RAM_G_SIZE)))
        {
            SIMULATOR_PRINT_RAM_G("<-- ");
        }

        for (i = DataPtr; i < length; i+=4)
        {
            DataRead = get_response_32();
            
            val = 0;
            for (j = 0; j < 4; j++)
            {
                DataToRead[DataPtr++] = DataRead & 0xff;
                val |= ((DataRead & 0xff) << (j * 8));
                DataRead = DataRead >> 8;
            }
            if ((address >= EVE_RAM_G) && (address < (EVE_RAM_G + EVE_RAM_G_SIZE)))
            {
                SIMULATOR_PRINT_RAM_G("%08x ", val);
            }
            SIMULATOR_PRINT_REGISTERS("%08x ", val);

            inc_address(4);
        }
        state = state_read_cont;
    }
    else if (state == state_read_cont)
    {
        for (i = DataPtr; i < length; i+=4)
        {
            DataRead = get_response_32();
            
            val = 0;
            for (j = 0; j < 4; j++)
            {
                DataToRead[DataPtr++] = DataRead & 0xff;
                val |= ((DataRead & 0xff) << (j * 8));
            }
            if ((address >= EVE_RAM_G) && (address < (EVE_RAM_G + EVE_RAM_G_SIZE)))
            {
                SIMULATOR_PRINT_RAM_G("%08x ", val);
            }
            SIMULATOR_PRINT_REGISTERS("%08x ", val);

            DataRead = DataRead >> 8;
            inc_address(4);
        }
    }
    else
    {
        SIMULATOR_ERROR("[%s] state error %s\n", __FUNCTION__, get_state());
        exit(-1);
    }
}

void MCU_SPIWrite(const uint8_t *DataToWrite, uint32_t length)
{
    uint16_t DataPtr = 0;
    uint32_t val;
    uint32_t i;
    uint32_t j;

    if (state == state_write)
    {
        SIMULATOR_PRINT_REGISTERS("%s Write ", get_register());

        for (i = 0; i < length; i+=4)
        {
            val = 0;
            for (j = 0; j < 4; j++)
            {
                val |= ((DataToWrite[DataPtr++] & 0xff) << (j * 8));
            }
            if (address == EVE_REG_CMDB_WRITE)
            {
                SIMULATOR_PRINT_RAM_G("%08x ", val);
                SIMULATOR_PRINT_CMDS("%08x ", val);
            }
            SIMULATOR_PRINT_REGISTERS("%08x ", val);

            inc_address(4);
        }
        state = state_write_cont;
    }
    else if (state == state_write_cont)
    {
        for (i = 0; i < length; i+=4)
        {
            val = 0;
            for (j = 0; j < 4; j++)
            {
                val |= ((DataToWrite[DataPtr++] & 0xff) << (j * 8));
            }
            if (address == EVE_REG_CMDB_WRITE)
            {
                SIMULATOR_PRINT_RAM_G("%08x ", val);
                SIMULATOR_PRINT_CMDS("%08x ", val);
            }
            SIMULATOR_PRINT_REGISTERS("%08x ", val);

            inc_address(4);
        }
    }
#if IS_EVE_API(5)
    else if (state == state_booting)
    {
        uint32_t i;
        for (i = 0; i < length; i++)
        {
            hostcmd[hostptr++] = DataToWrite[i];
            SIMULATOR_PRINT_BOOT("%02x", DataToWrite[i]);
            if (hostptr > HOSTCMDLEN)
            {
                SIMULATOR_ERROR("[%s] host command length error %d\n", __FUNCTION__, hostptr);
                exit(-1);
            }
        }
        SIMULATOR_PRINT_BOOT(" ");
    }
#endif
    else
    {
        SIMULATOR_ERROR("[%s] state error %s\n", __FUNCTION__, get_state());
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
