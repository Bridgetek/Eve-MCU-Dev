/**
 @file FT81x.h
 @brief Definitions for FT81x/BT88x devices.
 @details Supports FT810/FT811/FT812/FT813 and BT880/BT881/BT882/BT883.
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

#ifndef _FT81X_H_
#define _FT81X_H_

//###########################################################################
//#####################################  EVE 2 ##############################
//###########################################################################

/* For FT801 enable the switch in platform.h file */

// Clock lower boundary of trimming
#define EVE_LOW_FREQ_BOUND             58800000L //98% of 60Mhz

// Memory Map

#define EVE_RAM_G                      0x0ul
#if IS_EVE_API(2) && EVE_SUB_API == 2
#define EVE_RAM_G_SIZE                 (256*1024L)
#elif IS_EVE_API(2) && EVE_SUB_API == 1
#define EVE_RAM_G_SIZE                 (1024*1024L)
#else
#define EVE_RAM_G_SIZE                 (1024*1024L)
#endif
#define EVE_RAM_CMD                    0x308000ul
#define EVE_RAM_CMD_SIZE               (4*1024L)
#define EVE_RAM_DL                     0x300000ul
#define EVE_RAM_DL_SIZE                (8*1024L)
#define EVE_RAM_REG                    0x302000ul
#define EVE_RAM_ROMSUB                 0x30a000ul
#define EVE_ROMFONT_TABLEADDRESS       0x2ffffcul
#define EVE_ROM_CHIPID                 0x0c0000ul      // Chip ID

// Coprocessor Method

// Use REG_CMDB_WRITE to fill coprocessor buffer.
// Undefine to write directly to coprocessor buffer.
#define EVE_USE_CMDB_METHOD

// Chip ID Register

#define EVE_REG_ID                     0x302000ul

// Control Registers

#define EVE_REG_BIST_EN                0x302174ul
#define EVE_REG_BUSYBITS               0x3020e8ul // Remove?
#define EVE_REG_CPURESET               0x302020ul

// Display Registers 

#define EVE_REG_PCLK                   0x302070ul
#define EVE_REG_PCLK_POL               0x30206cul
#define EVE_REG_CSPREAD                0x302068ul
#define EVE_REG_SWIZZLE                0x302064ul
#define EVE_REG_DITHER                 0x302060ul
#define EVE_REG_OUTBITS                0x30205cul
#define EVE_REG_ROTATE                 0x302058ul
#define EVE_REG_VSYNC0                 0x30204cul
#define EVE_REG_VSYNC1                 0x302050ul
#define EVE_REG_VSIZE                  0x302048ul
#define EVE_REG_VOFFSET                0x302044ul
#define EVE_REG_VCYCLE                 0x302040ul
#define EVE_REG_HSYNC0                 0x302038ul
#define EVE_REG_HSYNC1                 0x30203cul
#define EVE_REG_HSIZE                  0x302034ul
#define EVE_REG_HOFFSET                0x302030ul
#define EVE_REG_HCYCLE                 0x30202cul

#define EVE_REG_TAP_MASK               0x302028ul
#define EVE_REG_TAP_CRC                0x302024ul

#define EVE_REG_DLSWAP                 0x302054ul

// Touch Registers 

#define EVE_REG_TAG                    0x30207cul
#define EVE_REG_TAG_X                  0x302074ul
#define EVE_REG_TAG_Y                  0x302078ul

#define EVE_REG_TOUCH_ADC_MODE         0x302108ul
#define EVE_REG_TOUCH_CHARGE           0x30210cul
#define EVE_REG_TOUCH_DIRECT_XY        0x30218cul
#define EVE_REG_TOUCH_DIRECT_Z1Z2      0x302190ul
#define EVE_REG_TOUCH_FAULT            0x302170ul
#define EVE_REG_TOUCH_MODE             0x302104ul
#define EVE_REG_TOUCH_OVERSAMPLE       0x302114ul
#define EVE_REG_TOUCH_RAW_XY           0x30211cul
#define EVE_REG_TOUCH_RZ               0x302120ul
#define EVE_REG_TOUCH_RZTHRESH         0x302118ul
#define EVE_REG_TOUCH_SCREEN_XY        0x302124ul
#define EVE_REG_TOUCH_SETTLE           0x302110ul
#define EVE_REG_TOUCH_TAG              0x30212cul
#define EVE_REG_TOUCH_TAG_XY           0x302128ul
#define EVE_REG_TOUCH_TAG1             0x302134ul
#define EVE_REG_TOUCH_TAG1_XY          0x302130ul
#define EVE_REG_TOUCH_TAG2             0x30213cul
#define EVE_REG_TOUCH_TAG2_XY          0x302138ul
#define EVE_REG_TOUCH_TAG3             0x302144ul
#define EVE_REG_TOUCH_TAG3_XY          0x302140ul
#define EVE_REG_TOUCH_TAG4             0x30214cul
#define EVE_REG_TOUCH_TAG4_XY          0x302148ul
#define EVE_REG_TOUCH_TRANSFORM_A      0x302150ul
#define EVE_REG_TOUCH_TRANSFORM_B      0x302154ul
#define EVE_REG_TOUCH_TRANSFORM_C      0x302158ul
#define EVE_REG_TOUCH_TRANSFORM_D      0x30215cul
#define EVE_REG_TOUCH_TRANSFORM_E      0x302160ul
#define EVE_REG_TOUCH_TRANSFORM_F      0x302164ul

