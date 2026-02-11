/**
 @file EVE_config.h
 */
/*
 * ============================================================================
 * (C) Copyright,  Bridgetek Pte. Ltd.
 * ============================================================================
 *
 * This source code ("the Software") is provided by Bridgetek Pte Ltd
 * ("Bridgetek") subject to the licence terms set out
 * https://brtchip.com/wp-content/uploads/2021/11/BRT_Software_License_Agreement.pdf ("the Licence Terms").
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

/** @note IMPORTANT This header file must be included before FT8xx.h.
 * @details The macro FT9XX_TYPE and the panel display settings (EVE_DISP_*) must 
 * be configured in this file. For BT82x the EVE_RAM_G_CONFIG_SIZE macro must
 * also be configured.
 * Values from the macros defined in this file can be used in code based on this library. 
 * To make a custom configuration file, edit this file as required as long as the macros
 * listed above are correctly defined. */
 
/** @brief Select Bridgetek EVE Module Types.
 *  @details The following options are defined:
 *    VM800B     - VM800B35A-BK with 3.5 inch display (FT800 with DP-0351-11A)
 *    VM800C35A  - VM800C35A-D with 3.5 inch display (FT800 with DP-0351-11A)
 *    VM800C43A  - VM800C43A-D with 4.3 inch display (FT800 with DP-0431-11A)
 *    VM800C50A  - VM800C50A-D with 4.3 inch display (FT800 with DP-0502-11A)
 *    VM810C     - VM810C50A-D with 5 inch display (FT810 with DP-0501-11A)
 *    ME812A     - ME812A-WH50R, ME812AU-WH50R with 5 inch display (FT812)
 *    ME813A     - ME813A-WH50C with 5 inch display (FT813)
 *    VM816C     - VM816C50A-D, VM816CU50A-D with 5 inch display (BT816 with DP-0502-11A)
 *    IDM204021R - IDM2040-21R (FT800)
 *    IDM204043A - IDM2040-43A (BT883 with DP-0431-11A)
 *    IDM20407A  - IDM2040-7A (BT817 with DP-0701-01A)
 * Setting MODULE_TYPE will select the correct FT8XX_TYPE, PANEL_TYPE and DISPLAY_RES.
 */
//@{
#define VM800B     800     // VM800B35A-BK 
#define VM800C35A  835     // VM800C35A-D 
#define VM800C43A  843     // VM800C43A-D 
#define VM800C50A  850     // VM800C50A-D 
#define VM810C     810     // VM810C50A-D 
#define ME812A     812     // ME812A-WH50R, ME812AU-WH50R 
#define ME813A     813     // ME813A-WH50C 
#define VM816C     816     // VM816C50A-D, VM816CU50A-D 
#define IDM204021R 204021  // IDM2040-21R 
#define IDM204043A 204043  // IDM2040-43A 
#define IDM20407A  20407   // IDM2040-7A 
#define NO_MODULE  0       // Select PANEL_TYPE and FT8XX_TYPE manually
//@}

/** @brief Module selection from above list.
 * Undefine or set to "NO_MODULE" if not used.
 */
#ifndef MODULE_TYPE
#define MODULE_TYPE NO_MODULE
#endif

/** @brief Select Bridgetek Panel Types
 */
//@{
#define DP_0351_11A 0351   // DP-0351-11A QVGA (Resistive)
#define DP_0431_11A 0431   // DP-0431-11A WQVGA (Resistive)
#define DP_0501_01A 050101 // DP-0501-01A WVGA (Capacitive)
#define DP_0501_11A 050111 // DP-0501-11A WVGA (Resistive)
#define DP_0502_11A 0502   // DP-0502-11A WQVGA (Resistive)
#define DP_0701_11A 0701   // DP-0701-11A WVGA (Capacitive)
#define DP_1012_01A 1012   // DP-1012-01A WUXGA (Capacitive)
#define DP_1561_01A 1561   // DP-1561-01A FullHD (Capacitive)
#define DP_IDM43A   43     // IDM204043A (Capacitive)
#define DP_IDM21R   21     // IDM204021R (Capacitive)
#define NO_PANEL    0      // Not defined
//@}

