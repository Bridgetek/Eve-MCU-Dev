/**
 @file EVE_API.c
 */
/*
 * ============================================================================
 * History
 * =======
 * Nov 2019		Initial beta for FT81x and FT80x
 * Mar 2020		Updated beta - added BT815/6 commands
 * Mar 2021		Beta with BT817/8 support added
 * Dec 2024		Add BT82x support
 *
 *
 *
 *
 * (C) Copyright,  Bridgetek Pte. Ltd.
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

#include <string.h>
#include <stdint.h> // for Uint8/16/32 and Int8/16/32 data types
#include <stdarg.h>

#include "../include/EVE.h"
#include "../include/HAL.h"

// Set beginning of graphics command memory
//static uint32_t RAMCommandBuffer = EVE_RAM_CMD;

//##############################################################################
// Library functions
//##############################################################################

void EVE_Init(void)
{
    int i;

    HAL_EVE_Init();

    // ------------------------- Display settings ------------------------------

    // LCD display parameters
#if IS_EVE_API(1, 2, 3, 4)
    uint8_t regGpio;

    // Active width of LCD display
    HAL_MemWrite16(EVE_REG_HSIZE,   EVE_DISP_WIDTH);
    // Total number of clocks per line
    HAL_MemWrite16(EVE_REG_HCYCLE,  EVE_DISP_HCYCLE);
    // Start of active line
    HAL_MemWrite16(EVE_REG_HOFFSET, EVE_DISP_HOFFSET);
    // Start of horizontal sync pulse
    HAL_MemWrite16(EVE_REG_HSYNC0,  EVE_DISP_HSYNC0);
    // End of horizontal sync pulse
    HAL_MemWrite16(EVE_REG_HSYNC1,  EVE_DISP_HSYNC1);
    // Active height of LCD display
    HAL_MemWrite16(EVE_REG_VSIZE,   EVE_DISP_HEIGHT);
    // Total number of lines per screen
    HAL_MemWrite16(EVE_REG_VCYCLE,  EVE_DISP_VCYCLE);
    // Start of active screen
    HAL_MemWrite16(EVE_REG_VOFFSET, EVE_DISP_VOFFSET);
    // Start of vertical sync pulse
    HAL_MemWrite16(EVE_REG_VSYNC0,  EVE_DISP_VSYNC0);
    // End of vertical sync pulse
    HAL_MemWrite16(EVE_REG_VSYNC1,  EVE_DISP_VSYNC1);
    // Define active edge of PCLK
    HAL_MemWrite8(EVE_REG_PCLK_POL, EVE_DISP_PCLKPOL);
    // Define RGB output pins
    HAL_MemWrite8(EVE_REG_SWIZZLE,  EVE_DISP_SWIZZLE);
    // Turn on or off CSpread
    HAL_MemWrite8(EVE_REG_CSPREAD,  EVE_DISP_CSPREAD);
    // Turn on or off Dither
    HAL_MemWrite8(EVE_REG_DITHER,  EVE_DISP_DITHER);

    // Write first display list
    HAL_MemWrite32((EVE_RAM_DL + 0), EVE_ENC_CLEAR_COLOR_RGB(0,0,0));
    HAL_MemWrite32((EVE_RAM_DL + 4), EVE_ENC_CLEAR(1,1,1));
    HAL_MemWrite32((EVE_RAM_DL + 8), EVE_ENC_DISPLAY());
    HAL_MemWrite8(EVE_REG_DLSWAP, EVE_DLSWAP_FRAME);

    // Read the  GPIO register for a read/modify/write operation
    regGpio = HAL_MemRead8(EVE_REG_GPIO);
    // set bit 7 of  GPIO register (DISP) - others are inputs
    regGpio = regGpio | 0x80;
    // Enable the DISP signal to the LCD panel
    HAL_MemWrite8(EVE_REG_GPIO, regGpio);

    // Write the PCLK or PCLK_FREQ register
    // If setting PCLK_FREQ then also set REG_PCLK to 1 to enable extsync mode
    #if IS_EVE_API(4) && (defined SET_PCLK_FREQ)
    HAL_MemWrite16(EVE_REG_PCLK_FREQ,  EVE_DISP_PCLK_FREQ);
    HAL_MemWrite8(EVE_REG_PCLK, 1);
    #else
    // Now start clocking data to the LCD panel
    HAL_MemWrite8(EVE_REG_PCLK, EVE_DISP_PCLK);
    #endif

    HAL_MemWrite8(EVE_REG_PWM_DUTY, 127);

    // ---------------------- Touch and Audio settings -------------------------
    // Eliminate any false touches
    HAL_MemWrite16(EVE_REG_TOUCH_RZTHRESH, 1200);

    // turn recorded audio volume down
    HAL_MemWrite8(EVE_REG_VOL_PB, EVE_VOL_ZERO);

    // turn synthesizer volume down
    HAL_MemWrite8(EVE_REG_VOL_SOUND, EVE_VOL_ZERO);
    // set synthesizer to mute
    HAL_MemWrite16(EVE_REG_SOUND, 0x6000);

#elif IS_EVE_API(5) 

    EVE_LIB_BeginCoProList();
    EVE_CMD_REGWRITE(EVE_REG_CPURESET, 2);
    EVE_CMD_REGWRITE(EVE_REG_TOUCH_CONFIG, EVE_TOUCH_CONFIG);
    EVE_CMD_REGWRITE(EVE_REG_CPURESET, 0);
    EVE_LIB_EndCoProList();
    EVE_LIB_AwaitCoProEmpty();

    EVE_LIB_BeginCoProList();
    EVE_CMD_REGWRITE(EVE_REG_SC0_SIZE, 2);
    EVE_CMD_REGWRITE(EVE_REG_SC0_PTR0, 10 << 20);
    EVE_CMD_REGWRITE(EVE_REG_SC0_PTR1, 18 << 20);
    EVE_LIB_EndCoProList();
    EVE_LIB_AwaitCoProEmpty();

    EVE_LIB_BeginCoProList();
    EVE_CMD_RENDERTARGET(EVE_SWAPCHAIN_0, EVE_FORMAT_RGB6, EVE_DISP_WIDTH, EVE_DISP_HEIGHT);
    EVE_CLEAR(1,1,1);
    EVE_CMD_SWAP();
    EVE_CMD_GRAPHICSFINISH();
    EVE_LIB_EndCoProList();
    EVE_LIB_AwaitCoProEmpty();

    EVE_LIB_BeginCoProList();
    EVE_CMD_REGWRITE(EVE_REG_GPIO, 0x80);
    EVE_CMD_REGWRITE(EVE_REG_DISP, 1);

    // Total number of clocks per line
    EVE_CMD_REGWRITE(EVE_REG_HCYCLE, EVE_DISP_HCYCLE);
    // Active width of LCD display
    EVE_CMD_REGWRITE(EVE_REG_HSIZE, EVE_DISP_WIDTH);
    // Start of active line
    EVE_CMD_REGWRITE(EVE_REG_HOFFSET, EVE_DISP_HOFFSET);
    // Start of horizontal sync pulse
    EVE_CMD_REGWRITE(EVE_REG_HSYNC0, EVE_DISP_HSYNC0);
    // End of horizontal sync pulse
    EVE_CMD_REGWRITE(EVE_REG_HSYNC1, EVE_DISP_HSYNC1);

    // Total number of lines per screen
    EVE_CMD_REGWRITE(EVE_REG_VCYCLE, EVE_DISP_VCYCLE);
    // Active height of LCD display
    EVE_CMD_REGWRITE(EVE_REG_VSIZE, EVE_DISP_HEIGHT);
    // Start of active screen
    EVE_CMD_REGWRITE(EVE_REG_VOFFSET, 10);
    // Start of vertical sync pulse
    EVE_CMD_REGWRITE(EVE_REG_VSYNC0, 0);
    // End of vertical sync pulse
    EVE_CMD_REGWRITE(EVE_REG_VSYNC1, 3);

    // Define active edge of PCLK
    EVE_CMD_REGWRITE(EVE_REG_PCLK_POL, 0);

    // 0: 1 pixel single // 1: 2 pixel single // 2: 2 pixel dual // 3: 4 pixel dual
    unsigned long extsyncmode = 3;
    unsigned long TXPLLDiv = 0x03;
    unsigned long valcfg = 0;
    if (TXPLLDiv > 4) valcfg = 0x00300870 + TXPLLDiv;
    else valcfg = 0x00301070 + TXPLLDiv;

    EVE_CMD_APBWRITE(EVE_REG_LVDSPLL_CFG, valcfg);
    EVE_CMD_APBWRITE(EVE_REG_LVDS_EN, 7); // Enable PLL

    EVE_CMD_REGWRITE(EVE_REG_SO_MODE, extsyncmode);
    EVE_CMD_REGWRITE(EVE_REG_SO_SOURCE, EVE_SWAPCHAIN_0);
    EVE_CMD_REGWRITE(EVE_REG_SO_FORMAT, EVE_FORMAT_RGB6);
    EVE_CMD_REGWRITE(EVE_REG_SO_EN, 1);
    
    //EVE_CMD_REGWRITE(EVE_REG_RE_DITHER, EVE_DISP_DITHER);
    EVE_LIB_EndCoProList();
    EVE_LIB_AwaitCoProEmpty();

#endif

    // --------------------- Clear screen ready to start -----------------------
    EVE_LIB_BeginCoProList();
    EVE_CMD_DLSTART();
    EVE_CLEAR_COLOR_RGB(0, 0, 0);
    EVE_CLEAR(1,1,1);
    EVE_DISPLAY();
    EVE_CMD_SWAP();
    EVE_LIB_EndCoProList();
    EVE_LIB_AwaitCoProEmpty();

#if IS_EVE_API(1)

       // ---------------------- Reset all bitmap properties ------------------------
    EVE_LIB_BeginCoProList();
    EVE_CMD_DLSTART();
    EVE_CLEAR_COLOR_RGB(0, 0, 0);
    EVE_CLEAR(1,1,1);
    for (i = 0; i < 16; i++)
    {
        EVE_BITMAP_HANDLE(i);
        //EVE_CMD_SETBITMAP(0,0,0,0);
        EVE_BITMAP_LAYOUT(0, 0, 0);
        EVE_BITMAP_SIZE(0, 0, 0, 0, 0);
    }
    EVE_DISPLAY();
    EVE_CMD_SWAP();
    EVE_LIB_EndCoProList();
    EVE_LIB_AwaitCoProEmpty();

#elif IS_EVE_API(2, 3, 4, 5)

    // ---------------------- Reset all bitmap properties ------------------------
    EVE_LIB_BeginCoProList();
    EVE_CMD_DLSTART();
    EVE_CLEAR_COLOR_RGB(0, 0, 0);
    EVE_CLEAR(1,1,1);
    for (i = 0; i < 16; i++)
    {
        EVE_BITMAP_HANDLE(i);
        EVE_CMD_SETBITMAP(0,0,0,0);
    }
    EVE_DISPLAY();
    EVE_CMD_SWAP();
    EVE_LIB_EndCoProList();
    EVE_LIB_AwaitCoProEmpty();

#endif
}

// Begins co-pro list for display creation
void EVE_LIB_BeginCoProList(void)
{
    // Wait for command FIFO to be empty and record current position in FIFO
    EVE_LIB_AwaitCoProEmpty();

    // Begins SPI transaction
    HAL_ChipSelect(1);

#if IS_EVE_API(1)
    // Send address for writing as the next free location in the co-pro buffer
    HAL_SetWriteAddress(EVE_RAM_CMD + HAL_GetCmdPointer());
#else
    // Send address for writing
    HAL_SetWriteAddress(EVE_REG_CMDB_WRITE);
#endif
}

// Ends co-pro list for display creation
void EVE_LIB_EndCoProList(void)
{
    // End SPI transaction
    HAL_ChipSelect(0);
    // Update the ring buffer pointer to start decode
#ifndef EVE_USE_CMDB_METHOD
    HAL_WriteCmdPointer();
#endif
}

// Waits for the read and write pointers to become equal
int EVE_LIB_AwaitCoProEmpty(void)
{
    // Await completion of processing
    return HAL_WaitCmdFifoEmpty();
}

// Gets a result from the command buffer 
uint32_t EVE_LIB_GetResult(int offset)
{
    uint32_t CmdBufPointer = (HAL_GetCmdPointer() - (offset * sizeof(uint32_t))) & (EVE_RAM_CMD_SIZE - 1);
    return HAL_MemRead32(EVE_RAM_CMD + CmdBufPointer);
}

#if IS_EVE_API(5)
// Obtain the coprocessor exception description
void EVE_LIB_GetCoProException(char* desc)
{
    uint32_t report = EVE_COPROC_REPORT;
    for (uint32_t j = 0; j < 256; j += 4)
    {
        uint32_t w = HAL_MemRead32(report + j);
        for (int i = 0; i < 4; i++)
        {
            char c = (w >> (i * 8)) & 0x7f;
            *desc++ = c;
            if (c == '\0') break;
        }
    }
}
#endif

// Writes a block of data to the RAM_G
void EVE_LIB_WriteDataToRAMG(const uint8_t *ImgData, uint32_t DataSize, uint32_t DestAddress)
{
    // Begins SPI transaction
    HAL_ChipSelect(1);
    // Send address to which first value will be written
    HAL_SetWriteAddress(DestAddress);

    // Pad data length to multiple of 4.
    DataSize = (DataSize + 3) & (~3);

    // Send data as 32 bits.
    while (DataSize)
    {
        HAL_Write32(*(uint32_t *)ImgData);
        ImgData += 4;
        DataSize -= 4;
    }

    // End SPI transaction
    HAL_ChipSelect(0);
}

// Reads a block of data from the RAM_G
void EVE_LIB_ReadDataFromRAMG(uint8_t *ImgData, uint32_t DataSize, uint32_t SrcAddress)
{
    // Begins SPI transaction
    HAL_ChipSelect(1);
    // Send address to which first value will be written
    HAL_SetReadAddress(SrcAddress);

    // Pad data length to multiple of 4.
    DataSize = (DataSize + 3) & (~3);

    // Send data as 32 bits.
    while (DataSize)
    {
        *(uint32_t *)ImgData = HAL_Read32();
        ImgData += 4;
        DataSize -= 4;
    }

    // End SPI transaction
    HAL_ChipSelect(0);
}

// Write a block of data to the coprocessor
void EVE_LIB_WriteDataToCMD(const uint8_t *ImgData, uint32_t DataSize)
{
    uint32_t CurrentIndex = 0;
    uint32_t ChunkSize = 0;
    const uint32_t MaxChunkSize = 128;
    uint8_t IsLastChunk = 0;
    uint16_t Freespace = 0;

    HAL_ChipSelect(0);

    // This code works by sending the data in a series of one or more bursts.
    // If the data is more than MaxChunkSize bytes, it is sent as a series of
    // one or more bursts and then the remainder. MaxChunkSize is a size which
    // is smaller than the command buffer on the EVE and small enough to gain
    // maximum buffering effect from the MCU SPI hardware.

    // Pad data length to multiple of 4.
    DataSize = (DataSize + 3) & (~3);

    // While not all data is sent
    while (CurrentIndex < DataSize)
    {
        // If more than ChunkSize bytes to send
        if ((DataSize - CurrentIndex) > MaxChunkSize)
        {
            // ... then add ChunkSize to the current target index to make new target
            ChunkSize = MaxChunkSize;
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

        // Wait until there is space
        Freespace = 0;
        while (Freespace < MaxChunkSize)
        {
            Freespace = HAL_CheckCmdFreeSpace();
        }

        // Begin an SPI burst write
        HAL_ChipSelect(1);

        // to the next location in the FIFO
#ifndef EVE_USE_CMDB_METHOD
        HAL_SetWriteAddress(EVE_RAM_CMD + HAL_GetCmdPointer());
#else
        HAL_SetWriteAddress(EVE_REG_CMDB_WRITE);
#endif
        HAL_Write(ImgData, ChunkSize);
        ImgData += ChunkSize;
        CurrentIndex += ChunkSize;

        // End the SPI burst
        HAL_ChipSelect(0);

        // Calculate where end of data lies
        HAL_IncCmdPointer(ChunkSize);
#ifndef EVE_USE_CMDB_METHOD
        HAL_WriteCmdPointer();
#endif

        // If this is the last chunk of the data,
        if (IsLastChunk)
        {
            break;
        }
    }

    HAL_ChipSelect(1);
}

// Writes a string over SPI
uint16_t EVE_LIB_SendString(const char* string)
{
    uint16_t length;
    uint16_t CommandSize;

    // Include the terminating null character in the string length.
    // Pad string length to a multiple of 4.
    length = ((strlen(string) + 1) + 3) & (~3);
    // Store command length to return.
    CommandSize = length;

#if MCU_UNALIGNED_ACCESSES 
    // Send string as 32 bit data.
    while (length)
    {
        HAL_Write32(*(uint32_t *)string);
        string += 4;
        length -= 4;
    }
#else
		uint32_t val32;
		while (length)
		{
				val32 = *string++;
				val32 |= (*string++ << 8);
				val32 |= (*string++ << 16);
				val32 |= (*string++ << 24);
        HAL_Write32(val32);
        length -= 4;
		}
#endif
	
    return CommandSize;
}

void EVE_LIB_GetProps(uint32_t *addr, uint32_t *width, uint32_t *height)
{
    uint32_t WritePointer = HAL_GetCmdPointer();
    EVE_LIB_BeginCoProList();
    // To read the result from CMD_GETPROPS we need to be clever and find out
    // where the CoProcessor is writing the command. We can then retrieve the
    // results from the place where they were written.
    // Send the command to the CoProcessor.
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

void EVE_LIB_GetPtr(uint32_t *addr)
{
    uint32_t WritePointer = HAL_GetCmdPointer();
    EVE_LIB_BeginCoProList();
    // Send the command to the CoProcessor.
    EVE_CMD_GETPTR(0);
    // Wait for it to finish.
    EVE_LIB_EndCoProList();
    EVE_LIB_AwaitCoProEmpty();
    // Obtain the results from the EVE_RAM_CMD in the CoProcessor.
    *addr = EVE_LIB_GetResult(1);
}

void EVE_LIB_GetMatrix(uint32_t *a, uint32_t *b, uint32_t *c, uint32_t *d, uint32_t *e, uint32_t *f)
{
    uint32_t WritePointer = HAL_GetCmdPointer();
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

void EVE_LIB_MemCrc(uint32_t ptr, uint32_t num, uint32_t *result)
{
    uint32_t WritePointer = HAL_GetCmdPointer();
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
void EVE_LIB_BitmapTransform( int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, 
                                int32_t tx0, int32_t ty0, int32_t tx1, int32_t ty1, int32_t tx2, int32_t ty2,
                                uint32_t *result )
{
    uint32_t WritePointer = HAL_GetCmdPointer();
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
void EVE_LIB_GetImage(uint32_t *addr, uint32_t *fmt, uint32_t *width, uint32_t *height, uint32_t *palette)
{
    uint32_t WritePointer = HAL_GetCmdPointer();
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
void EVE_LIB_RegRead(uint32_t addr, uint32_t *value)
{
    uint32_t WritePointer = HAL_GetCmdPointer();
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

//##############################################################################
// Display List commands for co-processor
//##############################################################################

void EVE_CMD(uint32_t c)
{
    HAL_Write32(c);
    HAL_IncCmdPointer(4);
}

void EVE_CLEAR_COLOR_RGB(uint8_t R, uint8_t G, uint8_t B)
{
    HAL_Write32(EVE_ENC_CLEAR_COLOR_RGB(R, G, B));
    HAL_IncCmdPointer(4);
}

void EVE_CLEAR_COLOR(uint32_t c)
{
    HAL_Write32(EVE_ENC_CLEAR_COLOR(c));
    HAL_IncCmdPointer(4);
}

void EVE_CLEAR(uint8_t C, uint8_t S, uint8_t T)
{
    HAL_Write32(EVE_ENC_CLEAR((C & 0x01),(S & 0x01),(T & 0x01)));
    HAL_IncCmdPointer(4);
}

void EVE_COLOR_RGB(uint8_t R, uint8_t G, uint8_t B)
{
    HAL_Write32(EVE_ENC_COLOR_RGB(R, G, B));
    HAL_IncCmdPointer(4);
}

void EVE_COLOR(uint32_t c)
{
    HAL_Write32(EVE_ENC_COLOR(c));
    HAL_IncCmdPointer(4);
}

void EVE_VERTEX2F(int16_t x, int16_t y)
{
    HAL_Write32(EVE_ENC_VERTEX2F(x, y));
    HAL_IncCmdPointer(4);
}

void EVE_VERTEX2II(uint16_t x, uint16_t y, uint8_t handle, uint8_t cell)
{
    HAL_Write32(EVE_ENC_VERTEX2II(x, y, handle, cell));
    HAL_IncCmdPointer(4);
}

void EVE_BITMAP_HANDLE(uint8_t handle)
{
    HAL_Write32(EVE_ENC_BITMAP_HANDLE(handle));
    HAL_IncCmdPointer(4);
}

void EVE_BITMAP_SOURCE(int32_t addr)
{
    HAL_Write32(EVE_ENC_BITMAP_SOURCE((int32_t)addr));
    HAL_IncCmdPointer(4);
}

#if IS_EVE_API(3, 4)
void EVE_BITMAP_SOURCE2(uint8_t flash_or_ram, int32_t addr)
{
    HAL_Write32(EVE_ENC_BITMAP_SOURCE2(flash_or_ram, (int32_t)addr));
    HAL_IncCmdPointer(4);
}
#endif

void EVE_BITMAP_LAYOUT(uint8_t format, uint16_t linestride, uint16_t height )
{
    HAL_Write32(EVE_ENC_BITMAP_LAYOUT(format, linestride, height));
    HAL_IncCmdPointer(4);
}

void EVE_BITMAP_SIZE(uint8_t filter, uint8_t wrapx, uint8_t wrapy, uint16_t width, uint16_t height)
{
    HAL_Write32(EVE_ENC_BITMAP_SIZE(filter, wrapx, wrapy, width, height));
    HAL_IncCmdPointer(4);
}

void EVE_CELL(uint8_t cell)
{
    HAL_Write32(EVE_ENC_CELL(cell));
    HAL_IncCmdPointer(4);
}

void EVE_TAG(uint8_t s)
{
    HAL_Write32(EVE_ENC_TAG(s));
    HAL_IncCmdPointer(4);
}

void EVE_ALPHA_FUNC(uint8_t func, uint8_t ref)
{
    HAL_Write32(EVE_ENC_ALPHA_FUNC(func, ref));
    HAL_IncCmdPointer(4);
}

void EVE_STENCIL_FUNC(uint8_t func, uint8_t ref, uint8_t mask)
{
    HAL_Write32(EVE_ENC_STENCIL_FUNC(func, ref, mask));
    HAL_IncCmdPointer(4);
}

void EVE_BLEND_FUNC(uint8_t src, uint8_t dst)
{
    HAL_Write32(EVE_ENC_BLEND_FUNC(src, dst));
    HAL_IncCmdPointer(4);
}

void EVE_STENCIL_OP(uint8_t sfail, uint8_t spass)
{
    HAL_Write32(EVE_ENC_STENCIL_OP(sfail, spass));
    HAL_IncCmdPointer(4);
}

void EVE_POINT_SIZE(uint16_t size)
{
    HAL_Write32(EVE_ENC_POINT_SIZE(size));
    HAL_IncCmdPointer(4);
}

void EVE_LINE_WIDTH(uint16_t width)
{
    HAL_Write32(EVE_ENC_LINE_WIDTH(width));
    HAL_IncCmdPointer(4);
}

void EVE_CLEAR_COLOR_A(uint8_t alpha)
{
    HAL_Write32(EVE_ENC_CLEAR_COLOR_A(alpha));
    HAL_IncCmdPointer(4);
}

void EVE_COLOR_A(uint8_t alpha)
{
    HAL_Write32(EVE_ENC_COLOR_A(alpha));
    HAL_IncCmdPointer(4);
}

void EVE_CLEAR_STENCIL(uint8_t s)
{
    HAL_Write32(EVE_ENC_CLEAR_STENCIL(s));
    HAL_IncCmdPointer(4);
}

void EVE_CLEAR_TAG(uint8_t s)
{
    HAL_Write32(EVE_ENC_CLEAR_TAG(s));
    HAL_IncCmdPointer(4);
}

void EVE_STENCIL_MASK(uint8_t mask)
{
    HAL_Write32(EVE_ENC_STENCIL_MASK(mask));
    HAL_IncCmdPointer(4);
}

void EVE_TAG_MASK(uint8_t mask)
{
    HAL_Write32(EVE_ENC_TAG_MASK(mask));
    HAL_IncCmdPointer(4);
}

void EVE_SCISSOR_XY(uint16_t x, uint16_t y)
{
    HAL_Write32(EVE_ENC_SCISSOR_XY(x, y));
    HAL_IncCmdPointer(4);
}

void EVE_SCISSOR_SIZE(uint16_t width, uint16_t height)
{
    HAL_Write32(EVE_ENC_SCISSOR_SIZE(width, height));
    HAL_IncCmdPointer(4);
}

void EVE_CALL(uint16_t dest)
{
    HAL_Write32(EVE_ENC_CALL(dest));
    HAL_IncCmdPointer(4);
}

void EVE_JUMP(uint16_t dest)
{
    HAL_Write32(EVE_ENC_JUMP(dest));
    HAL_IncCmdPointer(4);
}

void EVE_BEGIN(uint8_t prim)
{
    HAL_Write32(EVE_ENC_BEGIN(prim));
    HAL_IncCmdPointer(4);
}

void EVE_COLOR_MASK(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    HAL_Write32(EVE_ENC_COLOR_MASK(r, g, b, a));
    HAL_IncCmdPointer(4);
}

void EVE_END(void)
{
    HAL_Write32(EVE_ENC_END());
    HAL_IncCmdPointer(4);
}

void EVE_SAVE_CONTEXT(void)
{
    HAL_Write32(EVE_ENC_SAVE_CONTEXT());
    HAL_IncCmdPointer(4);
}

void EVE_RESTORE_CONTEXT(void)
{
    HAL_Write32(EVE_ENC_RESTORE_CONTEXT());
    HAL_IncCmdPointer(4);
}

void EVE_RETURN(void)
{
    HAL_Write32(EVE_ENC_RETURN());
    HAL_IncCmdPointer(4);
}

void EVE_MACRO(uint8_t m)
{
    HAL_Write32(EVE_ENC_MACRO(m));
    HAL_IncCmdPointer(4);
}

void EVE_DISPLAY(void)
{
    HAL_Write32(EVE_ENC_DISPLAY());
    HAL_IncCmdPointer(4);
}

void EVE_BITMAP_TRANSFORM_A(long a)
{
    HAL_Write32(EVE_ENC_BITMAP_TRANSFORM_A(a)); //	((21UL << 24) | (((a)&131071UL)<<0))
    HAL_IncCmdPointer(4);
}

void EVE_BITMAP_TRANSFORM_B(long b)
{
    HAL_Write32(EVE_ENC_BITMAP_TRANSFORM_B(b)); //  ((22UL << 24) | (((b)&131071UL)<<0))
    HAL_IncCmdPointer(4);
}

void EVE_BITMAP_TRANSFORM_C(long c)
{
    HAL_Write32(EVE_ENC_BITMAP_TRANSFORM_C(c)); //  ((23UL << 24) | (((c)&16777215UL)<<0))
    HAL_IncCmdPointer(4);
}

void EVE_BITMAP_TRANSFORM_D(long d)
{
    HAL_Write32(EVE_ENC_BITMAP_TRANSFORM_D(d)); //   ((24UL << 24) | (((d)&131071UL)<<0))
    HAL_IncCmdPointer(4);
}

void EVE_BITMAP_TRANSFORM_E(long e)
{
    HAL_Write32(EVE_ENC_BITMAP_TRANSFORM_E(e)); //   ((25UL << 24) | (((e)&131071UL)<<0))
    HAL_IncCmdPointer(4);
}

void EVE_BITMAP_TRANSFORM_F(long f)
{
    HAL_Write32(EVE_ENC_BITMAP_TRANSFORM_F(f)); //  ((26UL << 24) | (((f)&16777215UL)<<0))
    HAL_IncCmdPointer(4);
}

#if IS_EVE_API(2, 3, 4, 5) // FT81x API change

void EVE_VERTEX_FORMAT(uint8_t frac)
{
    HAL_Write32(EVE_ENC_VERTEX_FORMAT(frac));
    HAL_IncCmdPointer(4);
}

void EVE_BITMAP_LAYOUT_H(uint8_t linestride, uint8_t height)
{
    HAL_Write32(EVE_ENC_BITMAP_LAYOUT_H(linestride, height));
    HAL_IncCmdPointer(4);
}

void EVE_BITMAP_SIZE_H(uint8_t width, uint8_t height)
{
    HAL_Write32(EVE_ENC_BITMAP_SIZE_H(width, height));
    HAL_IncCmdPointer(4);
}

void EVE_PALETTE_SOURCE(uint32_t addr)
{
    HAL_Write32(EVE_ENC_PALETTE_SOURCE(addr));
    HAL_IncCmdPointer(4);
}

void EVE_VERTEX_TRANSLATE_X(uint32_t x)
{
    HAL_Write32(EVE_ENC_VERTEX_TRANSLATE_X(x));
    HAL_IncCmdPointer(4);
}

void EVE_VERTEX_TRANSLATE_Y(uint32_t y)
{
    HAL_Write32(EVE_ENC_VERTEX_TRANSLATE_Y(y));
    HAL_IncCmdPointer(4);
}

void EVE_NOP(void)
{
    HAL_Write32(EVE_ENC_NOP());
    HAL_IncCmdPointer(4);
}

#endif

#if IS_EVE_API(5) // BT82x extensions

void EVE_BITMAP_SOURCE_H(uint8_t addr)
{
    HAL_Write32(EVE_ENC_BITMAP_SOURCE_H(addr));
    HAL_IncCmdPointer(4);
}

void EVE_BITMAP_ZORDER(uint8_t o)
{
    HAL_Write32(EVE_ENC_BITMAP_ZORDER(o));
    HAL_IncCmdPointer(4);
}

void EVE_PALLETE_SOURCE_H(uint8_t addr)
{
    HAL_Write32(EVE_ENC_PALLETE_SOURCE_H(addr));
    HAL_IncCmdPointer(4);
}

void EVE_REGION(uint8_t y, uint8_t h, uint16_t dest)
{
    HAL_Write32(EVE_ENC_REGION(y, h, dest));
    HAL_IncCmdPointer(4);
}

#endif

//##############################################################################
// Co-Processor Widgets
//##############################################################################

void EVE_CMD_KEYS(int16_t x, int16_t y, int16_t w, int16_t h, int16_t font, uint16_t options, const char* string)
{
    uint32_t CommandSize;
    uint32_t StringLength;

    HAL_Write32(EVE_ENC_CMD_KEYS);
    HAL_Write32(((uint32_t)y << 16) | (x & 0xffff));
    HAL_Write32(((uint32_t)h << 16) | (w & 0xffff));
    HAL_Write32(((uint32_t)options << 16) | (font & 0xffff));
    CommandSize = 16;

    StringLength = EVE_LIB_SendString(string);
    CommandSize = CommandSize + StringLength;

    HAL_IncCmdPointer(CommandSize);
}

void EVE_CMD_NUMBER(int16_t x, int16_t y, int16_t font, uint16_t options, int32_t n)
{
    HAL_Write32(EVE_ENC_CMD_NUMBER);
    HAL_Write32(((uint32_t)y << 16) | (x & 0xffff));
    HAL_Write32(((uint32_t)options << 16) | (font & 0xffff));
    HAL_Write32(n);
    HAL_IncCmdPointer(16);
}

void EVE_CMD_LOADIDENTITY(void)
{
    HAL_Write32(EVE_ENC_CMD_LOADIDENTITY);
    HAL_IncCmdPointer(4);
}

/* Error handling for val is not done, so better to always use range of 65535 in order that needle is drawn within display region */
void EVE_CMD_GAUGE(int16_t x, int16_t y, int16_t r, uint16_t options, uint16_t major, uint16_t minor, uint16_t val, uint16_t range)
{
    HAL_Write32(EVE_ENC_CMD_GAUGE);
    HAL_Write32(((uint32_t)y << 16) | (x & 0xffff));
    HAL_Write32(((uint32_t)options << 16) | (r & 0xffff));
    HAL_Write32(((uint32_t)minor << 16) | (major & 0xffff));
    HAL_Write32(((uint32_t)range << 16) | (val & 0xffff));
    HAL_IncCmdPointer(20);
}

