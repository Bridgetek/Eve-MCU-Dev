/**
 * @file EVE_API.c
 * @details Function implementations for EVE-MCU-Dev library API layer.
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

/* EVE API INCLUDES */

#include <string.h>
#include <stdint.h> // for Uint8/16/32 and Int8/16/32 data types
#include <stdarg.h>

/* Include functions for EVE-MCU-Dev library API layer */
#include <EVE.h> // for IS_EVE_API() macro
/* Include functions for EVE-MCU-Dev library Hardware Abstraction layer */
#include <HAL.h> 
/* Include functions for EVE-MCU-Dev library MCU layer */
#include <MCU.h>
/* Include the EVE debug-output macro definitions */
#include <EVE_debug.h>

#if IS_EVE_API(5)
#include <extensions/bt82x_patch.h>
#endif

#if IS_EVE_API(2,3,4)
#if defined(EVE_CUSTOM_TOUCH)
#include <extensions/custom_touch_fw.h>
#endif
#endif

/* EVE API INCLUDES END */

/* EVE API */

/*
 * Include the EVE configuration to select the EVE API.
 */
#include <EVE_config.h>

/*
 * Report deprecated config items.
 * This is done here since it will only occur once.
 */
#if defined(FT8XX_TYPE)
#pragma message ("Warning: Configuration setting FT8XX_TYPE deprecated in favour of EVE_DEVICE.")
#endif // defined(FT8XX_TYPE)
#if defined(DISPLAY_RES)
#pragma message ("Warning: Configuration setting DISPLAY_RES deprecated in favour of EVE_DISPLAY_RES.")
#endif // defined(DISPLAY_RES)
#if defined(MODULE_TYPE)
#pragma message ("Warning: Configuration setting MODULE_TYPE deprecated in favour of EVE_MODULE.")
#endif // defined(MODULE_TYPE)
#if defined(PANEL_TYPE)
#pragma message ("Warning: Configuration setting PANEL_TYPE deprecated in favour of EVE_PANEL.")
#endif // defined(PANEL_TYPE)
#if defined(QUADSPI_ENABLE)
#pragma message ("Warning: Configuration setting QUADSPI_ENABLE deprecated in favour of EVE_QSPI_ENABLE.")
#endif // defined(QUADSPI_ENABLE)

/* EVE API Library functions */

/* Report the API (and SUB API) for this build. */
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#pragma message ("Compiling for EVE_API " STR(EVE_API))
#if defined(EVE_SUB_API)
#pragma message ("Compiling for EVE_SUB_API " STR(EVE_SUB_API))
#endif
#pragma message ("Display resolution " STR(EVE_DISP_WIDTH) "x" STR(EVE_DISP_HEIGHT))

// Initialise EVE API
int EVE_Init(void)
{
    uint8_t i;

    if (HAL_EVE_Init() < 0)
    {
        EVE_DEBUG_ERROR("ERROR: HAL_EVE_Init() non-zero return value.\n");
        return -1;
    }

    /* Setup Display Settings for Panel */ 

#if IS_EVE_API(1, 2, 3, 4)
    uint8_t regGpio;

    // LCD display parameters

    // Active width of LCD display
    HAL_MemWrite16(EVE_REG_HSIZE, (uint16_t)EVE_DISP_WIDTH);
    // Total number of clocks per line
    HAL_MemWrite16(EVE_REG_HCYCLE, (uint16_t)EVE_DISP_HCYCLE);
    // Start of active line
    HAL_MemWrite16(EVE_REG_HOFFSET, (uint16_t)EVE_DISP_HOFFSET);
    // Start of horizontal sync pulse
    HAL_MemWrite16(EVE_REG_HSYNC0, (uint16_t)EVE_DISP_HSYNC0);
    // End of horizontal sync pulse
    HAL_MemWrite16(EVE_REG_HSYNC1, (uint16_t)EVE_DISP_HSYNC1);
    // Active height of LCD display
    HAL_MemWrite16(EVE_REG_VSIZE, (uint16_t)EVE_DISP_HEIGHT);
    // Total number of lines per screen
    HAL_MemWrite16(EVE_REG_VCYCLE, (uint16_t)EVE_DISP_VCYCLE);
    // Start of active screen
    HAL_MemWrite16(EVE_REG_VOFFSET, (uint16_t)EVE_DISP_VOFFSET);
    // Start of vertical sync pulse
    HAL_MemWrite16(EVE_REG_VSYNC0, (uint16_t)EVE_DISP_VSYNC0);
    // End of vertical sync pulse
    HAL_MemWrite16(EVE_REG_VSYNC1, (uint16_t)EVE_DISP_VSYNC1);
    // Define active edge of PCLK
    HAL_MemWrite8(EVE_REG_PCLK_POL, (uint8_t)EVE_DISP_PCLKPOL);
    // Define RGB output pins
    HAL_MemWrite8(EVE_REG_SWIZZLE, (uint8_t)EVE_DISP_SWIZZLE);
    // Turn on or off CSpread
    HAL_MemWrite8(EVE_REG_CSPREAD, (uint8_t)EVE_DISP_CSPREAD);
    // Turn on or off Dither
    HAL_MemWrite8(EVE_REG_DITHER, (uint8_t)EVE_DISP_DITHER);
    // Turn on or off Adaptive Framerate (BT81x only)
#if defined(EVE_DISP_ADAPTIVE_FRAMERATE) && defined(EVE_REG_ADAPTIVE_FRAMERATE)
    HAL_MemWrite8(EVE_REG_TOUCH_CONFIG, (uint8_t)EVE_REG_ADAPTIVE_FRAMERATE);
#endif
    // Enable Adaptive HSYNC (BT817/8 only)
#if defined(EVE_DISP_AH_HCYCLE_MAX) && defined(EVE_REG_AH_HCYCLE_MAX)
#if (EVE_DISP_AH_HCYCLE_MAX > EVE_DISP_HCYCLE) 
    HAL_MemWrite16(EVE_REG_AH_HCYCLE_MAX, (uint16_t)EVE_DISP_AH_HCYCLE_MAX);
#endif
#endif

    /* Setup Touch settings */

    // Eliminate any false touches
    HAL_MemWrite16(EVE_REG_TOUCH_RZTHRESH, 1200);

    // Set touch i2c address (excludes FT80x)
#if defined(EVE_TOUCH_ADDR) && defined(EVE_REG_TOUCH_CONFIG)
    HAL_MemWrite8(EVE_REG_CPURESET, 2);
    HAL_MemWrite16(EVE_REG_TOUCH_CONFIG, (uint16_t)EVE_TOUCH_ADDR << 4);
    HAL_MemWrite8(EVE_REG_CPURESET, 0);
#endif

#if IS_EVE_API(2,3,4)
    // load custom touch FW (only supported on FT81X/BT88X/BT81X)
#if defined(EVE_CUSTOM_TOUCH)
    if (eve_loadcustomtouch() != 0) // send custom touch FW data to co-processor
    {
        EVE_DEBUG_ERROR("ERROR: Failed to load custom touch FW.\n");
        return -1;
    }
    EVE_DEBUG_PRINTF("[Custom Touch FW Loaded]\n");
#endif  // defined(EVE_CUSTOM_TOUCH)
#endif  // IS_EVE_API(2,3,4)

    /* Write first display list */

    // Clear Screen Ready to Start
    HAL_MemWrite32((EVE_RAM_DL + 4), EVE_ENC_CLEAR_COLOR_RGB(0, 0, 0));
    HAL_MemWrite32((EVE_RAM_DL + 8), EVE_ENC_CLEAR(1, 1, 1));
    HAL_MemWrite32((EVE_RAM_DL + 12), EVE_ENC_DISPLAY());
    HAL_MemWrite8(EVE_REG_DLSWAP, EVE_DLSWAP_FRAME);

    /* Enable the display */

    // Read the GPIO register for a read/modify/write operation
    regGpio = HAL_MemRead8(EVE_REG_GPIO);
    // set bit 7 of GPIO register (DISP) - others are inputs
    regGpio = regGpio | 0x80u;
    // Enable the DISP signal to the LCD panel
    HAL_MemWrite8(EVE_REG_GPIO, regGpio);

    // Write the PCLK or PCLK_FREQ register
    // If setting PCLK_FREQ then also set REG_PCLK to 1 to enable extsync mode
#if IS_EVE_API(4) 
#if defined (EVE_SET_PCLK_FREQ)
    // Write REG_PLCK_FREQ register
    HAL_MemWrite16(EVE_REG_PCLK_FREQ, (uint16_t)EVE_DISP_PCLK_FREQ);
    HAL_MemWrite8(EVE_REG_PCLK, 1);
#else
    // Now start clocking data to the LCD panel
    HAL_MemWrite8(EVE_REG_PCLK, (uint8_t)EVE_DISP_PCLK);
#endif // defined (EVE_SET_PCLK_FREQ)
#if defined(EVE_DISP_PLCK_2X)
    // NOTE: See BT81x Programmers Guide for requirements when using this register   
    HAL_MemWrite8(EVE_REG_PCLK_2X, (uint8_t)EVE_DISP_PLCK_2X);
#endif // defined (EVE_SET_PCLK_FREQ)
#else
    // Now start clocking data to the LCD panel
    HAL_MemWrite8(EVE_REG_PCLK, (uint16_t)EVE_DISP_PCLK);
#endif // IS_EVE_API(4) 

    // turn on LCD backlight
    HAL_MemWrite8(EVE_REG_PWM_DUTY, 0x80);

    /* Setup Audio settings */ 

    // turn recorded audio volume down
    HAL_MemWrite8(EVE_REG_VOL_PB, EVE_VOL_ZERO);

    // turn synthesizer volume down
    HAL_MemWrite8(EVE_REG_VOL_SOUND, EVE_VOL_ZERO);

    // set synthesizer to mute
    HAL_MemWrite16(EVE_REG_SOUND, 0x6000);

#if !defined(EVE_USE_CMDB_METHOD)
    HAL_MemWrite32(EVE_REG_CMD_READ, 0);
    HAL_ResetCmdPointer();
    HAL_WriteCmdPointer();
#endif // !defined(EVE_USE_CMDB_METHOD)

#if defined(EVE_COPROC_PROFILE)
    HAL_ResetProfilePointer();
#endif // defined(EVE_COPROC_PROFILE)

#elif IS_EVE_API(5) 
    EVE_LIB_BeginCoProList();
    EVE_CMD_REGWRITE(EVE_REG_SC0_SIZE, 2);
    EVE_CMD_REGWRITE(EVE_REG_SC0_PTR0, (EVE_RAM_G_SIZE - 0x280000UL - ((uint32_t)EVE_DISP_WIDTH * (uint32_t)EVE_DISP_HEIGHT * 3UL)));
    EVE_CMD_REGWRITE(EVE_REG_SC0_PTR1, (EVE_RAM_G_SIZE - 0x280000UL - (2UL * (uint32_t)EVE_DISP_WIDTH * (uint32_t)EVE_DISP_HEIGHT * 3UL)));
    EVE_LIB_EndCoProList();
    EVE_LIB_AwaitCoProEmpty();

    EVE_LIB_BeginCoProList();
    EVE_CMD_RENDERTARGET(EVE_SWAPCHAIN_0, (uint32_t)EVE_DISP_LVDSTXFORMAT, (uint32_t)EVE_DISP_WIDTH, (uint32_t)EVE_DISP_HEIGHT);
    EVE_CLEAR(1, 1, 1);
    EVE_CMD_SWAP();
    EVE_CMD_GRAPHICSFINISH();
    EVE_LIB_EndCoProList();
    EVE_LIB_AwaitCoProEmpty();

    EVE_LIB_BeginCoProList();
    EVE_CMD_REGWRITE(EVE_REG_GPIO, 0x80ul);
    EVE_CMD_REGWRITE(EVE_REG_DISP, 1ul);

    // Total number of clocks per line
    EVE_CMD_REGWRITE(EVE_REG_HCYCLE, (uint32_t)EVE_DISP_HCYCLE);
    // Active width of LCD display
    EVE_CMD_REGWRITE(EVE_REG_HSIZE, (uint32_t)EVE_DISP_WIDTH);
    // Start of active line
    EVE_CMD_REGWRITE(EVE_REG_HOFFSET, (uint32_t)EVE_DISP_HOFFSET);
    // Start of horizontal sync pulse
    EVE_CMD_REGWRITE(EVE_REG_HSYNC0, (uint32_t)EVE_DISP_HSYNC0);
    // End of horizontal sync pulse
    EVE_CMD_REGWRITE(EVE_REG_HSYNC1, (uint32_t)EVE_DISP_HSYNC1);

    // Total number of lines per screen
    EVE_CMD_REGWRITE(EVE_REG_VCYCLE, (uint32_t)EVE_DISP_VCYCLE);
    // Active height of LCD display
    EVE_CMD_REGWRITE(EVE_REG_VSIZE, (uint32_t)EVE_DISP_HEIGHT);
    // Start of active screen
    EVE_CMD_REGWRITE(EVE_REG_VOFFSET, (uint32_t)EVE_DISP_VOFFSET);
    // Start of vertical sync pulse
    EVE_CMD_REGWRITE(EVE_REG_VSYNC0, (uint32_t)EVE_DISP_VSYNC0);
    // End of vertical sync pulse
    EVE_CMD_REGWRITE(EVE_REG_VSYNC1, (uint32_t)EVE_DISP_VSYNC1);

    // Define active edge of PCLK
    EVE_CMD_REGWRITE(EVE_REG_PCLK_POL, 0ul);
    EVE_CMD_REGWRITE(EVE_REG_RE_DITHER, 1ul);

#if defined(EVE_TOUCH_ADDR) && defined(EVE_TOUCH_TYPE)
    EVE_CMD_REGWRITE(EVE_REG_TOUCH_CONFIG, ((uint32_t)EVE_TOUCH_ADDR << 4) | ((uint32_t)EVE_TOUCH_TYPE) | (1 << 11));
#endif // defined(EVE_TOUCH_ADDR) && defined(EVE_TOUCH_TYPE)

    // 0: 1 pixel single // 1: 2 pixel single // 2: 2 pixel dual // 3: 4 pixel dual
    uint32_t extsyncmode = 3;
    uint32_t lvdstlldiv = EVE_DISP_LVDSTXCLKDIV;
    uint32_t pllcfg = 0;
    if (lvdstlldiv > 4) pllcfg = 0x00300870 + lvdstlldiv;
    else pllcfg = 0x00301070 + lvdstlldiv;

    EVE_CMD_APBWRITE(EVE_REG_LVDSTX_PLLCFG, pllcfg);
    EVE_CMD_APBWRITE(EVE_REG_LVDSTX_EN, 6ul); // Enable PLLs for LVDS CH1 and CH2

    EVE_CMD_REGWRITE(EVE_REG_SO_MODE, extsyncmode);
    EVE_CMD_REGWRITE(EVE_REG_SO_SOURCE, EVE_SWAPCHAIN_0);
    EVE_CMD_REGWRITE(EVE_REG_SO_FORMAT, (uint32_t)EVE_DISP_LVDSTXFORMAT);
    EVE_CMD_REGWRITE(EVE_REG_SO_EN, 1ul);

    EVE_LIB_EndCoProList();
    EVE_LIB_AwaitCoProEmpty();

    // Clear Screen Ready to Start
    EVE_LIB_BeginCoProList();
    EVE_CMD_DLSTART();
    EVE_CLEAR_COLOR_RGB(0, 0, 0);
    EVE_CLEAR(1, 1, 1);
    EVE_DISPLAY();
    EVE_CMD_SWAP();
    EVE_LIB_EndCoProList();
    EVE_LIB_AwaitCoProEmpty();

    // Load base patch or project defined patch if overriden
    if (eve_loadpatch() != 0)
    {
        EVE_DEBUG_ERROR("ERROR: Failed to load/verify BT82x base patch.\n");
        return -1;
    }

#endif // IS_EVE_API(5)

    /* Reset All Bitmap Properties */ 
    EVE_LIB_BeginCoProList();
    EVE_CMD_DLSTART();
    EVE_CLEAR_COLOR_RGB(0, 0, 0);
    EVE_CLEAR(1, 1, 1);
    for (i = 0; i < 16; i++)
    {
#if IS_EVE_API(1)
        EVE_BITMAP_HANDLE(i);
        EVE_BITMAP_LAYOUT(0, 0, 0);
        EVE_BITMAP_SIZE(0, 0, 0, 0, 0);
#elif IS_EVE_API(2, 3, 4, 5)
        EVE_BITMAP_HANDLE(i);
        EVE_CMD_SETBITMAP(0, 0, 0, 0);
#endif // IS_EVE_API
    }
    EVE_DISPLAY();
    EVE_CMD_SWAP();
    EVE_LIB_EndCoProList();
    EVE_LIB_AwaitCoProEmpty();

    return 0;
}

