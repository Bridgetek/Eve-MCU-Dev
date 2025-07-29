/**
 @file BT82x.h
 @brief Definitions for BT82x devices. 
 @details Supports BT820.
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

#ifndef _BT82X_H_
#define    _BT82X_H_

//###########################################################################
//#####################################  EVE 5 ##############################
//###########################################################################

// Memory Map

#define EVE_RAM_G                      0x0UL
#define EVE_RAM_G_SIZE                 0x07ffffffUL
#define EVE_RAM_CMD                    0x7f000000UL
#define EVE_RAM_CMD_SIZE               (16*1024L)
#define EVE_RAM_DL                     0x7f008000UL
#define EVE_RAM_DL_SIZE                (16*1024L)
#define EVE_RAM_REG                    0x7f006000UL
#define EVE_RAM_ROMSUB                 0x7f027800UL
#define EVE_ROMFONT_TABLEADDRESS       0x07ffff00UL
#define EVE_RAM_TOP                    0x00304000UL
#define EVE_RAM_REPORT                 0x00309800UL
#define EVE_COPROC_REPORT              0x7f004800UL

// Coprocessor Method

// Use REG_CMDB_WRITE to fill coprocessor buffer.
// Undefine to write directly to coprocessor buffer.
#define EVE_USE_CMDB_METHOD

// Chip ID Register

#define EVE_REG_ID                     0x7f006000   //  Identification register, always reads as 0x7c

// Control Registers

#define EVE_RAM_BIST                   0x7f8001c0
#define EVE_REG_CPURESET               0x7f006088   //  Coprocessor reset bits 2:JA 1:JT 0:J1
#define EVE_REG_BOOT_CONTROL           0x7f006628   //  Boot control byte
#define EVE_REG_PIN_DRV_0              0x7f800408   //   Pin drive strength setting
#define EVE_REG_PIN_DRV_1              0x7f80040C   //   Pin drive strength setting
#define EVE_REG_PIN_SLEW_0             0x7f800410   //   Pin output slew rate setting
#define EVE_REG_PIN_TYPE_0             0x7f800414   //   Pin type setting
#define EVE_REG_PIN_TYPE_1             0x7f800418   //   Pin type setting
#define EVE_REG_SYS_CFG                0x7f800420   //   Miscellaneous system configuration
#define EVE_REG_SYS_STAT               0x7f800424   //   System status
#define EVE_REG_CHIP_ID                0x7f800448   //   CHIP_ID info
#define EVE_REG_BOOT_STATUS            0x7f80044C   //   EVE boot status
#define EVE_REG_DDR_TYPE               0x7f800454   //   DDR DRAM type setting
#define EVE_REG_PIN_DRV_2              0x7f800464   //   Pin drive strength setting
#define EVE_REG_PIN_SLEW_1             0x7f800468   //   Pin output slew rate setting
#define EVE_REG_PIN_TYPE_2             0x7f80046C   //   Pin type setting
#define EVE_REG_I2S_CFG                0x7f800800   //   I2S configuration registers
#define EVE_REG_I2S_CTL                0x7f800804   //   I2S control registers
#define EVE_REG_I2S_STAT               0x7f800810   //   I2S status
#define EVE_REG_I2S_PAD_CFG            0x7f800814   //   I2S padding configuration

// Display Registers 

#define EVE_REG_PCLK_POL               0x7f0060b8   //  PCLK polarity, 0 = clock on rising, 1 = falling
#define EVE_REG_VSYNC0                 0x7f0060ac   //  Vertical sync fall offset
#define EVE_REG_VSYNC1                 0x7f0060b0   //  Vertical sync rise offset
#define EVE_REG_VSIZE                  0x7f0060a8   //  Vertical display line count
#define EVE_REG_VOFFSET                0x7f0060a4   //  Vertical display start offset
#define EVE_REG_VCYCLE                 0x7f0060a0   //  Vertical total cycle count
#define EVE_REG_HSYNC0                 0x7f006098   //  Horizontal sync fall offset
#define EVE_REG_HSYNC1                 0x7f00609c   //  Horizontal sync rise offset
#define EVE_REG_HSIZE                  0x7f006094   //  Horizontal display pixel count
#define EVE_REG_HOFFSET                0x7f006090   //  Horizontal display start offset
#define EVE_REG_HCYCLE                 0x7f00608c   //  Horizontal total cycle count

#define EVE_REG_DLSWAP                 0x7f0060b4   //  Display list swap control

// Touch Registers 

#define EVE_REG_TAG                    0x7f0060c4   //  Tag query result
#define EVE_REG_TAG_X                  0x7f0060bc   //  Tag query X coordinate
#define EVE_REG_TAG_Y                  0x7f0060c0   //  Tag query Y coordinate
#define EVE_REG_TOUCH_MODE             0x7f006158   //  Touchscreen sampling mode
#define EVE_REG_TOUCH_RAW_XY           0x7f006164   //  Touchscreen raw $(x,y)$ (16, 16)
#define EVE_REG_TOUCH_SCREEN_XY        0x7f006160   //  Touchscreen screen $(x,y)$ (16, 16)
#define EVE_REG_TOUCH_TAG              0x7f006178   //  Touchscreen tag result 0
#define EVE_REG_TOUCH_TAG_XY           0x7f006174   //  Touchscreen screen $(x,y)$ used for tag lookup
#define EVE_REG_TOUCH_TAG1             0x7f006180   //  Touchscreen tag result 1
#define EVE_REG_TOUCH_TAG1_XY          0x7f00617c   //  Touchscreen screen $(x,y)$ used for tag lookup
#define EVE_REG_TOUCH_TAG2             0x7f006188   //  Touchscreen tag result 2
#define EVE_REG_TOUCH_TAG2_XY          0x7f006184   //  Touchscreen screen $(x,y)$ used for tag lookup
#define EVE_REG_TOUCH_TAG3             0x7f006190   //  Touchscreen tag result 3
#define EVE_REG_TOUCH_TAG3_XY          0x7f00618c   //  Touchscreen screen $(x,y)$ used for tag lookup
#define EVE_REG_TOUCH_TAG4             0x7f006198   //  Touchscreen tag result 4
#define EVE_REG_TOUCH_TAG4_XY          0x7f006194   //  Touchscreen screen $(x,y)$ used for tag lookup
#define EVE_REG_TOUCH_TRANSFORM_A      0x7f00619c   //  Touchscreen transform coefficient (s15.16)
#define EVE_REG_TOUCH_TRANSFORM_B      0x7f0061a0   //  Touchscreen transform coefficient (s15.16)
#define EVE_REG_TOUCH_TRANSFORM_C      0x7f0061a4   //  Touchscreen transform coefficient (s15.16)
#define EVE_REG_TOUCH_TRANSFORM_D      0x7f0061a8   //  Touchscreen transform coefficient (s15.16)
#define EVE_REG_TOUCH_TRANSFORM_E      0x7f0061ac   //  Touchscreen transform coefficient (s15.16)
#define EVE_REG_TOUCH_TRANSFORM_F      0x7f0061b0   //  Touchscreen transform coefficient (s15.16)

#define EVE_REG_CTOUCH_EXTENDED        0x7f00615c   //  0: single-touch, 1: multi-touch
#define EVE_REG_CTOUCH_TOUCH0_XY       0x7f006160   //  Touchscreen screen $(x,y)$ (16, 16) Extended Mode
#define EVE_REG_CTOUCH_TOUCH4_XY       0x7f006170   //  Touchscreen touch 4 Extended Mode
#define EVE_REG_CTOUCH_TOUCHA_XY       0x7f006164   //  Touchscreen raw $(x,y)$ (16, 16) Extended Mode
#define EVE_REG_CTOUCH_TOUCHB_XY       0x7f006168   //  Touchscreen touch 2 Extended Mode
#define EVE_REG_CTOUCH_TOUCHC_XY       0x7f00616c   //  Touchscreen touch 3 Extended Mode
#define EVE_REG_TOUCH_CONFIG           0x7f0061b4   //  Touchscreen configuration

// Media Registers

#define EVE_REG_PLAY                   0x7f0060d8   //  Start effect playback
#define EVE_REG_SOUND                  0x7f0060d4   //  Sound effect select
#define EVE_REG_VOL_L_PB               0x7f0060c8   //  Volume for playback left
#define EVE_REG_VOL_R_PB               0x7f0060cc   //  Volume for playback right
#define EVE_REG_VOL_SOUND              0x7f0060d0   //  Volume for synth sound
#define EVE_REG_PLAY_CONTROL           0x7f004050   //  Video playback control
#define EVE_REG_PLAYBACK_FORMAT        0x7f00611c   //  Audio playback format
#define EVE_REG_PLAYBACK_FREQ          0x7f006118   //  Audio playback frequency (Hz)
#define EVE_REG_PLAYBACK_LENGTH        0x7f006110   //  Audio playback sample length (bytes)
#define EVE_REG_PLAYBACK_LOOP          0x7f006120   //  Audio playback loop enable
#define EVE_REG_PLAYBACK_PAUSE         0x7f0065d0   //  audio playback pause, 0=play 1=pause
#define EVE_REG_PLAYBACK_PLAY          0x7f006124   //  Start audio playback
#define EVE_REG_PLAYBACK_READPTR       0x7f006114   //  Audio playback current read pointer
#define EVE_REG_PLAYBACK_START         0x7f00610c   //  Audio playback RAM start address
#define EVE_REG_MEDIAFIFO_READ         0x7f004014   //  media FIFO read offset
#define EVE_REG_MEDIAFIFO_WRITE        0x7f004018   //  media FIFO write offset
#define EVE_REG_ANIM_ACTIVE            0x7f00402c   //  32-bit mask of currently playing animations

// Display List Registers

#define EVE_REG_CMD_DL                 0x7f006154   //  Command DL offset
#define EVE_REG_CMD_READ               0x7f00614c   //  Command buffer read pointer
#define EVE_REG_CMD_WRITE              0x7f006150   //  Command buffer write pointer
#define EVE_REG_CMDB_WRITE             0x7f010000   //  Command write
#define EVE_REG_CMDB_SPACE             0x7f006594   //  Command (bulk) space available

// Tracking Registers

#define EVE_REG_TRACKER                0x7f004000   //  Tracker register 0
#define EVE_REG_TRACKER_1              0x7f004004   //  Tracker register 1
#define EVE_REG_TRACKER_2              0x7f004008   //  Tracker register 2
#define EVE_REG_TRACKER_3              0x7f00400c   //  Tracker register 3
#define EVE_REG_TRACKER_4              0x7f004010   //  Tracker register 4

// PWM Registers

#define EVE_REG_PWM_DUTY               0x7f00612c   //  PWM output duty cycle 0=0\%, 128=100\%
#define EVE_REG_PWM_HZ                 0x7f006128   //  PWM output frequency (Hz)

// Interrupt Registers

#define EVE_REG_INT_EN                 0x7f006104   //  Global interrupt enable
#define EVE_REG_INT_FLAGS              0x7f006100   //  Interrupt flags
#define EVE_REG_INT_MASK               0x7f006108   //  Interrupt enable mask

// Screenshot Registers

// Clock Registers

#define EVE_REG_FREQUENCY              0x7f00600c   //  Main clock frequency
#define EVE_REG_FRAMES                 0x7f006004   //  Frame counter, since reset
#define EVE_REG_CLOCK                  0x7f006008   //  Clock cycles, since reset

// Macro Registers

#define EVE_REG_MACRO_0                0x7f006130   //  Display list macro command 0
#define EVE_REG_MACRO_1                0x7f006134   //  Display list macro command 1

// GPIO Registers

#define EVE_REG_GPIO                   0x7f0060e0   //  GPIO read/write
#define EVE_REG_GPIO_DIR               0x7f0060dc   //  GPIO pin direction, 1 = output

// I2C Registers

#define EVE_REG_FREQUENCY_A            0x7f00671c   //  I2S adjusted main frequency
#define EVE_REG_I2S_EN                 0x7f006714   //  I2S interface enable
#define EVE_REG_I2S_FREQ               0x7f006718   //  I2S sample frequency

// Flash Controller Registers

#define EVE_REG_FLASH_DTR              0x7f0065ec   //  Flash DTR (Double Transfer Rate) enable
#define EVE_REG_FLASH_SIZE             0x7f004024   //  Detected flash capacity, in Mbytes
#define EVE_REG_FLASH_STATUS           0x7f0065d4   //  Flash status 0=INIT 1=DETACHED 2=BASIC 3=FULL

// Extent Registers

#define EVE_REG_EXTENT_X0              0x7f00403c   //  Previous widget pixel extents
#define EVE_REG_EXTENT_X1              0x7f004044   //  Previous widget pixel extents
#define EVE_REG_EXTENT_Y0              0x7f004040   //  Previous widget pixel extents
#define EVE_REG_EXTENT_Y1              0x7f004048   //  Previous widget pixel extents

// Render Engine Registers

#define EVE_REG_DISP                   0x7f0060e4   //  DISP output
#define EVE_REG_RE_ACTIVE              0x7f006028   //  RE write path active
#define EVE_REG_RE_DEST                0x7f006010   //  RE destination address
#define EVE_REG_RE_DITHER              0x7f006024   //  RE target dither enable
#define EVE_REG_RE_FORMAT              0x7f006014   //  RE format, as bitmap format
#define EVE_REG_RE_H                   0x7f006020   //  RE target height, in pixels
#define EVE_REG_RE_RENDERS             0x7f00602c   //  RE render counter
#define EVE_REG_RE_ROTATE              0x7f006018   //  RE rotate control
#define EVE_REG_RE_TESTMODE            0x7f006030
#define EVE_REG_RE_W                   0x7f00601c   //  RE target width, in pixels

// Swapchain Registers

#define EVE_REG_SC0_PTR0               0x7f00603c   //  Swapchain 0, pointer 0
#define EVE_REG_SC0_PTR1               0x7f006040   //  Swapchain 0, pointer 1
#define EVE_REG_SC0_PTR2               0x7f006044   //  Swapchain 0, pointer 2
#define EVE_REG_SC0_PTR3               0x7f006048   //  Swapchain 0, pointer 3
#define EVE_REG_SC0_RESET              0x7f006034   //  Swapchain 0, write to reset
#define EVE_REG_SC0_SIZE               0x7f006038   //  Swapchain 0, ring size 1-4
#define EVE_REG_SC1_I_PTR              0x7f006084   //  Swapchain 1, input pointer (rd)
#define EVE_REG_SC1_I_READY            0x7f006080   //  Swapchain 1, input ready (wr)
#define EVE_REG_SC1_I_VALID            0x7f00607c   //  Swapchain 1, input valid (rd)
#define EVE_REG_SC1_PTR0               0x7f006054   //  Swapchain 0, pointer 0
#define EVE_REG_SC1_PTR1               0x7f006058   //  Swapchain 0, pointer 1
#define EVE_REG_SC1_PTR2               0x7f00605c   //  Swapchain 0, pointer 2
#define EVE_REG_SC1_PTR3               0x7f006060   //  Swapchain 0, pointer 3
#define EVE_REG_SC1_RESET              0x7f00604c   //  Swapchain 0, write to reset
#define EVE_REG_SC1_SIZE               0x7f006050   //  Swapchain 0, ring size 1-4
#define EVE_REG_SC2_ADDR               0x7f006784   //  Swapchain 2 status, write to reset
#define EVE_REG_SC2_PTR0               0x7f00606c   //  Swapchain 0, pointer 0
#define EVE_REG_SC2_PTR1               0x7f006070   //  Swapchain 0, pointer 1
#define EVE_REG_SC2_PTR2               0x7f006074   //  Swapchain 0, pointer 2
#define EVE_REG_SC2_PTR3               0x7f006078   //  Swapchain 0, pointer 3
#define EVE_REG_SC2_RESET              0x7f006064   //  Swapchain 0, write to reset
#define EVE_REG_SC2_SIZE               0x7f006068   //  Swapchain 0, ring size 1-4
#define EVE_REG_SC2_STATUS             0x7f006780   //  Swapchain 2 status, write to reset
#define EVE_REG_SO_EN                  0x7f006600   //  Scanout 0 enable
#define EVE_REG_SO_FORMAT              0x7f0065fc   //  Scanout 0 format
#define EVE_REG_SO_MODE                0x7f0065f4   //  Scanout pixel mode
#define EVE_REG_SO_SOURCE              0x7f0065f8   //  Scanout 0 source

// LVDS Registers

#define EVE_REG_LVDSRX_CORE_ENABLE     0x7f006670   // LVDSRX enable register
#define EVE_REG_LVDSRX_CORE_CAPTURE    0x7f006674   // LVDSRX enable capture register
#define EVE_REG_LVDSRX_CORE_SETUP      0x7f006678   // LVDSRX pixel setup control register
#define EVE_REG_LVDSRX_CORE_DEST       0x7f00667c   // LVDSRX destination frame address register
#define EVE_REG_LVDSRX_CORE_FORMAT     0x7f006680   // LVDSRX output pixel format register
#define EVE_REG_LVDSRX_CORE_DITHER     0x7f006684   // LVDSRX enable dither register
#define EVE_REG_LVDSRX_CORE_FRAMES     0x7f006698   // LVDSRX frame counter
#define EVE_REG_LVDSRX_SETUP           0x7F800500   // LVDSRX system set-up
#define EVE_REG_LVDSRX_CTRL            0x7F800504   // LVDSRX analog block configuration
#define EVE_REG_LVDSRX_STAT            0x7F800508   // LVDSRX status register
#define EVE_REG_LVDSTX_EN              0x7f800300   // LVDS enables
#define EVE_REG_LVDSTX_PLLCFG          0x7f800304   // LVDS PLL and Clock configuration
#define EVE_REG_LVDSTX_CTRL_CH0        0x7f800314   // LVDS channel 0 control
#define EVE_REG_LVDSTX_CTRL_CH1        0x7f800318   // LVDS channel 1 control
#define EVE_REG_LVDSTX_STAT            0x7f80031c   // LVDS status
#define EVE_REG_LVDSTX_ERR_STAT        0x7f800320   // LVDS error status

// Completion Register

#define EVE_REG_OBJECT_COMPLETE        0x7f004038   //  Used with OPT_COMPLETEREG

// Bitmap Address Mask 

#define EVE_ENC_BITMAP_ADDR_MASK       0xffffffffUL

// Encoded Command Macros

#define EVE_ENC_ALPHA_FUNC(func,ref)        ((0x9UL<<24)|(((func)&0x7UL)<<8)|(((ref)&0xffUL)<<0))
#define EVE_ENC_BEGIN(prim)                 ((0x1fUL<<24)|(((prim)&0xfUL)<<0))
#define EVE_ENC_BITMAP_EXT_FORMAT(format)   ((0x2eUL<<24)|(((format)&0xffffUL)<<0))
#define EVE_ENC_BITMAP_HANDLE(handle)       ((0x5UL<<24)|(((handle)&0x3fUL)<<0))
#define EVE_ENC_BITMAP_LAYOUT(format,linestride,height) ((0x7UL<<24)|(((format)&0x1fUL)<<19)|(((linestride)&0x3ffUL)<<9)|(((height)&0x1FFUL)<<0))
#define EVE_ENC_BITMAP_LAYOUT_H(linestride,height) ((0x28UL<<24)|(((linestride)&0x3UL)<<2)|(((height)&0x3UL)<<0))
#define EVE_ENC_BITMAP_SIZE(filter,wrapx,wrapy,width,height) ((0x8UL<<24)|(((filter)&0x1UL)<<20)|(((wrapx)&0x1UL)<<19)|(((wrapy)&0x1UL)<<18)|(((width)&0x1FFUL)<<9)|(((height)&0x1FFUL)<<0))
#define EVE_ENC_BITMAP_SIZE_H(width,height) ((0x29UL<<24)|(((width)&0x3UL)<<2)|(((height)&0x3UL)<<0))
#define EVE_ENC_BITMAP_SOURCE(addr)         ((0x1UL << 24) | ((addr)&0xFFFFFFUL))
#define EVE_ENC_BITMAP_SOURCE_H(addr)       ((0x31UL << 24) | ((addr)&0xFFUL))
#define EVE_ENC_BITMAP_SWIZZLE(r,g,b,a)     ((0x2fUL<<24)|(((r)&0x7UL)<<9)|(((g)&0x7UL)<<6)|(((b)&0x7UL)<<3)|(((a)&0x7UL)<<0))
#define EVE_ENC_BITMAP_TRANSFORM_A_EXT(p,v) ((0x15UL<<24)|(((p)&0x1UL)<<17)|(((v)&0x1FFFFUL)<<0))
#define EVE_ENC_BITMAP_TRANSFORM_B_EXT(p,v) ((0x16UL<<24)|(((p)&0x1UL)<<17)|(((v)&0x1FFFFUL)<<0))
#define EVE_ENC_BITMAP_TRANSFORM_C(c)       ((0x17UL<<24)|(((c)&0xFFFFFFUL)<<0))
#define EVE_ENC_BITMAP_TRANSFORM_D_EXT(p,v) ((0x18UL<<24)|(((p)&0x1UL)<<17)|(((v)&0x1FFFFUL)<<0))
#define EVE_ENC_BITMAP_TRANSFORM_E_EXT(p,v) ((0x19UL<<24)|(((p)&0x1UL)<<17)|(((v)&0x1FFFFUL)<<0))
#define EVE_ENC_BITMAP_TRANSFORM_F(f)       ((0x1aUL<<24)|(((f)&0xFFFFFFUL)<<0))
#define EVE_ENC_BITMAP_ZORDER(o)            ((0x33UL<<24)|(((o)&0xffUL)<<0))
#define EVE_ENC_BLEND_FUNC(src,dst)         ((0xbUL<<24)|(((src)&0x7UL)<<3)|(((dst)&0x7UL)<<0))
#define EVE_ENC_CALL(dest)                  ((0x1dUL<<24)|(((dest)&0xffffUL)<<0))
#define EVE_ENC_CELL(cell)                  ((0x6UL<<24)|(((cell)&0x7FUL)<<0))
#define EVE_ENC_CLEAR(c,s,t)                ((0x26UL<<24)|(((c)&0x1UL)<<2)|(((s)&0x1UL)<<1)|(((t)&0x1UL)<<0))
#define EVE_ENC_CLEAR_COLOR(c)              ((0x2UL<<24)|(((uint32_t)(c)) & 0x00ffffffUL))
#define EVE_ENC_CLEAR_COLOR_A(alpha)        ((0xfUL<<24)|(((alpha)&0xffUL)<<0))
#define EVE_ENC_CLEAR_COLOR_RGB(red,green,blue) ((0x2UL<<24)|(((red)&0xffUL)<<16)|(((green)&0xffUL)<<8)|(((blue)&0xffUL)<<0))
#define EVE_ENC_CLEAR_STENCIL(s)            ((0x11UL<<24)|(((s)&0xffUL)<<0))
#define EVE_ENC_CLEAR_TAG(s)                ((0x12UL<<24)|(((s)&0xffUL)<<0))
#define EVE_ENC_COLOR(c)                    ((0x4UL<<24)|(((uint32_t)(c)) & 0x00ffffffUL))
#define EVE_ENC_COLOR_A(alpha)              ((0x10UL<<24)|(((alpha)&0xffUL)<<0))
#define EVE_ENC_COLOR_MASK(r,g,b,a)         ((0x20UL<<24)|(((r)&0x1UL)<<3)|(((g)&0x1UL)<<2)|(((b)&0x1UL)<<1)|(((a)&0x1UL)<<0))
#define EVE_ENC_COLOR_RGB(red,green,blue)   ((0x4UL<<24)|(((red)&0xffUL)<<16)|(((green)&0xffUL)<<8)|(((blue)&0xffUL)<<0))
#define EVE_ENC_DISPLAY()                   ((0x0UL<<24))
#define EVE_ENC_END()                       ((0x21UL<<24))
#define EVE_ENC_JUMP(dest)                  ((0x1fUL<<24)|(((dest)&0xffffUL)<<0))
#define EVE_ENC_LINE_WIDTH(width)           ((0xeUL<<24)|(((width)&0xFFFUL)<<0))
#define EVE_ENC_MACRO(m)                    ((0x25UL<<24)|(((m)&0x1UL)<<0))
#define EVE_ENC_NOP()                       ((0x2dUL<<24))
#define EVE_ENC_PALETTE_SOURCE(addr)        ((0x2aUL<<24)|(((addr)&0x3FFFFFUL)<<0))
#define EVE_ENC_POINT_SIZE(size)            ((0xdUL<<24)|(((size)&0x1FFFUL)<<0))
#define EVE_ENC_RESTORE_CONTEXT()           ((0x23UL<<24))
#define EVE_ENC_PALLETE_SOURCE_H(addr)      ((0x32UL<< 24) | ((addr)&0xFFUL))
#define EVE_ENC_REGION(y,h,dest)            ((0x34UL<<24)|(((y)&0x3fUL)<<18)|(((h)&0x3fUL)<<12)|(((dest)&0xfffUL)<<0))
#define EVE_ENC_RETURN()                    ((0x24UL<<24))
#define EVE_ENC_SAVE_CONTEXT()              ((0x22UL<<24))
#define EVE_ENC_SCISSOR_SIZE(width,height)  ((0x1cUL<<24)|(((width)&0xFFFUL)<<12)|(((height)&0xFFFUL)<<0))
#define EVE_ENC_SCISSOR_XY(x,y)             ((0x1bUL<<24)|(((x)&0x7FFUL)<<11)|(((y)&0x7FFUL)<<0))
#define EVE_ENC_STENCIL_FUNC(func,ref,mask) ((0xaUL<<24)|(((func)&0x7UL)<<16)|(((ref)&0xffUL)<<8)|(((mask)&0xffUL)<<0))
#define EVE_ENC_STENCIL_MASK(mask)          ((0x13UL<<24)|(((mask)&0xffUL)<<0))
#define EVE_ENC_STENCIL_OP(sfail,spass)     ((0xcUL<<24)|(((sfail)&0x7UL)<<3)|(((spass)&0x7UL)<<0))
#define EVE_ENC_TAG(s)                      ((0x3UL<<24)|(((s)&0xffffffUL)<<0))
#define EVE_ENC_TAG_MASK(mask)              ((0x14UL<<24)|(((mask)&0x1UL)<<0))
#define EVE_ENC_VERTEX2F(x,y)               ((0x1UL<<30)|(((x)&0x7fffUL)<<15)|(((y)&0x7fffUL)<<0))
#define EVE_ENC_VERTEX2II(x,y,handle,cell)  ((0x2UL<<30)|(((x)&0x1FFUL)<<21)|(((y)&0x1FFUL)<<12)|(((handle)&0x1fUL)<<7)|(((cell)&0x7FUL)<<0))
#define EVE_ENC_VERTEX_FORMAT(frac)         ((0x27UL<<24)|(((frac)&0x7UL)<<0))
#define EVE_ENC_VERTEX_TRANSLATE_X(x)       ((0x2bUL<<24)|(((x)&0x1FFFFUL)<<0))
#define EVE_ENC_VERTEX_TRANSLATE_Y(y)       ((0x2cUL<<24)|(((y)&0x1FFFFUL)<<0))

// Added to ensure previous macros are fine
#define EVE_ENC_BITMAP_TRANSFORM_A(a) EVE_ENC_BITMAP_TRANSFORM_A_EXT(0,a)
#define EVE_ENC_BITMAP_TRANSFORM_B(b) EVE_ENC_BITMAP_TRANSFORM_B_EXT(0,b)
#define EVE_ENC_BITMAP_TRANSFORM_D(d) EVE_ENC_BITMAP_TRANSFORM_D_EXT(0,d)
#define EVE_ENC_BITMAP_TRANSFORM_E(e) EVE_ENC_BITMAP_TRANSFORM_E_EXT(0,e)

// Co-processor Commands

#define EVE_ENC_CMD_ANIMDRAW           0xffffff4f
#define EVE_ENC_CMD_ANIMFRAME          0xffffff5e
#define EVE_ENC_CMD_ANIMSTART          0xffffff5f
#define EVE_ENC_CMD_ANIMSTOP           0xffffff4d
#define EVE_ENC_CMD_ANIMXY             0xffffff4e
#define EVE_ENC_CMD_APBREAD            0xffffff62
#define EVE_ENC_CMD_APBWRITE           0xffffff63
#define EVE_ENC_CMD_APPEND             0xffffff1c
#define EVE_ENC_CMD_APPENDF            0xffffff52
#define EVE_ENC_CMD_ARC                0xffffff87
#define EVE_ENC_CMD_BGCOLOR            0xffffff07
#define EVE_ENC_CMD_BITMAP_TRANSFORM   0xffffff1f
#define EVE_ENC_CMD_BUTTON             0xffffff0b
#define EVE_ENC_CMD_CALIBRATE          0xffffff13
#define EVE_ENC_CMD_CALIBRATESUB       0xffffff56
#define EVE_ENC_CMD_CALLLIST           0xffffff5b
#define EVE_ENC_CMD_CGRADIENT          0xffffff8a
#define EVE_ENC_CMD_CLOCK              0xffffff12
#define EVE_ENC_CMD_COLDSTART          0xffffff2e
#define EVE_ENC_CMD_COPYLIST           0xffffff88
#define EVE_ENC_CMD_DDRSHUTDOWN        0xffffff65
#define EVE_ENC_CMD_DDRSTARTUP         0xffffff66
#define EVE_ENC_CMD_DIAL               0xffffff29
#define EVE_ENC_CMD_DLSTART            0xffffff00
#define EVE_ENC_CMD_ENABLEREGION       0xffffff7e
#define EVE_ENC_CMD_ENDLIST            0xffffff5d
#define EVE_ENC_CMD_ENDREGION          0xffffff90
#define EVE_ENC_CMD_FENCE              0xffffff68
#define EVE_ENC_CMD_FGCOLOR            0xffffff08
#define EVE_ENC_CMD_FILLWIDTH          0xffffff51
#define EVE_ENC_CMD_FLASHATTACH        0xffffff43
#define EVE_ENC_CMD_FLASHDETACH        0xffffff42
#define EVE_ENC_CMD_FLASHERASE         0xffffff3e
#define EVE_ENC_CMD_FLASHFAST          0xffffff44
#define EVE_ENC_CMD_FLASHPROGRAM       0xffffff64
#define EVE_ENC_CMD_FLASHREAD          0xffffff40
#define EVE_ENC_CMD_FLASHSOURCE        0xffffff48
#define EVE_ENC_CMD_FLASHSPIDESEL      0xffffff45
#define EVE_ENC_CMD_FLASHSPIRX         0xffffff47
#define EVE_ENC_CMD_FLASHSPITX         0xffffff46
#define EVE_ENC_CMD_FLASHUPDATE        0xffffff41
#define EVE_ENC_CMD_FLASHWRITE         0xffffff3f
#define EVE_ENC_CMD_FSDIR              0xffffff8e
#define EVE_ENC_CMD_FSOPTIONS          0xffffff6d
#define EVE_ENC_CMD_FSREAD             0xffffff71
#define EVE_ENC_CMD_FSSIZE             0xffffff80
#define EVE_ENC_CMD_FSSOURCE           0xffffff7f
#define EVE_ENC_CMD_GAUGE              0xffffff11
#define EVE_ENC_CMD_GETIMAGE           0xffffff58
#define EVE_ENC_CMD_GETMATRIX          0xffffff2f
#define EVE_ENC_CMD_GETPROPS           0xffffff22
#define EVE_ENC_CMD_GETPTR             0xffffff20
#define EVE_ENC_CMD_GLOW               0xffffff8b
#define EVE_ENC_CMD_GRADCOLOR          0xffffff30
#define EVE_ENC_CMD_GRADIENT           0xffffff09
#define EVE_ENC_CMD_GRADIENTA          0xffffff50
#define EVE_ENC_CMD_GRAPHICSFINISH     0xffffff6b
#define EVE_ENC_CMD_I2SSTARTUP         0xffffff69
#define EVE_ENC_CMD_INFLATE            0xffffff4a
#define EVE_ENC_CMD_INTERRUPT          0xffffff02
#define EVE_ENC_CMD_KEYS               0xffffff0c
#define EVE_ENC_CMD_LOADASSET          0xffffff81
#define EVE_ENC_CMD_LOADIDENTITY       0xffffff23
#define EVE_ENC_CMD_LOADIMAGE          0xffffff21
#define EVE_ENC_CMD_LOADWAV            0xffffff85
#define EVE_ENC_CMD_LOADPATCH          0xffffff82
#define EVE_ENC_CMD_LOGO               0xffffff2d
#define EVE_ENC_CMD_MEDIAFIFO          0xffffff34
#define EVE_ENC_CMD_MEMCPY             0xffffff1b
#define EVE_ENC_CMD_MEMCRC             0xffffff16
#define EVE_ENC_CMD_MEMSET             0xffffff19
#define EVE_ENC_CMD_MEMWRITE           0xffffff18
#define EVE_ENC_CMD_MEMZERO            0xffffff1a
#define EVE_ENC_CMD_NEWLIST            0xffffff5c
#define EVE_ENC_CMD_NOP                0xffffff53
#define EVE_ENC_CMD_NUMBER             0xffffff2a
#define EVE_ENC_CMD_PLAYVIDEO          0xffffff35
#define EVE_ENC_CMD_PLAYWAV            0xffffff79
#define EVE_ENC_CMD_PROGRESS           0xffffff0d
#define EVE_ENC_CMD_REGION             0xffffff8f
#define EVE_ENC_CMD_REGREAD            0xffffff17
#define EVE_ENC_CMD_REGWRITE           0xffffff86
#define EVE_ENC_CMD_RENDERTARGET       0xffffff8d
#define EVE_ENC_CMD_RESETFONTS         0xffffff4c
#define EVE_ENC_CMD_RESTORECONTEXT     0xffffff7d
#define EVE_ENC_CMD_RESULT             0xffffff89
#define EVE_ENC_CMD_RETURN             0xffffff5a
#define EVE_ENC_CMD_ROMFONT            0xffffff39
#define EVE_ENC_CMD_ROTATE             0xffffff26
#define EVE_ENC_CMD_ROTATEAROUND       0xffffff4b
#define EVE_ENC_CMD_RUNANIM            0xffffff60
#define EVE_ENC_CMD_SAVECONTEXT        0xffffff7c
#define EVE_ENC_CMD_SCALE              0xffffff25
#define EVE_ENC_CMD_SCREENSAVER        0xffffff2b
#define EVE_ENC_CMD_SCROLLBAR          0xffffff0f
#define EVE_ENC_CMD_SDATTACH           0xffffff6e
#define EVE_ENC_CMD_SDBLOCKREAD        0xffffff6f
#define EVE_ENC_CMD_SETBASE            0xffffff33
#define EVE_ENC_CMD_SETBITMAP          0xffffff3d
#define EVE_ENC_CMD_SETFONT            0xffffff36
#define EVE_ENC_CMD_SETMATRIX          0xffffff27
#define EVE_ENC_CMD_SETROTATE          0xffffff31
#define EVE_ENC_CMD_SETSCRATCH         0xffffff37
#define EVE_ENC_CMD_SKETCH             0xffffff2c
#define EVE_ENC_CMD_SKIPCOND           0xffffff8c
#define EVE_ENC_CMD_SLIDER             0xffffff0e
#define EVE_ENC_CMD_SNAPSHOT           0xffffff1d
#define EVE_ENC_CMD_SPINNER            0xffffff14
#define EVE_ENC_CMD_STOP               0xffffff15
#define EVE_ENC_CMD_SWAP               0xffffff01
#define EVE_ENC_CMD_SYNC               0xffffff3c
#define EVE_ENC_CMD_TESTCARD           0xffffff57
#define EVE_ENC_CMD_TEXT               0xffffff0a
#define EVE_ENC_CMD_TEXTDIM            0xffffff84
#define EVE_ENC_CMD_TOGGLE             0xffffff10
#define EVE_ENC_CMD_TRACK              0xffffff28
#define EVE_ENC_CMD_TRANSLATE          0xffffff24
#define EVE_ENC_CMD_VIDEOFRAME         0xffffff3b
#define EVE_ENC_CMD_VIDEOSTART         0xffffff3a
#define EVE_ENC_CMD_WAIT               0xffffff59
#define EVE_ENC_CMD_WAITCHANGE         0xffffff67
#define EVE_ENC_CMD_WAITCOND           0xffffff78
#define EVE_ENC_CMD_WATCHDOG           0xffffff83

// Aliases 

#define FTPOINTS                       POINTS

// Hardware swapchains for render engine

#define EVE_SWAPCHAIN_0                0xffff00ff 
#define EVE_SWAPCHAIN_1                0xffff01ff
#define EVE_SWAPCHAIN_2                0xffff02ff

// Bitmap Formats

#define EVE_FORMAT_ARGB1555            0x0UL
#define EVE_FORMAT_L1                  0x1UL
#define EVE_FORMAT_L4                  0x2UL
#define EVE_FORMAT_L8                  0x3UL
#define EVE_FORMAT_RGB332              0x4UL
#define EVE_FORMAT_ARGB2               0x5UL
#define EVE_FORMAT_ARGB4               0x6UL
#define EVE_FORMAT_RGB565              0x7UL
#define EVE_FORMAT_PALETTED            0x8UL
#define EVE_FORMAT_TEXT8X8             0x9UL
#define EVE_FORMAT_TEXTVGA             0xaUL
#define EVE_FORMAT_BARGRAPH            0xbUL
#define EVE_FORMAT_PALETTED565         0xeUL
#define EVE_FORMAT_PALETTED4444        0xfUL
#define EVE_FORMAT_PALETTED8           0x10UL
#define EVE_FORMAT_L2                  0x11UL
#define EVE_FORMAT_RGB8                0x13UL
#define EVE_FORMAT_ARGB8               0x14UL
#define EVE_FORMAT_PALETTEDARGB8       0x15UL
#define EVE_FORMAT_RGB6                0x16UL
#define EVE_FORMAT_ARGB6               0x17UL
#define EVE_FORMAT_LA1                 0x18UL
#define EVE_FORMAT_LA2                 0x19UL
#define EVE_FORMAT_LA4                 0x1aUL
#define EVE_FORMAT_LA8                 0x1bUL
#define EVE_FORMAT_YCBCR               0x1cUL
// Extended Bitmap Format to enable BITMAP_EXT_FORMAT
#define EVE_GLFORMAT                   0x1fUL
// BITMAP_EXT_FORMATs
#define EVE_FORMAT_COMPRESSED_RGBA_ASTC_10x10_KHR 0x93bbUL
#define EVE_FORMAT_COMPRESSED_RGBA_ASTC_10x5_KHR  0x93b8UL
#define EVE_FORMAT_COMPRESSED_RGBA_ASTC_10x6_KHR  0x93b9UL
#define EVE_FORMAT_COMPRESSED_RGBA_ASTC_10x8_KHR  0x93baUL
#define EVE_FORMAT_COMPRESSED_RGBA_ASTC_12x10_KHR 0x93bcUL
#define EVE_FORMAT_COMPRESSED_RGBA_ASTC_12x12_KHR 0x93bdUL
#define EVE_FORMAT_COMPRESSED_RGBA_ASTC_4x4_KHR   0x93b0UL
#define EVE_FORMAT_COMPRESSED_RGBA_ASTC_5x4_KHR   0x93b1UL
#define EVE_FORMAT_COMPRESSED_RGBA_ASTC_5x5_KHR   0x93b2UL
#define EVE_FORMAT_COMPRESSED_RGBA_ASTC_6x5_KHR   0x93b3UL
#define EVE_FORMAT_COMPRESSED_RGBA_ASTC_6x6_KHR   0x93b4UL
#define EVE_FORMAT_COMPRESSED_RGBA_ASTC_8x5_KHR   0x93b5UL
#define EVE_FORMAT_COMPRESSED_RGBA_ASTC_8x6_KHR   0x93b6UL
#define EVE_FORMAT_COMPRESSED_RGBA_ASTC_8x8_KHR   0x93b7UL

// Command Options

#define EVE_BEGIN_BITMAPS              0x1UL
#define EVE_BEGIN_POINTS               0x2UL
#define EVE_BEGIN_LINES                0x3UL
#define EVE_BEGIN_LINE_STRIP           0x4UL
#define EVE_BEGIN_EDGE_STRIP_R         0x5UL
#define EVE_BEGIN_EDGE_STRIP_L         0x6UL
#define EVE_BEGIN_EDGE_STRIP_A         0x7UL
#define EVE_BEGIN_EDGE_STRIP_B         0x8UL
#define EVE_BEGIN_RECTS                0x9UL

#define EVE_TEST_NEVER                 0x0UL
#define EVE_TEST_LESS                  0x1UL
#define EVE_TEST_LEQUAL                0x2UL
#define EVE_TEST_GREATER               0x3UL
#define EVE_TEST_GEQUAL                0x4UL
#define EVE_TEST_EQUAL                 0x5UL
#define EVE_TEST_NOTEQUAL              0x6UL
#define EVE_TEST_ALWAYS                0x7UL

#define EVE_FILTER_NEAREST             0x0UL
#define EVE_FILTER_BILINEAR            0x1UL

#define EVE_WRAP_BORDER                0x0UL
#define EVE_WRAP_REPEAT                0x1UL

#define EVE_BLEND_ZERO                 0x0UL
#define EVE_BLEND_ONE                  0x1UL
#define EVE_BLEND_SRC_ALPHA            0x2UL
#define EVE_BLEND_ONE_MINUS_SRC_ALPHA  0x4UL
#define EVE_BLEND_DST_ALPHA            0x3UL
#define EVE_BLEND_ONE_MINUS_DST_ALPHA  0x5UL

#define EVE_STENCIL_ZERO               0x0UL
#define EVE_STENCIL_KEEP               0x1UL
#define EVE_STENCIL_REPLACE            0x2UL
#define EVE_STENCIL_INCR               0x3UL
#define EVE_STENCIL_DECR               0x4UL
#define EVE_STENCIL_INVERT             0x5UL

#define EVE_OPT_3D                     0x0UL
#define EVE_OPT_CENTER                 0x600UL
#define EVE_OPT_CENTERX                0x200UL
#define EVE_OPT_CENTERY                0x400UL
#define EVE_OPT_FILL                   0x2000UL
#define EVE_OPT_FLASH                  0x40UL
#define EVE_OPT_FLAT                   0x100UL
#define EVE_OPT_FORMAT                 0x1000UL
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
#define EVE_OPT_OVERLAY                0x80UL
#define EVE_OPT_RGB565                 0x0UL
#define EVE_OPT_RIGHTX                 0x800UL
#define EVE_OPT_SIGNED                 0x100UL
#define EVE_OPT_SOUND                  0x20UL
#define EVE_OPT_1BIT                   0x0UL
#define EVE_OPT_FULLSPEED              0x0UL

// Touchmode Options

#define EVE_TOUCHMODE_OFF              0x0UL
#define EVE_TOUCHMODE_ONESHOT          0x1UL
#define EVE_TOUCHMODE_FRAME            0x2UL
#define EVE_TOUCHMODE_CONTINUOUS       0x3UL
#define EVE_OPT_TOUCH_100KHZ           0x800
#define EVE_OPT_TOUCH_400KHZ           0x0
#define EVE_OPT_TOUCH_FOCALTECH        0x1
#define EVE_OPT_TOUCH_GOODIX           0x2
#define EVE_OPT_TOUCH_AR1021           0x3
#define EVE_OPT_TOUCH_ILI2511          0x4
#define EVE_OPT_TOUCH_TSC2007          0x5
#define EVE_OPT_TOUCH_QUICKSIM         0x8000

// Option to plot graph horizontally, data on Y-axis
#define EVE_OPT_PLOTHORIZONTAL          0x0000
// Option to plot graph vertically, data on X-axis
#define EVE_OPT_PLOTVERTICAL            0x1000 
// Option to remove duplicate points
#define EVE_OPT_PLOTFILTER              0x2000 
// Option to invert data
#define EVE_OPT_PLOTINVERT              0x4000
// Option to create subtractive overlay bitmap
#define EVE_OPT_PLOTOVERLAY             0x8000

// Display List Swap Options

#define EVE_DLSWAP_DONE                0x0UL
#define EVE_DLSWAP_FRAME               0x2UL

// Interrupt Options

#define EVE_INT_SWAP                   0x1UL
#define EVE_INT_TOUCH                  0x2UL
#define EVE_INT_TAG                    0x4UL
#define EVE_INT_SOUND                  0x8UL
#define EVE_INT_PLAYBACK               0x10UL
#define EVE_INT_CMDEMPTY               0x20UL
#define EVE_INT_CMDFLAG                0x40UL
#define EVE_INT_CONVCOMPLETE           0x80UL
#define EVE_INT_G8                     0x12UL
#define EVE_INT_L8C                    0xcUL
#define EVE_INT_VGA                    0xdUL

// Audio Playback Options

#define EVE_VOL_ZERO                   0x0UL
#define EVE_SAMPLES_LINEAR             0x0UL
#define EVE_SAMPLES_ULAW               0x1UL
#define EVE_SAMPLES_ADPCM              0x2UL
#define EVE_SAMPLES_S16                0x3UL
#define EVE_SAMPLES_S16S               0x4UL

// Channel Definitions

#define EVE_RED                        0x2UL
#define EVE_GREEN                      0x3UL
#define EVE_BLUE                       0x4UL
#define EVE_ALPHA                      0x5UL

// ADC Options

#define EVE_ADC_SINGLE_ENDED           0x0UL
#define EVE_ADC_DIFFERENTIAL           0x1UL

// Animation Options

#define EVE_ANIM_ONCE                  0x0UL
#define EVE_ANIM_LOOP                  0x1UL
#define EVE_ANIM_HOLD                  0x2UL

// Flash Status

#define EVE_FLASH_STATUS_INIT          0x0UL
#define EVE_FLASH_STATUS_DETACHED      0x1UL
#define EVE_FLASH_STATUS_BASIC         0x2UL
#define EVE_FLASH_STATUS_FULL          0x3UL

// CGradient Shape
#define EVE_CGRADIENT_CORNER_ZERO      0x0UL
#define EVE_CGRADIENT_EDGE_ZERO        0x1UL

// Touchmode settings
#define EVE_CTOUCHMODE_EXTENDED        0x0UL
#define EVE_CTOUCHMODE_COMPATIBILITY   0x1UL

// Arrays containing the bitmap sizes of ROM fonts

#define EVE_ROMFONT_MAX 34

#define EVE_ROMFONT_WIDTHS {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, \
    8,8,8,8,10,13,14,17,24,30, \
    14,15,18,22,28,38,48,62,83}
#define EVE_ROMFONT_HEIGHTS {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, \
    8,8,16,16,13,17,20,22,29,38, \
    16,18,22,27,33,46,58,74,98}
#define EVE_ROMFONT_FORMATS {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, \
    EVE_FORMAT_L1,EVE_FORMAT_L1,EVE_FORMAT_L1,EVE_FORMAT_L1,EVE_FORMAT_L1, \
    EVE_FORMAT_L1,EVE_FORMAT_L1,EVE_FORMAT_L1,EVE_FORMAT_L1,EVE_FORMAT_L1, \
    EVE_FORMAT_L4,EVE_FORMAT_L4,EVE_FORMAT_L4,EVE_FORMAT_L4,EVE_FORMAT_L4, \
    EVE_FORMAT_L4,EVE_FORMAT_L4,EVE_FORMAT_L4,EVE_FORMAT_L4}

#endif    /* _BT82X_H_ */

