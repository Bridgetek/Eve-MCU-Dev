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

#define EVE_RAM_G                      0x0UL
#if IS_EVE_API(2) && EVE_SUB_API == 2
#define EVE_RAM_G_SIZE                 (256*1024L)
#elif IS_EVE_API(2) && EVE_SUB_API == 1
#define EVE_RAM_G_SIZE                 (1024*1024L)
#else
#define EVE_RAM_G_SIZE                 (1024*1024L)
#endif
#define EVE_RAM_CMD                    0x308000UL
#define EVE_RAM_CMD_SIZE               (4*1024L)
#define EVE_RAM_DL                     0x300000UL
#define EVE_RAM_DL_SIZE                (8*1024L)
#define EVE_RAM_REG                    0x302000UL
#define EVE_RAM_ROMSUB                 0x30a000UL
#define EVE_ROMFONT_TABLEADDRESS       0x2ffffcUL
#define EVE_ROM_CHIPID                 0x0c0000UL      // Chip ID

// Coprocessor Method

// Use REG_CMDB_WRITE to fill coprocessor buffer.
// Undefine to write directly to coprocessor buffer.
#define EVE_USE_CMDB_METHOD

// Chip ID Register

#define EVE_REG_ID                     0x302000UL

// Control Registers

#define EVE_REG_BIST_EN                0x302174UL
#define EVE_REG_BUSYBITS               0x3020e8UL // Remove?
#define EVE_REG_CPURESET               0x302020UL

// Display Registers 

#define EVE_REG_PCLK                   0x302070UL
#define EVE_REG_PCLK_POL               0x30206cUL
#define EVE_REG_CSPREAD                0x302068UL
#define EVE_REG_SWIZZLE                0x302064UL
#define EVE_REG_DITHER                 0x302060UL
#define EVE_REG_OUTBITS                0x30205cUL
#define EVE_REG_ROTATE                 0x302058UL
#define EVE_REG_VSYNC0                 0x30204cUL
#define EVE_REG_VSYNC1                 0x302050UL
#define EVE_REG_VSIZE                  0x302048UL
#define EVE_REG_VOFFSET                0x302044UL
#define EVE_REG_VCYCLE                 0x302040UL
#define EVE_REG_HSYNC0                 0x302038UL
#define EVE_REG_HSYNC1                 0x30203cUL
#define EVE_REG_HSIZE                  0x302034UL
#define EVE_REG_HOFFSET                0x302030UL
#define EVE_REG_HCYCLE                 0x30202cUL

#define EVE_REG_TAP_MASK               0x302028UL
#define EVE_REG_TAP_CRC                0x302024UL

#define EVE_REG_DLSWAP                 0x302054UL

// Touch Registers 

#define EVE_REG_TAG                    0x30207cUL
#define EVE_REG_TAG_X                  0x302074UL
#define EVE_REG_TAG_Y                  0x302078UL

#define EVE_REG_TOUCH_ADC_MODE         0x302108UL
#define EVE_REG_TOUCH_CHARGE           0x30210cUL
#define EVE_REG_TOUCH_DIRECT_XY        0x30218cUL
#define EVE_REG_TOUCH_DIRECT_Z1Z2      0x302190UL
#define EVE_REG_TOUCH_FAULT            0x302170UL
#define EVE_REG_TOUCH_MODE             0x302104UL
#define EVE_REG_TOUCH_OVERSAMPLE       0x302114UL
#define EVE_REG_TOUCH_RAW_XY           0x30211cUL
#define EVE_REG_TOUCH_RZ               0x302120UL
#define EVE_REG_TOUCH_RZTHRESH         0x302118UL
#define EVE_REG_TOUCH_SCREEN_XY        0x302124UL
#define EVE_REG_TOUCH_SETTLE           0x302110UL
#define EVE_REG_TOUCH_TAG              0x30212cUL
#define EVE_REG_TOUCH_TAG_XY           0x302128UL
#define EVE_REG_TOUCH_TAG1             0x302134UL
#define EVE_REG_TOUCH_TAG1_XY          0x302130UL
#define EVE_REG_TOUCH_TAG2             0x30213cUL
#define EVE_REG_TOUCH_TAG2_XY          0x302138UL
#define EVE_REG_TOUCH_TAG3             0x302144UL
#define EVE_REG_TOUCH_TAG3_XY          0x302140UL
#define EVE_REG_TOUCH_TAG4             0x30214cUL
#define EVE_REG_TOUCH_TAG4_XY          0x302148UL
#define EVE_REG_TOUCH_TRANSFORM_A      0x302150UL
#define EVE_REG_TOUCH_TRANSFORM_B      0x302154UL
#define EVE_REG_TOUCH_TRANSFORM_C      0x302158UL
#define EVE_REG_TOUCH_TRANSFORM_D      0x30215cUL
#define EVE_REG_TOUCH_TRANSFORM_E      0x302160UL
#define EVE_REG_TOUCH_TRANSFORM_F      0x302164UL