// De-Initialise EVE API.
int EVE_Deinit(void)
{
    if (HAL_EVE_Deinit() < 0)
    {
        return -1;
    }

    return 0;
}

// Begin co-pro list for display creation
void EVE_LIB_BeginCoProList(void)
{
    // Begins SPI transaction
    HAL_ChipSelect(1);

#if !defined(EVE_USE_CMDB_METHOD)
    // Send address for writing as the next free location in the co-pro buffer.
    HAL_SetWriteAddress(EVE_RAM_CMD + HAL_GetCmdPointer());
#else
    // Send address for writing.
    HAL_SetWriteAddress(EVE_REG_CMDB_WRITE);
#endif
}

// End co-pro list for display creation
void EVE_LIB_EndCoProList(void)
{
    // End SPI transaction
    HAL_ChipSelect(0);

#if !defined(EVE_USE_CMDB_METHOD)
    // Update the ring buffer pointer to start decode.
    HAL_WriteCmdPointer();
#endif
}

// Waits for the read and write pointers to become equal
int EVE_LIB_AwaitCoProEmpty(void)
{
    // Await completion of processing.
    return HAL_WaitCmdFifoEmpty(0);
}

// Waits for the read and write pointers to become equal with a millisecond timeout
int EVE_LIB_AwaitCoProEmptyTimeout(uint32_t timeout)
{
    // Await completion of processing
    return HAL_WaitCmdFifoEmpty(timeout);
}

// Recovers the co-processor as described in "Fault Scenarios" in the Programming Guide
void EVE_LIB_RecoverCoPro(void)
{
#if IS_EVE_API(1,2,3,4)
    // EVE1-4 hold co-processor engine in reset 
    HAL_MemWrite8(EVE_REG_CPURESET, 1);
#endif  // IS_EVE_API(1, 2, 3, 4)

    // set REG_CMD_READ to zero
    HAL_MemWrite32(EVE_REG_CMD_READ, 0);

#if IS_EVE_API(5)
    // EVE5 await REG_CMD_WRITE change to zero
    while (HAL_MemRead32(EVE_REG_CMD_WRITE) != 0);
#endif // IS_EVE_API(5)

#if IS_EVE_API(2,3,4)
    // EVE 2,3,4 set REG_CMD_DL to zero
    HAL_MemWrite32(EVE_REG_CMD_DL, 0);
#endif // IS_EVE_API(2,3,4)

#if IS_EVE_API(1,2,3,4)
    // Set REG_CMD_WRITE to zero
    HAL_MemWrite32(EVE_REG_CMD_WRITE, 0);
     // EVE1-4 retart co-processor engine 
    HAL_MemWrite8(EVE_REG_CPURESET, 0);
#endif  // IS_EVE_API(1, 2, 3, 4)

#if !defined(EVE_USE_CMDB_METHOD)
    // reset command pointer
    HAL_ResetCmdPointer();
#endif // !defined(EVE_USE_CMDB_METHOD)
}

// Gets the free space in the co-processor list
uint16_t EVE_LIB_GetCoProSpace(void)
{
    // End SPI transaction
    HAL_ChipSelect(0);

    uint32_t readCmdPointer = HAL_CheckCmdFreeSpace();

    // Begins SPI transaction
    HAL_ChipSelect(1);

#if !defined(EVE_USE_CMDB_METHOD)
    // Send address for writing as the next free location in the co-pro buffer
    HAL_SetWriteAddress(EVE_RAM_CMD + HAL_GetCmdPointer());
#else
    // Send address for writing
    HAL_SetWriteAddress(EVE_REG_CMDB_WRITE);
#endif
    return readCmdPointer;
}

// Starts profiling a co-processor list
#if defined(EVE_COPROC_PROFILE)
void EVE_LIB_BeginCoProProfile(void)
{
    HAL_ResetProfilePointer();
}
#endif

// Gets the current size of the co-processor list
#if defined(EVE_COPROC_PROFILE)
uint16_t EVE_LIB_GetCoProProfile(void)
{
    return HAL_GetProfilePointer();
}
#endif

// Gets the current size of the display list
#if defined(EVE_COPROC_PROFILE)
uint16_t EVE_LIB_GetDlProfile(void)
{
    return HAL_MemRead32(EVE_REG_CMD_DL);
}
#endif

#if !defined (EVE_QSPI_ENABLE)
// Get the status of the interrupt line from EVE
int EVE_LIB_Int(void)
{
    return HAL_Int();
}
#endif // defined (EVE_QSPI_ENABLE)

