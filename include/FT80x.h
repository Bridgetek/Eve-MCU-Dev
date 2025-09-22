/**
 @file FT80x.h
 @brief Definitions for FT80x devices.
 @details Supports FT800/FT801.
 */
/*
 * ============================================================================
 * History
 * =======
 * Dec 2024        New file split from FT8xx.h
 *
 *
 *
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

#ifndef _FT80X_H_
#define _FT80X_H_

//###########################################################################
//#####################################  EVE 1 ##############################
//###########################################################################

// Clock lower boundary of trimming
#define EVE_LOW_FREQ_BOUND             47040000L//98% of 48Mhz

// Memory Map

#define EVE_RAM_G                      0x0ul           // RAM_G
#define EVE_RAM_G_SIZE                 (256*1024L)
#define EVE_RAM_CMD                    0x108000ul      // RAM_CMD
#define EVE_RAM_CMD_SIZE               (4*1024L)
#define EVE_RAM_DL                     0x100000ul      // RAM_DL
#define EVE_RAM_DL_SIZE                (8*1024L)
#define EVE_RAM_REG                    0x102400ul      // Registers
#define EVE_ROM_FONT                   0x0bb23cul      // Font table and bitmap
#define EVE_ROMFONT_TABLEADDRESS       0x0ffffcul      // Font table pointer address
#define EVE_RAM_PAL                    0x102000ul      // Palette RAM
#define EVE_ROM_CHIPID                 0x0c0000ul      // Chip ID

// Coprocessor Method

// Use REG_CMDB_WRITE not supported on FT80x.
#undef EVE_USE_CMDB_METHOD

// Chip ID Register

#define EVE_REG_ID                     0x102400ul

// Display Registers 

#define EVE_REG_PCLK                   0x10246cul
#define EVE_REG_PCLK_POL               0x102468ul
#define EVE_REG_CSPREAD                0x102464ul
#define EVE_REG_SWIZZLE                0x102460ul
#define EVE_REG_DITHER                 0x10245cul
#define EVE_REG_OUTBITS                0x102458ul
#define EVE_REG_ROTATE                 0x102454ul
#define EVE_REG_VSYNC1                 0x10244cul
#define EVE_REG_VSYNC0                 0x102448ul
#define EVE_REG_VSIZE                  0x102444ul
#define EVE_REG_VOFFSET                0x102440ul
#define EVE_REG_VCYCLE                 0x10243cul
#define EVE_REG_HSYNC1                 0x102438ul
#define EVE_REG_HSYNC0                 0x102434ul
#define EVE_REG_HSIZE                  0x102430ul
#define EVE_REG_HOFFSET                0x10242cul
#define EVE_REG_HCYCLE                 0x102428ul

#define EVE_REG_TAP_MASK               0x102424ul
#define EVE_REG_TAP_CRC                0x102420ul

#define EVE_REG_DLSWAP                 0x102450ul

// Touch Registers 

#define EVE_REG_TAG                    0x102478ul
#define EVE_REG_TAG_Y                  0x102474ul
#define EVE_REG_TAG_X                  0x102470ul

#define EVE_REG_TOUCH_ADC_MODE         0x1024f4ul
#define EVE_REG_TOUCH_SCREEN_XY        0x102510ul
#define EVE_REG_TOUCH_RAW_XY           0x102508ul
#define EVE_REG_TOUCH_DIRECT_XY        0x102574ul
#define EVE_REG_TOUCH_DIRECT_Z1Z2      0x102578ul
#define EVE_REG_ANALOG                 0x102538ul
#define EVE_REG_TOUCH_RZ               0x10250cul
#define EVE_REG_TOUCH_TRANSFORM_A      0x10251cul
#define EVE_REG_TOUCH_TRANSFORM_B      0x102520ul
#define EVE_REG_TOUCH_TRANSFORM_C      0x102524ul
#define EVE_REG_TOUCH_TRANSFORM_D      0x102528ul
#define EVE_REG_TOUCH_TRANSFORM_E      0x10252cul
#define EVE_REG_TOUCH_TRANSFORM_F      0x102530ul
#define EVE_REG_TOUCH_TAG              0x102518ul
// FT801 specific mapped to same addresses as FT800 above
#define EVE_REG_CTOUCH_EXTENDED        0x1024f4ul
#define EVE_REG_CTOUCH_TOUCH0_XY       0x102510ul
#define EVE_REG_CTOUCH_RAW_XY          0x102508ul   // FT801 Compatibility mode
#define EVE_REG_CTOUCH_TOUCH1_XY       0x102508ul   // FT801 Extended mode
#define EVE_REG_CTOUCH_TOUCH2_XY       0x102574ul   // FT801 Extended mode
#define EVE_REG_CTOUCH_TOUCH3_XY       0x102578ul   // FT801 Extended mode
#define EVE_REG_CTOUCH_TOUCH4_X        0x102538ul   // FT801 Extended mode
#define EVE_REG_CTOUCH_TOUCH4_Y        0x10250cul   // FT801 Extended mode
#define EVE_REG_CTOUCH_TRANSFORM_A     0x10251cul
#define EVE_REG_CTOUCH_TRANSFORM_B     0x102520ul
#define EVE_REG_CTOUCH_TRANSFORM_C     0x102524ul
#define EVE_REG_CTOUCH_TRANSFORM_D     0x102528ul
#define EVE_REG_CTOUCH_TRANSFORM_E     0x10252cul
#define EVE_REG_CTOUCH_TRANSFORM_F     0x102530ul
#define EVE_REG_CTOUCH_TAG             0x102518ul

#define EVE_REG_TOUCH_TAG_XY           0x102514ul
#define EVE_REG_TOUCH_RZTHRESH         0x102504ul
#define EVE_REG_TOUCH_OVERSAMPLE       0x102500ul
#define EVE_REG_TOUCH_SETTLE           0x1024fcul
#define EVE_REG_TOUCH_CHARGE           0x1024f8ul
#define EVE_REG_TOUCH_MODE             0x1024f0ul

// Media Registers

#define EVE_REG_PLAY                   0x102488ul
#define EVE_REG_SOUND                  0x102484ul
#define EVE_REG_VOL_SOUND              0x102480ul
#define EVE_REG_VOL_PB                 0x10247cul
#define EVE_REG_PLAYBACK_PLAY          0x1024bcul
#define EVE_REG_PLAYBACK_LOOP          0x1024b8ul
#define EVE_REG_PLAYBACK_FORMAT        0x1024b4ul
#define EVE_REG_PLAYBACK_FREQ          0x1024b0ul
#define EVE_REG_PLAYBACK_READPTR       0x1024acul
#define EVE_REG_PLAYBACK_LENGTH        0x1024a8ul
#define EVE_REG_PLAYBACK_START         0x1024a4ul

// Display List Registers

#define EVE_REG_CMD_DL                 0x1024ecul
#define EVE_REG_CMD_WRITE              0x1024e8ul
#define EVE_REG_CMD_READ               0x1024e4ul

// Tracking Registers

#define EVE_REG_TRACK                  0x109000ul

// PWM Registers

#define EVE_REG_PWM_DUTY               0x1024c4ul
#define EVE_REG_PWM_HZ                 0x1024c0ul

// Interrupt Registers

#define EVE_REG_INT_MASK               0x1024a0ul
#define EVE_REG_INT_EN                 0x10249cul
#define EVE_REG_INT_FLAGS              0x102498ul
#define EVE_REG_GPIO                   0x102490ul
#define EVE_REG_GPIO_DIR               0x10248cul
#define EVE_REG_CPURESET               0x10241cul

// Screenshot Registers

#define EVE_REG_SCREENSHOT_READ        0x102554ul
#define EVE_REG_SCREENSHOT_BUSY        0x1024d8ul
#define EVE_REG_SCREENSHOT_START       0x102418ul   //
#define EVE_REG_SCREENSHOT_Y           0x102414ul   //
#define EVE_REG_SCREENSHOT_EN          0x102410ul   //

// Clock Registers

#define EVE_REG_FREQUENCY              0x10240cul
#define EVE_REG_CLOCK                  0x102408ul
#define EVE_REG_FRAMES                 0x102404ul
#define EVE_REG_TRIM                   0x10256cul

// Macro Registers

#define EVE_REG_MACRO_0                0x1024c8ul
#define EVE_REG_MACRO_1                0x1024ccul

#define EVE_REG_ROMSUB_SEL             0x1024e0ul

// Bitmap Address Mask 

#define EVE_ENC_BITMAP_ADDR_MASK       0xffffful

// Encoded Command Macros

#define EVE_ENC_ALPHA_FUNC(func,ref)        ((0x9ul << 24)|(((func) & 0x7ul) << 8)|(((ref) & 0xfful) << 0))
#define EVE_ENC_BEGIN(prim)                 ((0x1ful << 24)|(((prim) & 0xFul) << 0))
#define EVE_ENC_BITMAP_HANDLE(handle)       ((0x5ul << 24)|(((handle) & 0x1ful) << 0))
#define EVE_ENC_BITMAP_LAYOUT(format,linestride,height) ((0x7ul << 24)|(((format) & 0x1ful) << 19)|(((linestride) & 0x3fful) << 9)|(((height) & 0x1fful) << 0))
#define EVE_ENC_BITMAP_SIZE(filter,wrapx,wrapy,width,height) ((0x8ul << 24)|(((filter) & 0x1ul) << 20)|(((wrapx) & 0x1ul) << 19)|(((wrapy) & 0x1ul) << 18)|(((width) & 0x1fful) << 9)|(((height) & 0x1fful) << 0))
#define EVE_ENC_BITMAP_SOURCE(addr)         ((0x1ul << 24) | ((addr)&EVE_ENC_BITMAP_ADDR_MASK))
#define EVE_ENC_BITMAP_TRANSFORM_A(a)       ((0x15ul << 24)|((((uint32_t)(a)) & 0x1FFFFul) << 0))
#define EVE_ENC_BITMAP_TRANSFORM_B(b)       ((0x16ul << 24)|((((uint32_t)(b)) & 0x1FFFFul) << 0))
#define EVE_ENC_BITMAP_TRANSFORM_C(c)       ((0x17ul << 24)|((((uint32_t)(c)) & 0xFFFFFFul) << 0))
#define EVE_ENC_BITMAP_TRANSFORM_D(d)       ((0x18ul << 24)|((((uint32_t)(d)) & 0x1FFFFul) << 0))
#define EVE_ENC_BITMAP_TRANSFORM_E(e)       ((0x19ul << 24)|((((uint32_t)(e)) & 0x1FFFFul) << 0))
#define EVE_ENC_BITMAP_TRANSFORM_F(f)       ((0x1aul << 24)|((((uint32_t)(f)) & 0xFFFFFFul) << 0))
#define EVE_ENC_BLEND_FUNC(src,dst)         ((0xbul << 24)|(((src) & 0x7ul) << 3)|(((dst) & 0x7ul) << 0))
#define EVE_ENC_CALL(dest)                  ((0x1dul << 24)|(((dest) & 0xFFFFul) << 0))
#define EVE_ENC_CELL(cell)                  ((0x6ul << 24)|(((cell) & 0x7ful) << 0))
#define EVE_ENC_CLEAR_COLOR_A(alpha)        ((0xful << 24)|(((alpha) & 0xfful) << 0))
#define EVE_ENC_CLEAR_COLOR_RGB(red,green,blue) ((0x2ul << 24)|(((red) & 0xfful) << 16)|(((green) & 0xfful) << 8)|(((blue) & 0xfful) << 0))
#define EVE_ENC_CLEAR_COLOR(c)              ((0x2ul << 24)|(((uint32_t)(c)) & 0x00fffffful))
#define EVE_ENC_CLEAR_STENCIL(s)            ((0x11ul << 24)|((((uint32_t)(s)) & 0xfful) << 0))
#define EVE_ENC_CLEAR_TAG(s)                ((0x12ul << 24)|((((uint32_t)(s)) & 0xfful) << 0))
#define EVE_ENC_CLEAR(c,s,t)                ((0x26ul << 24)|((((uint32_t)(c)) & 0x1ul) << 2)|((((uint32_t)(s)) & 0x1ul) << 1)|((((uint32_t)(t)) & 0x1ul) << 0))
#define EVE_ENC_COLOR_A(alpha)              ((0x10ul << 24)|(((alpha) & 0xfful) << 0))
#define EVE_ENC_COLOR_MASK(r,g,b,a)         ((0x20ul << 24)|((((uint32_t)(r)) & 0x1ul) << 3)|((((uint32_t)(g)) & 0x1ul) << 2)|((((uint32_t)(b)) & 0x1ul) << 1)|((((uint32_t)(a)) & 0x1ul) << 0))
#define EVE_ENC_COLOR_RGB(red,green,blue)   ((0x4ul << 24)|(((red) & 0xfful) << 16)|(((green) & 0xfful) << 8)|(((blue) & 0xfful) << 0))
#define EVE_ENC_COLOR(c)                    ((0x4ul << 24)|(((uint32_t)(c)) & 0x00fffffful))
#define EVE_ENC_DISPLAY()                   ((0x0ul << 24))
#define EVE_ENC_END()                       ((0x21ul << 24))
#define EVE_ENC_JUMP(dest)                  ((0x1eul << 24)|(((dest) & 0xFFFFul) << 0))
#define EVE_ENC_LINE_WIDTH(width)           ((0xeul << 24)|(((width) & 0xFFFul) << 0))
#define EVE_ENC_MACRO(m)                    ((0x25ul << 24)|((((uint32_t)(m)) & 0x1ul) << 0))
#define EVE_ENC_POINT_SIZE(size)            ((0xdul << 24)|(((size) & 0x1FFFul) << 0))
#define EVE_ENC_RESTORE_CONTEXT()           ((0x23ul << 24))
#define EVE_ENC_RETURN()                    ((0x24ul << 24))
#define EVE_ENC_SAVE_CONTEXT()              ((0x22ul << 24))
#define EVE_ENC_SCISSOR_SIZE(width,height)  ((0x1cul << 24)|(((width) & 0x3fful) << 10)|(((height) & 0x3fful) << 0))
#define EVE_ENC_SCISSOR_XY(x,y)             ((0x1bul << 24)|((((uint32_t)(x)) & 0x1fful) << 9)|((((uint32_t)(y)) & 0x1fful) << 0))
#define EVE_ENC_STENCIL_FUNC(func,ref,mask) ((0xaul << 24)|(((func) & 0x7ul) << 16)|(((ref) & 0xfful) << 8)|(((mask) & 0xfful) << 0))
#define EVE_ENC_STENCIL_MASK(mask)          ((0x13ul << 24)|(((mask) & 0xfful) << 0))
#define EVE_ENC_STENCIL_OP(sfail,spass)     ((0xcul << 24)|(((sfail) & 0x7ul) << 3)|(((spass) & 0x7ul) << 0))
#define EVE_ENC_TAG_MASK(mask)              ((0x14ul << 24)|(((mask) & 0x1ul) << 0))
#define EVE_ENC_TAG(s)                      ((0x3ul << 24)|((((uint32_t)(s)) & 0xfful) << 0))
#define EVE_ENC_VERTEX2F(x,y)               ((0x1ul << 30)|((((uint32_t)(x)) & 0xfffful) << 15)|((((uint32_t)(y)) & 0xfffful) << 0))
#define EVE_ENC_VERTEX2II(x,y,handle,cell)  ((0x2ul << 30)|((((uint32_t)(x)) & 0x1fful) << 21)|((((uint32_t)(y)) & 0x1fful) << 12)|(((handle) & 0x1ful) << 7)|(((cell) & 0x7ful) << 0))

// Co-processor Commands

#define EVE_ENC_CMD_APPEND             0xffffff1eul
#define EVE_ENC_CMD_BGCOLOR            0xffffff09ul
#define EVE_ENC_CMD_BITMAP_TRANSFORM   0xffffff21ul
#define EVE_ENC_CMD_BUTTON             0xffffff0dul
#define EVE_ENC_CMD_CALIBRATE          0xffffff15ul
#define EVE_ENC_CMD_CLOCK              0xffffff14ul
#define EVE_ENC_CMD_COLDSTART          0xffffff32ul
#define EVE_ENC_CMD_CSKETCH            0xffffff35ul
#define EVE_ENC_CMD_DIAL               0xffffff2dul
#define EVE_ENC_CMD_DLSTART            0xffffff00ul
#define EVE_ENC_CMD_FGCOLOR            0xffffff0aul
#define EVE_ENC_CMD_GAUGE              0xffffff13ul
#define EVE_ENC_CMD_GETMATRIX          0xffffff33ul
#define EVE_ENC_CMD_GETPROPS           0xffffff25ul
#define EVE_ENC_CMD_GETPTR             0xffffff23ul
#define EVE_ENC_CMD_GRADCOLOR          0xffffff34ul
#define EVE_ENC_CMD_GRADIENT           0xffffff0bul
#define EVE_ENC_CMD_INFLATE            0xffffff22ul
#define EVE_ENC_CMD_INTERRUPT          0xffffff02ul
#define EVE_ENC_CMD_KEYS               0xffffff0eul
#define EVE_ENC_CMD_LOADIDENTITY       0xffffff26ul
#define EVE_ENC_CMD_LOADIMAGE          0xffffff24ul
#define EVE_ENC_CMD_LOGO               0xffffff31ul
#define EVE_ENC_CMD_MEMCPY             0xffffff1dul
#define EVE_ENC_CMD_MEMCRC             0xffffff18ul
#define EVE_ENC_CMD_MEMSET             0xffffff1bul
#define EVE_ENC_CMD_MEMWRITE           0xffffff1aul
#define EVE_ENC_CMD_MEMZERO            0xffffff1cul
#define EVE_ENC_CMD_NUMBER             0xffffff2eul
#define EVE_ENC_CMD_PROGRESS           0xffffff0ful
#define EVE_ENC_CMD_REGREAD            0xffffff19ul
#define EVE_ENC_CMD_ROTATE             0xffffff29ul
#define EVE_ENC_CMD_SCALE              0xffffff28ul
#define EVE_ENC_CMD_SCREENSAVER        0xffffff2ful
#define EVE_ENC_CMD_SCROLLBAR          0xffffff11ul
#define EVE_ENC_CMD_SETFONT            0xffffff2bul
#define EVE_ENC_CMD_SETMATRIX          0xffffff2aul
#define EVE_ENC_CMD_SKETCH             0xffffff30ul
#define EVE_ENC_CMD_SLIDER             0xffffff10ul
#define EVE_ENC_CMD_SNAPSHOT           0xffffff1ful
#define EVE_ENC_CMD_SPINNER            0xffffff16ul
#define EVE_ENC_CMD_STOP               0xffffff17ul
#define EVE_ENC_CMD_SWAP               0xffffff01ul
#define EVE_ENC_CMD_TEXT               0xffffff0cul
#define EVE_ENC_CMD_TOGGLE             0xffffff12ul
#define EVE_ENC_CMD_TRACK              0xffffff2cul
#define EVE_ENC_CMD_TRANSLATE          0xffffff27ul

// Bitmap Formats

#define EVE_FORMAT_ARGB1555            0x0ul
#define EVE_FORMAT_ARGB2               0x5ul
#define EVE_FORMAT_ARGB4               0x6ul
#define EVE_FORMAT_BARGRAPH            0xbul
#define EVE_FORMAT_L1                  0x1ul
#define EVE_FORMAT_L4                  0x2ul
#define EVE_FORMAT_L8                  0x3ul
#define EVE_FORMAT_PALETTED            0x8ul
#define EVE_FORMAT_RGB332              0x4ul
#define EVE_FORMAT_RGB565              0x7ul
#define EVE_FORMAT_TEXT8X8             0x9ul
#define EVE_FORMAT_TEXTVGA             0xaul

// Command Options

#define EVE_BEGIN_BITMAPS              0x1ul
#define EVE_BEGIN_EDGE_STRIP_A         0x7ul
#define EVE_BEGIN_EDGE_STRIP_B         0x8ul
#define EVE_BEGIN_EDGE_STRIP_L         0x6ul
#define EVE_BEGIN_EDGE_STRIP_R         0x5ul
#define EVE_BEGIN_LINE_STRIP           0x4ul
#define EVE_BEGIN_LINES                0x3ul
#define EVE_BEGIN_POINTS               0x2ul
#define EVE_BEGIN_RECTS                0x9ul

#define EVE_TEST_ALWAYS                0x7ul
#define EVE_TEST_EQUAL                 0x5ul
#define EVE_TEST_GEQUAL                0x4ul
#define EVE_TEST_GREATER               0x3ul
#define EVE_TEST_LEQUAL                0x2ul
#define EVE_TEST_LESS                  0x1ul
#define EVE_TEST_NEVER                 0x0ul
#define EVE_TEST_NOTEQUAL              0x6ul

#define EVE_FILTER_BILINEAR            0x1ul
#define EVE_FILTER_NEAREST             0x0ul

#define EVE_WRAP_BORDER                0x0ul
#define EVE_WRAP_REPEAT                0x1ul

#define EVE_BLEND_DST_ALPHA            0x3ul
#define EVE_BLEND_ONE                  0x1ul
#define EVE_BLEND_ONE_MINUS_DST_ALPHA  0x5ul
#define EVE_BLEND_ONE_MINUS_SRC_ALPHA  0x4ul
#define EVE_BLEND_SRC_ALPHA            0x2ul
#define EVE_BLEND_ZERO                 0x0ul

#define EVE_STENCIL_DECR               0x4ul
#define EVE_STENCIL_INCR               0x3ul
#define EVE_STENCIL_INVERT             0x5ul
#define EVE_STENCIL_KEEP               0x1ul
#define EVE_STENCIL_REPLACE            0x2ul
#define EVE_STENCIL_ZERO               0x0ul

#define EVE_OPT_3D                     0x0ul
#define EVE_OPT_CENTER                 0x600ul
#define EVE_OPT_CENTERX                0x200ul
#define EVE_OPT_CENTERY                0x400ul
#define EVE_OPT_FLAT                   0x100ul
#define EVE_OPT_MONO                   0x1ul
#define EVE_OPT_NOBACK                 0x1000ul
#define EVE_OPT_NODL                   0x2ul
#define EVE_OPT_NOHANDS                0xc000ul
#define EVE_OPT_NOHM                   0x4000ul
#define EVE_OPT_NOPOINTER              0x4000ul
#define EVE_OPT_NOSECS                 0x8000ul
#define EVE_OPT_NOTICKS                0x2000ul
#define EVE_OPT_RGB565                 0x0ul
#define EVE_OPT_RIGHTX                 0x800ul
#define EVE_OPT_SIGNED                 0x100ul

// Touchmode Options

#define EVE_TOUCHMODE_CONTINUOUS       0x3ul
#define EVE_TOUCHMODE_FRAME            0x2ul
#define EVE_TOUCHMODE_OFF              0x0ul
#define EVE_TOUCHMODE_ONESHOT          0x1ul

// Display List Swap Options

#define EVE_DLSWAP_DONE                0x0ul
#define EVE_DLSWAP_FRAME               0x2ul
#define EVE_DLSWAP_LINE                0x1ul

// Interrupt Options

#define EVE_INT_CMDEMPTY               0x20ul
#define EVE_INT_CMDFLAG                0x40ul
#define EVE_INT_CONVCOMPLETE           0x80ul
#define EVE_INT_PLAYBACK               0x10ul
#define EVE_INT_SOUND                  0x8ul
#define EVE_INT_SWAP                   0x1ul
#define EVE_INT_TAG                    0x4ul
#define EVE_INT_TOUCH                  0x2ul

// Audio Playback Options

#define EVE_LINEAR_SAMPLES             0x0ul
#define EVE_ULAW_SAMPLES               0x1ul
#define EVE_ADPCM_SAMPLES              0x2ul
#define EVE_VOL_ZERO                   0x0ul

// ADC Options

#define EVE_ADC_DIFFERENTIAL           0x1ul
#define EVE_ADC_SINGLE_ENDED           0x0ul

// Arrays containing the bitmap sizes of ROM fonts

#define EVE_ROMFONT_MAX 31

#define EVE_ROMFONT_WIDTHS {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, \
    8,8,8,8,10,13,14,17,24,30, \
    12,16,18,22,28,36}
#define EVE_ROMFONT_HEIGHTS {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, \
    8,8,16,16,13,17,20,22,29,38, \
    16,20,25,28,36,49}
#define EVE_ROMFONT_FORMATS {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, \
    EVE_FORMAT_L1,EVE_FORMAT_L1,EVE_FORMAT_L1,EVE_FORMAT_L1,EVE_FORMAT_L1, \
    EVE_FORMAT_L1,EVE_FORMAT_L1,EVE_FORMAT_L1,EVE_FORMAT_L1,EVE_FORMAT_L1, \
    EVE_FORMAT_L4,EVE_FORMAT_L4,EVE_FORMAT_L4,EVE_FORMAT_L4,EVE_FORMAT_L4, \
    EVE_FORMAT_L4}

#endif    /* _FT80X_H_ */