void EVE_CMD_REGREAD(uint32_t ptr, uint32_t result)
{
    HAL_Write32(EVE_ENC_CMD_REGREAD);
    HAL_Write32(ptr);
    HAL_Write32(result);
    HAL_IncCmdPointer(12);
}

void EVE_CMD_GETPROPS(uint32_t ptr, uint32_t w, uint32_t h)
{
    HAL_Write32(EVE_ENC_CMD_GETPROPS);
    HAL_Write32(ptr);
    HAL_Write32(w);
    HAL_Write32(h);
    HAL_IncCmdPointer(16);
}

void EVE_CMD_MEMCPY(uint32_t dest, uint32_t src, uint32_t num)
{
    HAL_Write32(EVE_ENC_CMD_MEMCPY);
    HAL_Write32(dest);
    HAL_Write32(src);
    HAL_Write32(num);
    HAL_IncCmdPointer(16);
}

void EVE_CMD_SPINNER(int16_t x, int16_t y, uint16_t style, uint16_t scale)
{
    HAL_Write32(EVE_ENC_CMD_SPINNER);
    HAL_Write32(((uint32_t)y << 16) | (x & 0xffff));
    HAL_Write32(((uint32_t)scale << 16) | (style & 0xffff));
    HAL_IncCmdPointer(12);
}