/** @brief Select the EVE controller type and panel resolution.
 *  @details If a module is selected then the EVE controller type and panel
 * resolution are set correctly for the module.
 * In FT8xx.h the EVE controller type will lead to the selection of the EVE 
 * Programming support methods via macros "EVE_API" where the value depends on 
 * the level of the EVE device support. 
 * Alternatively, to override this directly set the EVE_API and EVE_SUB_API macro 
 * as required. 
 * The FT9XX_TYPE and the display settings must be configured before calling FT8xx.h.
 * "#define FT8XX_TYPE BT817" is equivalent to having "#define EVE_API 4".
 * Note the use of EVEx_ENABLE is deprecated but the macro is still defined.
 * The FT8XX_TYPE macro and PANEL_TYPE macro must not be expanded until their
 * allowable values are defined (FT8XX_TYPE in FT8xx.h).
 */
#if !defined(MODULE_TYPE) || MODULE_TYPE == NO_MODULE
/** @brief Manual selection of FT8XX_TYPE and PANEL_TYPE
 */
//@{
#ifndef FT8XX_TYPE
#define FT8XX_TYPE BT820
#endif
#ifndef PANEL_TYPE
#define PANEL_TYPE NO_PANEL
#endif
//@}

/** @brief Predefined Bridgetek module displays
 */
#elif MODULE_TYPE == VM800B
// VM800B35A-BK with 3.5 inch display
#define FT8XX_TYPE FT800
#define PANEL_TYPE DP_0351_11A

#elif MODULE_TYPE == VM800C35A
// VM800C35A-D with 3.5 inch display
#define FT8XX_TYPE FT800
#define PANEL_TYPE DP_0351_11A

#elif MODULE_TYPE == VM800C43A  
// VM800C43A-D with 4.3 inch display
#define FT8XX_TYPE FT800
#define PANEL_TYPE DP_0431_11A

#elif MODULE_TYPE == VM800C50A  
// VM800C50A-D with 4.3 inch display
#define FT8XX_TYPE FT800
#define PANEL_TYPE DP_0502_11A

#elif MODULE_TYPE == VM810C     
// VM810C50A-D with 5 inch display
#define FT8XX_TYPE FT810
#define PANEL_TYPE DP_0501_11A

#elif MODULE_TYPE == ME812A     
// ME812A-WH50R, ME812AU-WH50R with 5 inch display
#define FT8XX_TYPE FT812
#define PANEL_TYPE DP_0501_11A

#elif MODULE_TYPE == ME813A     
// ME813A-WH50C with 5 inch display
#define FT8XX_TYPE FT813
#define PANEL_TYPE DP_0501_01A

#elif MODULE_TYPE == VM816C     
// VM816C50A-D, VM816CU50A-D with 5 inch display
#define FT8XX_TYPE BT816
#define PANEL_TYPE DP_0501_11A

#elif MODULE_TYPE == IDM204021R 
// IDM2040-21R
#define FT8XX_TYPE FT800
#define PANEL_TYPE DP_IDM21R

#elif MODULE_TYPE == IDM204043A 
// IDM2040-43A
#define FT8XX_TYPE BT883
#define PANEL_TYPE DP_IDM43A

#elif MODULE_TYPE == IDM20407A  
// IDM2040-7A
#define FT8XX_TYPE BT817
#define PANEL_TYPE DP_0701_11A
#else

#error MODULE_TYPE must be configured.

#endif