#if defined (EVE_MANANGE_INTERRUPTS)
// Get the status of the interrupt flag register
uint8_t EVE_LIB_GetInterrupt(uint8_t mask)
{
    static uint8_t curr = 0;
    uint8_t val;
    
    curr |= HAL_MemRead32(EVE_REG_INT_FLAGS);
    // Test mask of set bits
    val = curr & mask;
    // Consume tested bits
    curr = curr & ~mask;

    return val;
}
#endif // defined (EVE_MANANGE_INTERRUPTS)

// Gets a result from the command buffer
uint32_t EVE_LIB_GetResult(int offset)
{
    uint32_t wp, rp;
    uint32_t CmdBufPointer;
    do {
        rp = HAL_MemRead32(EVE_REG_CMD_READ);
        wp = HAL_GetCmdPointer();//HAL_MemRead32(EVE_REG_CMD_WRITE); 
    } while (rp != wp);
    CmdBufPointer = (rp - (offset * sizeof(uint32_t))) & (EVE_RAM_CMD_SIZE - 1);
    return HAL_MemRead32(EVE_RAM_CMD + CmdBufPointer);
}

#if IS_EVE_API(3,4,5)
// Obtain the co-processor exception description (up-to 128 characters)
void EVE_LIB_GetCoProException(char* desc)
{
    uint8_t j;
    uint8_t i;
    char c;
    uint32_t w;

    for (j = 0; j < 128; j += 4)
    {
        // Read the text from the report register
        w = HAL_MemRead32(EVE_COPROC_REPORT + j);
        // Immediately clear the report register
        HAL_MemWrite32(EVE_COPROC_REPORT + j, 0);
        // Add the 4 characters to the report string
        for (i = 0; i < 4; i++)
        {
            c = (w >> (i * 8)) & 0x7f;
            *desc++ = c;
            // Break at the end of the report
            if (c == '\0') break;
        }
        if (c == '\0') break;
    }
}
#endif

// Writes a block of data to the RAM_G
void EVE_LIB_WriteDataToRAMG(const uint8_t* ImgData, uint32_t DataSize, uint32_t DestAddress)
{
    uint32_t CurrentIndex = 0;
    uint32_t ChunkSize = 0;
    uint8_t IsLastChunk = 0;

    // Pad data length to multiple of 4.
    DataSize = (DataSize + 3) & (~3);

    // While not all data is sent
    while (CurrentIndex < DataSize)
    {
        // If more than ChunkSize bytes to send
        if ((DataSize - CurrentIndex) > HAL_MAX_CHUNK_SIZE)
        {
            // ... then add ChunkSize to the current target index to make new target
            ChunkSize = HAL_MAX_CHUNK_SIZE;
            // ... and this is not the last chunk
            IsLastChunk = 0;
        }
        // or if all remaining bytes can fit in one chunk
        else
        {
            // ... then add the amount of data to the current target
            ChunkSize = DataSize - CurrentIndex;
            // .. and this is the last chunk
            IsLastChunk = 1;
        }

        // Begin an SPI burst write
        HAL_ChipSelect(1);
        // Send address to which first value will be written
        HAL_SetWriteAddress(DestAddress + CurrentIndex);
        HAL_Write(ImgData, ChunkSize);
        ImgData += ChunkSize;
        CurrentIndex += ChunkSize;
        // End the SPI burst
        HAL_ChipSelect(0);

        // If this is the last chunk of the data,
        if (IsLastChunk)
        {
            break;
        }
    }
}

// Reads a block of data from the RAM_G
void EVE_LIB_ReadDataFromRAMG(uint8_t* ImgData, uint32_t DataSize, uint32_t SrcAddress)
{
    uint32_t CurrentIndex = 0;
    uint32_t ChunkSize = 0;
    uint8_t IsLastChunk = 0;

    // While not all data is received
    while (CurrentIndex < DataSize)
    {
        // If more than ChunkSize bytes to receive
        if ((DataSize - CurrentIndex) > HAL_MAX_CHUNK_SIZE)
        {
            // ... then add ChunkSize to the current target index to make new target
            ChunkSize = HAL_MAX_CHUNK_SIZE;
            // ... and this is not the last chunk
            IsLastChunk = 0;
        }
        // or if all remaining bytes can fit in one chunk
        else
        {
            // ... then add the amount of data to the current target
            ChunkSize = DataSize - CurrentIndex;
            // .. and this is the last chunk
            IsLastChunk = 1;
        }

        // Begin an SPI burst read
        HAL_ChipSelect(1);
        // Send address to which first value will be read
        HAL_SetReadAddress(SrcAddress + CurrentIndex);
        HAL_Read(ImgData, ChunkSize);
        ImgData += ChunkSize;
        CurrentIndex += ChunkSize;
        // End the SPI burst
        HAL_ChipSelect(0);

        // If this is the last chunk of the data,
        if (IsLastChunk)
        {
            break;
        }
    }
}

// Write a block of data to the co-processor
void EVE_LIB_WriteDataToCMD(const uint8_t* ImgData, uint32_t DataSize)
{
    uint32_t CurrentIndex = 0;
    uint32_t ChunkSize = 0;
    uint8_t IsLastChunk = 0;

    // Finish the current transaction.
    EVE_LIB_EndCoProList();

    // This code works by sending the data in a series of one or more bursts.
    // If the data is more than HAL_MAX_CHUNK_SIZE bytes, it is sent as a series of
    // one or more bursts and then the remainder. HAL_MAX_CHUNK_SIZE is a size which
    // is smaller than the command buffer on the EVE and small enough to gain
    // maximum buffering effect from the MCU SPI hardware.

    // Pad data length to multiple of 4.
    DataSize = (DataSize + 3) & (~3);

    // While not all data is sent
    while (CurrentIndex < DataSize)
    {
        // If more than ChunkSize bytes to send.
        if ((DataSize - CurrentIndex) > HAL_MAX_CHUNK_SIZE)
        {
            // ... then add ChunkSize to the current target index to make new target.
            ChunkSize = HAL_MAX_CHUNK_SIZE;
            // ... and this is not the last chunk.
            IsLastChunk = 0;
        }
        // Or if all remaining bytes can fit in one chunk.
        else
        {
            // ... then add the amount of data to the current target.
            ChunkSize = DataSize - CurrentIndex;
            // .. and this is the last chunk.
            IsLastChunk = 1;
        }

#if !defined(EVE_USE_INTERRUPT_METHOD)
        // Wait until there is space.
        uint32_t Freespace = 0;
        while (Freespace < HAL_MAX_CHUNK_SIZE)
        {
            Freespace = HAL_CheckCmdFreeSpace();
        }
#endif // !defined(EVE_USE_INTERRUPT_METHOD)

        // Begin an SPI burst write to the next location in the FIFO.
        EVE_LIB_BeginCoProList();

#if !defined(EVE_USE_CMDB_METHOD)
        uint32_t c;
        for (c = 0; c < ChunkSize; c+=4)
        {
            HAL_WriteCmd(*(uint32_t *)(ImgData + c));
        }
#else
        HAL_Write(ImgData, ChunkSize);
#endif
        ImgData += ChunkSize;
        CurrentIndex += ChunkSize;

        // If this is the last chunk of the data break.
        if (IsLastChunk)
        {
            break;
        }

        // End the SPI burst.
        EVE_LIB_EndCoProList();
#if defined(EVE_USE_INTERRUPT_METHOD)
        if (EVE_LIB_AwaitCoProEmpty()) return;
#endif // !defined(EVE_USE_INTERRUPT_METHOD)

    }
}

void EVE_LIB_MemWrite32(uint32_t addr, uint32_t value)
{
    HAL_MemWrite32(addr, value);
}

uint32_t EVE_LIB_MemRead32(uint32_t address)
{
    return HAL_MemRead32(address);
}

#if IS_EVE_API(1, 2, 3, 4) // Not supported on BT82x.
void EVE_LIB_MemWrite16(uint32_t addr, uint16_t value)
{
    HAL_MemWrite16(addr, value);
}

uint16_t EVE_LIB_MemRead16(uint32_t address)
{
    return HAL_MemRead16(address);
}

void EVE_LIB_MemWrite8(uint32_t addr, uint8_t value)
{
    HAL_MemWrite8(addr, value);
}

uint8_t EVE_LIB_MemRead8(uint32_t address)
{
    return HAL_MemRead8(address);
}
#endif  // IS_EVE_API(1, 2, 3, 4)

// Writes a string over SPI.
uint16_t EVE_LIB_SendString(const char* string)
{
    uint16_t length;
    uint16_t CommandSize;

    // Include the terminating null character in the string length.
    // Pad string length to a multiple of 4.
    length = ((strlen(string) + 1) + 3) & (~3);
    CommandSize = length;

#if MCU_UNALIGNED_ACCESSES 
    // Send string as 32 bit data.
    while (length)
    {
        HAL_WriteCmd(*(uint32_t*)string);
        string += 4;
        length -= 4;
    }
#else
    uint32_t val32;
    while (length)
    {
        val32 = *string++;
        val32 |= ((uint32_t)*string++ << 8);
        val32 |= ((uint32_t)*string++ << 16);
        val32 |= ((uint32_t)*string++ << 24);
        HAL_WriteCmd(val32);
        length -= 4;
    }
#endif

    return CommandSize;
}

void EVE_LIB_GetProps(uint32_t* addr, uint32_t* width, uint32_t* height)
{
    // To read the result from CMD_GETPROPS we need to be clever and find out
    // where the CoProcessor is writing the command. We can then retrieve the
    // results from the place where they were written.
    // Send the command to the CoProcessor.
    EVE_LIB_BeginCoProList();
    EVE_CMD_GETPROPS(0, 0, 0);
    // Wait for it to finish.
    // TODO For BT82x (EVE5_API) use EVE_ENC_CMD_RESULT to retrive these results.
    EVE_LIB_EndCoProList();
    EVE_LIB_AwaitCoProEmpty();
    // Obtain the results from the EVE_RAM_CMD in the CoProcessor.
    *addr = EVE_LIB_GetResult(3);
    *width = EVE_LIB_GetResult(2);
    *height = EVE_LIB_GetResult(1);
}

void EVE_LIB_GetPtr(uint32_t* addr)
{
    EVE_LIB_BeginCoProList();
    // Send the command to the CoProcessor.
    EVE_CMD_GETPTR(0);
    // Wait for it to finish.
    EVE_LIB_EndCoProList();
    EVE_LIB_AwaitCoProEmpty();
    // Obtain the results from the EVE_RAM_CMD in the CoProcessor.
    *addr = EVE_LIB_GetResult(1);
}