void EVE_CMD_BGCOLOR(uint32_t c)
{
    HAL_Write32(EVE_ENC_CMD_BGCOLOR);
    HAL_Write32(c);
    HAL_IncCmdPointer(8);
}

void EVE_CMD_SWAP(void)
{
    HAL_Write32(EVE_ENC_CMD_SWAP);
    HAL_IncCmdPointer(4);
}

void EVE_CMD_INFLATE(uint32_t ptr)
{
    HAL_Write32(EVE_ENC_CMD_INFLATE);
    HAL_Write32(ptr);
    HAL_IncCmdPointer(8);
}

void EVE_CMD_TRANSLATE(int32_t tx, int32_t ty)
{
    HAL_Write32(EVE_ENC_CMD_TRANSLATE);
    HAL_Write32(tx);
    HAL_Write32(ty);
    HAL_IncCmdPointer(12);
}

void EVE_CMD_STOP(void)
{
    HAL_Write32(EVE_ENC_CMD_STOP);
    HAL_IncCmdPointer(4);
}

void EVE_CMD_SLIDER(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t options, uint16_t val, uint16_t range)
{
    HAL_Write32(EVE_ENC_CMD_SLIDER);
    HAL_Write32(((uint32_t)y << 16) | (x & 0xffff));
    HAL_Write32(((uint32_t)h << 16) | (w & 0xffff));
    HAL_Write32(((uint32_t)val << 16) | (options & 0xffff));
    HAL_Write32(range);
    HAL_IncCmdPointer(20);
}

