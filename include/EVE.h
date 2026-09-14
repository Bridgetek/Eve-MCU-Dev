/**
 * @file EVE.h
 * @brief Header file to include to access all required programming interface entry points and definitions.
 *
 * @details This is the main header file for applications using the EVE-MCU-Dev library.
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

#ifndef _EVE_HEADER_H
#define _EVE_HEADER_H

/* EVE API INCLUDES */

#include <stdint.h>

/*
 * Include the EVE settings derived from the EVE configuration, including API
 * selection macros and device-specific configuration.
 */
#include "EVE_settings.h"

/*
 * Include the command and register definitions for the selected EVE API.
 * A valid EVE API level must be selected before these headers are included.
 */
#if IS_EVE_API(1, 2, 3, 4, 5)
    #include "EVE_commands.h"
    #include "EVE_registers.h"
#else // EVE 1-5
    #error No EVE API selected.
#endif

/* Public BT82x patch interface for EVE API 5. */
#if IS_EVE_API(5)
    // Base patch for BT82x
    #include <extensions/bt82x_patch.h>
#endif //IS_EVE_API(5)

/* EVE API DEFINITIONS */

/* Touchscreen technology versions */
/* Note: CTOUCH_MODE_COMPATIBILITY and CTOUCH_MODE_EXTENDED definitions are deprecated */

/* Legacy Metric Block for Fonts */
/* Note: FT_GPU_* definitions are deprecated */
#define EVE_GPU_NUMCHAR_PERFONT    128  // Deprecated FT_GPU_NUMCHAR_PERFONT
#define EVE_GPU_FONT_TABLE_SIZE    148  // Deprecated FT_GPU_FONT_TABLE_SIZE

/* Font table structure */
/* On EVE4 onwards this is called the Legacy Font Structure. */
typedef struct
{
    /* Width of each character font from 0 to 127 */
    uint8_t    FontWidth[EVE_GPU_NUMCHAR_PERFONT];
    /* Bitmap format of font - this is the same as the EVE_FORMAT_x, where x is L1, L2, L4 etc. */
    uint32_t    FontBitmapFormat;
    /* Font line stride in bytes */
    uint32_t    FontLineStride;
    /* Font width in pixels */
    uint32_t    FontWidthInPixels;
    /* Font height in pixels */
    uint32_t    FontHeightInPixels;
    /* Pointer to font graphics raw data */
    uint32_t    PointerToFontGraphicsData;
} EVE_GPU_FONT_HEADER;

#if IS_EVE_API(4, 5)

/* On EVE4 onwards this is called the Extended Font Structure. */
typedef struct
{
    /* Signature of font structure - must be 0x0100AAFF */
    uint32_t    Signature;
    /* Total size of font block in bytes */
    uint32_t    Size;
    /* Bitmap format of font - this is the same as the EVE_FORMAT_x, where x is L1, L2, L4 etc. */
    uint32_t    FontBitmapFormat;
    /* Bitmap swizzle - as used in BITMAP_SWIZZLE command */
    uint32_t    FontBitmapSwizzle;
    /* Font layout width in bytes */
    uint32_t    FontLayoutWidth;
    /* Font layout height */
    uint32_t    FontLayoutHeight;
    /* Font width in pixels */
    uint32_t    FontWidthInPixels;
    /* Font height in pixels */
    uint32_t    FontHeightInPixels;
    /* Pointer to font graphics raw data */
    uint32_t    PointerToFontGraphicsData;
    /* Number of characters */
    uint32_t    FontNumberCharacters;
} EVE_GPU_EXT_FONT_HEADER;

#endif // IS_EVE_API(4, 5)

#if IS_EVE_API(5)

/* On EVE5 onwards this is called the Extended 2 Font Structure. */
typedef struct
{
    /* Signature of font structure - must be 0x0200AAFF */
    uint32_t    Signature;
    /* Total size of font block in bytes */
    uint32_t    Size;
    /* Bitmap format and flags of font */
    /* Format is the same as the EVE_FORMAT_x, where x is L1, L2, L4 etc. */
    /* Flags denotes binary and line break behaviour */
    uint32_t    FontBitmapFormat;
    /* Bitmap swizzle - as used in BITMAP_SWIZZLE command */
    uint32_t    FontBitmapSwizzle;
    /* Font layout width in bytes */
    uint32_t    FontLayoutWidth;
    /* Font layout height */
    uint32_t    FontLayoutHeight;
    /* Font width in pixels */
    uint32_t    FontWidthInPixels;
    /* Font height in pixels */
    uint32_t    FontHeightInPixels;
    /* Font padding and leading in pixels */
    uint32_t    FontPadLeadInPixels;
    /* Number of characters */
    uint32_t    FontNumberCharacters;
    /* Font midline and baseline in pixels */
    uint32_t    FontMidlineBaselineInPixels;
} EVE_GPU_EXT2_FONT_HEADER;