void EVE_LIB_GetMatrix(uint32_t* a, uint32_t* b, uint32_t* c, uint32_t* d, uint32_t* e, uint32_t* f)
{
    EVE_LIB_BeginCoProList();
    // Send the command to the CoProcessor.
    EVE_CMD_GETMATRIX(0, 0, 0, 0, 0, 0);
    // Wait for it to finish.
    EVE_LIB_EndCoProList();
    EVE_LIB_AwaitCoProEmpty();
    // Obtain the results from the EVE_RAM_CMD in the CoProcessor.
    *a = EVE_LIB_GetResult(6);
    *b = EVE_LIB_GetResult(5);
    *c = EVE_LIB_GetResult(4);
    *d = EVE_LIB_GetResult(3);
    *e = EVE_LIB_GetResult(2);
    *f = EVE_LIB_GetResult(1);
}

void EVE_LIB_MemCrc(uint32_t ptr, uint32_t num, uint32_t* result)
{
    EVE_LIB_BeginCoProList();
    // Send the command to the CoProcessor.
    EVE_CMD_MEMCRC(ptr, num, 0);
    // Wait for it to finish.
    EVE_LIB_EndCoProList();
    EVE_LIB_AwaitCoProEmpty();
    // Obtain the results from the EVE_RAM_CMD in the CoProcessor.
    *result = EVE_LIB_GetResult(1);
}

#if IS_EVE_API(2, 3, 4, 5)
void EVE_LIB_BitmapTransform(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2,
    int32_t tx0, int32_t ty0, int32_t tx1, int32_t ty1, int32_t tx2, int32_t ty2,
    uint32_t* result)
{
    EVE_LIB_BeginCoProList();
    // Send the command to the CoProcessor.
    EVE_CMD_BITMAP_TRANSFORM(x0, y0, x1, y1, x2, y2, tx0, ty0, tx1, ty1, tx2, ty2, 0);
    // Wait for it to finish.
    EVE_LIB_EndCoProList();
    EVE_LIB_AwaitCoProEmpty();
    // Obtain the results from the EVE_RAM_CMD in the CoProcessor.
    *result = EVE_LIB_GetResult(1);
}
#endif

#if IS_EVE_API(4, 5)
void EVE_LIB_GetImage(uint32_t* addr, uint32_t* fmt, uint32_t* width, uint32_t* height, uint32_t* palette)
{
    EVE_LIB_BeginCoProList();
    // Send the command to the CoProcessor.
    EVE_CMD_GETIMAGE(0, 0, 0, 0, 0);
    // Wait for it to finish.
    EVE_LIB_EndCoProList();
    EVE_LIB_AwaitCoProEmpty();
    // Obtain the results from the EVE_RAM_CMD in the CoProcessor.
    *addr = EVE_LIB_GetResult(5);
    *fmt = EVE_LIB_GetResult(4);
    *width = EVE_LIB_GetResult(3);
    *height = EVE_LIB_GetResult(2);
    *palette = EVE_LIB_GetResult(1);
}
#endif

#if IS_EVE_API(5)
void EVE_LIB_RegRead(uint32_t addr, uint32_t* value)
{
    EVE_LIB_BeginCoProList();
    // Send the command to the CoProcessor.
    EVE_CMD_REGREAD(addr, 0);
    // Wait for it to finish.
    EVE_LIB_EndCoProList();
    EVE_LIB_AwaitCoProEmpty();
    // Obtain the results from the EVE_RAM_CMD in the CoProcessor.
    *value = EVE_LIB_GetResult(1);
}
#endif

/*  Display List Commands for the Co-processor */

void EVE_CMD(uint32_t c)
{
    HAL_WriteCmd(c);
}

void EVE_CLEAR_COLOR_RGB(uint8_t R, uint8_t G, uint8_t B)
{
    HAL_WriteCmd(EVE_ENC_CLEAR_COLOR_RGB(R, G, B));
}

void EVE_CLEAR_COLOR(uint32_t c)
{
    HAL_WriteCmd(EVE_ENC_CLEAR_COLOR(c));
}

void EVE_CLEAR(uint8_t C, uint8_t S, uint8_t T)
{
    HAL_WriteCmd(EVE_ENC_CLEAR((C & 0x01), (S & 0x01), (T & 0x01)));
}

void EVE_COLOR_RGB(uint8_t R, uint8_t G, uint8_t B)
{
    HAL_WriteCmd(EVE_ENC_COLOR_RGB(R, G, B));
}

void EVE_COLOR(uint32_t c)
{
    HAL_WriteCmd(EVE_ENC_COLOR(c));
}

void EVE_VERTEX2F(int16_t x, int16_t y)
{
    HAL_WriteCmd(EVE_ENC_VERTEX2F(x, y));
}

void EVE_VERTEX2II(uint16_t x, uint16_t y, uint8_t handle, uint8_t cell)
{
    HAL_WriteCmd(EVE_ENC_VERTEX2II(x, y, handle, cell));
}

void EVE_BITMAP_HANDLE(uint8_t handle)
{
    HAL_WriteCmd(EVE_ENC_BITMAP_HANDLE(handle));
}

void EVE_BITMAP_SOURCE(int32_t addr)
{
    HAL_WriteCmd(EVE_ENC_BITMAP_SOURCE((int32_t)addr));
}

#if IS_EVE_API(3, 4)
void EVE_BITMAP_SOURCE2(uint8_t flash_or_ram, int32_t addr)
{
    HAL_WriteCmd(EVE_ENC_BITMAP_SOURCE2((uint32_t)flash_or_ram, (int32_t)addr));
}
#endif

void EVE_BITMAP_LAYOUT(uint8_t format, uint16_t linestride, uint16_t height)
{
    HAL_WriteCmd(EVE_ENC_BITMAP_LAYOUT(format, linestride, height));
}

void EVE_BITMAP_SIZE(uint8_t filter, uint8_t wrapx, uint8_t wrapy, uint16_t width, uint16_t height)
{
    HAL_WriteCmd(EVE_ENC_BITMAP_SIZE(filter, wrapx, wrapy, width, height));
}

void EVE_CELL(uint8_t cell)
{
    HAL_WriteCmd(EVE_ENC_CELL(cell));
}

void EVE_TAG(uint8_t s)
{
    HAL_WriteCmd(EVE_ENC_TAG(s));
}

void EVE_ALPHA_FUNC(uint8_t func, uint8_t ref)
{
    HAL_WriteCmd(EVE_ENC_ALPHA_FUNC(func, ref));
}

void EVE_STENCIL_FUNC(uint8_t func, uint8_t ref, uint8_t mask)
{
    HAL_WriteCmd(EVE_ENC_STENCIL_FUNC(func, ref, mask));
}

void EVE_BLEND_FUNC(uint8_t src, uint8_t dst)
{
    HAL_WriteCmd(EVE_ENC_BLEND_FUNC(src, dst));
}

void EVE_STENCIL_OP(uint8_t sfail, uint8_t spass)
{
    HAL_WriteCmd(EVE_ENC_STENCIL_OP(sfail, spass));
}

void EVE_POINT_SIZE(uint16_t size)
{
    HAL_WriteCmd(EVE_ENC_POINT_SIZE(size));
}

void EVE_LINE_WIDTH(uint16_t width)
{
    HAL_WriteCmd(EVE_ENC_LINE_WIDTH(width));
}

void EVE_CLEAR_COLOR_A(uint8_t alpha)
{
    HAL_WriteCmd(EVE_ENC_CLEAR_COLOR_A(alpha));
}

void EVE_COLOR_A(uint8_t alpha)
{
    HAL_WriteCmd(EVE_ENC_COLOR_A(alpha));
}

void EVE_CLEAR_STENCIL(uint8_t s)
{
    HAL_WriteCmd(EVE_ENC_CLEAR_STENCIL(s));
}

void EVE_CLEAR_TAG(uint8_t s)
{
    HAL_WriteCmd(EVE_ENC_CLEAR_TAG(s));
}

void EVE_STENCIL_MASK(uint8_t mask)
{
    HAL_WriteCmd(EVE_ENC_STENCIL_MASK(mask));
}

void EVE_TAG_MASK(uint8_t mask)
{
    HAL_WriteCmd(EVE_ENC_TAG_MASK(mask));
}

void EVE_SCISSOR_XY(uint16_t x, uint16_t y)
{
    HAL_WriteCmd(EVE_ENC_SCISSOR_XY(x, y));
}

void EVE_SCISSOR_SIZE(uint16_t width, uint16_t height)
{
    HAL_WriteCmd(EVE_ENC_SCISSOR_SIZE(width, height));
}

void EVE_CALL(uint16_t dest)
{
    HAL_WriteCmd(EVE_ENC_CALL(dest));
}

void EVE_JUMP(uint16_t dest)
{
    HAL_WriteCmd(EVE_ENC_JUMP(dest));
}

void EVE_BEGIN(uint8_t prim)
{
    HAL_WriteCmd(EVE_ENC_BEGIN(prim));
}

void EVE_COLOR_MASK(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    HAL_WriteCmd(EVE_ENC_COLOR_MASK(r, g, b, a));
}

void EVE_END(void)
{
    HAL_WriteCmd(EVE_ENC_END());
}

void EVE_SAVE_CONTEXT(void)
{
    HAL_WriteCmd(EVE_ENC_SAVE_CONTEXT());
}

void EVE_RESTORE_CONTEXT(void)
{
    HAL_WriteCmd(EVE_ENC_RESTORE_CONTEXT());
}

void EVE_RETURN(void)
{
    HAL_WriteCmd(EVE_ENC_RETURN());
}

void EVE_MACRO(uint8_t m)
{
    HAL_WriteCmd(EVE_ENC_MACRO(m));
}

void EVE_DISPLAY(void)
{
    HAL_WriteCmd(EVE_ENC_DISPLAY());
}

void EVE_BITMAP_TRANSFORM_A(long a)
{
    HAL_WriteCmd(EVE_ENC_BITMAP_TRANSFORM_A(a));
}

void EVE_BITMAP_TRANSFORM_B(long b)
{
    HAL_WriteCmd(EVE_ENC_BITMAP_TRANSFORM_B(b));
}

void EVE_BITMAP_TRANSFORM_C(long c)
{
    HAL_WriteCmd(EVE_ENC_BITMAP_TRANSFORM_C(c));
}

void EVE_BITMAP_TRANSFORM_D(long d)
{
    HAL_WriteCmd(EVE_ENC_BITMAP_TRANSFORM_D(d));
}

void EVE_BITMAP_TRANSFORM_E(long e)
{
    HAL_WriteCmd(EVE_ENC_BITMAP_TRANSFORM_E(e));
}