void EVE_CMD_INTERRUPT(uint32_t ms)
{
    HAL_Write32(EVE_ENC_CMD_INTERRUPT);
    HAL_Write32(ms);
    HAL_IncCmdPointer(8);
}

void EVE_CMD_FGCOLOR(uint32_t c)
{
    HAL_Write32(EVE_ENC_CMD_FGCOLOR);
    HAL_Write32(c);
    HAL_IncCmdPointer(8);
}

void EVE_CMD_ROTATE(int32_t a)
{
    HAL_Write32(EVE_ENC_CMD_ROTATE);
    HAL_Write32(a);
    HAL_IncCmdPointer(8);
}

void EVE_CMD_MEMWRITE(uint32_t ptr, uint32_t num)
{
    HAL_Write32(EVE_ENC_CMD_MEMWRITE);
    HAL_Write32(ptr);
    HAL_Write32(num);
    HAL_IncCmdPointer(12);
}

void EVE_CMD_SCROLLBAR(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t options, uint16_t val, uint16_t size, uint16_t range)
{
    HAL_Write32(EVE_ENC_CMD_SCROLLBAR);
    HAL_Write32(((uint32_t)y << 16) | (x & 0xffff));
    HAL_Write32(((uint32_t)h << 16) | (w & 0xffff));
    HAL_Write32(((uint32_t)val << 16) | (options & 0xffff));
    HAL_Write32(((uint32_t)range << 16) | (size & 0xffff));
    HAL_IncCmdPointer(20);
}