#define EVE_REG_CTOUCH_EXTENDED        0x302108ul
#define EVE_REG_CTOUCH_TOUCH0_XY       0x302124ul
#define EVE_REG_CTOUCH_TOUCH1_XY       0x30211cul
#define EVE_REG_CTOUCH_TOUCH2_XY       0x30218cul
#define EVE_REG_CTOUCH_TOUCH3_XY       0x302190ul
#define EVE_REG_CTOUCH_TOUCH4_X        0x30216cul
#define EVE_REG_CTOUCH_TOUCH4_Y        0x302120ul
#define EVE_REG_TOUCH_CONFIG           0x302168ul

// Media Registers

#define EVE_REG_PLAY                   0x30208cul
#define EVE_REG_SOUND                  0x302088ul
#define EVE_REG_VOL_PB                 0x302080ul
#define EVE_REG_VOL_SOUND              0x302084ul
#define EVE_REG_PLAYBACK_FORMAT        0x3020c4ul
#define EVE_REG_PLAYBACK_FREQ          0x3020c0ul
#define EVE_REG_PLAYBACK_LENGTH        0x3020b8ul
#define EVE_REG_PLAYBACK_LOOP          0x3020c8ul
#define EVE_REG_PLAYBACK_PLAY          0x3020ccul
#define EVE_REG_PLAYBACK_READPTR       0x3020bcul
#define EVE_REG_PLAYBACK_START         0x3020b4ul
#define EVE_REG_MEDIAFIFO_READ         0x309014ul
#define EVE_REG_MEDIAFIFO_WRITE        0x309018ul

// Display List Registers

#define EVE_REG_CMD_DL                 0x302100ul
#define EVE_REG_CMD_READ               0x3020f8ul
#define EVE_REG_CMD_WRITE              0x3020fcul
#define EVE_REG_CMDB_SPACE             0x302574ul
#define EVE_REG_CMDB_WRITE             0x302578ul

// Tracking Registers

#define EVE_REG_TRACKER                0x309000ul
#define EVE_REG_TRACKER_1              0x309004ul
#define EVE_REG_TRACKER_2              0x309008ul
#define EVE_REG_TRACKER_3              0x30900cul
#define EVE_REG_TRACKER_4              0x309010ul

// PWM Registers

#define EVE_REG_PWM_DUTY               0x3020d4ul
#define EVE_REG_PWM_HZ                 0x3020d0ul

// Interrupt Registers

#define EVE_REG_INT_EN                 0x3020acul
#define EVE_REG_INT_FLAGS              0x3020a8ul
#define EVE_REG_INT_MASK               0x3020b0ul

// Screenshot Registers

#define EVE_REG_SNAPFORMAT             0x30201cul
#define EVE_REG_SNAPSHOT               0x302018ul
#define EVE_REG_SNAPY                  0x302014ul

// Clock Registers

#define EVE_REG_FREQUENCY              0x30200cul
#define EVE_REG_CLOCK                  0x302008ul
#define EVE_REG_FRAMES                 0x302004ul
#define EVE_REG_TRIM                   0x302180ul

// Macro Registers

#define EVE_REG_MACRO_0                0x3020d8ul
#define EVE_REG_MACRO_1                0x3020dcul

// GPIO Registers

#define EVE_REG_GPIO                   0x302094ul
#define EVE_REG_GPIO_DIR               0x302090ul
#define EVE_REG_GPIOX                  0x30209cul
#define EVE_REG_GPIOX_DIR              0x302098ul

// Registers

#define EVE_REG_RENDERMODE             0x302010ul
#define EVE_REG_ROMSUB_SEL             0x3020f0ul
#define EVE_REG_SPI_EARLY_TX           0x30217cul
#define EVE_REG_SPI_WIDTH              0x302188ul