void EVE_BITMAP_TRANSFORM_F(long f)
{
    HAL_WriteCmd(EVE_ENC_BITMAP_TRANSFORM_F(f));
}

#if IS_EVE_API(2, 3, 4, 5)

void EVE_VERTEX_FORMAT(uint8_t frac)
{
    HAL_WriteCmd(EVE_ENC_VERTEX_FORMAT(frac));
}

void EVE_BITMAP_LAYOUT_H(uint8_t linestride, uint8_t height)
{
    HAL_WriteCmd(EVE_ENC_BITMAP_LAYOUT_H(linestride, height));
}

void EVE_BITMAP_SIZE_H(uint8_t width, uint8_t height)
{
    HAL_WriteCmd(EVE_ENC_BITMAP_SIZE_H(width, height));
}

void EVE_PALETTE_SOURCE(uint32_t addr)
{
    HAL_WriteCmd(EVE_ENC_PALETTE_SOURCE(addr));
}

void EVE_VERTEX_TRANSLATE_X(uint32_t x)
{
    HAL_WriteCmd(EVE_ENC_VERTEX_TRANSLATE_X(x));
}

void EVE_VERTEX_TRANSLATE_Y(uint32_t y)
{
    HAL_WriteCmd(EVE_ENC_VERTEX_TRANSLATE_Y(y));
}

void EVE_NOP(void)
{
    HAL_WriteCmd(EVE_ENC_NOP());
}

#endif

#if IS_EVE_API(3, 4, 5)

void EVE_BITMAP_EXT_FORMAT(uint16_t fmt)
{
    HAL_WriteCmd(EVE_ENC_BITMAP_EXT_FORMAT(fmt));
}

void EVE_BITMAP_SWIZZLE(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    HAL_WriteCmd(EVE_ENC_BITMAP_SWIZZLE(r, g, b, a));
}

#endif

#if IS_EVE_API(5) // BT82x extensions.

void EVE_BITMAP_SOURCE_H(uint8_t addr)
{
    HAL_WriteCmd(EVE_ENC_BITMAP_SOURCE_H(addr));
}

void EVE_BITMAP_ZORDER(uint8_t o)
{
    HAL_WriteCmd(EVE_ENC_BITMAP_ZORDER(o));
}

void EVE_PALLETE_SOURCE_H(uint8_t addr)
{
    HAL_WriteCmd(EVE_ENC_PALLETE_SOURCE_H(addr));
}

void EVE_REGION(uint8_t y, uint8_t h, uint16_t dest)
{
    HAL_WriteCmd(EVE_ENC_REGION(y, h, dest));
}

#endif

/* Co-Processor Widget Commands */

void EVE_CMD_KEYS(int16_t x, int16_t y, int16_t w, int16_t h, int16_t font, uint16_t options, const char* string)
{

    HAL_WriteCmd(EVE_ENC_CMD_KEYS);
    HAL_WriteCmd(((uint32_t)y << 16) | (x & 0xffff));
    HAL_WriteCmd(((uint32_t)h << 16) | (w & 0xffff));
    HAL_WriteCmd(((uint32_t)options << 16) | (font & 0xffff));

    EVE_LIB_SendString(string);
}

void EVE_CMD_NUMBER(int16_t x, int16_t y, int16_t font, uint16_t options, int32_t n)
{
    HAL_WriteCmd(EVE_ENC_CMD_NUMBER);
    HAL_WriteCmd(((uint32_t)y << 16) | (x & 0xffff));
    HAL_WriteCmd(((uint32_t)options << 16) | (font & 0xffff));
    HAL_WriteCmd(n);
}

void EVE_CMD_LOADIDENTITY(void)
{
    HAL_WriteCmd(EVE_ENC_CMD_LOADIDENTITY);
}

/* Error handling for val is not done, so better to always use range of 65535 in order that needle is drawn within display region */
void EVE_CMD_GAUGE(int16_t x, int16_t y, int16_t r, uint16_t options, uint16_t major, uint16_t minor, uint16_t val, uint16_t range)
{
    HAL_WriteCmd(EVE_ENC_CMD_GAUGE);
    HAL_WriteCmd(((uint32_t)y << 16) | (x & 0xffff));
    HAL_WriteCmd(((uint32_t)options << 16) | (r & 0xffff));
    HAL_WriteCmd(((uint32_t)minor << 16) | (major & 0xffff));
    HAL_WriteCmd(((uint32_t)range << 16) | (val & 0xffff));
}

void EVE_CMD_REGREAD(uint32_t ptr, uint32_t result)
{
    HAL_WriteCmd(EVE_ENC_CMD_REGREAD);
    HAL_WriteCmd(ptr);
    HAL_WriteCmd(result);
}

void EVE_CMD_GETPROPS(uint32_t ptr, uint32_t w, uint32_t h)
{
    HAL_WriteCmd(EVE_ENC_CMD_GETPROPS);
    HAL_WriteCmd(ptr);
    HAL_WriteCmd(w);
    HAL_WriteCmd(h);
}

void EVE_CMD_MEMCPY(uint32_t dest, uint32_t src, uint32_t num)
{
    HAL_WriteCmd(EVE_ENC_CMD_MEMCPY);
    HAL_WriteCmd(dest);
    HAL_WriteCmd(src);
    HAL_WriteCmd(num);
}

void EVE_CMD_SPINNER(int16_t x, int16_t y, uint16_t style, uint16_t scale)
{
    HAL_WriteCmd(EVE_ENC_CMD_SPINNER);
    HAL_WriteCmd(((uint32_t)y << 16) | (x & 0xffff));
    HAL_WriteCmd(((uint32_t)scale << 16) | (style & 0xffff));
}

void EVE_CMD_BGCOLOR(uint32_t c)
{
    HAL_WriteCmd(EVE_ENC_CMD_BGCOLOR);
    HAL_WriteCmd(c);
}

void EVE_CMD_SWAP(void)
{
    HAL_WriteCmd(EVE_ENC_CMD_SWAP);
}

void EVE_CMD_TRANSLATE(int32_t tx, int32_t ty)
{
    HAL_WriteCmd(EVE_ENC_CMD_TRANSLATE);
    HAL_WriteCmd(tx);
    HAL_WriteCmd(ty);
}

void EVE_CMD_STOP(void)
{
    HAL_WriteCmd(EVE_ENC_CMD_STOP);
}

void EVE_CMD_SLIDER(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t options, uint16_t val, uint16_t range)
{
    HAL_WriteCmd(EVE_ENC_CMD_SLIDER);
    HAL_WriteCmd(((uint32_t)y << 16) | (x & 0xffff));
    HAL_WriteCmd(((uint32_t)h << 16) | (w & 0xffff));
    HAL_WriteCmd(((uint32_t)val << 16) | (options & 0xffff));
    HAL_WriteCmd(range);
}

void EVE_CMD_INTERRUPT(uint32_t ms)
{
    HAL_WriteCmd(EVE_ENC_CMD_INTERRUPT);
    HAL_WriteCmd(ms);
}

void EVE_CMD_FGCOLOR(uint32_t c)
{
    HAL_WriteCmd(EVE_ENC_CMD_FGCOLOR);
    HAL_WriteCmd(c);
}

void EVE_CMD_ROTATE(int32_t a)
{
    HAL_WriteCmd(EVE_ENC_CMD_ROTATE);
    HAL_WriteCmd(a);
}

void EVE_CMD_MEMWRITE(uint32_t ptr, uint32_t num)
{
    HAL_WriteCmd(EVE_ENC_CMD_MEMWRITE);
    HAL_WriteCmd(ptr);
    HAL_WriteCmd(num);
}

void EVE_CMD_SCROLLBAR(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t options, uint16_t val, uint16_t size, uint16_t range)
{
    HAL_WriteCmd(EVE_ENC_CMD_SCROLLBAR);
    HAL_WriteCmd(((uint32_t)y << 16) | (x & 0xffff));
    HAL_WriteCmd(((uint32_t)h << 16) | (w & 0xffff));
    HAL_WriteCmd(((uint32_t)val << 16) | (options & 0xffff));
    HAL_WriteCmd(((uint32_t)range << 16) | (size & 0xffff));
}

void EVE_CMD_GETMATRIX(int32_t a, int32_t b, int32_t c, int32_t d, int32_t e, int32_t f)
{
    HAL_WriteCmd(EVE_ENC_CMD_GETMATRIX);
    HAL_WriteCmd(a);
    HAL_WriteCmd(b);
    HAL_WriteCmd(c);
    HAL_WriteCmd(d);
    HAL_WriteCmd(e);
    HAL_WriteCmd(f);
}

void EVE_CMD_SKETCH(int16_t x, int16_t y, uint16_t w, uint16_t h, uint32_t ptr, uint16_t format)
{
    HAL_WriteCmd(EVE_ENC_CMD_SKETCH);
    HAL_WriteCmd(((uint32_t)y << 16) | (x & 0xffff));
    HAL_WriteCmd(((uint32_t)h << 16) | (w & 0xffff));
    HAL_WriteCmd(ptr);
    HAL_WriteCmd(format);
}

void EVE_CMD_MEMSET(uint32_t ptr, uint32_t value, uint32_t num)
{
    HAL_WriteCmd(EVE_ENC_CMD_MEMSET);
    HAL_WriteCmd(ptr);
    HAL_WriteCmd(value);
    HAL_WriteCmd(num);
}

void EVE_CMD_GRADCOLOR(uint32_t c)
{
    HAL_WriteCmd(EVE_ENC_CMD_GRADCOLOR);
    HAL_WriteCmd(c);
}

void EVE_CMD_BITMAP_TRANSFORM(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t tx0, int32_t ty0, int32_t tx1, int32_t ty1, int32_t tx2, int32_t ty2, uint16_t result)
{
    HAL_WriteCmd(EVE_ENC_CMD_BITMAP_TRANSFORM);
    HAL_WriteCmd(x0);
    HAL_WriteCmd(y0);
    HAL_WriteCmd(x1);
    HAL_WriteCmd(y1);
    HAL_WriteCmd(x2);
    HAL_WriteCmd(y2);
    HAL_WriteCmd(tx0);
    HAL_WriteCmd(ty0);
    HAL_WriteCmd(tx1);
    HAL_WriteCmd(ty1);
    HAL_WriteCmd(tx2);
    HAL_WriteCmd(ty2);
    HAL_WriteCmd(result);
}

void EVE_CMD_CALIBRATE(uint32_t result)
{
    HAL_WriteCmd(EVE_ENC_CMD_CALIBRATE);
    HAL_WriteCmd(result);
}

#if IS_EVE_API(1, 2, 3, 4) // FT82x API change