void EVE_CMD_GETMATRIX(int32_t a, int32_t b, int32_t c, int32_t d, int32_t e, int32_t f)
{
    HAL_Write32(EVE_ENC_CMD_GETMATRIX);
    HAL_Write32(a);
    HAL_Write32(b);
    HAL_Write32(c);
    HAL_Write32(d);
    HAL_Write32(e);
    HAL_Write32(f);
    HAL_IncCmdPointer(28);
}

void EVE_CMD_SKETCH(int16_t x, int16_t y, uint16_t w, uint16_t h, uint32_t ptr, uint16_t format)
{
    HAL_Write32(EVE_ENC_CMD_SKETCH);
    HAL_Write32(((uint32_t)y << 16) | (x & 0xffff));
    HAL_Write32(((uint32_t)h << 16) | (w & 0xffff));
    HAL_Write32(ptr);
    HAL_Write32(format);
    HAL_IncCmdPointer(20);
}

void EVE_CMD_MEMSET(uint32_t ptr, uint32_t value, uint32_t num)
{
    HAL_Write32(EVE_ENC_CMD_MEMSET);
    HAL_Write32(ptr);
    HAL_Write32(value);
    HAL_Write32(num);
    HAL_IncCmdPointer(16);
}

void EVE_CMD_GRADCOLOR(uint32_t c)
{
    HAL_Write32(EVE_ENC_CMD_GRADCOLOR);
    HAL_Write32(c);
    HAL_IncCmdPointer(8);
}

void EVE_CMD_BITMAP_TRANSFORM(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t tx0, int32_t ty0, int32_t tx1, int32_t ty1, int32_t tx2, int32_t ty2, uint16_t result)
{
    HAL_Write32(EVE_ENC_CMD_BITMAP_TRANSFORM);
    HAL_Write32(x0);
    HAL_Write32(y0);
    HAL_Write32(x1);
    HAL_Write32(y1);
    HAL_Write32(x2);
    HAL_Write32(y2);
    HAL_Write32(tx0);
    HAL_Write32(ty0);
    HAL_Write32(tx1);
    HAL_Write32(ty1);
    HAL_Write32(tx2);
    HAL_Write32(ty2);
    HAL_Write32(result);
    HAL_IncCmdPointer(56);
}

void EVE_CMD_CALIBRATE(uint32_t result)
{
    HAL_Write32(EVE_ENC_CMD_CALIBRATE);
    HAL_Write32(result);
    HAL_IncCmdPointer(8);
}

#if IS_EVE_API(1, 2, 3, 4) // FT82x API change
void EVE_CMD_SETFONT(uint32_t font, uint32_t ptr)
{
    HAL_Write32(EVE_ENC_CMD_SETFONT);
    HAL_Write32(font);
    HAL_Write32(ptr);
    HAL_IncCmdPointer(12);
}
#elif IS_EVE_API(5) // FT81x API change
void EVE_CMD_SETFONT(uint32_t font, uint32_t ptr, uint32_t firstchar)
{
    HAL_Write32(EVE_ENC_CMD_SETFONT);
    HAL_Write32(font);
    HAL_Write32(ptr);
    HAL_Write32(firstchar);
    HAL_IncCmdPointer(16);
}
#endif

void EVE_CMD_LOGO(void)
{
    HAL_Write32(EVE_ENC_CMD_LOGO);
    HAL_IncCmdPointer(4);
}

void EVE_CMD_APPEND(uint32_t ptr, uint32_t num)
{
    HAL_Write32(EVE_ENC_CMD_APPEND);
    HAL_Write32(ptr);
    HAL_Write32(num);
    HAL_IncCmdPointer(12);
}

void EVE_CMD_MEMZERO(uint32_t ptr, uint32_t num)
{
    HAL_Write32(EVE_ENC_CMD_MEMZERO);
    HAL_Write32(ptr);
    HAL_Write32(num);
    HAL_IncCmdPointer(12);
}

void EVE_CMD_SCALE(int32_t sx, int32_t sy)
{
    HAL_Write32(EVE_ENC_CMD_SCALE);
    HAL_Write32(sx);
    HAL_Write32(sy);
    HAL_IncCmdPointer(12);
}

void EVE_CMD_CLOCK(int16_t x, int16_t y, int16_t r, uint16_t options, uint16_t h, uint16_t m, uint16_t s, uint16_t ms)
{
    HAL_Write32(EVE_ENC_CMD_CLOCK);
    HAL_Write32(((uint32_t)y << 16) | (x & 0xffff));
    HAL_Write32(((uint32_t)options << 16) | (r & 0xffff));
    HAL_Write32(((uint32_t)m << 16) | (h & 0xffff));
    HAL_Write32(((uint32_t)ms << 16) | (s & 0xffff));
    HAL_IncCmdPointer(20);
}

void EVE_CMD_GRADIENT(int16_t x0, int16_t y0, uint32_t rgb0, int16_t x1, int16_t y1, uint32_t rgb1)
{
    HAL_Write32(EVE_ENC_CMD_GRADIENT);
    HAL_Write32(((uint32_t)y0 << 16) | (x0 & 0xffff));
    HAL_Write32(rgb0);
    HAL_Write32(((uint32_t)y1 << 16) | (x1 & 0xffff));
    HAL_Write32(rgb1);
    HAL_IncCmdPointer(20);
}

void EVE_CMD_SETMATRIX(void)
{
    HAL_Write32(EVE_ENC_CMD_SETMATRIX);
    HAL_IncCmdPointer(4);
}

void EVE_CMD_TRACK(int16_t x, int16_t y, int16_t w, int16_t h, int16_t tag)
{
    HAL_Write32(EVE_ENC_CMD_TRACK);
    HAL_Write32(((uint32_t)y << 16) | (x & 0xffff));
    HAL_Write32(((uint32_t)h << 16) | (w & 0xffff));
    HAL_Write32(tag);
    HAL_IncCmdPointer(16);
}

void EVE_CMD_GETPTR(uint32_t result)
{
    HAL_Write32(EVE_ENC_CMD_GETPTR);
    HAL_Write32(result);
    HAL_IncCmdPointer(8);
}

void EVE_CMD_PROGRESS(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t options, uint16_t val, uint16_t range)
{
    HAL_Write32(EVE_ENC_CMD_PROGRESS);
    HAL_Write32(((uint32_t)y << 16) | (x & 0xffff));
    HAL_Write32(((uint32_t)h << 16) | (w & 0xffff));
    HAL_Write32(((uint32_t)val << 16) | (options & 0xffff));
    HAL_Write32(range);
    HAL_IncCmdPointer(20);
}

void EVE_CMD_COLDSTART(void)
{
    HAL_Write32(EVE_ENC_CMD_COLDSTART);
    HAL_IncCmdPointer(4);
}

void EVE_CMD_DIAL(int16_t x, int16_t y, int16_t r, uint16_t options, uint16_t val)
{
    HAL_Write32(EVE_ENC_CMD_DIAL);
    HAL_Write32(((uint32_t)y << 16) | (x & 0xffff));
    HAL_Write32(((uint32_t)options << 16) | (r & 0xffff));
    HAL_Write32(val);
    HAL_IncCmdPointer(16);
}

void EVE_CMD_LOADIMAGE(uint32_t ptr, uint32_t options)
{
    HAL_Write32(EVE_ENC_CMD_LOADIMAGE);
    HAL_Write32(ptr);
    HAL_Write32(options);
    HAL_IncCmdPointer(12);
}

void EVE_CMD_DLSTART(void)
{
    HAL_Write32(EVE_ENC_CMD_DLSTART);
    HAL_IncCmdPointer(4);
}

void EVE_CMD_SNAPSHOT(uint32_t ptr)
{
    HAL_Write32(EVE_ENC_CMD_SNAPSHOT);
    HAL_Write32(ptr);
    HAL_IncCmdPointer(8);
}

void EVE_CMD_SCREENSAVER(void)
{
    HAL_Write32(EVE_ENC_CMD_SCREENSAVER);
    HAL_IncCmdPointer(4);
}

void EVE_CMD_MEMCRC(uint32_t ptr, uint32_t num, uint32_t result)
{
    HAL_Write32(EVE_ENC_CMD_MEMCRC);
    HAL_Write32(ptr);
    HAL_Write32(num);
    HAL_Write32(result);
    HAL_IncCmdPointer(16);
}