typedef struct
{
    /* Pointer to font graphics raw data */
    uint32_t    PointerToFontGraphicsData;
    /* Width of font character */
    uint32_t    Width;
} EVE_GPU_EXT2_CHAR_DESCRIPTOR;

#endif // IS_EVE_API(5)

/* EVE API */

/**
 * @brief Initialise EVE API.
 * @details Initialise the EVE API layer, HAL layer and MCU-specific hardware layer.
 * @return 0 for success or -1 for failure (device not found or unsupported).
 */
int EVE_Init(void);

/**
 * @brief De-Initialise EVE API.
 * @details This will call the HAL layer and MCU-specific de-initialisation routines.
 * @return 0 for success or -1 for failure.
 */
int EVE_Deinit(void);

/**
 * @brief EVE API: Begin co-processor list
 * @details Starts a co-processor list. Initialises the API and HAL ready to start
 *      transmitting a co-processor list to the EVE.
 *      This will typically assert chip select to allow the SPI interface to
 *      send data to the EVE.
 *      This must be called before any co-processor commands, display list commands,
 *      and command buffer data is sent to the EVE device.
 *      Certain API commands cannot be sent during a co-processor list as they 
 *      perform standalone functions with EVE RAM_G or registers. These are clearly
 *      marked in "details" section.
 */
void EVE_LIB_BeginCoProList(void);

/**
 * @brief EVE API: End co-processor list
 * @details Ends a co-processor list. This will perform any operations in the API
 *      and HAL to finish a co-processor list. 
 *      This will typically deassert chip select after updating any registers
 *      on the EVE device that will signal the end of the co-processor list.
 *      This must be called after a call to `EVE_LIB_BeginCoProList`.
 */
void EVE_LIB_EndCoProList(void);

/**
 * @brief EVE API: Waits for co-processor list to end
 * @details Will poll the co-processor command list until it has been completed.
 *      This must be called after `EVE_LIB_EndCoProList` for the program to wait
 *      for the completion of the co-processor actions.
 *      If the co-processor method is set to `EVE_COPROC_INT` then it will wait 
 *      for an interrupt signal on the INT# line before testing for correct completion.
 * @returns 0 for successful completion, 0xff for co-processor exception.
 */
int EVE_LIB_AwaitCoProEmpty(void);

/**
 * @brief EVE API: Waits for co-processor list to end with a timeout value
 * @details Will poll the co-processor command list until it has been completed.
 *      This must be called after `EVE_LIB_EndCoProList` for the program to wait
 *      for the completion of the co-processor actions.
 *      If the co-processor method is set to `EVE_COPROC_INT` then it will wait 
 *      for an interrupt signal on the INT# line before testing for correct completion.
 * @param timeout - timeout in milliseconds (zero for NO timeout)
 * @returns 0 for successful completion, 0xff for co-processor exception, 0xfe for a timeout.
 */
int EVE_LIB_AwaitCoProEmptyTimeout(uint32_t timeout);

/**
 * @brief EVE API: Recovers the co-processor in the event of an exception
 * @details Will reset the co-processor after an exception is reported by 
 *      EVE_LIB_AwaitCoProEmpty or EVE_LIB_AwaitCoProEmptyTimeout.
 */
void EVE_LIB_RecoverCoPro(void);

/**
 * @brief EVE API: Free space in co-processor list 
 * @details Obtains the free space in the co-processor circular buffer. 
 *      This operation may have an effect on the performance of the device.
 *      This must be called during a co-processor list:
 *      after a call to `EVE_LIB_BeginCoProList` and before a call to 
 *      `EVE_LIB_EndCoProList`.
 * @returns The number of free instructions in the co-processor circular buffer.
 */
uint16_t EVE_LIB_GetCoProSpace(void);

#if defined(EVE_COPROC_PROFILE)
/**
 * @brief EVE API: Resets the co-processor list profiling length
 * @details Sets the profiling pointer to zero to restart profiling.
 *      This function can be called at any time.
 */
void EVE_LIB_BeginCoProProfile(void);

/**
 * @brief EVE API: Size of co-processor list since last reset
 * @details Obtains the current profiling pointer for the co-processor list.
 * @returns The number of instructions added to the co-processor list since 
 *      the last reset of the profiling pointer.
 *      This function can be called at any time.
 */
