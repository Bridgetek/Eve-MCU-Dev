/**
 @file BT82x.h
 @brief Definitions for BT82x devices. 
 @details Supports BT820.
 */
/*
 * ============================================================================
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

#define EVE_RAM_G                      0x0ul
#define EVE_RAM_G_SIZE                 EVE_RAM_G_CONFIG_SIZE
#define EVE_RAM_CMD                    0x7f000000ul
#define EVE_RAM_CMD_SIZE               (16*1024L)
#define EVE_RAM_DL                     0x7f008000ul
#define EVE_RAM_DL_SIZE                (16*1024L)
#define EVE_RAM_REG                    0x7f006000ul
#define EVE_RAM_ROMSUB                 0x7f027800ul
#define EVE_ROMFONT_TABLEADDRESS       0x07ffff00ul
#define EVE_RAM_TOP                    0x00304000ul
#define EVE_RAM_REPORT                 0x00309800ul
#define EVE_COPROC_REPORT              0x7f004800ul

// Co-processor Method

// Use REG_CMDB_WRITE to fill co-processor buffer.
// Undefine to write directly to co-processor buffer.
#define EVE_USE_CMDB_METHOD

// Chip ID Register

#define EVE_REG_ID                     0x7f006000ul   //  Identification register, always reads as 0x7cul

// Control Registers

#define EVE_RAM_BIST                   0x7f8001c0ul
#define EVE_REG_CPURESET               0x7f006088ul   //  Co-processor reset bits 2:JA 1:JT 0:J1
#define EVE_REG_BOOT_CONTROL           0x7f006628ul   //  Boot control byte
#define EVE_REG_PIN_DRV_0              0x7f800408ul   //   Pin drive strength setting
#define EVE_REG_PIN_DRV_1              0x7f80040Cul   //   Pin drive strength setting
#define EVE_REG_PIN_SLEW_0             0x7f800410ul   //   Pin output slew rate setting
#define EVE_REG_PIN_TYPE_0             0x7f800414ul   //   Pin type setting
#define EVE_REG_PIN_TYPE_1             0x7f800418ul   //   Pin type setting
#define EVE_REG_SYS_CFG                0x7f800420ul   //   Miscellaneous system configuration
#define EVE_REG_SYS_STAT               0x7f800424ul   //   System status
#define EVE_REG_CHIP_ID                0x7f800448ul   //   CHIP_ID info
#define EVE_REG_BOOT_STATUS            0x7f80044Cul   //   EVE boot status
#define EVE_REG_DDR_TYPE               0x7f800454ul   //   DDR DRAM type setting
#define EVE_REG_PIN_DRV_2              0x7f800464ul   //   Pin drive strength setting
#define EVE_REG_PIN_SLEW_1             0x7f800468ul   //   Pin output slew rate setting
#define EVE_REG_PIN_TYPE_2             0x7f80046Cul   //   Pin type setting
#define EVE_REG_I2S_CFG                0x7f800800ul   //   I2S configuration registers
#define EVE_REG_I2S_CTL                0x7f800804ul   //   I2S control registers
#define EVE_REG_I2S_STAT               0x7f800810ul   //   I2S status
#define EVE_REG_I2S_PAD_CFG            0x7f800814ul   //   I2S padding configuration

// Display Registers 

#define EVE_REG_PCLK_POL               0x7f0060b8ul   //  PCLK polarity, 0 = clock on rising, 1 = falling
#define EVE_REG_VSYNC0                 0x7f0060acul   //  Vertical sync fall offset
#define EVE_REG_VSYNC1                 0x7f0060b0ul   //  Vertical sync rise offset
#define EVE_REG_VSIZE                  0x7f0060a8ul   //  Vertical display line count
#define EVE_REG_VOFFSET                0x7f0060a4ul   //  Vertical display start offset
#define EVE_REG_VCYCLE                 0x7f0060a0ul   //  Vertical total cycle count
#define EVE_REG_HSYNC0                 0x7f006098ul   //  Horizontal sync fall offset
#define EVE_REG_HSYNC1                 0x7f00609cul   //  Horizontal sync rise offset
#define EVE_REG_HSIZE                  0x7f006094ul   //  Horizontal display pixel count
#define EVE_REG_HOFFSET                0x7f006090ul   //  Horizontal display start offset
#define EVE_REG_HCYCLE                 0x7f00608cul   //  Horizontal total cycle count

#define EVE_REG_DLSWAP                 0x7f0060b4ul   //  Display list swap control

// Touch Registers 

#define EVE_REG_TAG                    0x7f0060c4ul   //  Tag query result
#define EVE_REG_TAG_X                  0x7f0060bcul   //  Tag query X coordinate
#define EVE_REG_TAG_Y                  0x7f0060c0ul   //  Tag query Y coordinate
#define EVE_REG_TOUCH_MODE             0x7f006158ul   //  Touchscreen sampling mode
#define EVE_REG_TOUCH_RAW_XY           0x7f006164ul   //  Touchscreen raw $(x,y)$ (16, 16)
#define EVE_REG_TOUCH_SCREEN_XY        0x7f006160ul   //  Touchscreen screen $(x,y)$ (16, 16)
#define EVE_REG_TOUCH_TAG              0x7f006178ul   //  Touchscreen tag result 0
#define EVE_REG_TOUCH_TAG_XY           0x7f006174ul   //  Touchscreen screen $(x,y)$ used for tag lookup
#define EVE_REG_TOUCH_TAG1             0x7f006180ul   //  Touchscreen tag result 1
#define EVE_REG_TOUCH_TAG1_XY          0x7f00617cul   //  Touchscreen screen $(x,y)$ used for tag lookup
#define EVE_REG_TOUCH_TAG2             0x7f006188ul   //  Touchscreen tag result 2
#define EVE_REG_TOUCH_TAG2_XY          0x7f006184ul   //  Touchscreen screen $(x,y)$ used for tag lookup
#define EVE_REG_TOUCH_TAG3             0x7f006190ul   //  Touchscreen tag result 3
#define EVE_REG_TOUCH_TAG3_XY          0x7f00618cul   //  Touchscreen screen $(x,y)$ used for tag lookup
#define EVE_REG_TOUCH_TAG4             0x7f006198ul   //  Touchscreen tag result 4
#define EVE_REG_TOUCH_TAG4_XY          0x7f006194ul   //  Touchscreen screen $(x,y)$ used for tag lookup
#define EVE_REG_TOUCH_TRANSFORM_A      0x7f00619cul   //  Touchscreen transform coefficient (s15.16)
#define EVE_REG_TOUCH_TRANSFORM_B      0x7f0061a0ul   //  Touchscreen transform coefficient (s15.16)
#define EVE_REG_TOUCH_TRANSFORM_C      0x7f0061a4ul   //  Touchscreen transform coefficient (s15.16)
#define EVE_REG_TOUCH_TRANSFORM_D      0x7f0061a8ul   //  Touchscreen transform coefficient (s15.16)
#define EVE_REG_TOUCH_TRANSFORM_E      0x7f0061acul   //  Touchscreen transform coefficient (s15.16)
#define EVE_REG_TOUCH_TRANSFORM_F      0x7f0061b0ul   //  Touchscreen transform coefficient (s15.16)

#define EVE_REG_CTOUCH_EXTENDED        0x7f00615cul   //  0: single-touch, 1: multi-touch
#define EVE_REG_CTOUCH_TOUCH0_XY       0x7f006160ul   //  Touchscreen screen $(x,y)$ (16, 16) Extended Mode
#define EVE_REG_CTOUCH_TOUCH4_XY       0x7f006170ul   //  Touchscreen touch 4 Extended Mode
#define EVE_REG_CTOUCH_TOUCHA_XY       0x7f006164ul   //  Touchscreen raw $(x,y)$ (16, 16) Extended Mode
#define EVE_REG_CTOUCH_TOUCHB_XY       0x7f006168ul   //  Touchscreen touch 2 Extended Mode
#define EVE_REG_CTOUCH_TOUCHC_XY       0x7f00616cul   //  Touchscreen touch 3 Extended Mode
#define EVE_REG_TOUCH_CONFIG           0x7f0061b4ul   //  Touchscreen configuration

// Media Registers

#define EVE_REG_PLAY                   0x7f0060d8ul   //  Start effect playback
#define EVE_REG_SOUND                  0x7f0060d4ul   //  Sound effect select
#define EVE_REG_VOL_L_PB               0x7f0060c8ul   //  Volume for playback left
#define EVE_REG_VOL_R_PB               0x7f0060ccul   //  Volume for playback right
#define EVE_REG_VOL_SOUND              0x7f0060d0ul   //  Volume for synth sound
#define EVE_REG_PLAY_CONTROL           0x7f004050ul   //  Video playback control
#define EVE_REG_PLAYBACK_FORMAT        0x7f00611cul   //  Audio playback format
#define EVE_REG_PLAYBACK_FREQ          0x7f006118ul   //  Audio playback frequency (Hz)
#define EVE_REG_PLAYBACK_LENGTH        0x7f006110ul   //  Audio playback sample length (bytes)
#define EVE_REG_PLAYBACK_LOOP          0x7f006120ul   //  Audio playback loop enable
#define EVE_REG_PLAYBACK_PAUSE         0x7f0065d0ul   //  audio playback pause, 0=play 1=pause
#define EVE_REG_PLAYBACK_PLAY          0x7f006124ul   //  Start audio playback
#define EVE_REG_PLAYBACK_READPTR       0x7f006114ul   //  Audio playback current read pointer
#define EVE_REG_PLAYBACK_START         0x7f00610cul   //  Audio playback RAM start address
#define EVE_REG_MEDIAFIFO_READ         0x7f004014ul   //  media FIFO read offset
#define EVE_REG_MEDIAFIFO_WRITE        0x7f004018ul   //  media FIFO write offset
#define EVE_REG_ANIM_ACTIVE            0x7f00402cul   //  32-bit mask of currently playing animations

// Display List Registers

#define EVE_REG_CMD_DL                 0x7f006154ul   //  Command DL offset
#define EVE_REG_CMD_READ               0x7f00614cul   //  Command buffer read pointer
#define EVE_REG_CMD_WRITE              0x7f006150ul   //  Command buffer write pointer
#define EVE_REG_CMDB_WRITE             0x7f010000ul   //  Command write
#define EVE_REG_CMDB_SPACE             0x7f006594ul   //  Command (bulk) space available

// Tracking Registers

#define EVE_REG_TRACKER                0x7f004000ul   //  Tracker register 0
#define EVE_REG_TRACKER_1              0x7f004004ul   //  Tracker register 1
#define EVE_REG_TRACKER_2              0x7f004008ul   //  Tracker register 2
#define EVE_REG_TRACKER_3              0x7f00400cul   //  Tracker register 3
#define EVE_REG_TRACKER_4              0x7f004010ul   //  Tracker register 4

// PWM Registers

#define EVE_REG_PWM_DUTY               0x7f00612cul   //  PWM output duty cycle 0=0\%, 128=100\%
#define EVE_REG_PWM_HZ                 0x7f006128ul   //  PWM output frequency (Hz)

// Interrupt Registers

#define EVE_REG_INT_EN                 0x7f006104ul   //  Global interrupt enable
#define EVE_REG_INT_FLAGS              0x7f006100ul   //  Interrupt flags
#define EVE_REG_INT_MASK               0x7f006108ul   //  Interrupt enable mask

// Screenshot Registers

// Clock Registers

#define EVE_REG_FREQUENCY              0x7f00600cul   //  Main clock frequency
#define EVE_REG_FRAMES                 0x7f006004ul   //  Frame counter, since reset
#define EVE_REG_CLOCK                  0x7f006008ul   //  Clock cycles, since reset

// Macro Registers

#define EVE_REG_MACRO_0                0x7f006130ul   //  Display list macro command 0
#define EVE_REG_MACRO_1                0x7f006134ul   //  Display list macro command 1

// GPIO Registers

#define EVE_REG_GPIO                   0x7f0060e0ul   //  GPIO read/write
#define EVE_REG_GPIO_DIR               0x7f0060dcul   //  GPIO pin direction, 1 = output

// I2C Registers

#define EVE_REG_FREQUENCY_A            0x7f00671cul   //  I2S adjusted main frequency
#define EVE_REG_I2S_EN                 0x7f006714ul   //  I2S interface enable
#define EVE_REG_I2S_FREQ               0x7f006718ul   //  I2S sample frequency

// Flash Controller Registers

#define EVE_REG_FLASH_DTR              0x7f0065ecul   //  Flash DTR (Double Transfer Rate) enable
#define EVE_REG_FLASH_SIZE             0x7f004024ul   //  Detected flash capacity, in Mbytes
#define EVE_REG_FLASH_STATUS           0x7f0065d4ul   //  Flash status 0=INIT 1=DETACHED 2=BASIC 3=FULL

// Extent Registers

#define EVE_REG_EXTENT_X0              0x7f00403cul   //  Previous widget pixel extents
#define EVE_REG_EXTENT_X1              0x7f004044ul   //  Previous widget pixel extents
#define EVE_REG_EXTENT_Y0              0x7f004040ul   //  Previous widget pixel extents
#define EVE_REG_EXTENT_Y1              0x7f004048ul   //  Previous widget pixel extents

// Render Engine Registers

#define EVE_REG_DISP                   0x7f0060e4ul   //  DISP output
#define EVE_REG_RE_ACTIVE              0x7f006028ul   //  RE write path active
#define EVE_REG_RE_DEST                0x7f006010ul   //  RE destination address
#define EVE_REG_RE_DITHER              0x7f006024ul   //  RE target dither enable
#define EVE_REG_RE_FORMAT              0x7f006014ul   //  RE format, as bitmap format
#define EVE_REG_RE_H                   0x7f006020ul   //  RE target height, in pixels
#define EVE_REG_RE_RENDERS             0x7f00602cul   //  RE render counter
#define EVE_REG_RE_ROTATE              0x7f006018ul   //  RE rotate control
#define EVE_REG_RE_TESTMODE            0x7f006030ul
#define EVE_REG_RE_W                   0x7f00601cul   //  RE target width, in pixels

// Swapchain Registers

#define EVE_REG_SC0_PTR0               0x7f00603cul   //  Swapchain 0, pointer 0
#define EVE_REG_SC0_PTR1               0x7f006040ul   //  Swapchain 0, pointer 1
#define EVE_REG_SC0_PTR2               0x7f006044ul   //  Swapchain 0, pointer 2
#define EVE_REG_SC0_PTR3               0x7f006048ul   //  Swapchain 0, pointer 3
#define EVE_REG_SC0_RESET              0x7f006034ul   //  Swapchain 0, write to reset
#define EVE_REG_SC0_SIZE               0x7f006038ul   //  Swapchain 0, ring size 1-4
#define EVE_REG_SC1_I_PTR              0x7f006084ul   //  Swapchain 1, input pointer (rd)
#define EVE_REG_SC1_I_READY            0x7f006080ul   //  Swapchain 1, input ready (wr)
#define EVE_REG_SC1_I_VALID            0x7f00607cul   //  Swapchain 1, input valid (rd)
#define EVE_REG_SC1_PTR0               0x7f006054ul   //  Swapchain 0, pointer 0
#define EVE_REG_SC1_PTR1               0x7f006058ul   //  Swapchain 0, pointer 1
#define EVE_REG_SC1_PTR2               0x7f00605cul   //  Swapchain 0, pointer 2
#define EVE_REG_SC1_PTR3               0x7f006060ul   //  Swapchain 0, pointer 3
#define EVE_REG_SC1_RESET              0x7f00604cul   //  Swapchain 0, write to reset
#define EVE_REG_SC1_SIZE               0x7f006050ul   //  Swapchain 0, ring size 1-4
#define EVE_REG_SC2_ADDR               0x7f006784ul   //  Swapchain 2 status, write to reset
#define EVE_REG_SC2_PTR0               0x7f00606cul   //  Swapchain 0, pointer 0
#define EVE_REG_SC2_PTR1               0x7f006070ul   //  Swapchain 0, pointer 1
#define EVE_REG_SC2_PTR2               0x7f006074ul   //  Swapchain 0, pointer 2
#define EVE_REG_SC2_PTR3               0x7f006078ul   //  Swapchain 0, pointer 3
#define EVE_REG_SC2_RESET              0x7f006064ul   //  Swapchain 0, write to reset
#define EVE_REG_SC2_SIZE               0x7f006068ul   //  Swapchain 0, ring size 1-4
#define EVE_REG_SC2_STATUS             0x7f006780ul   //  Swapchain 2 status, write to reset
#define EVE_REG_SO_EN                  0x7f006600ul   //  Scanout 0 enable
#define EVE_REG_SO_FORMAT              0x7f0065fcul   //  Scanout 0 format
#define EVE_REG_SO_MODE                0x7f0065f4ul   //  Scanout pixel mode
#define EVE_REG_SO_SOURCE              0x7f0065f8ul   //  Scanout 0 source

// LVDS Registers

#define EVE_REG_LVDSRX_CORE_ENABLE     0x7f006670ul   // LVDSRX enable register
#define EVE_REG_LVDSRX_CORE_CAPTURE    0x7f006674ul   // LVDSRX enable capture register
#define EVE_REG_LVDSRX_CORE_SETUP      0x7f006678ul   // LVDSRX pixel setup control register
#define EVE_REG_LVDSRX_CORE_DEST       0x7f00667cul   // LVDSRX destination frame address register
#define EVE_REG_LVDSRX_CORE_FORMAT     0x7f006680ul   // LVDSRX output pixel format register
#define EVE_REG_LVDSRX_CORE_DITHER     0x7f006684ul   // LVDSRX enable dither register
#define EVE_REG_LVDSRX_CORE_FRAMES     0x7f006698ul   // LVDSRX frame counter
#define EVE_REG_LVDSRX_SETUP           0x7F800500ul   // LVDSRX system set-up
#define EVE_REG_LVDSRX_CTRL            0x7F800504ul   // LVDSRX analog block configuration
#define EVE_REG_LVDSRX_STAT            0x7F800508ul   // LVDSRX status register
#define EVE_REG_LVDSTX_EN              0x7f800300ul   // LVDS enables
#define EVE_REG_LVDSTX_PLLCFG          0x7f800304ul   // LVDS PLL and Clock configuration
#define EVE_REG_LVDSTX_CTRL_CH0        0x7f800314ul   // LVDS channel 0 control
#define EVE_REG_LVDSTX_CTRL_CH1        0x7f800318ul   // LVDS channel 1 control
#define EVE_REG_LVDSTX_STAT            0x7f80031cul   // LVDS status
#define EVE_REG_LVDSTX_ERR_STAT        0x7f800320ul   // LVDS error status

// Completion Register

#define EVE_REG_OBJECT_COMPLETE        0x7f004038ul   //  Used with OPT_COMPLETEREG

// Bitmap Address Mask 

#define EVE_ENC_BITMAP_ADDR_MASK       0xfffffffful

// Encoded Command Macros

#define EVE_ENC_ALPHA_FUNC(func,ref)        ((0x9ul<<24)|(((func)&0x7ul)<<8)|(((ref)&0xfful)<<0))
#define EVE_ENC_BEGIN(prim)                 ((0x1ful<<24)|(((prim)&0xful)<<0))
#define EVE_ENC_BITMAP_EXT_FORMAT(format)   ((0x2eul<<24)|(((format)&0xfffful)<<0))
#define EVE_ENC_BITMAP_HANDLE(handle)       ((0x5ul<<24)|(((handle)&0x3ful)<<0))
#define EVE_ENC_BITMAP_LAYOUT(format,linestride,height) ((0x7ul<<24)|(((format)&0x1ful)<<19)|(((linestride)&0x3fful)<<9)|(((height)&0x1FFul)<<0))
#define EVE_ENC_BITMAP_LAYOUT_H(linestride,height) ((0x28ul<<24)|(((linestride)&0x3ul)<<2)|(((height)&0x3ul)<<0))
#define EVE_ENC_BITMAP_SIZE(filter,wrapx,wrapy,width,height) ((0x8ul<<24)|(((filter)&0x1ul)<<20)|(((wrapx)&0x1ul)<<19)|(((wrapy)&0x1ul)<<18)|(((width)&0x1FFul)<<9)|(((height)&0x1FFul)<<0))
#define EVE_ENC_BITMAP_SIZE_H(width,height) ((0x29ul<<24)|(((width)&0x3ul)<<2)|(((height)&0x3ul)<<0))
#define EVE_ENC_BITMAP_SOURCE(addr)         ((0x1ul << 24) | ((addr)&0xFFFFFFul))
#define EVE_ENC_BITMAP_SOURCE_H(addr)       ((0x31ul << 24) | ((addr)&0xFFul))
#define EVE_ENC_BITMAP_SWIZZLE(r,g,b,a)     ((0x2ful<<24)|(((r)&0x7ul)<<9)|(((g)&0x7ul)<<6)|(((b)&0x7ul)<<3)|(((a)&0x7ul)<<0))
#define EVE_ENC_BITMAP_TRANSFORM_A_EXT(p,v) ((0x15ul<<24)|(((p)&0x1ul)<<17)|(((v)&0x1FFFFul)<<0))
#define EVE_ENC_BITMAP_TRANSFORM_B_EXT(p,v) ((0x16ul<<24)|(((p)&0x1ul)<<17)|(((v)&0x1FFFFul)<<0))
#define EVE_ENC_BITMAP_TRANSFORM_C(c)       ((0x17ul<<24)|(((c)&0xFFFFFFul)<<0))
#define EVE_ENC_BITMAP_TRANSFORM_D_EXT(p,v) ((0x18ul<<24)|(((p)&0x1ul)<<17)|(((v)&0x1FFFFul)<<0))
#define EVE_ENC_BITMAP_TRANSFORM_E_EXT(p,v) ((0x19ul<<24)|(((p)&0x1ul)<<17)|(((v)&0x1FFFFul)<<0))
#define EVE_ENC_BITMAP_TRANSFORM_F(f)       ((0x1aul<<24)|(((f)&0xFFFFFFul)<<0))
#define EVE_ENC_BITMAP_ZORDER(o)            ((0x33ul<<24)|(((o)&0xfful)<<0))
#define EVE_ENC_BLEND_FUNC(src,dst)         ((0xbul<<24)|(((src)&0x7ul)<<3)|(((dst)&0x7ul)<<0))
#define EVE_ENC_CALL(dest)                  ((0x1dul<<24)|(((dest)&0xfffful)<<0))
#define EVE_ENC_CELL(cell)                  ((0x6ul<<24)|(((cell)&0x7Ful)<<0))
#define EVE_ENC_CLEAR(c,s,t)                ((0x26ul<<24)|(((c)&0x1ul)<<2)|(((s)&0x1ul)<<1)|(((t)&0x1ul)<<0))
#define EVE_ENC_CLEAR_COLOR(c)              ((0x2ul<<24)|(((uint32_t)(c)) & 0x00fffffful))
#define EVE_ENC_CLEAR_COLOR_A(alpha)        ((0xful<<24)|(((alpha)&0xfful)<<0))
#define EVE_ENC_CLEAR_COLOR_RGB(red,green,blue) ((0x2ul<<24)|(((red)&0xfful)<<16)|(((green)&0xfful)<<8)|(((blue)&0xfful)<<0))
#define EVE_ENC_CLEAR_STENCIL(s)            ((0x11ul<<24)|(((s)&0xfful)<<0))
#define EVE_ENC_CLEAR_TAG(s)                ((0x12ul<<24)|(((s)&0xfful)<<0))
#define EVE_ENC_COLOR(c)                    ((0x4ul<<24)|(((uint32_t)(c)) & 0x00fffffful))
#define EVE_ENC_COLOR_A(alpha)              ((0x10ul<<24)|(((alpha)&0xfful)<<0))
#define EVE_ENC_COLOR_MASK(r,g,b,a)         ((0x20ul<<24)|(((r)&0x1ul)<<3)|(((g)&0x1ul)<<2)|(((b)&0x1ul)<<1)|(((a)&0x1ul)<<0))
#define EVE_ENC_COLOR_RGB(red,green,blue)   ((0x4ul<<24)|(((red)&0xfful)<<16)|(((green)&0xfful)<<8)|(((blue)&0xfful)<<0))
#define EVE_ENC_DISPLAY()                   ((0x0ul<<24))
#define EVE_ENC_END()                       ((0x21ul<<24))
#define EVE_ENC_JUMP(dest)                  ((0x1ful<<24)|(((dest)&0xfffful)<<0))
#define EVE_ENC_LINE_WIDTH(width)           ((0xeul<<24)|(((width)&0xFFFul)<<0))
#define EVE_ENC_MACRO(m)                    ((0x25ul<<24)|(((m)&0x1ul)<<0))
#define EVE_ENC_NOP()                       ((0x2dul<<24))
#define EVE_ENC_PALETTE_SOURCE(addr)        ((0x2aul<<24)|(((addr)&0x3FFFFFul)<<0))
#define EVE_ENC_POINT_SIZE(size)            ((0xdul<<24)|(((size)&0x1FFFul)<<0))
#define EVE_ENC_RESTORE_CONTEXT()           ((0x23ul<<24))
#define EVE_ENC_PALLETE_SOURCE_H(addr)      ((0x32ul<< 24) | ((addr)&0xFFul))
#define EVE_ENC_REGION(y,h,dest)            ((0x34ul<<24)|(((y)&0x3ful)<<18)|(((h)&0x3ful)<<12)|(((dest)&0xffful)<<0))
#define EVE_ENC_RETURN()                    ((0x24ul<<24))
#define EVE_ENC_SAVE_CONTEXT()              ((0x22ul<<24))
#define EVE_ENC_SCISSOR_SIZE(width,height)  ((0x1cul<<24)|(((width)&0xFFFul)<<12)|(((height)&0xFFFul)<<0))
#define EVE_ENC_SCISSOR_XY(x,y)             ((0x1bul<<24)|(((x)&0x7FFul)<<11)|(((y)&0x7FFul)<<0))
#define EVE_ENC_STENCIL_FUNC(func,ref,mask) ((0xaul<<24)|(((func)&0x7ul)<<16)|(((ref)&0xfful)<<8)|(((mask)&0xfful)<<0))
#define EVE_ENC_STENCIL_MASK(mask)          ((0x13ul<<24)|(((mask)&0xfful)<<0))
#define EVE_ENC_STENCIL_OP(sfail,spass)     ((0xcul<<24)|(((sfail)&0x7ul)<<3)|(((spass)&0x7ul)<<0))
#define EVE_ENC_TAG(s)                      ((0x3ul<<24)|(((s)&0xfffffful)<<0))
#define EVE_ENC_TAG_MASK(mask)              ((0x14ul<<24)|(((mask)&0x1ul)<<0))
#define EVE_ENC_VERTEX2F(x,y)               ((0x1ul<<30)|(((x)&0x7ffful)<<15)|(((y)&0x7ffful)<<0))
#define EVE_ENC_VERTEX2II(x,y,handle,cell)  ((0x2ul<<30)|(((x)&0x1FFul)<<21)|(((y)&0x1FFul)<<12)|(((handle)&0x1ful)<<7)|(((cell)&0x7Ful)<<0))
#define EVE_ENC_VERTEX_FORMAT(frac)         ((0x27ul<<24)|(((frac)&0x7ul)<<0))
#define EVE_ENC_VERTEX_TRANSLATE_X(x)       ((0x2bul<<24)|(((x)&0x1FFFFul)<<0))
#define EVE_ENC_VERTEX_TRANSLATE_Y(y)       ((0x2cul<<24)|(((y)&0x1FFFFul)<<0))

// Added to ensure previous macros are fine
#define EVE_ENC_BITMAP_TRANSFORM_A(a) EVE_ENC_BITMAP_TRANSFORM_A_EXT(0,a)
#define EVE_ENC_BITMAP_TRANSFORM_B(b) EVE_ENC_BITMAP_TRANSFORM_B_EXT(0,b)
#define EVE_ENC_BITMAP_TRANSFORM_D(d) EVE_ENC_BITMAP_TRANSFORM_D_EXT(0,d)
#define EVE_ENC_BITMAP_TRANSFORM_E(e) EVE_ENC_BITMAP_TRANSFORM_E_EXT(0,e)

// Co-processor Commands

#define EVE_ENC_CMD_ANIMDRAW           0xffffff4ful
#define EVE_ENC_CMD_ANIMFRAME          0xffffff5eul
#define EVE_ENC_CMD_ANIMSTART          0xffffff5ful
#define EVE_ENC_CMD_ANIMSTOP           0xffffff4dul
#define EVE_ENC_CMD_ANIMXY             0xffffff4eul
#define EVE_ENC_CMD_APBREAD            0xffffff62ul
#define EVE_ENC_CMD_APBWRITE           0xffffff63ul
#define EVE_ENC_CMD_APPEND             0xffffff1cul
#define EVE_ENC_CMD_APPENDF            0xffffff52ul
#define EVE_ENC_CMD_ARC                0xffffff87ul
#define EVE_ENC_CMD_BGCOLOR            0xffffff07ul
#define EVE_ENC_CMD_BITMAP_TRANSFORM   0xffffff1ful
#define EVE_ENC_CMD_BUTTON             0xffffff0bul
#define EVE_ENC_CMD_CALIBRATE          0xffffff13ul
#define EVE_ENC_CMD_CALIBRATESUB       0xffffff56ul
#define EVE_ENC_CMD_CALLLIST           0xffffff5bul
#define EVE_ENC_CMD_CGRADIENT          0xffffff8aul
#define EVE_ENC_CMD_CLOCK              0xffffff12ul
#define EVE_ENC_CMD_COLDSTART          0xffffff2eul
#define EVE_ENC_CMD_COPYLIST           0xffffff88ul
#define EVE_ENC_CMD_DDRSHUTDOWN        0xffffff65ul
#define EVE_ENC_CMD_DDRSTARTUP         0xffffff66ul
#define EVE_ENC_CMD_DIAL               0xffffff29ul
#define EVE_ENC_CMD_DLSTART            0xffffff00ul
#define EVE_ENC_CMD_ENABLEREGION       0xffffff7eul
#define EVE_ENC_CMD_ENDLIST            0xffffff5dul
#define EVE_ENC_CMD_ENDREGION          0xffffff90ul
#define EVE_ENC_CMD_FENCE              0xffffff68ul
#define EVE_ENC_CMD_FGCOLOR            0xffffff08ul
#define EVE_ENC_CMD_FILLWIDTH          0xffffff51ul
#define EVE_ENC_CMD_FLASHATTACH        0xffffff43ul
#define EVE_ENC_CMD_FLASHDETACH        0xffffff42ul
#define EVE_ENC_CMD_FLASHERASE         0xffffff3eul
#define EVE_ENC_CMD_FLASHFAST          0xffffff44ul
#define EVE_ENC_CMD_FLASHPROGRAM       0xffffff64ul
#define EVE_ENC_CMD_FLASHREAD          0xffffff40ul
#define EVE_ENC_CMD_FLASHSOURCE        0xffffff48ul
#define EVE_ENC_CMD_FLASHSPIDESEL      0xffffff45ul
#define EVE_ENC_CMD_FLASHSPIRX         0xffffff47ul
#define EVE_ENC_CMD_FLASHSPITX         0xffffff46ul
#define EVE_ENC_CMD_FLASHUPDATE        0xffffff41ul
#define EVE_ENC_CMD_FLASHWRITE         0xffffff3ful
#define EVE_ENC_CMD_FSDIR              0xffffff8eul
#define EVE_ENC_CMD_FSOPTIONS          0xffffff6dul
#define EVE_ENC_CMD_FSREAD             0xffffff71ul
#define EVE_ENC_CMD_FSSIZE             0xffffff80ul
#define EVE_ENC_CMD_FSSOURCE           0xffffff7ful
#define EVE_ENC_CMD_GAUGE              0xffffff11ul
#define EVE_ENC_CMD_GETIMAGE           0xffffff58ul
#define EVE_ENC_CMD_GETMATRIX          0xffffff2ful
#define EVE_ENC_CMD_GETPROPS           0xffffff22ul
#define EVE_ENC_CMD_GETPTR             0xffffff20ul
#define EVE_ENC_CMD_GLOW               0xffffff8bul
#define EVE_ENC_CMD_GRADCOLOR          0xffffff30ul
#define EVE_ENC_CMD_GRADIENT           0xffffff09ul
#define EVE_ENC_CMD_GRADIENTA          0xffffff50ul
#define EVE_ENC_CMD_GRAPHICSFINISH     0xffffff6bul
#define EVE_ENC_CMD_I2SSTARTUP         0xffffff69ul
#define EVE_ENC_CMD_INFLATE            0xffffff4aul
#define EVE_ENC_CMD_INTERRUPT          0xffffff02ul
#define EVE_ENC_CMD_KEYS               0xffffff0cul
#define EVE_ENC_CMD_LOADASSET          0xffffff81ul
#define EVE_ENC_CMD_LOADIDENTITY       0xffffff23ul
#define EVE_ENC_CMD_LOADIMAGE          0xffffff21ul
#define EVE_ENC_CMD_LOADWAV            0xffffff85ul
#define EVE_ENC_CMD_LOADPATCH          0xffffff82ul
#define EVE_ENC_CMD_LOGO               0xffffff2dul
#define EVE_ENC_CMD_MEDIAFIFO          0xffffff34ul
#define EVE_ENC_CMD_MEMCPY             0xffffff1bul
#define EVE_ENC_CMD_MEMCRC             0xffffff16ul
#define EVE_ENC_CMD_MEMSET             0xffffff19ul
#define EVE_ENC_CMD_MEMWRITE           0xffffff18ul
#define EVE_ENC_CMD_MEMZERO            0xffffff1aul
#define EVE_ENC_CMD_NEWLIST            0xffffff5cul
#define EVE_ENC_CMD_NOP                0xffffff53ul
#define EVE_ENC_CMD_NUMBER             0xffffff2aul
#define EVE_ENC_CMD_PLAYVIDEO          0xffffff35ul
#define EVE_ENC_CMD_PLAYWAV            0xffffff79ul
#define EVE_ENC_CMD_PROGRESS           0xffffff0dul
#define EVE_ENC_CMD_REGION             0xffffff8ful
#define EVE_ENC_CMD_REGREAD            0xffffff17ul
#define EVE_ENC_CMD_REGWRITE           0xffffff86ul
#define EVE_ENC_CMD_RENDERTARGET       0xffffff8dul
#define EVE_ENC_CMD_RESETFONTS         0xffffff4cul
#define EVE_ENC_CMD_RESTORECONTEXT     0xffffff7dul
#define EVE_ENC_CMD_RESULT             0xffffff89ul
#define EVE_ENC_CMD_RETURN             0xffffff5aul
#define EVE_ENC_CMD_ROMFONT            0xffffff39ul
#define EVE_ENC_CMD_ROTATE             0xffffff26ul
#define EVE_ENC_CMD_ROTATEAROUND       0xffffff4bul
#define EVE_ENC_CMD_RUNANIM            0xffffff60ul
#define EVE_ENC_CMD_SAVECONTEXT        0xffffff7cul
#define EVE_ENC_CMD_SCALE              0xffffff25ul
#define EVE_ENC_CMD_SCREENSAVER        0xffffff2bul
#define EVE_ENC_CMD_SCROLLBAR          0xffffff0ful
#define EVE_ENC_CMD_SDATTACH           0xffffff6eul
#define EVE_ENC_CMD_SDBLOCKREAD        0xffffff6ful
#define EVE_ENC_CMD_SETBASE            0xffffff33ul
#define EVE_ENC_CMD_SETBITMAP          0xffffff3dul
#define EVE_ENC_CMD_SETFONT            0xffffff36ul
#define EVE_ENC_CMD_SETMATRIX          0xffffff27ul
#define EVE_ENC_CMD_SETROTATE          0xffffff31ul
#define EVE_ENC_CMD_SETSCRATCH         0xffffff37ul
#define EVE_ENC_CMD_SKETCH             0xffffff2cul
#define EVE_ENC_CMD_SKIPCOND           0xffffff8cul
#define EVE_ENC_CMD_SLIDER             0xffffff0eul
#define EVE_ENC_CMD_SNAPSHOT           0xffffff1dul
#define EVE_ENC_CMD_SPINNER            0xffffff14ul
#define EVE_ENC_CMD_STOP               0xffffff15ul
#define EVE_ENC_CMD_SWAP               0xffffff01ul
#define EVE_ENC_CMD_SYNC               0xffffff3cul
#define EVE_ENC_CMD_TESTCARD           0xffffff57ul
#define EVE_ENC_CMD_TEXT               0xffffff0aul
#define EVE_ENC_CMD_TEXTDIM            0xffffff84ul
#define EVE_ENC_CMD_TOGGLE             0xffffff10ul
#define EVE_ENC_CMD_TRACK              0xffffff28ul
#define EVE_ENC_CMD_TRANSLATE          0xffffff24ul
#define EVE_ENC_CMD_VIDEOFRAME         0xffffff3bul
#define EVE_ENC_CMD_VIDEOSTART         0xffffff3aul
#define EVE_ENC_CMD_WAIT               0xffffff59ul
#define EVE_ENC_CMD_WAITCHANGE         0xffffff67ul
#define EVE_ENC_CMD_WAITCOND           0xffffff78ul
#define EVE_ENC_CMD_WATCHDOG           0xffffff83ul

// Aliases 

#define FTPOINTS                       POINTS

// Hardware swapchains for render engine

#define EVE_SWAPCHAIN_0                0xffff00fful 
#define EVE_SWAPCHAIN_1                0xffff01fful
#define EVE_SWAPCHAIN_2                0xffff02fful

// Bitmap Formats

#define EVE_FORMAT_ARGB1555            0x0ul
#define EVE_FORMAT_L1                  0x1ul
#define EVE_FORMAT_L4                  0x2ul
#define EVE_FORMAT_L8                  0x3ul
#define EVE_FORMAT_RGB332              0x4ul
#define EVE_FORMAT_ARGB2               0x5ul
#define EVE_FORMAT_ARGB4               0x6ul
#define EVE_FORMAT_RGB565              0x7ul
#define EVE_FORMAT_PALETTED            0x8ul
#define EVE_FORMAT_TEXT8X8             0x9ul
#define EVE_FORMAT_TEXTVGA             0xaul
#define EVE_FORMAT_BARGRAPH            0xbul
#define EVE_FORMAT_PALETTED565         0xeul
#define EVE_FORMAT_PALETTED4444        0xful
#define EVE_FORMAT_PALETTED8           0x10ul
#define EVE_FORMAT_L2                  0x11ul
#define EVE_FORMAT_RGB8                0x13ul
#define EVE_FORMAT_ARGB8               0x14ul
#define EVE_FORMAT_PALETTEDARGB8       0x15ul
#define EVE_FORMAT_RGB6                0x16ul
#define EVE_FORMAT_ARGB6               0x17ul
#define EVE_FORMAT_LA1                 0x18ul
#define EVE_FORMAT_LA2                 0x19ul
#define EVE_FORMAT_LA4                 0x1aul
#define EVE_FORMAT_LA8                 0x1bul
#define EVE_FORMAT_YCBCR               0x1cul
// Extended Bitmap Format to enable BITMAP_EXT_FORMAT
#define EVE_GLFORMAT                   0x1ful
// BITMAP_EXT_FORMATs
#define EVE_FORMAT_COMPRESSED_RGBA_ASTC_10x10_KHR 0x93bbul
#define EVE_FORMAT_COMPRESSED_RGBA_ASTC_10x5_KHR  0x93b8ul
#define EVE_FORMAT_COMPRESSED_RGBA_ASTC_10x6_KHR  0x93b9ul
#define EVE_FORMAT_COMPRESSED_RGBA_ASTC_10x8_KHR  0x93baul
#define EVE_FORMAT_COMPRESSED_RGBA_ASTC_12x10_KHR 0x93bcul
#define EVE_FORMAT_COMPRESSED_RGBA_ASTC_12x12_KHR 0x93bdul
#define EVE_FORMAT_COMPRESSED_RGBA_ASTC_4x4_KHR   0x93b0ul
#define EVE_FORMAT_COMPRESSED_RGBA_ASTC_5x4_KHR   0x93b1ul
#define EVE_FORMAT_COMPRESSED_RGBA_ASTC_5x5_KHR   0x93b2ul
#define EVE_FORMAT_COMPRESSED_RGBA_ASTC_6x5_KHR   0x93b3ul
#define EVE_FORMAT_COMPRESSED_RGBA_ASTC_6x6_KHR   0x93b4ul
#define EVE_FORMAT_COMPRESSED_RGBA_ASTC_8x5_KHR   0x93b5ul
#define EVE_FORMAT_COMPRESSED_RGBA_ASTC_8x6_KHR   0x93b6ul
#define EVE_FORMAT_COMPRESSED_RGBA_ASTC_8x8_KHR   0x93b7ul

// Command Options

#define EVE_BEGIN_BITMAPS              0x1ul
#define EVE_BEGIN_POINTS               0x2ul
#define EVE_BEGIN_LINES                0x3ul
#define EVE_BEGIN_LINE_STRIP           0x4ul
#define EVE_BEGIN_EDGE_STRIP_R         0x5ul
#define EVE_BEGIN_EDGE_STRIP_L         0x6ul
#define EVE_BEGIN_EDGE_STRIP_A         0x7ul
#define EVE_BEGIN_EDGE_STRIP_B         0x8ul
#define EVE_BEGIN_RECTS                0x9ul

#define EVE_TEST_NEVER                 0x0ul
#define EVE_TEST_LESS                  0x1ul
#define EVE_TEST_LEQUAL                0x2ul
#define EVE_TEST_GREATER               0x3ul
#define EVE_TEST_GEQUAL                0x4ul
#define EVE_TEST_EQUAL                 0x5ul
#define EVE_TEST_NOTEQUAL              0x6ul
#define EVE_TEST_ALWAYS                0x7ul

#define EVE_FILTER_NEAREST             0x0ul
#define EVE_FILTER_BILINEAR            0x1ul

#define EVE_WRAP_BORDER                0x0ul
#define EVE_WRAP_REPEAT                0x1ul

#define EVE_BLEND_ZERO                 0x0ul
#define EVE_BLEND_ONE                  0x1ul
#define EVE_BLEND_SRC_ALPHA            0x2ul
#define EVE_BLEND_ONE_MINUS_SRC_ALPHA  0x4ul
#define EVE_BLEND_DST_ALPHA            0x3ul
#define EVE_BLEND_ONE_MINUS_DST_ALPHA  0x5ul

#define EVE_STENCIL_ZERO               0x0ul
#define EVE_STENCIL_KEEP               0x1ul
#define EVE_STENCIL_REPLACE            0x2ul
#define EVE_STENCIL_INCR               0x3ul
#define EVE_STENCIL_DECR               0x4ul
#define EVE_STENCIL_INVERT             0x5ul

#define EVE_OPT_3D                     0x0ul
#define EVE_OPT_CENTER                 0x600ul
#define EVE_OPT_CENTERX                0x200ul
#define EVE_OPT_CENTERY                0x400ul
#define EVE_OPT_FILL                   0x2000ul
#define EVE_OPT_FLASH                  0x40ul
#define EVE_OPT_FLAT                   0x100ul
#define EVE_OPT_FORMAT                 0x1000ul
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
#define EVE_OPT_OVERLAY                0x80ul
#define EVE_OPT_RGB565                 0x0ul
#define EVE_OPT_RIGHTX                 0x800ul
#define EVE_OPT_SIGNED                 0x100ul
#define EVE_OPT_SOUND                  0x20ul
#define EVE_OPT_1BIT                   0x0ul
#define EVE_OPT_FULLSPEED              0x0ul

// Touchmode Options

#define EVE_TOUCHMODE_OFF              0x0ul
#define EVE_TOUCHMODE_ONESHOT          0x1ul
#define EVE_TOUCHMODE_FRAME            0x2ul
#define EVE_TOUCHMODE_CONTINUOUS       0x3ul
#define EVE_OPT_TOUCH_100KHZ           0x800ul
#define EVE_OPT_TOUCH_400KHZ           0x0ul
#define EVE_OPT_TOUCH_FOCALTECH        0x1ul
#define EVE_OPT_TOUCH_GOODIX           0x2ul
#define EVE_OPT_TOUCH_AR1021           0x3ul
#define EVE_OPT_TOUCH_ILI2511          0x4ul
#define EVE_OPT_TOUCH_TSC2007          0x5ul
#define EVE_OPT_TOUCH_QUICKSIM         0x8000ul

// Display List Swap Options

#define EVE_DLSWAP_DONE                0x0ul
#define EVE_DLSWAP_FRAME               0x2ul

// Interrupt Options

#define EVE_INT_SWAP                   0x1ul
#define EVE_INT_TOUCH                  0x2ul
#define EVE_INT_TAG                    0x4ul
#define EVE_INT_SOUND                  0x8ul
#define EVE_INT_PLAYBACK               0x10ul
#define EVE_INT_CMDEMPTY               0x20ul
#define EVE_INT_CMDFLAG                0x40ul
#define EVE_INT_CONVCOMPLETE           0x80ul
#define EVE_INT_G8                     0x12ul
#define EVE_INT_L8C                    0xcul
#define EVE_INT_VGA                    0xdul

// Audio Playback Options

#define EVE_VOL_ZERO                   0x0ul
#define EVE_SAMPLES_LINEAR             0x0ul
#define EVE_SAMPLES_ULAW               0x1ul
#define EVE_SAMPLES_ADPCM              0x2ul
#define EVE_SAMPLES_S16                0x3ul
#define EVE_SAMPLES_S16S               0x4ul

// Channel Definitions

#define EVE_RED                        0x2ul
#define EVE_GREEN                      0x3ul
#define EVE_BLUE                       0x4ul
#define EVE_ALPHA                      0x5ul

// ADC Options

#define EVE_ADC_SINGLE_ENDED           0x0ul
#define EVE_ADC_DIFFERENTIAL           0x1ul

// Animation Options

#define EVE_ANIM_ONCE                  0x0ul
#define EVE_ANIM_LOOP                  0x1ul
#define EVE_ANIM_HOLD                  0x2ul

// Flash Status

#define EVE_FLASH_STATUS_INIT          0x0ul
#define EVE_FLASH_STATUS_DETACHED      0x1ul
#define EVE_FLASH_STATUS_BASIC         0x2ul
#define EVE_FLASH_STATUS_FULL          0x3ul

// CGradient Shape
#define EVE_CGRADIENT_CORNER_ZERO      0x0ul
#define EVE_CGRADIENT_EDGE_ZERO        0x1ul

// Touchmode settings
#define EVE_CTOUCHMODE_EXTENDED        0x0ul
#define EVE_CTOUCHMODE_COMPATIBILITY   0x1ul

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