uint8_t COUNT_ARGS(const char* string)
{
    uint8_t count = 0;
    const char *tmp = string;

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
    uint32_t CommandSize;
    uint32_t StringLength;
    uint8_t i, num=0;

    va_start(args, string);

#if IS_EVE_API(3, 4, 5)
    num = (options & EVE_OPT_FORMAT) ? (COUNT_ARGS(string)) : (0); //Only check % characters if option OPT_FORMAT is set
#endif

    HAL_Write32(EVE_ENC_CMD_TEXT);
    HAL_Write32(((uint32_t)y << 16) | (x & 0xffff));
    HAL_Write32(((uint32_t)options << 16) | (font & 0xffff));
    CommandSize = 12;

    StringLength = EVE_LIB_SendString(string);

    for (i = 0; i < num; i++)
    {
        HAL_Write32((uint32_t)va_arg(args, uint32_t));
    }

    CommandSize = CommandSize + StringLength + (num*4);

    HAL_IncCmdPointer(CommandSize);

    va_end(args);
}

void EVE_CMD_BUTTON(int16_t x, int16_t y, int16_t w, int16_t h, int16_t font, uint16_t options, const char* string, ...)
{
    va_list args;
    uint32_t CommandSize;
    uint32_t StringLength;
    uint8_t i, num=0;

    va_start(args, string);

#if IS_EVE_API(3, 4, 5)
    num = (options & EVE_OPT_FORMAT) ? (COUNT_ARGS(string)) : (0); //Only check % characters if option OPT_FORMAT is set
#endif

    HAL_Write32(EVE_ENC_CMD_BUTTON);
    HAL_Write32(((uint32_t)y << 16) | (x & 0xffff));
    HAL_Write32(((uint32_t)h << 16) | (w & 0xffff));
    HAL_Write32(((uint32_t)options << 16) | (font & 0xffff));
    CommandSize = 16;

    StringLength = EVE_LIB_SendString(string);

    for (i = 0; i < num; i++)
    {
        HAL_Write32((uint32_t)va_arg(args, uint32_t));
    }

    CommandSize = CommandSize + StringLength + (num*4);

    HAL_IncCmdPointer(CommandSize);

    va_end(args);
}

void EVE_CMD_TOGGLE(int16_t x, int16_t y, int16_t w, int16_t font, uint16_t options, uint16_t state, const char* string, ...)
{
    va_list args;
    uint32_t CommandSize;
    uint32_t StringLength;
    uint8_t i, num=0;

    va_start(args, string);

#if IS_EVE_API(3, 4, 5)
    num = (options & EVE_OPT_FORMAT) ? (COUNT_ARGS(string)) : (0); //Only check % characters if option OPT_FORMAT is set
#endif

    HAL_Write32(EVE_ENC_CMD_TOGGLE);
    HAL_Write32(((uint32_t)y << 16) | (x & 0xffff));
    HAL_Write32(((uint32_t)font << 16) | (w & 0xffff));
    HAL_Write32(((uint32_t)state << 16)|options);
    CommandSize = 16;

    StringLength = EVE_LIB_SendString(string);

     for (i = 0; i < num; i++)
     {
         HAL_Write32((uint32_t)va_arg(args, uint32_t));
     }

     CommandSize = CommandSize + StringLength + (num*4);

     HAL_IncCmdPointer(CommandSize);

     va_end(args);
}

#if IS_EVE_API(2)
void EVE_CMD_CSKETCH(int16_t x, int16_t y, uint16_t w, uint16_t h, uint32_t ptr, uint16_t format, uint16_t freq)
{
    HAL_Write32(EVE_ENC_CMD_CSKETCH);
    HAL_Write32(((uint32_t)y << 16) | (x & 0xffff));
    HAL_Write32(((uint32_t)h << 16) | (w & 0xffff));
    HAL_Write32(ptr);
    HAL_Write32(((uint32_t)freq << 16) | (format & 0xffff));
    HAL_IncCmdPointer(20);
}
#endif

#if IS_EVE_API(2, 3, 4, 5)

void EVE_CMD_SETROTATE(uint32_t r)
{
    HAL_Write32(EVE_ENC_CMD_SETROTATE);
    HAL_Write32(r);
    HAL_IncCmdPointer(8);
}

void EVE_CMD_MEDIAFIFO(uint32_t ptr, uint32_t size)
{
    HAL_Write32(EVE_ENC_CMD_MEDIAFIFO);
    HAL_Write32(ptr);
    HAL_Write32(size);
    HAL_IncCmdPointer(12);
}

void EVE_CMD_SYNC(void)
{
    HAL_Write32(EVE_ENC_CMD_SYNC);
    HAL_IncCmdPointer(4);
}

void EVE_CMD_ROMFONT(uint32_t font, uint32_t romslot)
{
    HAL_Write32(EVE_ENC_CMD_ROMFONT);
    HAL_Write32(font);
    HAL_Write32(romslot);
    HAL_IncCmdPointer(12);
}

void EVE_CMD_PLAYVIDEO(uint32_t options)
{
    HAL_Write32(EVE_ENC_CMD_PLAYVIDEO);
    HAL_Write32(options);
    HAL_IncCmdPointer(8);
}

void EVE_CMD_VIDEOFRAME(uint32_t dst, uint32_t ptr)
{
    HAL_Write32(EVE_ENC_CMD_VIDEOFRAME);
    HAL_Write32(dst);
    HAL_Write32(ptr);
    HAL_IncCmdPointer(12);
}

void EVE_CMD_VIDEOSTART(void)
{
    HAL_Write32(EVE_ENC_CMD_VIDEOSTART);
    HAL_IncCmdPointer(4);
}

void EVE_CMD_SETBASE(uint32_t eve_base)
{
    HAL_Write32(EVE_ENC_CMD_SETBASE);
    HAL_Write32(eve_base);
    HAL_IncCmdPointer(8);
}

void EVE_CMD_SETBITMAP(uint32_t source, uint16_t fmt, uint16_t w, uint16_t h)
{
    HAL_Write32(EVE_ENC_CMD_SETBITMAP);
    HAL_Write32(source);
    HAL_Write32(((uint32_t)w << 16) | (fmt & 0xffff));
    HAL_Write32(h);
    HAL_IncCmdPointer(16);
}

void EVE_CMD_SETSCRATCH(uint32_t handle)
{
    HAL_Write32(EVE_ENC_CMD_SETSCRATCH);
    HAL_Write32(handle);
    HAL_IncCmdPointer(8);
}

#endif

#if IS_EVE_API(2, 3, 4) // FT80x, BT82x API change

void EVE_CMD_SETFONT2(uint32_t font, uint32_t ptr, uint32_t firstchar)
{
    HAL_Write32(EVE_ENC_CMD_SETFONT2);
    HAL_Write32(font);
    HAL_Write32(ptr);
    HAL_Write32(firstchar);
    HAL_IncCmdPointer(16);
}

void EVE_CMD_SNAPSHOT2(uint32_t fmt, uint32_t ptr, int16_t x, int16_t y, int16_t w, int16_t h)
{
    HAL_Write32(EVE_ENC_CMD_SNAPSHOT2);
    HAL_Write32(fmt);
    HAL_Write32(ptr);
    HAL_Write32(((uint32_t)y << 16) | (x & 0xffff));
    HAL_Write32(((uint32_t)h << 16) | (w & 0xffff));
    HAL_IncCmdPointer(20);
}

#endif

#if IS_EVE_API(3, 4)

void EVE_CMD_INFLATE2(uint32_t ptr, uint32_t options)
{
  HAL_Write32(EVE_ENC_CMD_INFLATE2);
  HAL_Write32(ptr);
  HAL_Write32(options);
  HAL_IncCmdPointer(12);
}

void EVE_CMD_CLEARCACHE()
{
  HAL_Write32(EVE_ENC_CMD_CLEARCACHE);
  HAL_IncCmdPointer(4);
}

void EVE_CMD_VIDEOSTARTF()
{
    HAL_Write32(EVE_ENC_CMD_VIDEOSTARTF);
    HAL_IncCmdPointer(4);
}

#endif

#if IS_EVE_API(3, 4, 5)

void EVE_CMD_ANIMSTART(int32_t ch, uint32_t aoptr, uint32_t loop)
{
    HAL_Write32(EVE_ENC_CMD_ANIMSTART);
    HAL_Write32(ch);
    HAL_Write32(aoptr);
    HAL_Write32(loop);
    HAL_IncCmdPointer(16);
}

void EVE_CMD_ANIMSTOP(int32_t ch)
{
    HAL_Write32(EVE_ENC_CMD_ANIMSTOP);
    HAL_Write32(ch);
    HAL_IncCmdPointer(8);
}

void EVE_CMD_ANIMXY(int32_t ch, int16_t x, int16_t y)
{
    HAL_Write32(EVE_ENC_CMD_ANIMXY);
    HAL_Write32(ch);
    HAL_Write32(((uint32_t)y<<16) |(x & 0xFFFF));
    HAL_IncCmdPointer(12);
}

void EVE_CMD_ANIMDRAW(int32_t ch)
{
    HAL_Write32(EVE_ENC_CMD_ANIMDRAW);
    HAL_Write32(ch);
    HAL_IncCmdPointer(8);
}