uint16_t EVE_LIB_GetCoProProfile(void);

/**
 * @brief EVE API: Size of display list
 * @details Obtains the current size of the display list.
 *      This function cannot be used within a co-processor list.
 * @returns The number of instructions currently in the display list.
 */
uint16_t EVE_LIB_GetDlProfile(void);
#endif  // defined(EVE_COPROC_PROFILE)

/**
 * @brief Test interrupt input line
 * @details This function will check the interrupt input INT# from
 *      the EVE device.
 *      This function can be called at any time.
 * @returns zero if there is no interrupt; >0 if the EVE device is
 *      asserting an interrupt; or -1 if the MCU or Platform does not support reading the interrupt line.
 */
int EVE_LIB_Int(void);

#if defined(EVE_MANAGE_INTERRUPTS)
/**
 * @brief EVE API: Test whether one or more interrupt flags are set.
 * @details Reads the EVE interrupt flag register and accumulates any newly
 *      pending interrupt flags in a software status value. Reading the
 *      interrupt flag register clears the hardware flags, so pending bits are
 *      retained in software until they are consumed by the mask supplied to
 *      this function.
 *      This function cannot be used within a co-processor list.
 * @param mask Bit mask of interrupts to query and consume.
 * @returns 0 for no interrupts in the mask being set, if any interrupts are
 *      set then the return value will contain bits set from the mask parameter.
 */
uint8_t EVE_LIB_GetInterrupt(uint8_t mask);
#endif // defined(EVE_MANAGE_INTERRUPTS)

/**
 * @brief EVE API: Returns a result from the co-processor command buffer
 * @details Will return a result value from "offset" words back in the command buffer.
 *      If the value of offset is 1 then the previous value from the co-processor
 *      command buffer is returned.
 *      This function cannot be used within a co-processor list.
 * @param offset - Number of 32-bit words to go back in the command buffer for
 *      the result.
 * @returns result of a previous co-processor command.
 */
uint32_t EVE_LIB_GetResult(int offset);

#if IS_EVE_API(3, 4, 5)
/**
 * @brief EVE API: Get co-processor exception description
 * @details Will query the co-processor exception description to a string.
 *      This function cannot be used within a co-processor list.
 * @param desc - Buffer to receive the text of the exception description.
 * @returns Co-processor exception description. This is a pointer to a string
 *      and must be sufficient to hold 128 characters.
 */
void EVE_LIB_GetCoProException(char *desc);
#endif // IS_EVE_API(3, 4, 5)

/**
 * @brief EVE API: Write a buffer to memory mapped RAM
 * @details Writes a block of data via SPI to the EVE.
 *      This function cannot be used within a co-processor list.
 * @param ImgData - Pointer to start of data buffer.
 * @param DataSize - Number of bytes in buffer.
 * @param DestAddress - 24-bit/32-bit memory mapped address on EVE.
 */
void EVE_LIB_WriteDataToRAMG(const uint8_t *ImgData, uint32_t DataSize, uint32_t DestAddress);

/**
 * @brief EVE API: Read a buffer from memory mapped RAM
 * @details Reads a block of data via SPI from the EVE.
 *      This function cannot be used within a co-processor list.
 * @param ImgData - Pointer to start of receive data buffer.
 * @param DataSize - Number of bytes to read (rounded up to be 32-bit aligned).
 * @param SrcAddress - 24-bit/32-bit memory mapped address on EVE.
 */
void EVE_LIB_ReadDataFromRAMG(uint8_t *ImgData, uint32_t DataSize, uint32_t SrcAddress);

/**
 * @brief EVE API: Write a buffer to the co-processor command memory
 * @details Writes a block of data via SPI to the EVE co-processor.
 *      This must be part of a co-processor list. It will typically be called
 *      after a co-processor command to provide data for the operation.
 *      The data will be added to the co-processor command list therefore the
 *      write will block on available space in this list.
 * @param ImgData - Pointer to start of data buffer.
 * @param DataSize - Number of bytes in buffer.
 */
void EVE_LIB_WriteDataToCMD(const uint8_t *ImgData, uint32_t DataSize);

/**
 * @brief EVE API: Write a string to the co-processor command memory
 * @details Writes a string via SPI to the EVE co-processor.
 *      This must be part of a co-processor list. It will typically be called
 *      after a co-processor command to provide a string for the operation.
 *      The data will be added to the co-processor command list therefore the
 *      write will block on available space in this list.
 * @param string - String to be sent to the co-processor memory.
 * @returns size - Number of bytes in string.
 */