#define EVE_REG_CTOUCH_EXTENDED        0x302108UL
#define EVE_REG_CTOUCH_TOUCH0_XY       0x302124UL
#define EVE_REG_CTOUCH_TOUCH1_XY       0x30211cUL
#define EVE_REG_CTOUCH_TOUCH2_XY       0x30218cUL
#define EVE_REG_CTOUCH_TOUCH3_XY       0x302190UL
#define EVE_REG_CTOUCH_TOUCH4_X        0x30216cUL
#define EVE_REG_CTOUCH_TOUCH4_Y        0x302120UL
#define EVE_REG_TOUCH_CONFIG           0x302168UL

// Media Registers

#define EVE_REG_PLAY                   0x30208cUL
#define EVE_REG_SOUND                  0x302088UL
#define EVE_REG_VOL_PB                 0x302080UL
#define EVE_REG_VOL_SOUND              0x302084UL
#define EVE_REG_PLAYBACK_FORMAT        0x3020c4UL
#define EVE_REG_PLAYBACK_FREQ          0x3020c0UL
#define EVE_REG_PLAYBACK_LENGTH        0x3020b8UL
#define EVE_REG_PLAYBACK_LOOP          0x3020c8UL
#define EVE_REG_PLAYBACK_PLAY          0x3020ccUL
#define EVE_REG_PLAYBACK_READPTR       0x3020bcUL
#define EVE_REG_PLAYBACK_START         0x3020b4UL
#define EVE_REG_MEDIAFIFO_READ         0x309014UL
#define EVE_REG_MEDIAFIFO_WRITE        0x309018UL

// Display List Registers

#define EVE_REG_CMD_DL                 0x302100UL
#define EVE_REG_CMD_READ               0x3020f8UL
#define EVE_REG_CMD_WRITE              0x3020fcUL
#define EVE_REG_CMDB_SPACE             0x302574UL
#define EVE_REG_CMDB_WRITE             0x302578UL

// Tracking Registers

#define EVE_REG_TRACKER                0x309000UL
#define EVE_REG_TRACKER_1              0x309004UL
#define EVE_REG_TRACKER_2              0x309008UL
#define EVE_REG_TRACKER_3              0x30900cUL
#define EVE_REG_TRACKER_4              0x309010UL

// PWM Registers

#define EVE_REG_PWM_DUTY               0x3020d4UL
#define EVE_REG_PWM_HZ                 0x3020d0UL

// Interrupt Registers

#define EVE_REG_INT_EN                 0x3020acUL
#define EVE_REG_INT_FLAGS              0x3020a8UL
#define EVE_REG_INT_MASK               0x3020b0UL

// Screenshot Registers

#define EVE_REG_SNAPFORMAT             0x30201cUL
#define EVE_REG_SNAPSHOT               0x302018UL
#define EVE_REG_SNAPY                  0x302014UL

// Clock Registers