void EVE_CMD_INFLATE(uint32_t ptr)
{
    HAL_WriteCmd(EVE_ENC_CMD_INFLATE);
    HAL_WriteCmd(ptr);
}

void EVE_CMD_SETFONT(uint32_t font, uint32_t ptr)
{
    HAL_WriteCmd(EVE_ENC_CMD_SETFONT);
    HAL_WriteCmd(font);
    HAL_WriteCmd(ptr);
}

#elif IS_EVE_API(5) // FT81x API change

void EVE_CMD_INFLATE(uint32_t ptr, uint32_t options)
{
    HAL_WriteCmd(EVE_ENC_CMD_INFLATE);
    HAL_WriteCmd(ptr);
    HAL_WriteCmd(options);
}

void EVE_CMD_SETFONT(uint32_t font, uint32_t ptr, uint32_t firstchar)
{
    HAL_WriteCmd(EVE_ENC_CMD_SETFONT);
    HAL_WriteCmd(font);
    HAL_WriteCmd(ptr);
    HAL_WriteCmd(firstchar);
}

#endif

void EVE_CMD_LOGO(void)
{
    HAL_WriteCmd(EVE_ENC_CMD_LOGO);
}

void EVE_CMD_APPEND(uint32_t ptr, uint32_t num)
{
    HAL_WriteCmd(EVE_ENC_CMD_APPEND);
    HAL_WriteCmd(ptr);
    HAL_WriteCmd(num);
}

void EVE_CMD_MEMZERO(uint32_t ptr, uint32_t num)
{
    HAL_WriteCmd(EVE_ENC_CMD_MEMZERO);
    HAL_WriteCmd(ptr);
    HAL_WriteCmd(num);
}

void EVE_CMD_SCALE(int32_t sx, int32_t sy)
{
    HAL_WriteCmd(EVE_ENC_CMD_SCALE);
    HAL_WriteCmd(sx);
    HAL_WriteCmd(sy);
}

void EVE_CMD_CLOCK(int16_t x, int16_t y, int16_t r, uint16_t options, uint16_t h, uint16_t m, uint16_t s, uint16_t ms)
{
    HAL_WriteCmd(EVE_ENC_CMD_CLOCK);
    HAL_WriteCmd(((uint32_t)y << 16) | (x & 0xffff));
    HAL_WriteCmd(((uint32_t)options << 16) | (r & 0xffff));
    HAL_WriteCmd(((uint32_t)m << 16) | (h & 0xffff));
    HAL_WriteCmd(((uint32_t)ms << 16) | (s & 0xffff));
}

void EVE_CMD_GRADIENT(int16_t x0, int16_t y0, uint32_t rgb0, int16_t x1, int16_t y1, uint32_t rgb1)
{
    HAL_WriteCmd(EVE_ENC_CMD_GRADIENT);
    HAL_WriteCmd(((uint32_t)y0 << 16) | (x0 & 0xffff));
    HAL_WriteCmd(rgb0);
    HAL_WriteCmd(((uint32_t)y1 << 16) | (x1 & 0xffff));
    HAL_WriteCmd(rgb1);
}

void EVE_CMD_SETMATRIX(void)
{
    HAL_WriteCmd(EVE_ENC_CMD_SETMATRIX);
}

void EVE_CMD_TRACK(int16_t x, int16_t y, int16_t w, int16_t h, int16_t tag)
{
    HAL_WriteCmd(EVE_ENC_CMD_TRACK);
    HAL_WriteCmd(((uint32_t)y << 16) | (x & 0xffff));
    HAL_WriteCmd(((uint32_t)h << 16) | (w & 0xffff));
    HAL_WriteCmd(tag);
}

void EVE_CMD_GETPTR(uint32_t result)
{
    HAL_WriteCmd(EVE_ENC_CMD_GETPTR);
    HAL_WriteCmd(result);
}

void EVE_CMD_PROGRESS(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t options, uint16_t val, uint16_t range)
{
    HAL_WriteCmd(EVE_ENC_CMD_PROGRESS);
    HAL_WriteCmd(((uint32_t)y << 16) | (x & 0xffff));
    HAL_WriteCmd(((uint32_t)h << 16) | (w & 0xffff));
    HAL_WriteCmd(((uint32_t)val << 16) | (options & 0xffff));
    HAL_WriteCmd(range);
}

void EVE_CMD_COLDSTART(void)
{
    HAL_WriteCmd(EVE_ENC_CMD_COLDSTART);
}

void EVE_CMD_DIAL(int16_t x, int16_t y, int16_t r, uint16_t options, uint16_t val)
{
    HAL_WriteCmd(EVE_ENC_CMD_DIAL);
    HAL_WriteCmd(((uint32_t)y << 16) | (x & 0xffff));
    HAL_WriteCmd(((uint32_t)options << 16) | (r & 0xffff));
    HAL_WriteCmd(val);
}

void EVE_CMD_LOADIMAGE(uint32_t ptr, uint32_t options)
{
    HAL_WriteCmd(EVE_ENC_CMD_LOADIMAGE);
    HAL_WriteCmd(ptr);
    HAL_WriteCmd(options);
}

void EVE_CMD_DLSTART(void)
{
    HAL_WriteCmd(EVE_ENC_CMD_DLSTART);
}

void EVE_CMD_SNAPSHOT(uint32_t ptr)
{
    HAL_WriteCmd(EVE_ENC_CMD_SNAPSHOT);
    HAL_WriteCmd(ptr);
}

void EVE_CMD_SCREENSAVER(void)
{
    HAL_WriteCmd(EVE_ENC_CMD_SCREENSAVER);
}

void EVE_CMD_MEMCRC(uint32_t ptr, uint32_t num, uint32_t result)
{
    HAL_WriteCmd(EVE_ENC_CMD_MEMCRC);
    HAL_WriteCmd(ptr);
    HAL_WriteCmd(num);
    HAL_WriteCmd(result);
}

uint8_t COUNT_ARGS(const char* string)
{
    uint8_t count = 0;
    const char* tmp = string;

    while ((tmp = strstr(tmp, "%")))
    {
        if (*(tmp + 1) == '%') {
            tmp += 2;
        }
        else {
            count++;
            tmp++;
        }
    }
    return count;
}

void EVE_CMD_TEXT(int16_t x, int16_t y, int16_t font, uint16_t options, const char* string, ...)
{
    va_list args;
    uint8_t i, num = 0;

    va_start(args, string);

#if IS_EVE_API(3, 4, 5)
    // Only check % characters if option OPT_FORMAT is set.
    num = (options & EVE_OPT_FORMAT) ? (COUNT_ARGS(string)) : (0);
#endif

    HAL_WriteCmd(EVE_ENC_CMD_TEXT);
    HAL_WriteCmd(((uint32_t)y << 16) | (x & 0xffff));
    HAL_WriteCmd(((uint32_t)options << 16) | (font & 0xffff));

    EVE_LIB_SendString(string);

    for (i = 0; i < num; i++)
    {
        HAL_WriteCmd((uint32_t)va_arg(args, uint32_t));
    }

    va_end(args);
}

void EVE_CMD_BUTTON(int16_t x, int16_t y, int16_t w, int16_t h, int16_t font, uint16_t options, const char* string, ...)
{
    va_list args;
    uint8_t i, num = 0;

    va_start(args, string);

#if IS_EVE_API(3, 4, 5)
    // Only check % characters if option OPT_FORMAT is set.
    num = (options & EVE_OPT_FORMAT) ? (COUNT_ARGS(string)) : (0); 
#endif

    HAL_WriteCmd(EVE_ENC_CMD_BUTTON);
    HAL_WriteCmd(((uint32_t)y << 16) | (x & 0xffff));
    HAL_WriteCmd(((uint32_t)h << 16) | (w & 0xffff));
    HAL_WriteCmd(((uint32_t)options << 16) | (font & 0xffff));

    EVE_LIB_SendString(string);

    for (i = 0; i < num; i++)
    {
        HAL_WriteCmd((uint32_t)va_arg(args, uint32_t));
    }

    va_end(args);
}

void EVE_CMD_TOGGLE(int16_t x, int16_t y, int16_t w, int16_t font, uint16_t options, uint16_t state, const char* string, ...)
{
    va_list args;
    uint8_t i, num = 0;

    va_start(args, string);

#if IS_EVE_API(3, 4, 5)
    num = (options & EVE_OPT_FORMAT) ? (COUNT_ARGS(string)) : (0); //Only check % characters if option OPT_FORMAT is set
#endif

    HAL_WriteCmd(EVE_ENC_CMD_TOGGLE);
    HAL_WriteCmd(((uint32_t)y << 16) | (x & 0xffff));
    HAL_WriteCmd(((uint32_t)font << 16) | (w & 0xffff));
    HAL_WriteCmd(((uint32_t)state << 16) | options);

    EVE_LIB_SendString(string);

    for (i = 0; i < num; i++)
    {
        HAL_WriteCmd((uint32_t)va_arg(args, uint32_t));
    }

    va_end(args);
}

#if IS_EVE_API(2)
void EVE_CMD_CSKETCH(int16_t x, int16_t y, uint16_t w, uint16_t h, uint32_t ptr, uint16_t format, uint16_t freq)
{
    HAL_WriteCmd(EVE_ENC_CMD_CSKETCH);
    HAL_WriteCmd(((uint32_t)y << 16) | (x & 0xffff));
    HAL_WriteCmd(((uint32_t)h << 16) | (w & 0xffff));
    HAL_WriteCmd(ptr);
    HAL_WriteCmd(((uint32_t)freq << 16) | (format & 0xffff));
}
#endif

#if IS_EVE_API(2, 3, 4, 5)

void EVE_CMD_SETROTATE(uint32_t r)
{
    HAL_WriteCmd(EVE_ENC_CMD_SETROTATE);
    HAL_WriteCmd(r);
}

void EVE_CMD_MEDIAFIFO(uint32_t ptr, uint32_t size)
{
    HAL_WriteCmd(EVE_ENC_CMD_MEDIAFIFO);
    HAL_WriteCmd(ptr);
    HAL_WriteCmd(size);
}

void EVE_CMD_SYNC(void)
{
    HAL_WriteCmd(EVE_ENC_CMD_SYNC);
}

void EVE_CMD_ROMFONT(uint32_t font, uint32_t romslot)
{
    HAL_WriteCmd(EVE_ENC_CMD_ROMFONT);
    HAL_WriteCmd(font);
    HAL_WriteCmd(romslot);
}

void EVE_CMD_PLAYVIDEO(uint32_t options)
{
    HAL_WriteCmd(EVE_ENC_CMD_PLAYVIDEO);
    HAL_WriteCmd(options);
}