uint16_t EVE_LIB_SendString(const char* string);

/**
 * @brief EVE API: Get properties of a CMD_LOADIMAGE operation
 * @details Obtains the details of an image decoded by the CMD_LOADIMAGE
 *      co-processor command. The properties of the image are taken from
 *      the co-processor command list.
 *      This function cannot be used within a co-processor list.
 * @param addr - Pointer to variable to receive the image start address.
 * @param width - Pointer to variable to receive the image width.
 * @param height - Pointer to variable to receive the image height.
 */
void EVE_LIB_GetProps(uint32_t *addr, uint32_t *width, uint32_t *height);

/**
 * @brief EVE API: Get current allocation pointer
 * @details Obtains the automatic allocation pointer of the last address
 *      used for certain co-processor operations.
 *      This function cannot be used within a co-processor list.
 * @param addr - Last allocation address rounded up to the next 32-bit 
 *      boundary.
 */
void EVE_LIB_GetPtr(uint32_t *addr);

/**
 * @brief EVE API: Get the touchscreen transformation matrix.
 * @details Obtains the transformation matrix from a CMD_CALIBRATE operation.
 *      This function cannot be used within a co-processor list.
 * @param a -  pointer of variable to receive matrix a.
 * @param b -  pointer of variable to receive matrix b.
 * @param c -  pointer of variable to receive matrix c.
 * @param d -  pointer of variable to receive matrix d.
 * @param e -  pointer of variable to receive matrix e.
 * @param f -  pointer of variable to receive matrix f.
 */
void EVE_LIB_GetMatrix(uint32_t *a, uint32_t *b, uint32_t *c, uint32_t *d, uint32_t *e, uint32_t *f);

/**
 * @brief EVE API: Calculate the CRC of a memory area.
 * @details Obtains the CRC of a memory area.
 *      This function cannot be used within a co-processor list.
 * @param ptr - Start of memory area.
 * @param num - Number of bytes to CRC.
 * @param result - pointer to receive the CRC.
 */
void EVE_LIB_MemCrc(uint32_t ptr, uint32_t num, uint32_t *result);

#if IS_EVE_API(2, 3, 4, 5)
/**
 * @brief EVE API: Computes a bitmap transformation matrix.
 * @details  It computes the transform given three corresponding points in screen space and bitmap space.
 *      This function cannot be used within a co-processor list.
 * @param x0, y0 - Point 0 screen coordinate, in pixels.
 * @param x1, y1 - Point 1 screen coordinate, in pixels.
 * @param x2, y2 - Point 2 screen coordinate, in pixels.
 * @param tx0, ty0 - Point 0 bitmap coordinate, in pixels.
 * @param tx1, ty1 - Point 1 bitmap coordinate, in pixels.
 * @param tx2, ty2 - Point 2 bitmap coordinate, in pixels.
 * @param result - set to -1 on success, or 0 if it is not possible to find the solution matrix
 */
void EVE_LIB_BitmapTransform( int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, 
                              int32_t tx0, int32_t ty0, int32_t tx1, int32_t ty1, int32_t tx2, int32_t ty2,
                              uint32_t *result );
#endif // IS_EVE_API(2, 3, 4, 5)

#if IS_EVE_API(4, 5)
/**
 * @brief EVE API: Get properties of a CMD_LOADIMAGE operation.
 * @details From the last CMD_LOADIMAGE get the address, size, format and palette of the loaded image.
 *      This function cannot be used within a co-processor list.
 * @param *addr - pointer to variable to receive the address the image was loaded to.
 * @param *fmt - pointer to variable to receive the format of the loaded image.
 * @param *width - pointer to variable to receive the width of the loaded image.
 * @param *height - pointer to variable to receive the height of the loaded image.
 * @param *palette - pointer to variable to receive the palette of the loaded image.
 */
void EVE_LIB_GetImage(uint32_t *addr, uint32_t *fmt, uint32_t *width, uint32_t *height, uint32_t *palette);
#endif // IS_EVE_API(4, 5)

#if IS_EVE_API(5)
/**
 * @brief EVE API: Read a register.
 * @details Reads a register value.
 *      This function cannot be used within a co-processor list.
 * @param addr - Address of register to read.
 * @param value - pointer to receive the contents of the register.
 */
void EVE_LIB_RegRead(uint32_t addr, uint32_t *value);
#endif // IS_EVE_API(5)

/**
 * @brief EVE API: Write a memory location.
 * @details Writes a memory location value.
 *      This function cannot be used within a co-processor list.
 * @param addr - Address of memory location to write.
 * @param value - Value to write to memory.
 */