void EVE_CMD_ANIMFRAME(int16_t x, int16_t y, uint32_t aoptr, uint32_t frame)
{
    HAL_Write32(EVE_ENC_CMD_ANIMFRAME);
    HAL_Write32(((uint32_t)y << 16) | (x & 0xFFFF));
    HAL_Write32(aoptr);
    HAL_Write32(frame);
    HAL_IncCmdPointer(16);
}

void EVE_CMD_APPENDF(uint32_t ptr, uint32_t num)
{
    HAL_Write32(EVE_ENC_CMD_APPENDF);
    HAL_Write32(ptr);
    HAL_Write32(num);
    HAL_IncCmdPointer(12);
}

#endif

#if IS_EVE_API(4)

void EVE_CMD_ANIMFRAMERAM(int16_t x, int16_t y, uint32_t aoptr, uint32_t frame )
{
    //CMD_ANIMFRAME (0xFFFF FF5A)
    HAL_Write32(EVE_ENC_CMD_ANIMFRAMERAM);
    HAL_Write32(((uint32_t)y<<16) |(x & 0xFFFF));
    HAL_Write32(aoptr);
    HAL_Write32(frame);
    HAL_IncCmdPointer(16);
}

void EVE_CMD_ANIMSTARTRAM(int32_t ch, uint32_t aoptr, uint32_t loop)
{
    //CMD_ANIMSTARTRAM(0xFFFF FF6E)
    HAL_Write32(EVE_ENC_CMD_ANIMSTARTRAM);
    HAL_Write32(ch);
    HAL_Write32(aoptr);
    HAL_Write32(loop);
    HAL_IncCmdPointer(16);
}

void EVE_CMD_APILEVEL(uint32_t level)
{
    //CMD_APILEVEL (0xFFFF FF63)
    HAL_Write32(EVE_ENC_CMD_APILEVEL);
    HAL_Write32(level);
    HAL_IncCmdPointer(8);
}

void EVE_CMD_FONTCACHE(uint32_t font, int32_t ptr, uint32_t num)
{
    //CMD_FONTCACHE(0xFFFF FF6B)
    HAL_Write32(EVE_ENC_CMD_FONTCACHE);
    HAL_Write32(font);
    HAL_Write32(ptr);
    HAL_Write32(num);
    HAL_IncCmdPointer(16);
}

void EVE_CMD_FONTCACHEQUERY(uint32_t total, int32_t used)
{
    //CMD_FONTCACHEQUERY(0xFFFF FF6C)
    HAL_Write32(EVE_ENC_CMD_FONTCACHEQUERY);
    HAL_Write32(total);
    HAL_Write32(used);
    HAL_IncCmdPointer(12);
}

void EVE_CMD_HSF(uint32_t w )
{
    //CMD_HSF (0xFFFF FF62)
    HAL_Write32(EVE_ENC_CMD_HSF);
    HAL_Write32(w);
    HAL_IncCmdPointer(8);
}

void EVE_CMD_PCLKFREQ(uint32_t ftarget, int32_t rounding, uint32_t factual)
{
    //CMD_PCLKFREQ (0xFFFF FF6A)
    HAL_Write32(EVE_ENC_CMD_PCLKFREQ);
    HAL_Write32(ftarget);
    HAL_Write32(rounding);
    HAL_Write32(factual);
    HAL_IncCmdPointer(16);
}

#endif

#if IS_EVE_API(4, 5)

void EVE_CMD_RUNANIM(uint32_t waitmask, uint32_t play)
{
    //CMD_RUNANIM(0xFFFF FF6F)
    HAL_Write32(EVE_ENC_CMD_RUNANIM);
    HAL_Write32(waitmask);
    HAL_Write32(play);
    HAL_IncCmdPointer(12);
}

void EVE_CMD_TESTCARD()
{
    //CMD_TESTCARD(0xFFFF FF61)
    HAL_Write32(EVE_ENC_CMD_TESTCARD);
    HAL_IncCmdPointer(4);
}

void EVE_CMD_WAIT(uint32_t us)
{
    //CMD_WAIT(0xFFFF FF65)
    HAL_Write32(EVE_ENC_CMD_WAIT);
    HAL_Write32(us);
    HAL_IncCmdPointer(8);
}

void EVE_CMD_NEWLIST(uint32_t a)
{
    //CMD_NEWLIST(0xFFFF FF68)
    HAL_Write32(EVE_ENC_CMD_NEWLIST);
    HAL_Write32(a);
    HAL_IncCmdPointer(8);
}

void EVE_CMD_ENDLIST()
{
    //CMD_ENDLIST(0xFFFF FF69)
    HAL_Write32(EVE_ENC_CMD_ENDLIST);
    HAL_IncCmdPointer(4);
}

void EVE_CMD_CALLLIST(uint32_t a)
{
    // CMD_CALLLIST(0xFFFF FF67)
    HAL_Write32(EVE_ENC_CMD_CALLLIST);
    HAL_Write32(a);
    HAL_IncCmdPointer(8);
}

void EVE_CMD_RETURN()
{
    //CMD_RETURN(0xFFFF FF66)
    HAL_Write32(EVE_ENC_CMD_RETURN);
    HAL_IncCmdPointer(4);
}

#endif

#if IS_EVE_API(3, 4, 5)

void EVE_CMD_NOP()
{
  HAL_Write32(EVE_ENC_CMD_NOP);
  HAL_IncCmdPointer(4);
}

void EVE_CMD_FILLWIDTH(uint32_t s)
{
    HAL_Write32(EVE_ENC_CMD_FILLWIDTH);
    HAL_Write32(s);
    HAL_IncCmdPointer(8);
}

void EVE_CMD_ROTATEAROUND(int32_t x, int32_t y, int32_t a, int32_t s)
{
  HAL_Write32(EVE_ENC_CMD_ROTATEAROUND);
  HAL_Write32(x);
  HAL_Write32(y);
  HAL_Write32(a);
  HAL_Write32(s);
  HAL_IncCmdPointer(20);
}

void EVE_CMD_RESETFONTS()
{
  HAL_Write32(EVE_ENC_CMD_RESETFONTS);
  HAL_IncCmdPointer(4);
}

void EVE_CMD_GRADIENTA(int16_t x0, int16_t y0, uint32_t argb0, int16_t x1, int16_t y1, uint32_t argb1)
{
    HAL_Write32(EVE_ENC_CMD_GRADIENTA);
    HAL_Write32(((uint32_t)y0<<16) |(x0 & 0xFFFF));
    HAL_Write32(argb0);
    HAL_Write32(((uint32_t)y1<<16) |(x1 & 0xFFFF));
    HAL_Write32(argb1);
    HAL_IncCmdPointer(20);
}

void EVE_CMD_FLASHERASE()
{
  HAL_Write32(EVE_ENC_CMD_FLASHERASE);
  HAL_IncCmdPointer(4);
}

/*
   Write data to flash. Constraints:
     - Destination flash address must be virgin (not used before)
     - data array must be aligned 256-bit
     */
void EVE_CMD_FLASHWRITEEXT(uint32_t dest, uint32_t num, uint8_t *data)
{
  uint32_t i, send_data32=0, totalnum = (num+3)/4;

  HAL_Write32(EVE_ENC_CMD_FLASHWRITE);
  HAL_Write32(dest);
  HAL_Write32(num);
  for (i = 0; i < num; i=i+4)
  {
      /* Pack 4 bytes into a 32-bit data each sending package */
      send_data32 = *data++;
      send_data32 |= (*data++) << 8;
      send_data32 |= (*data++) << 16;
      send_data32 |= (*data++) << 24;
      HAL_Write32(send_data32);
  }
  HAL_IncCmdPointer(4*(3+totalnum));
}

void EVE_CMD_FLASHWRITE(uint32_t ptr, uint32_t num)
{
  HAL_Write32(EVE_ENC_CMD_FLASHWRITE);
  HAL_Write32(ptr);
  HAL_Write32(num);
  HAL_IncCmdPointer(12);
}
/*
    Writes the given data to flash.
    If the data matches the existing contents of flash, nothing is done.
    Otherwise the flash is erased in 4K units, and the data is written.
*/
void EVE_CMD_FLASHUPDATE(uint32_t dest, uint32_t src, uint32_t num)
{
    HAL_Write32(EVE_ENC_CMD_FLASHUPDATE);
    HAL_Write32(dest);
    HAL_Write32(src);
    HAL_Write32(num);
    HAL_IncCmdPointer(16);
}

/*
    Read data from flash into main memory
*/
void EVE_CMD_FLASHREAD(uint32_t dest, uint32_t src, uint32_t num)
{
  HAL_Write32(EVE_ENC_CMD_FLASHREAD);
  HAL_Write32(dest);
  HAL_Write32(src);
  HAL_Write32(num);
  HAL_IncCmdPointer(16);
}

/*
    Program data to flash
*/
void EVE_CMD_FLASHPROGRAM(uint32_t dest, uint32_t src, uint32_t num)
{
  HAL_Write32(EVE_ENC_CMD_FLASHPROGRAM);
  HAL_Write32(dest);
  HAL_Write32(src);
  HAL_Write32(num);
  HAL_IncCmdPointer(16);
}