void EVE_CMD_VIDEOFRAME(uint32_t dst, uint32_t ptr)
{
    HAL_WriteCmd(EVE_ENC_CMD_VIDEOFRAME);
    HAL_WriteCmd(dst);
    HAL_WriteCmd(ptr);
}

void EVE_CMD_VIDEOSTART(void)
{
    HAL_WriteCmd(EVE_ENC_CMD_VIDEOSTART);
}

void EVE_CMD_SETBASE(uint32_t base)
{
    HAL_WriteCmd(EVE_ENC_CMD_SETBASE);
    HAL_WriteCmd(base);
}

void EVE_CMD_SETBITMAP(uint32_t source, uint16_t fmt, uint16_t w, uint16_t h)
{
    HAL_WriteCmd(EVE_ENC_CMD_SETBITMAP);
    HAL_WriteCmd(source);
    HAL_WriteCmd(((uint32_t)w << 16) | (fmt & 0xffff));
    HAL_WriteCmd(h);
}

void EVE_CMD_SETSCRATCH(uint32_t handle)
{
    HAL_WriteCmd(EVE_ENC_CMD_SETSCRATCH);
    HAL_WriteCmd(handle);
}

#endif

#if IS_EVE_API(2, 3, 4) // FT80x, BT82x API change

void EVE_CMD_SETFONT2(uint32_t font, uint32_t ptr, uint32_t firstchar)
{
    HAL_WriteCmd(EVE_ENC_CMD_SETFONT2);
    HAL_WriteCmd(font);
    HAL_WriteCmd(ptr);
    HAL_WriteCmd(firstchar);
}

void EVE_CMD_SNAPSHOT2(uint32_t fmt, uint32_t ptr, int16_t x, int16_t y, int16_t w, int16_t h)
{
    HAL_WriteCmd(EVE_ENC_CMD_SNAPSHOT2);
    HAL_WriteCmd(fmt);
    HAL_WriteCmd(ptr);
    HAL_WriteCmd(((uint32_t)y << 16) | (x & 0xffff));
    HAL_WriteCmd(((uint32_t)h << 16) | (w & 0xffff));
}

#endif

#if IS_EVE_API(3, 4)

void EVE_CMD_INFLATE2(uint32_t ptr, uint32_t options)
{
    HAL_WriteCmd(EVE_ENC_CMD_INFLATE2);
    HAL_WriteCmd(ptr);
    HAL_WriteCmd(options);
}

void EVE_CMD_CLEARCACHE()
{
    HAL_WriteCmd(EVE_ENC_CMD_CLEARCACHE);
}

void EVE_CMD_VIDEOSTARTF()
{
    HAL_WriteCmd(EVE_ENC_CMD_VIDEOSTARTF);
}

#endif

#if IS_EVE_API(3, 4, 5)

void EVE_CMD_ANIMSTART(int32_t ch, uint32_t aoptr, uint32_t loop)
{
    HAL_WriteCmd(EVE_ENC_CMD_ANIMSTART);
    HAL_WriteCmd(ch);
    HAL_WriteCmd(aoptr);
    HAL_WriteCmd(loop);
}

void EVE_CMD_ANIMSTOP(int32_t ch)
{
    HAL_WriteCmd(EVE_ENC_CMD_ANIMSTOP);
    HAL_WriteCmd(ch);
}

void EVE_CMD_ANIMXY(int32_t ch, int16_t x, int16_t y)
{
    HAL_WriteCmd(EVE_ENC_CMD_ANIMXY);
    HAL_WriteCmd(ch);
    HAL_WriteCmd(((uint32_t)y << 16) | (x & 0xFFFF));
}

void EVE_CMD_ANIMDRAW(int32_t ch)
{
    HAL_WriteCmd(EVE_ENC_CMD_ANIMDRAW);
    HAL_WriteCmd(ch);
}

void EVE_CMD_ANIMFRAME(int16_t x, int16_t y, uint32_t aoptr, uint32_t frame)
{
    HAL_WriteCmd(EVE_ENC_CMD_ANIMFRAME);
    HAL_WriteCmd(((uint32_t)y << 16) | (x & 0xFFFF));
    HAL_WriteCmd(aoptr);
    HAL_WriteCmd(frame);
}

void EVE_CMD_APPENDF(uint32_t ptr, uint32_t num)
{
    HAL_WriteCmd(EVE_ENC_CMD_APPENDF);
    HAL_WriteCmd(ptr);
    HAL_WriteCmd(num);
}

#endif

#if IS_EVE_API(4)

void EVE_CMD_ANIMFRAMERAM(int16_t x, int16_t y, uint32_t aoptr, uint32_t frame)
{
    HAL_WriteCmd(EVE_ENC_CMD_ANIMFRAMERAM);
    HAL_WriteCmd(((uint32_t)y << 16) | (x & 0xFFFF));
    HAL_WriteCmd(aoptr);
    HAL_WriteCmd(frame);
}

void EVE_CMD_ANIMSTARTRAM(int32_t ch, uint32_t aoptr, uint32_t loop)
{
    HAL_WriteCmd(EVE_ENC_CMD_ANIMSTARTRAM);
    HAL_WriteCmd(ch);
    HAL_WriteCmd(aoptr);
    HAL_WriteCmd(loop);
}

void EVE_CMD_APILEVEL(uint32_t level)
{
    //CMD_APILEVEL (0xFFFF FF63)
    HAL_WriteCmd(EVE_ENC_CMD_APILEVEL);
    HAL_WriteCmd(level);
}

void EVE_CMD_FONTCACHE(uint32_t font, int32_t ptr, uint32_t num)
{
    HAL_WriteCmd(EVE_ENC_CMD_FONTCACHE);
    HAL_WriteCmd(font);
    HAL_WriteCmd(ptr);
    HAL_WriteCmd(num);
}

void EVE_CMD_FONTCACHEQUERY(uint32_t total, int32_t used)
{
    HAL_WriteCmd(EVE_ENC_CMD_FONTCACHEQUERY);
    HAL_WriteCmd(total);
    HAL_WriteCmd(used);
}

void EVE_CMD_HSF(uint32_t w)
{
    HAL_WriteCmd(EVE_ENC_CMD_HSF);
    HAL_WriteCmd(w);
}

void EVE_CMD_PCLKFREQ(uint32_t ftarget, int32_t rounding, uint32_t factual)
{
    HAL_WriteCmd(EVE_ENC_CMD_PCLKFREQ);
    HAL_WriteCmd(ftarget);
    HAL_WriteCmd(rounding);
    HAL_WriteCmd(factual);
}

#endif

#if IS_EVE_API(4, 5)

void EVE_CMD_RUNANIM(uint32_t waitmask, uint32_t play)
{
    HAL_WriteCmd(EVE_ENC_CMD_RUNANIM);
    HAL_WriteCmd(waitmask);
    HAL_WriteCmd(play);
}

void EVE_CMD_TESTCARD(void)
{
    HAL_WriteCmd(EVE_ENC_CMD_TESTCARD);
}

void EVE_CMD_WAIT(uint32_t us)
{
    HAL_WriteCmd(EVE_ENC_CMD_WAIT);
    HAL_WriteCmd(us);
}

void EVE_CMD_NEWLIST(uint32_t a)
{
    HAL_WriteCmd(EVE_ENC_CMD_NEWLIST);
    HAL_WriteCmd(a);
}

void EVE_CMD_ENDLIST(void)
{
    HAL_WriteCmd(EVE_ENC_CMD_ENDLIST);
}

void EVE_CMD_CALLLIST(uint32_t a)
{
    HAL_WriteCmd(EVE_ENC_CMD_CALLLIST);
    HAL_WriteCmd(a);
}

void EVE_CMD_RETURN(void)
{
    HAL_WriteCmd(EVE_ENC_CMD_RETURN);
}

#endif

#if IS_EVE_API(3, 4, 5)

void EVE_CMD_NOP(void)
{
    HAL_WriteCmd(EVE_ENC_CMD_NOP);
}

void EVE_CMD_FILLWIDTH(uint32_t s)
{
    HAL_WriteCmd(EVE_ENC_CMD_FILLWIDTH);
    HAL_WriteCmd(s);
}

void EVE_CMD_ROTATEAROUND(int32_t x, int32_t y, int32_t a, int32_t s)
{
    HAL_WriteCmd(EVE_ENC_CMD_ROTATEAROUND);
    HAL_WriteCmd(x);
    HAL_WriteCmd(y);
    HAL_WriteCmd(a);
    HAL_WriteCmd(s);
}

void EVE_CMD_RESETFONTS(void)
{
    HAL_WriteCmd(EVE_ENC_CMD_RESETFONTS);
}

void EVE_CMD_GRADIENTA(int16_t x0, int16_t y0, uint32_t argb0, int16_t x1, int16_t y1, uint32_t argb1)
{
    HAL_WriteCmd(EVE_ENC_CMD_GRADIENTA);
    HAL_WriteCmd(((uint32_t)y0 << 16) | (x0 & 0xFFFF));
    HAL_WriteCmd(argb0);
    HAL_WriteCmd(((uint32_t)y1 << 16) | (x1 & 0xFFFF));
    HAL_WriteCmd(argb1);
}

void EVE_CMD_FLASHERASE(void)
{
    HAL_WriteCmd(EVE_ENC_CMD_FLASHERASE);
}

/*
 *  Write data to flash. Constraints:
 *    - Destination flash address must be virgin (not used before)
 *    - data array must be aligned 256-bit
 */
void EVE_CMD_FLASHWRITEEXT(uint32_t dest, uint32_t num, uint8_t* fdata)
{
    uint32_t i, send_data32 = 0;

    HAL_WriteCmd(EVE_ENC_CMD_FLASHWRITE);
    HAL_WriteCmd(dest);
    HAL_WriteCmd(num);
    for (i = 0; i < num; i = i + 4)
    {
        /* Pack 4 bytes into a 32-bit data each sending package */
        send_data32 = *fdata++;
        send_data32 |= (uint32_t)(*fdata++) << 8;
        send_data32 |= (uint32_t)(*fdata++) << 16;
        send_data32 |= (uint32_t)(*fdata++) << 24;
        HAL_WriteCmd(send_data32);
    }
}

void EVE_CMD_FLASHWRITE(uint32_t ptr, uint32_t num)
{
    HAL_WriteCmd(EVE_ENC_CMD_FLASHWRITE);
    HAL_WriteCmd(ptr);
    HAL_WriteCmd(num);
}

/*
 *  Writes the given data to flash.
 *  If the data matches the existing contents of flash, nothing is done.
 *  Otherwise the flash is erased in 4K units, and the data is written.
 */