#define EVE_REG_FREQUENCY              0x30200cUL
#define EVE_REG_CLOCK                  0x302008UL
#define EVE_REG_FRAMES                 0x302004UL
#define EVE_REG_TRIM                   0x302180UL

// Macro Registers

#define EVE_REG_MACRO_0                0x3020d8UL
#define EVE_REG_MACRO_1                0x3020dcUL

// GPIO Registers

#define EVE_REG_GPIO                   0x302094UL
#define EVE_REG_GPIO_DIR               0x302090UL
#define EVE_REG_GPIOX                  0x30209cUL
#define EVE_REG_GPIOX_DIR              0x302098UL

// Registers

#define EVE_REG_RENDERMODE             0x302010UL
#define EVE_REG_ROMSUB_SEL             0x3020f0UL
#define EVE_REG_SPI_EARLY_TX           0x30217cUL
#define EVE_REG_SPI_WIDTH              0x302188UL

// Bitmap Address Mask 

#define EVE_ENC_BITMAP_ADDR_MASK       0x3fffffUL

// Encoded Command Macros

#define EVE_ENC_ALPHA_FUNC(func,ref)        ((0x9UL << 24)|(((func) & 0x7UL) << 8)|(((ref) & 0xffUL) << 0))
#define EVE_ENC_BEGIN(prim)                 ((0x1fUL << 24)|(((prim)&0xfUL) << 0))
#define EVE_ENC_BITMAP_HANDLE(handle)       ((0x5UL << 24)|(((handle) & 0x1fUL) << 0))
#define EVE_ENC_BITMAP_LAYOUT(format,linestride,height) ((0x7UL << 24)|(((format) & 0x1fUL) << 19)|(((linestride) & 0x3ffUL) << 9)|(((height) & 0x1ffUL) << 0))
#define EVE_ENC_BITMAP_LAYOUT_H(linestride,height) ((0x28UL << 24)|(((linestride) & 0x3UL) << 2)|(((height) & 0x3UL) << 0))
#define EVE_ENC_BITMAP_SIZE(filter,wrapx,wrapy,width,height) ((0x8UL << 24)|(((filter) & 0x1UL) << 20)|(((wrapx) & 0x1UL) << 19)|(((wrapy) & 0x1UL) << 18)|(((width) & 0x1ffUL) << 9)|(((height) & 0x1ffUL) << 0))
#define EVE_ENC_BITMAP_SIZE_H(width,height) ((0x29UL << 24)|(((width) & 0x3UL) << 2)|(((height) & 0x3UL) << 0))
#define EVE_ENC_BITMAP_SOURCE(addr)         ((0x1UL << 24) | ((addr)&EVE_ENC_BITMAP_ADDR_MASK))
#define EVE_ENC_BITMAP_TRANSFORM_A(a)       ((0x15UL << 24)|((((uint32_t)(a)) & 0x1FFFFUL) << 0))
#define EVE_ENC_BITMAP_TRANSFORM_B(b)       ((0x16UL << 24)|((((uint32_t)(b)) & 0x1FFFFUL) << 0))
#define EVE_ENC_BITMAP_TRANSFORM_C(c)       ((0x17UL << 24)|((((uint32_t)(c)) & 0xFFFFFFUL) << 0))
#define EVE_ENC_BITMAP_TRANSFORM_D(d)       ((0x18UL << 24)|((((uint32_t)(d)) & 0x1FFFFUL) << 0))
#define EVE_ENC_BITMAP_TRANSFORM_E(e)       ((0x19UL << 24)|((((uint32_t)(e)) & 0x1FFFFUL) << 0))
#define EVE_ENC_BITMAP_TRANSFORM_F(f)       ((0x1aUL << 24)|((((uint32_t)(f)) & 0xFFFFFFUL) << 0))
#define EVE_ENC_BLEND_FUNC(src,dst)         ((0xbUL << 24)|(((src) & 0x7UL) << 3)|(((dst) & 0x7UL) << 0))
#define EVE_ENC_CALL(dest)                  ((0x1dUL << 24)|(((dest) & 0xFFFFUL) << 0))
#define EVE_ENC_CELL(cell)                  ((0x6UL << 24)|(((cell) & 0x7fUL) << 0))
#define EVE_ENC_CLEAR(c,s,t)                ((0x26UL << 24)|((((uint32_t)(c)) & 0x1UL) << 2)|((((uint32_t)(s)) & 0x1UL) << 1)|((((uint32_t)(t)) & 0x1UL) << 0))
#define EVE_ENC_CLEAR_COLOR_A(alpha)        ((0xfUL << 24)|(((alpha) & 0xffUL) << 0))
#define EVE_ENC_CLEAR_COLOR_RGB(red,green,blue) ((0x2UL << 24)|(((red) & 0xffUL) << 16)|(((green) & 0xffUL) << 8)|(((blue) & 0xffUL) << 0))
#define EVE_ENC_CLEAR_COLOR(c)              ((0x2UL << 24)|(((uint32_t)(c)) & 0x00ffffffUL))
#define EVE_ENC_CLEAR_STENCIL(s)            ((0x11UL << 24)|((((uint32_t)(s)) & 0xffUL) << 0))
#define EVE_ENC_CLEAR_TAG(s)                ((0x12UL << 24)|((((uint32_t)(s)) & 0xffUL) << 0))
#define EVE_ENC_COLOR_A(alpha)              ((0x10UL << 24)|(((alpha) & 0xffUL) << 0))
#define EVE_ENC_COLOR_MASK(r,g,b,a)         ((0x20UL << 24)|((((uint32_t)(r)) & 0x1UL) << 3)|((((uint32_t)(g)) & 0x1UL) << 2)|((((uint32_t)(b)) & 0x1UL) << 1)|((((uint32_t)(a)) & 0x1UL) << 0))
#define EVE_ENC_COLOR_RGB(red,green,blue)   ((0x4UL << 24)|(((red) & 0xffUL) << 16)|(((green) & 0xffUL) << 8)|(((blue) & 0xffUL) << 0))
#define EVE_ENC_COLOR(c)                    ((0x4UL << 24)|(((uint32_t)(c)) & 0x00ffffffUL))
#define EVE_ENC_DISPLAY()                   ((0x0UL << 24))
#define EVE_ENC_END()                       ((0x21UL << 24))
#define EVE_ENC_JUMP(dest)                  ((0x1eUL << 24)|(((dest) & 0xFFFFUL) << 0))
#define EVE_ENC_LINE_WIDTH(width)           ((0xeUL << 24)|(((width) & 0xFFFUL) << 0))
#define EVE_ENC_MACRO(m)                    ((0x25UL << 24)|((((uint32_t)(m)) & 0x1UL) << 0))
#define EVE_ENC_NOP()                       ((0x2dUL << 24))
#define EVE_ENC_PALETTE_SOURCE(addr)        ((0x2aUL << 24)|(((addr) & 0x3FFFFFUL) << 0))
#define EVE_ENC_POINT_SIZE(size)            ((0xdUL << 24)|(((size) & 0x1FFFUL) << 0))
#define EVE_ENC_RESTORE_CONTEXT()           ((0x23UL << 24))
#define EVE_ENC_RETURN()                    ((0x24UL << 24))
#define EVE_ENC_SAVE_CONTEXT()              ((0x22UL << 24))
#define EVE_ENC_SCISSOR_SIZE(width,height)  ((0x1cUL << 24)|(((width) & 0xFFFUL) << 12)|(((height) & 0xFFFUL) << 0))
#define EVE_ENC_SCISSOR_XY(x,y)             ((0x1bUL << 24)|((((uint32_t)(x)) & 0x7FFUL) << 11)|((((uint32_t)(y)) & 0x7FFUL) << 0))
#define EVE_ENC_STENCIL_FUNC(func,ref,mask) ((0xaUL << 24)|(((func) & 0x7UL) << 16)|(((ref) & 0xffUL) << 8)|(((mask) & 0xffUL) << 0))
#define EVE_ENC_STENCIL_MASK(mask)          ((0x13UL << 24)|(((mask) & 0xffUL) << 0))
#define EVE_ENC_STENCIL_OP(sfail,spass)     ((0xcUL << 24)|(((sfail) & 0x7UL) << 3)|(((spass) & 0x7UL) << 0))
#define EVE_ENC_TAG_MASK(mask)              ((0x14UL << 24)|(((mask) & 0x1UL) << 0))
#define EVE_ENC_TAG(s)                      ((0x3UL << 24)|((((uint32_t)(s)) & 0xffUL) << 0))
#define EVE_ENC_VERTEX_FORMAT(frac)         ((0x27UL << 24)|(((frac) & 0x7UL) << 0))
#define EVE_ENC_VERTEX_TRANSLATE_X(x)       ((0x2bUL << 24)|((((uint32_t)(x)) & 0x1FFFFUL) << 0))
#define EVE_ENC_VERTEX_TRANSLATE_Y(y)       ((0x2cUL << 24)|((((uint32_t)(y)) & 0x1FFFFUL) << 0))
#define EVE_ENC_VERTEX2F(x,y)               ((0x1UL << 30)|((((uint32_t)(x)) & 0xffffUL) << 15)|((((uint32_t)(y)) & 0xffffUL) << 0))
#define EVE_ENC_VERTEX2II(x,y,handle,cell)  ((0x2UL << 30)|((((uint32_t)(x)) & 0x1ffUL) << 21)|((((uint32_t)(y)) & 0x1ffUL) << 12)|(((handle) & 0x1fUL) << 7)|(((cell) & 0x7fUL) << 0))