//@{
void EVE_LIB_MemWrite32(uint32_t addr, uint32_t value);
#if IS_EVE_API(1, 2, 3, 4) // Not supported on BT82x
void EVE_LIB_MemWrite16(uint32_t addr, uint16_t value);
void EVE_LIB_MemWrite8(uint32_t addr, uint8_t value);
#endif // IS_EVE_API(1, 2, 3, 4)
//@}

/**
 * @brief EVE API: Read a memory location.
 * @details Reads a memory location value.
 *      This function cannot be used within a co-processor list.
 * @param addr - Address of memory location to read.
 * @return - Value read from memory.
 */
//@{
uint32_t EVE_LIB_MemRead32(uint32_t address);
#if IS_EVE_API(1, 2, 3, 4) // Not supported on BT82x
uint16_t EVE_LIB_MemRead16(uint32_t address);
uint8_t EVE_LIB_MemRead8(uint32_t address);
#endif // IS_EVE_API(1, 2, 3, 4)
//@}

//##################################################################################################

// Display List encodings.
// Common to all generations.

void EVE_CMD(uint32_t c);
// Graphics instructions
void EVE_CLEAR_COLOR_RGB(uint8_t r, uint8_t g, uint8_t b);
void EVE_CLEAR_COLOR(uint32_t c);
void EVE_CLEAR(uint8_t c, uint8_t s, uint8_t t);
void EVE_COLOR_RGB(uint8_t r, uint8_t g, uint8_t b);
void EVE_COLOR(uint32_t c);
void EVE_VERTEX2F(int16_t x, int16_t y);
void EVE_VERTEX2II(uint16_t x, uint16_t y, uint8_t handle,uint8_t cell);
void EVE_BITMAP_HANDLE(uint8_t handle);
void EVE_BITMAP_SOURCE(int32_t addr);
#if IS_EVE_API(3, 4) // BT81x BT88x API change
void EVE_BITMAP_SOURCE2(uint8_t flash_or_ram, int32_t addr);
#endif // IS_EVE_API(3, 4)
void EVE_BITMAP_LAYOUT(uint8_t format, uint16_t linestride, uint16_t height);
void EVE_BITMAP_SIZE(uint8_t filter, uint8_t wrapx, uint8_t wrapy, uint16_t width, uint16_t height);
void EVE_CELL(uint8_t cell);
void EVE_TAG(uint8_t s);
void EVE_ALPHA_FUNC(uint8_t func, uint8_t ref);
void EVE_STENCIL_FUNC(uint8_t func, uint8_t ref, uint8_t mask);
void EVE_BLEND_FUNC(uint8_t src, uint8_t dst);
void EVE_STENCIL_OP(uint8_t sfail, uint8_t spass);
void EVE_POINT_SIZE(uint16_t size);
void EVE_LINE_WIDTH(uint16_t width);
void EVE_CLEAR_COLOR_A(uint8_t alpha);
void EVE_COLOR_A(uint8_t alpha);
void EVE_CLEAR_STENCIL(uint8_t s);
void EVE_CLEAR_TAG(uint8_t s);
void EVE_STENCIL_MASK(uint8_t mask);
void EVE_TAG_MASK(uint8_t mask);
void EVE_SCISSOR_XY(uint16_t x, uint16_t y);
void EVE_SCISSOR_SIZE(uint16_t width, uint16_t height);
void EVE_CALL(uint16_t dest);
void EVE_JUMP(uint16_t dest);
void EVE_BEGIN(uint8_t prim);
void EVE_COLOR_MASK(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void EVE_END(void);
void EVE_SAVE_CONTEXT(void);
void EVE_RESTORE_CONTEXT(void);
void EVE_RETURN(void);
void EVE_MACRO(uint8_t m);
void EVE_DISPLAY(void);
void EVE_BITMAP_TRANSFORM_A(long a);
void EVE_BITMAP_TRANSFORM_B(long b);
void EVE_BITMAP_TRANSFORM_C(long c);
void EVE_BITMAP_TRANSFORM_D(long d);
void EVE_BITMAP_TRANSFORM_E(long e);
void EVE_BITMAP_TRANSFORM_F(long f);

#if IS_EVE_API(2, 3, 4, 5) // FT81x API change
void EVE_VERTEX_FORMAT(uint8_t frac);
void EVE_BITMAP_LAYOUT_H(uint8_t linestride, uint8_t height);
void EVE_BITMAP_SIZE_H(uint8_t width, uint8_t height);
void EVE_PALETTE_SOURCE(uint32_t addr);
void EVE_VERTEX_TRANSLATE_X(uint32_t x);
void EVE_VERTEX_TRANSLATE_Y(uint32_t y);
void EVE_NOP(void);
#endif // IS_EVE_API(2, 3, 4, 5)

#if IS_EVE_API(3, 4, 5)
void EVE_BITMAP_EXT_FORMAT(uint16_t fmt);
void EVE_BITMAP_SWIZZLE(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
#endif // IS_EVE_API(3, 4, 5)

#if IS_EVE_API(5) // BT82x extensions
void EVE_BITMAP_SOURCE_H(uint8_t addr);
void EVE_BITMAP_ZORDER(uint8_t o);
void EVE_PALETTE_SOURCE_H(uint8_t addr);
void EVE_REGION(uint8_t y, uint8_t h, uint16_t dest);
#endif

// Co-Processor Widgets.
// Common to all generations.

void EVE_CMD_KEYS(int16_t x, int16_t y, int16_t w, int16_t h, int16_t font, uint16_t options, const char* string);
void EVE_CMD_NUMBER(int16_t x, int16_t y, int16_t font, uint16_t options, int32_t n);
void EVE_CMD_LOADIDENTITY(void);
void EVE_CMD_GAUGE(int16_t x, int16_t y, int16_t r, uint16_t options, uint16_t major, uint16_t minor, uint16_t val, uint16_t range);
void EVE_CMD_REGREAD(uint32_t ptr, uint32_t result);
void EVE_CMD_GETPROPS(uint32_t ptr, uint32_t w, uint32_t h);
void EVE_CMD_MEMCPY(uint32_t dest, uint32_t src, uint32_t num);
void EVE_CMD_SPINNER(int16_t x, int16_t y, uint16_t style, uint16_t scale);
void EVE_CMD_BGCOLOR(uint32_t c);
void EVE_CMD_SWAP(void);
void EVE_CMD_TRANSLATE(int32_t tx, int32_t ty);
void EVE_CMD_STOP(void);
void EVE_CMD_SLIDER(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t options, uint16_t val, uint16_t range);
void EVE_CMD_INTERRUPT(uint32_t ms);
void EVE_CMD_FGCOLOR(uint32_t c);
void EVE_CMD_ROTATE(int32_t a);
void EVE_CMD_MEMWRITE(uint32_t ptr, uint32_t num);
void EVE_CMD_SCROLLBAR(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t options, uint16_t val, uint16_t size, uint16_t range);
void EVE_CMD_GETMATRIX(int32_t a, int32_t b, int32_t c, int32_t d, int32_t e, int32_t f);
void EVE_CMD_SKETCH(int16_t x, int16_t y, uint16_t w, uint16_t h, uint32_t ptr, uint16_t format);
void EVE_CMD_MEMSET(uint32_t ptr, uint32_t value, uint32_t num);
void EVE_CMD_GRADCOLOR(uint32_t c);
void EVE_CMD_BITMAP_TRANSFORM(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t tx0, int32_t ty0, int32_t tx1, int32_t ty1, int32_t tx2, int32_t ty2, uint16_t result);
void EVE_CMD_CALIBRATE(uint32_t result);
#if IS_EVE_API(1, 2, 3, 4) // BT82x API change
void EVE_CMD_INFLATE(uint32_t ptr);
void EVE_CMD_SETFONT(uint32_t font, uint32_t ptr);
#else // EVE 5
void EVE_CMD_INFLATE(uint32_t ptr, uint32_t options);
void EVE_CMD_SETFONT(uint32_t font, uint32_t ptr, uint32_t firstchar);
#endif // IS_EVE_API(1, 2, 3, 4)
void EVE_CMD_LOGO(void);
void EVE_CMD_APPEND(uint32_t ptr, uint32_t num);
void EVE_CMD_MEMZERO(uint32_t ptr, uint32_t num);
void EVE_CMD_SCALE(int32_t sx, int32_t sy);
void EVE_CMD_CLOCK(int16_t x, int16_t y, int16_t r, uint16_t options, uint16_t h, uint16_t m, uint16_t s, uint16_t ms);
void EVE_CMD_GRADIENT(int16_t x0, int16_t y0, uint32_t rgb0, int16_t x1, int16_t y1, uint32_t rgb1);
void EVE_CMD_SETMATRIX(void);
void EVE_CMD_TRACK(int16_t x, int16_t y, int16_t w, int16_t h, int16_t tag);
void EVE_CMD_GETPTR(uint32_t result);
void EVE_CMD_PROGRESS(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t options, uint16_t val, uint16_t range);
void EVE_CMD_COLDSTART(void);
void EVE_CMD_DIAL(int16_t x, int16_t y, int16_t r, uint16_t options, uint16_t val);
void EVE_CMD_LOADIMAGE(uint32_t ptr, uint32_t options);
void EVE_CMD_DLSTART(void);
void EVE_CMD_SNAPSHOT(uint32_t ptr);
void EVE_CMD_SCREENSAVER(void);
void EVE_CMD_MEMCRC(uint32_t ptr, uint32_t num, uint32_t result);
uint8_t COUNT_ARGS(const char* string);
void EVE_CMD_TEXT(int16_t x, int16_t y, int16_t font, uint16_t options, const char* string, ...);
void EVE_CMD_BUTTON(int16_t x, int16_t y, int16_t w, int16_t h, int16_t font, uint16_t options, const char* string, ...);
void EVE_CMD_TOGGLE(int16_t x, int16_t y, int16_t w, int16_t font, uint16_t options, uint16_t state, const char* string, ...);
#if IS_EVE_API(2)
void EVE_CMD_CSKETCH(int16_t x, int16_t y, uint16_t w, uint16_t h, uint32_t ptr, uint16_t format, uint16_t freq);
#endif // IS_EVE_API(2)
#if IS_EVE_API(2, 3, 4, 5)
void EVE_CMD_SETROTATE(uint32_t r);
void EVE_CMD_MEDIAFIFO(uint32_t ptr, uint32_t size);
void EVE_CMD_SYNC(void);
void EVE_CMD_ROMFONT(uint32_t font, uint32_t romslot);
void EVE_CMD_PLAYVIDEO(uint32_t options);
void EVE_CMD_VIDEOFRAME(uint32_t dst, uint32_t ptr);
void EVE_CMD_VIDEOSTART(void);
void EVE_CMD_SETBASE(uint32_t base);
void EVE_CMD_SETBITMAP(uint32_t source, uint16_t fmt, uint16_t w, uint16_t h);
void EVE_CMD_SETSCRATCH(uint32_t handle);
#endif // IS_EVE_API(2, 3, 4, 5)

#if IS_EVE_API(2, 3, 4) // FT80x, BT82x API change
void EVE_CMD_SETFONT2(uint32_t font, uint32_t ptr, uint32_t firstchar);
void EVE_CMD_SNAPSHOT2(uint32_t fmt, uint32_t ptr, int16_t x, int16_t y, int16_t w, int16_t h);
#endif // IS_EVE_API(2, 3, 4)

#if IS_EVE_API(3, 4)
void EVE_CMD_INFLATE2(uint32_t ptr, uint32_t options);
void EVE_CMD_CLEARCACHE(void);
void EVE_CMD_INTRAMSHARED(uint32_t ptr);
void EVE_CMD_VIDEOSTARTF(void);
#endif // IS_EVE_API(3, 4)

#if IS_EVE_API(3, 4, 5)
void EVE_CMD_ANIMSTART(int32_t ch, uint32_t aoptr, uint32_t loop);
void EVE_CMD_ANIMSTOP(int32_t ch);
void EVE_CMD_ANIMXY(int32_t ch, int16_t x, int16_t y);
void EVE_CMD_ANIMDRAW(int32_t ch);
void EVE_CMD_ANIMFRAME(int16_t x, int16_t y, uint32_t aoptr, uint32_t frame);
void EVE_CMD_APPENDF(uint32_t ptr, uint32_t num);
#endif // IS_EVE_API(3, 4, 5)

#if IS_EVE_API(4)
void EVE_CMD_ANIMFRAMERAM(int16_t x, int16_t y, uint32_t aoptr, uint32_t frame );
void EVE_CMD_ANIMSTARTRAM(int32_t ch, uint32_t aoptr, uint32_t loop);
void EVE_CMD_APILEVEL(uint32_t level);
void EVE_CMD_FONTCACHE(uint32_t font, int32_t ptr, uint32_t num);
void EVE_CMD_FONTCACHEQUERY(uint32_t total, int32_t used);
void EVE_CMD_HSF(uint32_t w );
void EVE_CMD_PCLKFREQ(uint32_t ftarget, int32_t rounding, uint32_t factual);
#endif // IS_EVE_API(4)

#if IS_EVE_API(4, 5)
void EVE_CMD_RUNANIM(uint32_t waitmask, uint32_t play);
void EVE_CMD_TESTCARD(void);
void EVE_CMD_WAIT(uint32_t us);
void EVE_CMD_NEWLIST(uint32_t a);
void EVE_CMD_ENDLIST(void);
void EVE_CMD_CALLLIST(uint32_t a);
void EVE_CMD_RETURN(void);
#endif // IS_EVE_API(4, 5)

#if IS_EVE_API(3, 4, 5)
void EVE_CMD_NOP(void);
void EVE_CMD_FILLWIDTH(uint32_t s); 
void EVE_CMD_ROTATEAROUND(int32_t x, int32_t y, int32_t a, int32_t s);
void EVE_CMD_RESETFONTS(void);
void EVE_CMD_GRADIENTA(int16_t x0, int16_t y0, uint32_t argb0, int16_t x1, int16_t y1, uint32_t argb1);
void EVE_CMD_FLASHERASE(void);
void EVE_CMD_FLASHWRITEEXT(uint32_t dest, uint32_t num, uint8_t *data);
void EVE_CMD_FLASHWRITE(uint32_t ptr, uint32_t num);
void EVE_CMD_FLASHUPDATE(uint32_t dest, uint32_t src, uint32_t num);
void EVE_CMD_FLASHREAD(uint32_t dest, uint32_t src, uint32_t num);
void EVE_CMD_FLASHPROGRAM(uint32_t dest, uint32_t src, uint32_t num);
void EVE_CMD_FLASHSOURCE(uint32_t ptr);
void EVE_CMD_FLASHSPITX(uint32_t num);
void EVE_CMD_FLASHFAST(uint32_t result);
void EVE_CMD_FLASHSPIRX(uint32_t ptr, uint32_t num);
void EVE_CMD_FLASHATTACH(void);
void EVE_CMD_FLASHDETATCH(void);
void EVE_CMD_FLASHSPIDESEL(void);
#endif // IS_EVE_API(3, 4, 5)

#if IS_EVE_API(4, 5)
void EVE_CMD_GETIMAGE(uint32_t source, uint32_t fmt, uint32_t w, uint32_t h, uint32_t palette);
void EVE_CMD_CALIBRATESUB(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t result);
#endif // IS_EVE_API(4, 5)

#if IS_EVE_API(5)
void EVE_CMD_COPYLIST(uint32_t dst);
void EVE_CMD_CGRADIENT(uint32_t shape, int16_t x, int16_t y, int16_t w, int16_t h, uint32_t rgb0, uint32_t rgb1);
void EVE_CMD_TEXTDIM(uint32_t dimensions, int16_t font, uint16_t options, const char* string, ...);
void EVE_CMD_ARC(int16_t x, int16_t y, uint16_t r0, uint16_t r1, uint16_t a0, uint16_t a1);
void EVE_CMD_RENDERTARGET(uint32_t dest, uint16_t fmt, uint16_t w, uint16_t h);
void EVE_CMD_ENABLEREGION(uint32_t en);
void EVE_CMD_FENCE(void);
void EVE_CMD_GRAPHICSFINISH(void);
void EVE_CMD_REGWRITE(uint32_t a, uint32_t b);
void EVE_CMD_APBWRITE(uint32_t a, uint32_t b);
void EVE_CMD_APBREAD(uint32_t a, uint32_t result);
void EVE_CMD_LOADWAV(uint32_t dst, uint32_t options);
void EVE_CMD_LOADASSET(uint32_t dst, uint32_t options);
void EVE_CMD_LOADPATCH(uint32_t options);
void EVE_CMD_GLOW(int16_t x, int16_t y, int16_t w, int16_t h);
void EVE_CMD_SDATTACH(uint32_t options, uint32_t result);
void EVE_CMD_FSOPTIONS(uint32_t options);
void EVE_CMD_FSREAD(uint32_t dst, const char* filename, uint32_t result);
void EVE_CMD_FSSIZE(const char* filename, uint32_t size);
void EVE_CMD_FSSOURCE(const char* filename, uint32_t result);
void EVE_CMD_FSDIR(uint32_t dst, uint32_t num, const char* path, uint32_t result);
void EVE_CMD_SDBLOCKREAD(uint32_t dst, uint32_t src, uint32_t count, uint32_t result);
void EVE_CMD_WAITCHANGE(uint32_t a);
void EVE_CMD_WAITCOND(uint32_t a, uint32_t func, uint32_t ref, uint32_t mask);
void EVE_CMD_RESULT(uint32_t a);
void EVE_CMD_I2SSTARTUP(uint32_t freq);
#endif // IS_EVE_API(5) 

#endif  /* _EVE_HEADER_H */
