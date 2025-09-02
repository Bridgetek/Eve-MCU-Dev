/**
 @file EVE_config.h
 */
/*
 * ============================================================================
 * History
 * =======
 * Nov 2019        Initial beta for FT81x and FT80x
 * Mar 2020        Updated beta - added BT815/6 commands
 * Mar 2021        Beta with BT817/8 support added
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

#ifndef _EVE_CONFIG_H_
#define _EVE_CONFIG_H_

// Select the EVE controller type from the supported list in FT8xx.h.
// Note: In FT8xx.h this will lead to the selection of the EVE Programming
// support methods via macros "EVE_GEN" where the value depends on the level of
// the EVE device support. Alternatively directly set the EVE_GEN and EVE_SUB_GEN
// macro as required. This must be called prior to including FT8xx.h.
// "#define FT8XX_TYPE BT817" is equivalant to having "#define EVE_GEN 4".
// Note the use of EVEx_ENABLE is deprecated but the macro is still defined.
#ifndef FT8XX_TYPE
#define FT8XX_TYPE BT820
#endif

// Definitions used for target display resolution selection
#define WQVGA   480        // e.g. VM800B with 5 or 4.3 inch display
#define WVGA    800        // e.g. ME813A-WH50C or VM816
#define WSVGA   1024       // e.g. ME817EV with 7 inch display
#define WXGA    1280       // e.g. ME817EV with 10.1 inch display
#define HD      1920       // e.g. 15 inch high definition display
#define WUXGA   19201200   // e.g. 10 inch high definition display

// Select the resolution
#ifndef DISPLAY_RES
#define DISPLAY_RES WUXGA
#endif

// Definitions used for touch controllers
#define TOUCH_ADDR_FOCALTECH 0x38 // Focaltech FT5206
#define TOUCH_TYPE_FOCALTECH 1
#define TOUCH_ADDR_GOODIX 0x5d // Goodix GT911
#define TOUCH_TYPE_GOODIX 2

// Select the touchscreen
#ifndef EVE_TOUCH_ADDR
#define EVE_TOUCH_ADDR TOUCH_ADDR_GOODIX
#endif
#ifndef EVE_TOUCH_TYPE
#define EVE_TOUCH_TYPE TOUCH_TYPE_GOODIX
#endif

// Explicitly disable QuadSPI
#ifdef QUADSPI_ENABLE
#undef QUADSPI_ENABLE
#endif

// Setup RAM_G size for BT82X only
// Available options are in Gigabits: 0.5Gb, 1Gb, 2Gb, 4Gb or 8Gb
#define EVE_RAM_G_32_MBIT  0x100000UL
#define EVE_RAM_G_64_MBIT  0x200000UL
#define EVE_RAM_G_128_MBIT 0x400000UL
#define EVE_RAM_G_256_MBIT 0x800000UL
#define EVE_RAM_G_512_MBIT 0x4000000UL 
#define EVE_RAM_G_1_GBIT   0x8000000UL
#define EVE_RAM_G_2_GBIT   0x10000000UL
#define EVE_RAM_G_4_GBIT   0x20000000UL
#define EVE_RAM_G_8_GBIT   0x40000000UL

#ifndef EVE_RAM_G_CONFIG_SIZE
#define EVE_RAM_G_CONFIG_SIZE EVE_RAM_G_1_GBIT
#endif

// Setup default parameters for various displays.
// These can be overridden for different display modules.
#undef SET_PCLK_FREQ

#if DISPLAY_RES == WQVGA

#define EVE_DISP_WIDTH 480 // Active width of LCD display
#define EVE_DISP_HEIGHT 272 // Active height of LCD display
#define EVE_DISP_HCYCLE 548 // Total number of clocks per line
#define EVE_DISP_HOFFSET 43 // Start of active line
#define EVE_DISP_HSYNC0 0 // Start of horizontal sync pulse
#define EVE_DISP_HSYNC1 41 // End of horizontal sync pulse
#define EVE_DISP_VCYCLE 292 // Total number of lines per screen
#define EVE_DISP_VOFFSET 12 // Start of active screen
#define EVE_DISP_VSYNC0 0 // Start of vertical sync pulse
#define EVE_DISP_VSYNC1 10 // End of vertical sync pulse
#define EVE_DISP_PCLK 5 // Pixel Clock
#define EVE_DISP_SWIZZLE 0 // Define RGB output pins
#define EVE_DISP_PCLKPOL 1 // Define active edge of PCLK
#define EVE_DISP_CSPREAD 0
#define EVE_DISP_DITHER 1
// BT82x settings
#define EVE_DISP_LVDSTXCLKDIV 3
#define EVE_DISP_LVDSTXFORMAT EVE_FORMAT_RGB6

#elif DISPLAY_RES == WVGA

#define EVE_DISP_WIDTH 800 // Active width of LCD display
#define EVE_DISP_HEIGHT 480 // Active height of LCD display
#define EVE_DISP_HCYCLE 928 // Total number of clocks per line
#define EVE_DISP_HOFFSET 88 // Start of active line
#define EVE_DISP_HSYNC0 0 // Start of horizontal sync pulse
#define EVE_DISP_HSYNC1 48 // End of horizontal sync pulse
#define EVE_DISP_VCYCLE 525 // Total number of lines per screen
#define EVE_DISP_VOFFSET 32 // Start of active screen
#define EVE_DISP_VSYNC0 0 // Start of vertical sync pulse
#define EVE_DISP_VSYNC1 3 // End of vertical sync pulse
#define EVE_DISP_PCLK 2 // Pixel Clock
#define EVE_DISP_SWIZZLE 0 // Define RGB output pins
#define EVE_DISP_PCLKPOL 1 // Define active edge of PCLK
#define EVE_DISP_CSPREAD 0
#define EVE_DISP_DITHER 1
// BT82x settings
#define EVE_DISP_LVDSTXCLKDIV 3
#define EVE_DISP_LVDSTXFORMAT EVE_FORMAT_RGB6

#elif DISPLAY_RES == WSVGA

#define EVE_DISP_WIDTH 1024 // Active width of LCD display
#define EVE_DISP_HEIGHT 600 // Active height of LCD display
#define EVE_DISP_HCYCLE 1344 // Total number of clocks per line
#define EVE_DISP_HOFFSET 160 // Start of active line
#define EVE_DISP_HSYNC0 0 // Start of horizontal sync pulse
#define EVE_DISP_HSYNC1 100 // End of horizontal sync pulse
#define EVE_DISP_VCYCLE 635 // Total number of lines per screen
#define EVE_DISP_VOFFSET 23 // Start of active screen
#define EVE_DISP_VSYNC0 0 // Start of vertical sync pulse
#define EVE_DISP_VSYNC1 10 // End of vertical sync pulse
#define EVE_DISP_PCLK 1 // Pixel Clock
#define EVE_DISP_SWIZZLE 0 // Define RGB output pins
#define EVE_DISP_PCLKPOL 1 // Define active edge of PCLK
#define EVE_DISP_CSPREAD 0
#define EVE_DISP_DITHER 1
// Set the PCLK frequency to 51MHz (recommend to use the CMD_PCLKFREQ for easier calculation)
#define SET_PCLK_FREQ
#define EVE_DISP_PCLK_FREQ  0xD12    // set 51MHz (must also define SET_PCLK_FREQ in line above to use this)
// BT82x settings
#define EVE_DISP_LVDSTXCLKDIV 3
#define EVE_DISP_LVDSTXFORMAT EVE_FORMAT_RGB6

#elif DISPLAY_RES == WXGA

#define EVE_DISP_WIDTH 1280 // Active width of LCD display
#define EVE_DISP_HEIGHT 800 // Active height of LCD display
#define EVE_DISP_HCYCLE 1411 // Total number of clocks per line
#define EVE_DISP_HOFFSET 120 // Start of active line
#define EVE_DISP_HSYNC0 0 // Start of horizontal sync pulse
#define EVE_DISP_HSYNC1 100 // End of horizontal sync pulse
#define EVE_DISP_VCYCLE 815 // Total number of lines per screen
#define EVE_DISP_VOFFSET 14 // Start of active screen
#define EVE_DISP_VSYNC0 0 // Start of vertical sync pulse
#define EVE_DISP_VSYNC1 10 // End of vertical sync pulse
#define EVE_DISP_PCLK 1 // Pixel Clock
#define EVE_DISP_SWIZZLE 0 // Define RGB output pins
#define EVE_DISP_PCLKPOL 0 // Define active edge of PCLK
#define EVE_DISP_CSPREAD 0
#define EVE_DISP_DITHER 0
// Set the PCLK frequency to 51MHz (recommend to use the CMD_PCLKFREQ for easier calculation)
#define SET_PCLK_FREQ
#define EVE_DISP_PCLK_FREQ  0x8B1    // set 51MHz (must also define SET_PCLK_FREQ in line above to use this)
// BT82x settings
#define EVE_DISP_LVDSTXCLKDIV 3
#define EVE_DISP_LVDSTXFORMAT EVE_FORMAT_RGB6

#elif DISPLAY_RES == HD
 
#define EVE_DISP_WIDTH 1920 // Active width of LCD display
#define EVE_DISP_HEIGHT 1080 // Active height of LCD display
#define EVE_DISP_HCYCLE 2140 // Total number of clocks per line
#define EVE_DISP_HOFFSET 220 // Start of active line
#define EVE_DISP_HSYNC0 0 // Start of horizontal sync pulse
#define EVE_DISP_HSYNC1 20 // End of horizontal sync pulse
#define EVE_DISP_VCYCLE 1108 // Total number of lines per screen
#define EVE_DISP_VOFFSET 28 // Start of active screen
#define EVE_DISP_VSYNC0 0 // Start of vertical sync pulse
#define EVE_DISP_VSYNC1 4 // End of vertical sync pulse
#define EVE_DISP_PCLK 1 // Pixel Clock
#define EVE_DISP_SWIZZLE 0 // Define RGB output pins
#define EVE_DISP_PCLKPOL 0 // Define active edge of PCLK
#define EVE_DISP_CSPREAD 0
#define EVE_DISP_DITHER 1
// BT82x settings
#define EVE_DISP_LVDSTXCLKDIV 3
#define EVE_DISP_LVDSTXFORMAT EVE_FORMAT_RGB6

#elif DISPLAY_RES == WUXGA

#define EVE_DISP_WIDTH 1920 // Active width of LCD display
#define EVE_DISP_HEIGHT 1200 // Active height of LCD display
#define EVE_DISP_HCYCLE (1920 + 180) // Total number of clocks per line
#define EVE_DISP_HOFFSET 50 // Start of active line
#define EVE_DISP_HSYNC0 0 // Start of horizontal sync pulse
#define EVE_DISP_HSYNC1 30 // End of horizontal sync pulse
#define EVE_DISP_VCYCLE (1200 + 45) // Total number of lines per screen
#define EVE_DISP_VOFFSET 10 // Start of active screen
#define EVE_DISP_VSYNC0 0 // Start of vertical sync pulse
#define EVE_DISP_VSYNC1 3 // End of vertical sync pulse
#define EVE_DISP_PCLK 2 // Pixel Clock
#define EVE_DISP_SWIZZLE 0 // Define RGB output pins
#define EVE_DISP_PCLKPOL 0 // Define active edge of PCLK
#define EVE_DISP_CSPREAD 0
#define EVE_DISP_DITHER 1
// BT82x settings
#define EVE_DISP_LVDSTXCLKDIV 3
#define EVE_DISP_LVDSTXFORMAT EVE_FORMAT_RGB6

#else

#error EVE_DISP_* parameters must be configured.

#endif

#endif /* _EVE_CONFIG_H_ */