// Co-processor Commands

#define EVE_ENC_CMD_APPEND             0xffffff1eUL
#define EVE_ENC_CMD_BGCOLOR            0xffffff09UL
#define EVE_ENC_CMD_BITMAP_TRANSFORM   0xffffff21UL
#define EVE_ENC_CMD_BUTTON             0xffffff0dUL
#define EVE_ENC_CMD_CALIBRATE          0xffffff15UL
#define EVE_ENC_CMD_CLOCK              0xffffff14UL
#define EVE_ENC_CMD_COLDSTART          0xffffff32UL
#define EVE_ENC_CMD_CSKETCH            0xffffff35UL
#define EVE_ENC_CMD_DIAL               0xffffff2dUL
#define EVE_ENC_CMD_DLSTART            0xffffff00UL
#define EVE_ENC_CMD_FGCOLOR            0xffffff0aUL
#define EVE_ENC_CMD_GAUGE              0xffffff13UL
#define EVE_ENC_CMD_GETMATRIX          0xffffff33UL
#define EVE_ENC_CMD_GETPROPS           0xffffff25UL
#define EVE_ENC_CMD_GETPTR             0xffffff23UL
#define EVE_ENC_CMD_GRADCOLOR          0xffffff34UL
#define EVE_ENC_CMD_GRADIENT           0xffffff0bUL
#define EVE_ENC_CMD_INFLATE            0xffffff22UL
#define EVE_ENC_CMD_INTERRUPT          0xffffff02UL
#define EVE_ENC_CMD_KEYS               0xffffff0eUL
#define EVE_ENC_CMD_LOADIDENTITY       0xffffff26UL
#define EVE_ENC_CMD_LOADIMAGE          0xffffff24UL
#define EVE_ENC_CMD_LOGO               0xffffff31UL
#define EVE_ENC_CMD_MEDIAFIFO          0xffffff39UL
#define EVE_ENC_CMD_MEMCPY             0xffffff1dUL
#define EVE_ENC_CMD_MEMCRC             0xffffff18UL
#define EVE_ENC_CMD_MEMSET             0xffffff1bUL
#define EVE_ENC_CMD_MEMWRITE           0xffffff1aUL
#define EVE_ENC_CMD_MEMZERO            0xffffff1cUL
#define EVE_ENC_CMD_NUMBER             0xffffff2eUL
#define EVE_ENC_CMD_PLAYVIDEO          0xffffff3aUL
#define EVE_ENC_CMD_PROGRESS           0xffffff0fUL
#define EVE_ENC_CMD_REGREAD            0xffffff19UL
#define EVE_ENC_CMD_ROMFONT            0xffffff3fUL
#define EVE_ENC_CMD_ROTATE             0xffffff29UL
#define EVE_ENC_CMD_SCALE              0xffffff28UL
#define EVE_ENC_CMD_SCREENSAVER        0xffffff2fUL
#define EVE_ENC_CMD_SCROLLBAR          0xffffff11UL
#define EVE_ENC_CMD_SETBASE            0xffffff38UL
#define EVE_ENC_CMD_SETBITMAP          0xffffff43UL
#define EVE_ENC_CMD_SETFONT            0xffffff2bUL
#define EVE_ENC_CMD_SETFONT2           0xffffff3bUL
#define EVE_ENC_CMD_SETMATRIX          0xffffff2aUL
#define EVE_ENC_CMD_SETROTATE          0xffffff36UL
#define EVE_ENC_CMD_SETSCRATCH         0xffffff3cUL
#define EVE_ENC_CMD_SKETCH             0xffffff30UL
#define EVE_ENC_CMD_SLIDER             0xffffff10UL
#define EVE_ENC_CMD_SNAPSHOT           0xffffff1fUL
#define EVE_ENC_CMD_SNAPSHOT2          0xffffff37UL
#define EVE_ENC_CMD_SPINNER            0xffffff16UL
#define EVE_ENC_CMD_STOP               0xffffff17UL
#define EVE_ENC_CMD_SWAP               0xffffff01UL
#define EVE_ENC_CMD_SYNC               0xffffff42UL
#define EVE_ENC_CMD_TEXT               0xffffff0cUL
#define EVE_ENC_CMD_TOGGLE             0xffffff12UL
#define EVE_ENC_CMD_TRACK              0xffffff2cUL
#define EVE_ENC_CMD_TRANSLATE          0xffffff27UL
#define EVE_ENC_CMD_VIDEOFRAME         0xffffff41UL
#define EVE_ENC_CMD_VIDEOSTART         0xffffff40UL