void EVE_CMD_FLASHUPDATE(uint32_t dest, uint32_t src, uint32_t num)
{
    HAL_WriteCmd(EVE_ENC_CMD_FLASHUPDATE);
    HAL_WriteCmd(dest);
    HAL_WriteCmd(src);
    HAL_WriteCmd(num);
}

/*
 *  Read data from flash into main memory.
 */
void EVE_CMD_FLASHREAD(uint32_t dest, uint32_t src, uint32_t num)
{
    HAL_WriteCmd(EVE_ENC_CMD_FLASHREAD);
    HAL_WriteCmd(dest);
    HAL_WriteCmd(src);
    HAL_WriteCmd(num);
}

/*
 *  Program data to flash.
 */
void EVE_CMD_FLASHPROGRAM(uint32_t dest, uint32_t src, uint32_t num)
{
    HAL_WriteCmd(EVE_ENC_CMD_FLASHPROGRAM);
    HAL_WriteCmd(dest);
    HAL_WriteCmd(src);
    HAL_WriteCmd(num);
}

void EVE_CMD_FLASHSOURCE(uint32_t ptr)
{
    HAL_WriteCmd(EVE_ENC_CMD_FLASHSOURCE);
    HAL_WriteCmd(ptr);
}

void EVE_CMD_FLASHSPITX(uint32_t num)
{
    HAL_WriteCmd(EVE_ENC_CMD_FLASHSPITX);
    HAL_WriteCmd(num);
}

void EVE_CMD_FLASHFAST(uint32_t result)
{
    HAL_WriteCmd(EVE_ENC_CMD_FLASHFAST);
    HAL_WriteCmd(result);
}

void EVE_CMD_FLASHSPIRX(uint32_t ptr, uint32_t num)
{
    HAL_WriteCmd(EVE_ENC_CMD_FLASHSPIRX);
    HAL_WriteCmd(ptr);
    HAL_WriteCmd(num);
}

void EVE_CMD_FLASHATTACH(void)
{
    HAL_WriteCmd(EVE_ENC_CMD_FLASHATTACH);
}

void EVE_CMD_FLASHDETATCH(void)
{
    HAL_WriteCmd(EVE_ENC_CMD_FLASHDETACH);
}

void EVE_CMD_FLASHSPIDESEL(void)
{
    HAL_WriteCmd(EVE_ENC_CMD_FLASHSPIDESEL);
}

#endif

#if IS_EVE_API(4, 5)

void EVE_CMD_GETIMAGE(uint32_t source, uint32_t fmt, uint32_t w, uint32_t h, uint32_t palette)
{
    HAL_WriteCmd(EVE_ENC_CMD_GETIMAGE);
    HAL_WriteCmd(source);
    HAL_WriteCmd(fmt);
    HAL_WriteCmd(w);
    HAL_WriteCmd(h);
    HAL_WriteCmd(palette);
}

void EVE_CMD_CALIBRATESUB(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t result)
{
    HAL_WriteCmd(EVE_ENC_CMD_CALIBRATESUB);
    HAL_WriteCmd(((uint32_t)y << 16) | (x & 0xFFFF));
    HAL_WriteCmd(((uint32_t)h << 16) | (w & 0xFFFF));
    HAL_WriteCmd(result);
}

#endif

#if IS_EVE_API(5)

void EVE_CMD_COPYLIST(uint32_t dst)
{
    HAL_WriteCmd(EVE_ENC_CMD_COPYLIST);
    HAL_WriteCmd(dst);
}

void EVE_CMD_CGRADIENT(uint32_t shape, int16_t x, int16_t y, int16_t w, int16_t h, uint32_t rgb0, uint32_t rgb1)
{
    HAL_WriteCmd(EVE_ENC_CMD_CGRADIENT);
    HAL_WriteCmd(shape);
    HAL_WriteCmd(((uint32_t)y << 16) | (x & 0xFFFF));
    HAL_WriteCmd(((uint32_t)h << 16) | (w & 0xFFFF));
    HAL_WriteCmd(rgb0);
    HAL_WriteCmd(rgb1);
}

void EVE_CMD_TEXTDIM(uint32_t dimensions, int16_t font, uint16_t options, const char* string, ...)
{
    va_list args;
    uint8_t i, num = 0;

    va_start(args, string);

    num = (options & EVE_OPT_FORMAT) ? (COUNT_ARGS(string)) : (0); //Only check % characters if option OPT_FORMAT is set

    HAL_WriteCmd(EVE_ENC_CMD_TEXTDIM);
    HAL_WriteCmd(dimensions);
    HAL_WriteCmd(((uint32_t)options << 16) | (font & 0xFFFF));

    EVE_LIB_SendString(string);

    for (i = 0; i < num; i++)
    {
        HAL_WriteCmd((uint32_t)va_arg(args, uint32_t));
    }

    va_end(args);
}

void EVE_CMD_ARC(int16_t x, int16_t y, uint16_t r0, uint16_t r1, uint16_t a0, uint16_t a1)
{
    HAL_WriteCmd(EVE_ENC_CMD_ARC);
    HAL_WriteCmd(((uint32_t)y << 16) | (x & 0xFFFF));
    HAL_WriteCmd(((uint32_t)r1 << 16) | (r0 & 0xFFFF));
    HAL_WriteCmd(((uint32_t)a1 << 16) | (a0 & 0xFFFF));
}

void EVE_CMD_RENDERTARGET(uint32_t dest, uint16_t fmt, uint16_t w, uint16_t h)
{
    HAL_WriteCmd(EVE_ENC_CMD_RENDERTARGET);
    HAL_WriteCmd(dest);
    HAL_WriteCmd(fmt | ((uint32_t)w << 16));
    HAL_WriteCmd(h);
}

void EVE_CMD_ENABLEREGION(uint32_t en)
{
    HAL_WriteCmd(EVE_ENC_CMD_ENABLEREGION);
    HAL_WriteCmd(en);
}

void EVE_CMD_FENCE(void)
{
    HAL_WriteCmd(EVE_ENC_CMD_FENCE);
}

void EVE_CMD_GRAPHICSFINISH(void)
{
    HAL_WriteCmd(EVE_ENC_CMD_GRAPHICSFINISH);
}

void EVE_CMD_REGWRITE(uint32_t a, uint32_t b)
{
    HAL_WriteCmd(EVE_ENC_CMD_REGWRITE);
    HAL_WriteCmd(a);
    HAL_WriteCmd(b);
}

void EVE_CMD_APBWRITE(uint32_t a, uint32_t b)
{
    HAL_WriteCmd(EVE_ENC_CMD_APBWRITE);
    HAL_WriteCmd(a);
    HAL_WriteCmd(b);
}

void EVE_CMD_APBREAD(uint32_t a, uint32_t result)
{
    HAL_WriteCmd(EVE_ENC_CMD_APBREAD);
    HAL_WriteCmd(a);
    HAL_WriteCmd(result);
}

void EVE_CMD_LOADWAV(uint32_t dst, uint32_t options)
{
    HAL_WriteCmd(EVE_ENC_CMD_LOADWAV);
    HAL_WriteCmd(dst);
    HAL_WriteCmd(options);
}

void EVE_CMD_LOADASSET(uint32_t dst, uint32_t options)
{
    HAL_WriteCmd(EVE_ENC_CMD_LOADASSET);
    HAL_WriteCmd(dst);
    HAL_WriteCmd(options);
}

void EVE_CMD_LOADPATCH(uint32_t options)
{
    HAL_WriteCmd(EVE_ENC_CMD_LOADPATCH);
    HAL_WriteCmd(options);
}

void EVE_CMD_GLOW(int16_t x, int16_t y, int16_t w, int16_t h)
{
    HAL_WriteCmd(EVE_ENC_CMD_GLOW);
    HAL_WriteCmd(((uint32_t)y << 16) | (x & 0xFFFF));
    HAL_WriteCmd(((uint32_t)h << 16) | (w & 0xFFFF));
}

void EVE_CMD_SDATTACH(uint32_t options, uint32_t result)
{
    HAL_WriteCmd(EVE_ENC_CMD_SDATTACH);
    HAL_WriteCmd(options);
    HAL_WriteCmd(result);
}

void EVE_CMD_FSOPTIONS(uint32_t options)
{
    HAL_WriteCmd(EVE_ENC_CMD_FSOPTIONS);
    HAL_WriteCmd(options);
}

void EVE_CMD_FSREAD(uint32_t dst, const char* filename, uint32_t result)
{
    HAL_WriteCmd(EVE_ENC_CMD_FSREAD);
    HAL_WriteCmd(dst);
    EVE_LIB_SendString(filename);
    HAL_WriteCmd(result);
}

void EVE_CMD_FSSIZE(const char* filename, uint32_t size)
{
    HAL_WriteCmd(EVE_ENC_CMD_FSSIZE);
    EVE_LIB_SendString(filename);
    HAL_WriteCmd(size);
}

void EVE_CMD_FSSOURCE(const char* filename, uint32_t result)
{
    HAL_WriteCmd(EVE_ENC_CMD_FSSOURCE);
    EVE_LIB_SendString(filename);
    HAL_WriteCmd(result);
}

void EVE_CMD_FSDIR(uint32_t dst, uint32_t num, const char* path, uint32_t result)
{
    HAL_WriteCmd(EVE_ENC_CMD_FSDIR);
    HAL_WriteCmd(dst);
    HAL_WriteCmd(num);
    EVE_LIB_SendString(path);
    HAL_WriteCmd(result);
}

void EVE_CMD_SDBLOCKREAD(uint32_t dst, uint32_t src, uint32_t count, uint32_t result)
{
    HAL_WriteCmd(EVE_ENC_CMD_FSDIR);
    HAL_WriteCmd(dst);
    HAL_WriteCmd(src);
    HAL_WriteCmd(count);
    HAL_WriteCmd(result);
}

void EVE_CMD_WAITCHANGE(uint32_t a)
{
    HAL_WriteCmd(EVE_ENC_CMD_WAITCHANGE);
    HAL_WriteCmd(a);
}

void EVE_CMD_WAITCOND(uint32_t a, uint32_t func, uint32_t ref, uint32_t mask)
{
    HAL_WriteCmd(EVE_ENC_CMD_WAITCOND);
    HAL_WriteCmd(a);
    HAL_WriteCmd(func);
    HAL_WriteCmd(ref);
    HAL_WriteCmd(mask);
}

void EVE_CMD_RESULT(uint32_t a)
{
    HAL_WriteCmd(EVE_ENC_CMD_RESULT);
    HAL_WriteCmd(a);
}

void EVE_CMD_I2SSTARTUP(uint32_t freq)
{
    HAL_WriteCmd(EVE_ENC_CMD_I2SSTARTUP);
    HAL_WriteCmd(freq);
}

#endif

/* EVE API END */