// Bitmap Address Mask 

#define EVE_ENC_BITMAP_ADDR_MASK       0x3ffffful

// Encoded Command Macros

#define EVE_ENC_ALPHA_FUNC(func,ref)        ((0x9ul << 24)|(((func) & 0x7ul) << 8)|(((ref) & 0xfful) << 0))
#define EVE_ENC_BEGIN(prim)                 ((0x1ful << 24)|(((prim)&0xful) << 0))
#define EVE_ENC_BITMAP_HANDLE(handle)       ((0x5ul << 24)|(((handle) & 0x1ful) << 0))
#define EVE_ENC_BITMAP_LAYOUT(format,linestride,height) ((0x7ul << 24)|(((format) & 0x1ful) << 19)|(((linestride) & 0x3fful) << 9)|(((height) & 0x1fful) << 0))
#define EVE_ENC_BITMAP_LAYOUT_H(linestride,height) ((0x28ul << 24)|(((linestride) & 0x3ul) << 2)|(((height) & 0x3ul) << 0))
#define EVE_ENC_BITMAP_SIZE(filter,wrapx,wrapy,width,height) ((0x8ul << 24)|(((filter) & 0x1ul) << 20)|(((wrapx) & 0x1ul) << 19)|(((wrapy) & 0x1ul) << 18)|(((width) & 0x1fful) << 9)|(((height) & 0x1fful) << 0))
#define EVE_ENC_BITMAP_SIZE_H(width,height) ((0x29ul << 24)|(((width) & 0x3ul) << 2)|(((height) & 0x3ul) << 0))
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
#define EVE_ENC_CLEAR(c,s,t)                ((0x26ul << 24)|((((uint32_t)(c)) & 0x1ul) << 2)|((((uint32_t)(s)) & 0x1ul) << 1)|((((uint32_t)(t)) & 0x1ul) << 0))
#define EVE_ENC_CLEAR_COLOR_A(alpha)        ((0xful << 24)|(((alpha) & 0xfful) << 0))
#define EVE_ENC_CLEAR_COLOR_RGB(red,green,blue) ((0x2ul << 24)|(((red) & 0xfful) << 16)|(((green) & 0xfful) << 8)|(((blue) & 0xfful) << 0))
#define EVE_ENC_CLEAR_COLOR(c)              ((0x2ul << 24)|(((uint32_t)(c)) & 0x00fffffful))
#define EVE_ENC_CLEAR_STENCIL(s)            ((0x11ul << 24)|((((uint32_t)(s)) & 0xfful) << 0))
#define EVE_ENC_CLEAR_TAG(s)                ((0x12ul << 24)|((((uint32_t)(s)) & 0xfful) << 0))
#define EVE_ENC_COLOR_A(alpha)              ((0x10ul << 24)|(((alpha) & 0xfful) << 0))
#define EVE_ENC_COLOR_MASK(r,g,b,a)         ((0x20ul << 24)|((((uint32_t)(r)) & 0x1ul) << 3)|((((uint32_t)(g)) & 0x1ul) << 2)|((((uint32_t)(b)) & 0x1ul) << 1)|((((uint32_t)(a)) & 0x1ul) << 0))
#define EVE_ENC_COLOR_RGB(red,green,blue)   ((0x4ul << 24)|(((red) & 0xfful) << 16)|(((green) & 0xfful) << 8)|(((blue) & 0xfful) << 0))
#define EVE_ENC_COLOR(c)                    ((0x4ul << 24)|(((uint32_t)(c)) & 0x00fffffful))
#define EVE_ENC_DISPLAY()                   ((0x0ul << 24))
#define EVE_ENC_END()                       ((0x21ul << 24))
#define EVE_ENC_JUMP(dest)                  ((0x1eul << 24)|(((dest) & 0xFFFFul) << 0))
#define EVE_ENC_LINE_WIDTH(width)           ((0xeul << 24)|(((width) & 0xFFFul) << 0))
#define EVE_ENC_MACRO(m)                    ((0x25ul << 24)|((((uint32_t)(m)) & 0x1ul) << 0))
#define EVE_ENC_NOP()                       ((0x2dul << 24))
#define EVE_ENC_PALETTE_SOURCE(addr)        ((0x2aul << 24)|(((addr) & 0x3FFFFFul) << 0))
#define EVE_ENC_POINT_SIZE(size)            ((0xdul << 24)|(((size) & 0x1FFFul) << 0))
#define EVE_ENC_RESTORE_CONTEXT()           ((0x23ul << 24))
#define EVE_ENC_RETURN()                    ((0x24ul << 24))
#define EVE_ENC_SAVE_CONTEXT()              ((0x22ul << 24))
#define EVE_ENC_SCISSOR_SIZE(width,height)  ((0x1cul << 24)|(((width) & 0xFFFul) << 12)|(((height) & 0xFFFul) << 0))
#define EVE_ENC_SCISSOR_XY(x,y)             ((0x1bul << 24)|((((uint32_t)(x)) & 0x7FFul) << 11)|((((uint32_t)(y)) & 0x7FFul) << 0))
#define EVE_ENC_STENCIL_FUNC(func,ref,mask) ((0xaul << 24)|(((func) & 0x7ul) << 16)|(((ref) & 0xfful) << 8)|(((mask) & 0xfful) << 0))
#define EVE_ENC_STENCIL_MASK(mask)          ((0x13ul << 24)|(((mask) & 0xfful) << 0))
#define EVE_ENC_STENCIL_OP(sfail,spass)     ((0xcul << 24)|(((sfail) & 0x7ul) << 3)|(((spass) & 0x7ul) << 0))
#define EVE_ENC_TAG_MASK(mask)              ((0x14ul << 24)|(((mask) & 0x1ul) << 0))
#define EVE_ENC_TAG(s)                      ((0x3ul << 24)|((((uint32_t)(s)) & 0xfful) << 0))
#define EVE_ENC_VERTEX_FORMAT(frac)         ((0x27ul << 24)|(((frac) & 0x7ul) << 0))
#define EVE_ENC_VERTEX_TRANSLATE_X(x)       ((0x2bul << 24)|((((uint32_t)(x)) & 0x1FFFFul) << 0))
#define EVE_ENC_VERTEX_TRANSLATE_Y(y)       ((0x2cul << 24)|((((uint32_t)(y)) & 0x1FFFFul) << 0))
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
#define EVE_ENC_CMD_MEDIAFIFO          0xffffff39ul
#define EVE_ENC_CMD_MEMCPY             0xffffff1dul
#define EVE_ENC_CMD_MEMCRC             0xffffff18ul
#define EVE_ENC_CMD_MEMSET             0xffffff1bul
#define EVE_ENC_CMD_MEMWRITE           0xffffff1aul
#define EVE_ENC_CMD_MEMZERO            0xffffff1cul
#define EVE_ENC_CMD_NUMBER             0xffffff2eul
#define EVE_ENC_CMD_PLAYVIDEO          0xffffff3aul
#define EVE_ENC_CMD_PROGRESS           0xffffff0ful
#define EVE_ENC_CMD_REGREAD            0xffffff19ul
#define EVE_ENC_CMD_ROMFONT            0xffffff3ful
#define EVE_ENC_CMD_ROTATE             0xffffff29ul
#define EVE_ENC_CMD_SCALE              0xffffff28ul
#define EVE_ENC_CMD_SCREENSAVER        0xffffff2ful
#define EVE_ENC_CMD_SCROLLBAR          0xffffff11ul
#define EVE_ENC_CMD_SETBASE            0xffffff38ul
#define EVE_ENC_CMD_SETBITMAP          0xffffff43ul
#define EVE_ENC_CMD_SETFONT            0xffffff2bul
#define EVE_ENC_CMD_SETFONT2           0xffffff3bul
#define EVE_ENC_CMD_SETMATRIX          0xffffff2aul
#define EVE_ENC_CMD_SETROTATE          0xffffff36ul
#define EVE_ENC_CMD_SETSCRATCH         0xffffff3cul
#define EVE_ENC_CMD_SKETCH             0xffffff30ul
#define EVE_ENC_CMD_SLIDER             0xffffff10ul
#define EVE_ENC_CMD_SNAPSHOT           0xffffff1ful
#define EVE_ENC_CMD_SNAPSHOT2          0xffffff37ul
#define EVE_ENC_CMD_SPINNER            0xffffff16ul
#define EVE_ENC_CMD_STOP               0xffffff17ul
#define EVE_ENC_CMD_SWAP               0xffffff01ul
#define EVE_ENC_CMD_SYNC               0xffffff42ul
#define EVE_ENC_CMD_TEXT               0xffffff0cul
#define EVE_ENC_CMD_TOGGLE             0xffffff12ul
#define EVE_ENC_CMD_TRACK              0xffffff2cul
#define EVE_ENC_CMD_TRANSLATE          0xffffff27ul
#define EVE_ENC_CMD_VIDEOFRAME         0xffffff41ul
#define EVE_ENC_CMD_VIDEOSTART         0xffffff40ul