// Bitmap Formats

#define EVE_FORMAT_ARGB1555            0x0UL
#define EVE_FORMAT_ARGB2               0x5UL
#define EVE_FORMAT_ARGB4               0x6UL
#define EVE_FORMAT_BARGRAPH            0xbUL
#define EVE_FORMAT_L1                  0x1UL
#define EVE_FORMAT_L4                  0x2UL
#define EVE_FORMAT_L8                  0x3UL
#define EVE_FORMAT_PALETTED            0x8UL
#define EVE_FORMAT_PALETTED4444        0xfUL
#define EVE_FORMAT_PALETTED565         0xeUL
#define EVE_FORMAT_PALETTED8           0x10UL
#define EVE_FORMAT_RGB332              0x4UL
#define EVE_FORMAT_RGB565              0x7UL
#define EVE_FORMAT_TEXT8X8             0x9UL
#define EVE_FORMAT_TEXTVGA             0xaUL

// Command Options

#define EVE_BEGIN_BITMAPS              0x1UL
#define EVE_BEGIN_EDGE_STRIP_A         0x7UL
#define EVE_BEGIN_EDGE_STRIP_B         0x8UL
#define EVE_BEGIN_EDGE_STRIP_L         0x6UL
#define EVE_BEGIN_EDGE_STRIP_R         0x5UL
#define EVE_BEGIN_LINE_STRIP           0x4UL
#define EVE_BEGIN_LINES                0x3UL
#define EVE_BEGIN_POINTS               0x2UL
#define EVE_BEGIN_RECTS                0x9UL