void EVE_CMD_FLASHSOURCE(uint32_t ptr)
{
  HAL_Write32(EVE_ENC_CMD_FLASHSOURCE);
  HAL_Write32(ptr);
  HAL_IncCmdPointer(8);
}

void EVE_CMD_FLASHSPITX(uint32_t num)
{
  HAL_Write32(EVE_ENC_CMD_FLASHSPITX);
  HAL_Write32(num);
  HAL_IncCmdPointer(8);
}

void EVE_CMD_FLASHFAST(uint32_t result)
{
  HAL_Write32(EVE_ENC_CMD_FLASHFAST);
  HAL_Write32(result);
  HAL_IncCmdPointer(8);
}
void EVE_CMD_FLASHSPIRX(uint32_t ptr, uint32_t num)
{
  HAL_Write32(EVE_ENC_CMD_FLASHSPIRX);
  HAL_Write32(ptr);
  HAL_Write32(num);
  HAL_IncCmdPointer(12);
}

void EVE_CMD_FLASHATTACH()
{
  HAL_Write32(EVE_ENC_CMD_FLASHATTACH);
  HAL_IncCmdPointer(4);
}

void EVE_CMD_FLASHDETATCH()
{
  HAL_Write32(EVE_ENC_CMD_FLASHDETACH);
  HAL_IncCmdPointer(4);
}

void EVE_CMD_FLASHSPIDESEL()
{
  HAL_Write32(EVE_ENC_CMD_FLASHSPIDESEL);
  HAL_IncCmdPointer(4);
}

#endif

#if IS_EVE_API(4, 5)

void EVE_CMD_GETIMAGE(uint32_t source, uint32_t fmt, uint32_t w, uint32_t h, uint32_t palette)
{
    HAL_Write32(EVE_ENC_CMD_GETIMAGE);
    HAL_Write32(source);
    HAL_Write32(fmt);
    HAL_Write32(w);
    HAL_Write32(h);
    HAL_Write32(palette);
    HAL_IncCmdPointer(24);
}

void EVE_CMD_CALIBRATESUB(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t result)
{
    HAL_Write32(EVE_ENC_CMD_CALIBRATESUB);
    HAL_Write32(((uint32_t)y<<16) |(x & 0xFFFF));
    HAL_Write32(((uint32_t)h<<16) |(w & 0xFFFF));
    HAL_Write32(result);
    HAL_IncCmdPointer(16);
}

#endif

#if IS_EVE_API(5)

void EVE_CMD_CGRADIENT(uint32_t shape, int16_t x, int16_t y, int16_t w, int16_t h, uint32_t rgb0, uint32_t rgb1)
{
    HAL_Write32(EVE_ENC_CMD_CGRADIENT);
    HAL_Write32(shape);
    HAL_Write32(((uint32_t)y<<16) |(x & 0xFFFF));
    HAL_Write32(((uint32_t)h<<16) |(w & 0xFFFF));
    HAL_Write32(rgb0);
    HAL_Write32(rgb1);
    HAL_IncCmdPointer(24);
}

void EVE_CMD_TEXTDIM(uint32_t dimensions, int16_t font, uint16_t options, const char* string, ...)
{
    va_list args;
    uint32_t CommandSize;
    uint32_t StringLength;
    uint8_t i, num=0;

    va_start(args, string);

    num = (options & EVE_OPT_FORMAT) ? (COUNT_ARGS(string)) : (0); //Only check % characters if option OPT_FORMAT is set

    HAL_Write32(EVE_ENC_CMD_TEXTDIM);
    HAL_Write32(dimensions);
    CommandSize = 8;

    StringLength = EVE_LIB_SendString(string);

    for (i = 0; i < num; i++)
    {
        HAL_Write32((uint32_t)va_arg(args, uint32_t));
    }

    CommandSize = CommandSize + StringLength + (num*4);

    HAL_IncCmdPointer(CommandSize);

    va_end(args);
}

void EVE_CMD_ARC(int16_t x, int16_t y, uint16_t r0, uint16_t r1, uint16_t a0, uint16_t a1)
{
    HAL_Write32(EVE_ENC_CMD_ARC);
    HAL_Write32(((uint32_t)y<<16) |(x & 0xFFFF));
    HAL_Write32(((uint32_t)r1<<16) |(r0 & 0xFFFF));
    HAL_Write32(((uint32_t)a1<<16) |(a0 & 0xFFFF));
    HAL_IncCmdPointer(16);
}

void EVE_CMD_RENDERTARGET(uint32_t dest, uint16_t fmt, uint16_t w, uint16_t h)
{
    HAL_Write32(EVE_ENC_CMD_RENDERTARGET);
    HAL_Write32(dest);
    HAL_Write32(fmt | (w << 16));
    HAL_Write32(h);
    HAL_IncCmdPointer(16);
}

void EVE_CMD_ENABLEREGION(uint32_t en)
{
    HAL_Write32(EVE_ENC_CMD_ENABLEREGION);
    HAL_Write32(en);
    HAL_IncCmdPointer(8);
}

void EVE_CMD_FENCE(void)
{
    HAL_Write32(EVE_ENC_CMD_FENCE);
    HAL_IncCmdPointer(4);
}

void EVE_CMD_GRAPHICSFINISH(void)
{
    HAL_Write32(EVE_ENC_CMD_GRAPHICSFINISH);
    HAL_IncCmdPointer(4);
}

void EVE_CMD_REGWRITE(uint32_t a, uint32_t b)
{
    HAL_Write32(EVE_ENC_CMD_REGWRITE);
    HAL_Write32(a);
    HAL_Write32(b);
    HAL_IncCmdPointer(12);
}

void EVE_CMD_APBWRITE(uint32_t a, uint32_t b)
{
    HAL_Write32(EVE_ENC_CMD_APBWRITE);
    HAL_Write32(a);
    HAL_Write32(b);
    HAL_IncCmdPointer(12);
}

void EVE_CMD_APBREAD(uint32_t a, uint32_t result)
{
    HAL_Write32(EVE_ENC_CMD_APBREAD);
    HAL_Write32(a);
    HAL_Write32(result);
    HAL_IncCmdPointer(12);
}

void EVE_CMD_LOADWAV(uint32_t dst, uint32_t options)
{
    HAL_Write32(EVE_ENC_CMD_LOADWAV);
    HAL_Write32(dst);
    HAL_Write32(options);
    HAL_IncCmdPointer(12);
}

void EVE_CMD_GLOW(int16_t x, int16_t y, int16_t w, int16_t h )
{
    HAL_Write32(EVE_ENC_CMD_GLOW);
    HAL_Write32(((uint32_t)y<<16) |(x & 0xFFFF));
    HAL_Write32(((uint32_t)h<<16) |(w & 0xFFFF));
    HAL_IncCmdPointer(12);
}

void EVE_CMD_SDATTACH(uint32_t options, uint32_t result)
{
    HAL_Write32(EVE_ENC_CMD_SDATTACH);
    HAL_Write32(options);
    HAL_Write32(result);
    HAL_IncCmdPointer(12);
}

void EVE_CMD_FSOPTIONS(uint32_t options)
{
    HAL_Write32(EVE_ENC_CMD_FSOPTIONS);
    HAL_Write32(options);
    HAL_IncCmdPointer(8);
}

void EVE_CMD_FSREAD(uint32_t dst, const char* filename, uint32_t result)
{
    uint32_t StringLength;

    HAL_Write32(EVE_ENC_CMD_FSREAD);
    StringLength = EVE_LIB_SendString(filename);
    HAL_Write32(result);
    HAL_IncCmdPointer(8 + StringLength);
}

void EVE_CMD_FSSIZE(const char* filename, uint32_t size)
{
    uint32_t StringLength;

    HAL_Write32(EVE_ENC_CMD_FSSIZE);
    StringLength = EVE_LIB_SendString(filename);
    HAL_Write32(size);
    HAL_IncCmdPointer(8 + StringLength);
}

void EVE_CMD_FSSOURCE(const char* filename, uint32_t result)
{
    uint32_t StringLength;

    HAL_Write32(EVE_ENC_CMD_FSSOURCE);
    StringLength = EVE_LIB_SendString(filename);
    HAL_Write32(result);
    HAL_IncCmdPointer(8 + StringLength);
}

void EVE_CMD_FSDIR(uint32_t dst, uint32_t num, const char* path, uint32_t result)
{
    uint32_t StringLength;

    HAL_Write32(EVE_ENC_CMD_FSDIR);
    HAL_Write32(dst);
    HAL_Write32(num);
    StringLength = EVE_LIB_SendString(path);
    HAL_Write32(result);
    HAL_IncCmdPointer(16 + StringLength);
}

void EVE_CMD_SDBLOCKREAD(uint32_t dst, uint32_t src, uint32_t count, uint32_t result)
{
    HAL_Write32(EVE_ENC_CMD_FSDIR);
    HAL_Write32(dst);
    HAL_Write32(src);
    HAL_Write32(count);
    HAL_Write32(result);
    HAL_IncCmdPointer(20);
}

#endif

// ############################  All EVE  ##########################################