// Bitmap Formats

#define EVE_FORMAT_ARGB1555            0x0ul
#define EVE_FORMAT_ARGB2               0x5ul
#define EVE_FORMAT_ARGB4               0x6ul
#define EVE_FORMAT_BARGRAPH            0xbul
#define EVE_FORMAT_L1                  0x1ul
#define EVE_FORMAT_L4                  0x2ul
#define EVE_FORMAT_L8                  0x3ul
#define EVE_FORMAT_PALETTED            0x8ul
#define EVE_FORMAT_PALETTED4444        0xful
#define EVE_FORMAT_PALETTED565         0xeul
#define EVE_FORMAT_PALETTED8           0x10ul
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
#define EVE_OPT_FULLSCREEN             0x8ul
#define EVE_OPT_MEDIAFIFO              0x10ul
#define EVE_OPT_MONO                   0x1ul
#define EVE_OPT_NOBACK                 0x1000ul
#define EVE_OPT_NODL                   0x2ul
#define EVE_OPT_NOHANDS                0xc000ul
#define EVE_OPT_NOHM                   0x4000ul
#define EVE_OPT_NOPOINTER              0x4000ul
#define EVE_OPT_NOSECS                 0x8000ul
#define EVE_OPT_NOTEAR                 0x4ul
#define EVE_OPT_NOTICKS                0x2000ul
#define EVE_OPT_RGB565                 0x0ul
#define EVE_OPT_RIGHTX                 0x800ul
#define EVE_OPT_SIGNED                 0x100ul
#define EVE_OPT_SOUND                  0x20ul

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
#define EVE_INT_G8                     0x12ul
#define EVE_INT_L8C                    0xcul
#define EVE_INT_PLAYBACK               0x10ul
#define EVE_INT_SOUND                  0x8ul
#define EVE_INT_SWAP                   0x1ul
#define EVE_INT_TAG                    0x4ul
#define EVE_INT_TOUCH                  0x2ul
#define EVE_INT_VGA                    0xdul