#define EVE_TEST_ALWAYS                0x7UL
#define EVE_TEST_EQUAL                 0x5UL
#define EVE_TEST_GEQUAL                0x4UL
#define EVE_TEST_GREATER               0x3UL
#define EVE_TEST_LEQUAL                0x2UL
#define EVE_TEST_LESS                  0x1UL
#define EVE_TEST_NEVER                 0x0UL
#define EVE_TEST_NOTEQUAL              0x6UL

#define EVE_FILTER_BILINEAR            0x1UL
#define EVE_FILTER_NEAREST             0x0UL

#define EVE_WRAP_BORDER                0x0UL
#define EVE_WRAP_REPEAT                0x1UL

#define EVE_BLEND_DST_ALPHA            0x3UL
#define EVE_BLEND_ONE                  0x1UL
#define EVE_BLEND_ONE_MINUS_DST_ALPHA  0x5UL
#define EVE_BLEND_ONE_MINUS_SRC_ALPHA  0x4UL
#define EVE_BLEND_SRC_ALPHA            0x2UL
#define EVE_BLEND_ZERO                 0x0UL

#define EVE_STENCIL_DECR               0x4UL
#define EVE_STENCIL_INCR               0x3UL
#define EVE_STENCIL_INVERT             0x5UL
#define EVE_STENCIL_KEEP               0x1UL
#define EVE_STENCIL_REPLACE            0x2UL
#define EVE_STENCIL_ZERO               0x0UL