/** @brief Select Panel Resolution for Bridgetek Panels
 *  @details The following options are defined:
 *     QVGA      - 320x240   e.g. VM800B with 3.5 inch display
 *     WQVGA     - 480x272   e.g. IDM2040-43A with 4.3 inch display
 *     WQVGAR    - 480x480   e.g. IDM2040-21R with 2.1 inch round display
 *     WVGA      - 800x480   e.g. ME813A-WH50C with 5 inch display
 *     WSVGA     - 1024x600  e.g. ME817EV with 7 inch display
 *     WXGA      - 1280x800  e.g. ME817EV with 10.1 inch display
 *     FULLHD    - 1920x1080 e.g. BT820 with 15 inch high definition display
 *     WUXGA     - 1920x1200 e.g. BT820 with 10 inch high definition display
 *     DP101201A - same as WUXGA  e.g. DP-1012-01A
 *     DP156101A - same as FULLHD e.g. DP-1561-01A
 */
//@{
#define QVGA    320        // 320x240   e.g. VM800B with 3.5 inch display
#define WQVGA   480        // 480x272   e.g. IDM2040-43A with 4.3 inch display
#define WQVGAR  480480     // 480x480   e.g. IDM2040-21R with 2.1 inch round display
#define WVGA    800        // 800x480   e.g. ME813A-WH50C with 5 inch display
#define WSVGA   1024       // 1024x600  e.g. ME817EV with 7 inch display
#define WXGA    1280       // 1280x800  e.g. ME817EV with 10.1 inch display
#define FULLHD  1920       // 1920x1080 e.g. BT820 with 15 inch high definition display
#define WUXGA   19201200   // 1920x1200 e.g. BT820 with 10 inch high definition display
#define DP101201A  WUXGA   // DP-1012-01A
#define DP156101A  FULLHD  // DP-1561-01A
//@}

/** @brief Match display resolution to panel type
 */
//@{
#if !defined(PANEL_TYPE) || PANEL_TYPE == NO_PANEL
/** @brief Manual selection of DISPLAY_RES
 */
//@{
#ifndef DISPLAY_RES
#define DISPLAY_RES WUXGA
#endif
//@}

#elif PANEL_TYPE == DP_0351_11A
// DP-0351-11A QVGA (Resistive)
#define DISPLAY_RES QVGA

#elif PANEL_TYPE == DP_0431_11A
// DP-0431-11A WQVGA (Resistive)
#define DISPLAY_RES WQVGA   

#elif PANEL_TYPE == DP_0501_01A
// DP-0501-01A WVGA (Capacitive)
#define DISPLAY_RES WVGA    

#elif PANEL_TYPE == DP_0501_11A
// DP-0351-11A WVGA (Resistive)
#define DISPLAY_RES WVGA    

#elif PANEL_TYPE == DP_0502_11A
// DP-0502-11A WQVGA (Resistive)
#define DISPLAY_RES WQVGA   

#elif PANEL_TYPE == DP_0701_11A
// DP-0701-11A WVGA (Capacitive)
#define DISPLAY_RES WVGA

#elif PANEL_TYPE == DP_1012_01A
// DP-1012-01A WUXGA (Capacitive)
#define DISPLAY_RES WUXGA

#elif PANEL_TYPE == DP_1561_01A
// DP-1561-01A FullHD (Capacitive)
#define DISPLAY_RES FULLHD

#elif PANEL_TYPE == DP_IDM43A
// IDM204043A (Capacitive)
#define DISPLAY_RES WQVGA

#elif PANEL_TYPE == DP_IDM21R
// IDM204021R (Capacitive)
#define DISPLAY_RES WQVGAR  
#endif


/** @brief Definitions used for touch controllers
 */
//@{
#define TOUCH_ADDR_FOCALTECH 0x38 // Focaltech FT5206
#define TOUCH_TYPE_FOCALTECH 1
#define TOUCH_ADDR_GOODIX 0x5d // Goodix GT911
#define TOUCH_TYPE_GOODIX 2
//@}

/** @brief Select the touchscreen automatically
 */
//@{
#undef EVE_TOUCH_ADDR
#undef EVE_TOUCH_TYPE
//@}

/** @brief Explicitly disable QuadSPI
 */