// Audio Playback Options

#define EVE_LINEAR_SAMPLES             0x0ul
#define EVE_ULAW_SAMPLES               0x1ul
#define EVE_ADPCM_SAMPLES              0x2ul
#define EVE_VOL_ZERO                   0x0ul

// ADC Options

#define EVE_ADC_DIFFERENTIAL           0x1ul
#define EVE_ADC_SINGLE_ENDED           0x0ul

// Arrays containing the bitmap sizes of ROM fonts

#if EVE_SUB_API == 1

#define EVE_ROMFONT_MAX 34

#define EVE_ROMFONT_WIDTHS {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, \
    8,8,8,8,11,13,17,18,25,34, \
    13,15,19,21,28,37,49,63,82}
#define EVE_ROMFONT_HEIGHTS {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, \
    8,8,16,16,13,17,20,22,29,38, \
    16,20,25,28,36,49,63,83,108}
#define EVE_ROMFONT_FORMATS {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, \
    EVE_FORMAT_L1,EVE_FORMAT_L1,EVE_FORMAT_L1,EVE_FORMAT_L1,EVE_FORMAT_L1, \
    EVE_FORMAT_L1,EVE_FORMAT_L1,EVE_FORMAT_L1,EVE_FORMAT_L1,EVE_FORMAT_L1, \
    EVE_FORMAT_L4,EVE_FORMAT_L4,EVE_FORMAT_L4,EVE_FORMAT_L4,EVE_FORMAT_L4, \
    EVE_FORMAT_L4,EVE_FORMAT_L4,EVE_FORMAT_L4,EVE_FORMAT_L4}

#elif EVE_SUB_API == 2

#define EVE_ROMFONT_MAX 31

#define EVE_ROMFONT_WIDTHS {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, \
    8,8,8,8,11,13,17,18,25,34, \
    13,15,19,21,28,37}
#define EVE_ROMFONT_HEIGHTS {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, \
    8,8,16,16,13,17,20,22,29,38, \
    16,20,25,28,36,49}
#define EVE_ROMFONT_FORMATS {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, \
    EVE_FORMAT_L1,EVE_FORMAT_L1,EVE_FORMAT_L1,EVE_FORMAT_L1,EVE_FORMAT_L1, \
    EVE_FORMAT_L1,EVE_FORMAT_L1,EVE_FORMAT_L1,EVE_FORMAT_L1,EVE_FORMAT_L1, \
    EVE_FORMAT_L4,EVE_FORMAT_L4,EVE_FORMAT_L4,EVE_FORMAT_L4,EVE_FORMAT_L4, \
    EVE_FORMAT_L4}

#endif

#endif    /* _FT81X_H_ */