#define EVE_OPT_3D                     0x0UL
#define EVE_OPT_CENTER                 0x600UL
#define EVE_OPT_CENTERX                0x200UL
#define EVE_OPT_CENTERY                0x400UL
#define EVE_OPT_FLAT                   0x100UL
#define EVE_OPT_FULLSCREEN             0x8UL
#define EVE_OPT_MEDIAFIFO              0x10UL
#define EVE_OPT_MONO                   0x1UL
#define EVE_OPT_NOBACK                 0x1000UL
#define EVE_OPT_NODL                   0x2UL
#define EVE_OPT_NOHANDS                0xc000UL
#define EVE_OPT_NOHM                   0x4000UL
#define EVE_OPT_NOPOINTER              0x4000UL
#define EVE_OPT_NOSECS                 0x8000UL
#define EVE_OPT_NOTEAR                 0x4UL
#define EVE_OPT_NOTICKS                0x2000UL
#define EVE_OPT_RGB565                 0x0UL
#define EVE_OPT_RIGHTX                 0x800UL
#define EVE_OPT_SIGNED                 0x100UL
#define EVE_OPT_SOUND                  0x20UL

// Touchmode Options

#define EVE_TOUCHMODE_CONTINUOUS       0x3UL
#define EVE_TOUCHMODE_FRAME            0x2UL
#define EVE_TOUCHMODE_OFF              0x0UL
#define EVE_TOUCHMODE_ONESHOT          0x1UL

// Display List Swap Options

#define EVE_DLSWAP_DONE                0x0UL
#define EVE_DLSWAP_FRAME               0x2UL
#define EVE_DLSWAP_LINE                0x1UL

// Interrupt Options

#define EVE_INT_CMDEMPTY               0x20UL
#define EVE_INT_CMDFLAG                0x40UL
#define EVE_INT_CONVCOMPLETE           0x80UL
#define EVE_INT_G8                     0x12UL
#define EVE_INT_L8C                    0xcUL
#define EVE_INT_PLAYBACK               0x10UL
#define EVE_INT_SOUND                  0x8UL
#define EVE_INT_SWAP                   0x1UL
#define EVE_INT_TAG                    0x4UL
#define EVE_INT_TOUCH                  0x2UL
#define EVE_INT_VGA                    0xdUL

// Audio Playback Options

#define EVE_LINEAR_SAMPLES             0x0UL
#define EVE_ULAW_SAMPLES               0x1UL
#define EVE_ADPCM_SAMPLES              0x2UL
#define EVE_VOL_ZERO                   0x0UL

// ADC Options

#define EVE_ADC_DIFFERENTIAL           0x1UL
#define EVE_ADC_SINGLE_ENDED           0x0UL

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