//@{
#ifdef QUADSPI_ENABLE
#undef QUADSPI_ENABLE
#endif
//@}

/** @brief RAM_G size options for BT82X only
 * @details Available options are in Gigabits: 0.5Gb, 1Gb, 2Gb, 4Gb or 8Gb
 */
//@{
#define EVE_RAM_G_32_MBIT  0x100000UL
#define EVE_RAM_G_64_MBIT  0x200000UL
#define EVE_RAM_G_128_MBIT 0x400000UL
#define EVE_RAM_G_256_MBIT 0x800000UL
#define EVE_RAM_G_512_MBIT 0x4000000UL 
#define EVE_RAM_G_1_GBIT   0x8000000UL
#define EVE_RAM_G_2_GBIT   0x10000000UL
#define EVE_RAM_G_4_GBIT   0x20000000UL
#define EVE_RAM_G_8_GBIT   0x40000000UL
//@}

/** @brief Setup RAM_G size for BT82X only
 */
#ifndef EVE_RAM_G_CONFIG_SIZE
#define EVE_RAM_G_CONFIG_SIZE EVE_RAM_G_1_GBIT
#endif

/** @brief Setup default parameters for various displays.
 * These can be overridden for different display modules.
 */
//@{
#undef SET_PCLK_FREQ

#if DISPLAY_RES == QVGA

#define EVE_DISP_WIDTH 320 // Active width of LCD display
#define EVE_DISP_HEIGHT 240 // Active height of LCD display
#define EVE_DISP_HCYCLE 408 // Total number of clocks per line
#define EVE_DISP_HOFFSET 70 // Start of active line
#define EVE_DISP_HSYNC0 0 // Start of horizontal sync pulse
#define EVE_DISP_HSYNC1 10 // End of horizontal sync pulse
#define EVE_DISP_VCYCLE 263 // Total number of lines per screen
#define EVE_DISP_VOFFSET 13 // Start of active screen
#define EVE_DISP_VSYNC0 0 // Start of vertical sync pulse
#define EVE_DISP_VSYNC1 2 // End of vertical sync pulse
#define EVE_DISP_PCLK 8 // Pixel Clock
#define EVE_DISP_SWIZZLE 2 // Define RGB output pins
#define EVE_DISP_PCLKPOL 0 // Define active edge of PCLK
#define EVE_DISP_CSPREAD 0
#define EVE_DISP_DITHER 1
// BT82x settings
#define EVE_DISP_LVDSTXCLKDIV 3
#define EVE_DISP_LVDSTXFORMAT EVE_FORMAT_RGB6

#elif DISPLAY_RES == WQVGA

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

#elif DISPLAY_RES == FULLHD
 
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

#elif DISPLAY_RES == WQVGAR

#define EVE_DISP_WIDTH 480 // Active width of LCD display
#define EVE_DISP_HEIGHT 480 // Active height of LCD display
#define EVE_DISP_HCYCLE 578 // Total number of clocks per line
#define EVE_DISP_HOFFSET 40 // Start of active line
#define EVE_DISP_HSYNC0 0 // Start of horizontal sync pulse
#define EVE_DISP_HSYNC1 40 // End of horizontal sync pulse
#define EVE_DISP_VCYCLE 576 // Total number of lines per screen
#define EVE_DISP_VOFFSET 12 // Start of active screen
#define EVE_DISP_VSYNC0 0 // Start of vertical sync pulse
#define EVE_DISP_VSYNC1 10 // End of vertical sync pulse
#define EVE_DISP_PCLK 3 // Pixel Clock
#define EVE_DISP_SWIZZLE 0 // Define RGB output pins
#define EVE_DISP_PCLKPOL 0 // Define active edge of PCLK
#define EVE_DISP_CSPREAD 0
#define EVE_DISP_DITHER 1

#else

#error EVE_DISP_* parameters must be configured.

#endif
//@}

#endif /* _EVE_CONFIG_H_ */
