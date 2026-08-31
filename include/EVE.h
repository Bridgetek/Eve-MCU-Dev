/**
 * @file EVE.h
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

#ifndef EVE_HEADER_H
#define EVE_HEADER_H

#include <stdint.h>

/*
 * Include the EVE configuration defintions.
 */
#include <EVE_defs.h>

/*
 * Include the EVE configuration to select the EVE API.
 */
#include <EVE_config.h>

/*
 * Include the EVE debug-output macros.
 */
#include <EVE_debug.h>

/** Macros to allow us to select which API a command applies to.
 * For APIs supported use the following:
 *
 * #if IS_EVE_API(api1, api2, ...)
 *
 * Where the APIs supported are in the macro parenthesis. The EVE
 * API is set in the EVE_API macro.
 * So, to support BT815, BT817, BT820 this will need EVE APIs 3, 4, and 5.
 *
 * #if IS_EVE_API(3,4,5)
 *
 * On Visual Studio it is necessary to enable the "/Zc:preprocessor" option
 * to enable preprocessor conformance mode.
 * This works by counting the number of parameters then calling a chain of
 * macros to make a chain of conditions for each parameters in the macros
 * IS_EVE_API_1/2/3/4/5. (This can be expanded to more than 5 in future)
 * (x == n)||(y == n)||...
 * The parameters are counted by pasting the passed parameters into the
 * parameters passed from NUM_ARGS to _NUM_ARGS and selecting the Nth one,
 * which then becomes one of the numbers after __VA_ARGS__ in NUM_ARGS.
 * The first arguments in _NUM_ARGS are dummies.
 */
#ifndef IS_EVE_API
#define _NUM_ARGS(X,X5,X4,X3,X2,X1,N,...) N
#define NUM_ARGS(...) _NUM_ARGS(0, ## __VA_ARGS__ ,5,4,3,2,1,0)

#define IS_EVE_API_1(a)     (a == EVE_API)
#define IS_EVE_API_2(a,b)   (IS_EVE_API_1(a) || IS_EVE_API_1(b) )
#define IS_EVE_API_3(a,...) (IS_EVE_API_1(a) || IS_EVE_API_2( __VA_ARGS__) )
#define IS_EVE_API_4(a,...) (IS_EVE_API_1(a) || IS_EVE_API_3( __VA_ARGS__) )
#define IS_EVE_API_5(a,...) (IS_EVE_API_1(a) || IS_EVE_API_4( __VA_ARGS__) )

#define _IS_EVE_API_N(N, ...) IS_EVE_API_ ## N(__VA_ARGS__)
#define _IS_EVE_API(N, ...)  _IS_EVE_API_N(N, __VA_ARGS__)
#define IS_EVE_API(...)      _IS_EVE_API(NUM_ARGS(__VA_ARGS__), ## __VA_ARGS__)

#define IS_EVE_SUB_API(a)      (a == EVE_SUB_API)
#endif

/** Macros to select a value from a set depending on the EVE_API.
 * The values must be constants and cannot be further preprocessor directives.
 */
#ifndef EVE_API_SELECT
#define EVE_API_SELECT(a1, a2, a3, a4, a5)             \
    ((EVE_API == 1) ? (a1) : (EVE_API == 2) ? (a2)     \
                           : (EVE_API == 3) ? (a3)     \
                           : (EVE_API == 4) ? (a4)     \
                                            : (a5))
#endif

#if !defined(IS_ARDUINO_LIB) /* This block is not used in Arduino libraries */

/**
 * @brief Select the EVE controller type and panel resolution.
 * @details If a module is selected then the EVE controller type and panel
 *      resolution are set correctly for the module.
 *      In EVE.h the EVE controller type will lead to the selection of the EVE 
 *      Programming support methods via macros "EVE_API" where the value depends on 
 *      the level of the EVE device support. 
 *      Alternatively, to override this directly set the EVE_API and EVE_SUB_API macro 
 *      as required. 
 *      The FT8XX_TYPE and the display settings must be configured before calling EVE.h.
 *      "#define FT8XX_TYPE BT817" is equivalent to having "#define EVE_API 4".
 *      Note the use of EVEx_ENABLE is deprecated but the macro is still defined.
 *      The FT8XX_TYPE macro and PANEL_TYPE macro must not be expanded until their
 *      allowable values are defined (FT8XX_TYPE in EVE.h).
 */
//@{
#if defined(MODULE_TYPE) && (MODULE_TYPE != MODULE_TYPE_NONE)
#undef FT8XX_TYPE
#undef PANEL_TYPE

/** 
 * @brief Predefined Bridgetek module displays
 */
#if MODULE_TYPE == VM800B
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
// VM800C50A-D with 5 inch display
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

#elif MODULE_TYPE == VM880C
// VM880C with 4.3 inch display
#define FT8XX_TYPE BT880
#define PANEL_TYPE DP_0431_11A

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
#define PANEL_TYPE DP_0701_01A

#elif MODULE_TYPE == VM820B10A
// VM820B10A with 10.1 inch display (BT820 with DP-1011-02A)
#define FT8XX_TYPE BT820
#define PANEL_TYPE DP_1011_02A

#elif MODULE_TYPE == VM820B15A
// VM820B15A with 15.6 inch display (BT820 with DP-1561-02A)
#define FT8XX_TYPE BT820
#define PANEL_TYPE DP_1561_02A

#elif MODULE_TYPE == IDK_FT810_43A
// FT810 IC Development Kit with 4.3 - inch display (FT810 with DP-0431-11A)
#define FT8XX_TYPE FT810
#define PANEL_TYPE DP_0431_11A

#elif MODULE_TYPE == IDK_BT816_50A
// BT816 IC Development Kit with 5.0-inch display (BT816 with DP-0501-11A)
#define FT8XX_TYPE BT816
#define PANEL_TYPE DP_0501_11A

#elif MODULE_TYPE == IDK_BT817_70A
// BT817 IC Development Kit with 7.0-inch display (BT817 with DP-0701-01A)
#define FT8XX_TYPE BT817
#define PANEL_TYPE DP_0701_01A

#elif MODULE_TYPE == IDK_BT817_101A
// BT817 IC Development Kit with 10.1-inch display (BT817 with DP-1011-02A)
#define FT8XX_TYPE BT817
#define PANEL_TYPE DP_1011_02A

#elif MODULE_TYPE == IDK_BT820_101A
// BT820B IC Development Kit with 10.1-inch display (BT820 with DP-1012-01A)
#define FT8XX_TYPE BT820
#define PANEL_TYPE DP_1012_01A

#else

#error MODULE_TYPE must be configured.

#endif // MODULE_TYPE
#endif // defined(MODULE_TYPE)
//@}

#endif // !defined(IS_ARDUINO_LIB) 

#ifndef EVE_API
#undef EVE_SUB_API

#if (FT8XX_TYPE == FT800)
#define EVE_API 1

#elif (FT8XX_TYPE == FT801)
#define EVE_API 1

#elif (FT8XX_TYPE == FT810)
#define EVE_API 2
#define EVE_SUB_API 1

#elif (FT8XX_TYPE == FT811)
#define EVE_API 2
#define EVE_SUB_API 1

#elif (FT8XX_TYPE == FT812)
#define EVE_API 2
#define EVE_SUB_API 1

#elif (FT8XX_TYPE == FT813)
#define EVE_API 2
#define EVE_SUB_API 1

#elif (FT8XX_TYPE == BT880)
#define EVE_API 2
#define EVE_SUB_API 2

#elif (FT8XX_TYPE == BT881)
#define EVE_API 2
#define EVE_SUB_API 2

#elif (FT8XX_TYPE == BT882)
#define EVE_API 2
#define EVE_SUB_API 2

#elif (FT8XX_TYPE == BT883)
#define EVE_API 2
#define EVE_SUB_API 2

#elif (FT8XX_TYPE == BT815)
#define EVE_API 3

#elif (FT8XX_TYPE == BT816)
#define EVE_API 3

#elif (FT8XX_TYPE == BT817)
#define EVE_API 4

#elif (FT8XX_TYPE == BT818)
#define EVE_API 4

#elif (FT8XX_TYPE == BT820)
#define EVE_API 5

#else
#ifndef EVE_API
#error FT8XX_TYPE definition not recognised.
#endif
#endif

#else // EVE_API

#if (EVE_API == 2)
#ifndef EVE_SUB_API
#error EVE_SUB_API definition required for EVE API 2.
#endif // EVE_SUB_API
#endif // (EVE_API == 2)

#endif // EVE_API

#undef EVE1_ENABLE // deprecated
#undef EVE2_ENABLE // deprecated
#undef EVE3_ENABLE // deprecated
#undef EVE4_ENABLE // deprecated
#undef EVE5_ENABLE // deprecated
#if EVE_API == 1
#define EVE1_ENABLE // deprecated
#elif EVE_API == 2
#define EVE2_ENABLE // deprecated
#elif EVE_API == 3
#define EVE3_ENABLE // deprecated
#elif EVE_API == 4
#define EVE4_ENABLE // deprecated
#elif EVE_API == 5
#define EVE5_ENABLE // deprecated
#endif

/** EVE API definitions. */

#if IS_EVE_API(1, 2, 3, 4, 5)
    #include "EVE_commands.h"
    #include "EVE_registers.h"
#else
    #error No EVE API selected.
#endif

/** Co-processor transfer method.
 *
 *   EVE1 always writes directly to the co-processor ring buffer then
 *   REG_CMD_WRITE is updated.
 *   EVE2+ can write directly to the co-processor ring buffer via the
 *   REG_CMDB_WRITE register.
 * 
 *   If COPROCESSOR_TRANSFER is undefined then EVE_TRANSFER_CMD_WRITE is 
 *   used for EVE1 and EVE_TRANSFER_CMDB_WRITE is used for EVE2 onwards.
 *   If COPROCESSOR_TRANSFER is set for EVE_TRANSFER_CMDB_WRITE on EVE1 
 *   then the setting will be modified to EVE_TRANSFER_CMD_WRITE.
 *   The EVE_TRANSFER_INT flag can be used to modify the 
 *   EVE_TRANSFER_CMD_WRITE method to use the hardware INT# line as well.
 *   This requires support from the port in the MCU layer. It is not 
 *   compatible with QuadSPI as the INT# line is used as a data line.
 */
#if IS_EVE_API(1)
    // Always undefined for EVE1
    #undef EVE_USE_CMDB_METHOD
#else // IS_EVE_API(2,3,4,5)
    // Default setting for EVE2 onwards
    #define EVE_USE_CMDB_METHOD
    #if defined(COPROCESSOR_TRANSFER)
        #if (COPROCESSOR_TRANSFER & EVE_TRANSFER_CMD_WRITE)
            #undef EVE_USE_CMDB_METHOD
        #endif // EVE_TRANSFER_CMD_WRITE
    #endif // defined(COPROCESSOR_TRANSFER)
#endif

#if defined(EVE_USE_CMD_METHOD) && !defined(QUADSPI_ENABLE)
    #if defined(COPROCESSOR_TRANSFER)
        #if (COPROCESSOR_TRANSFER & EVE_TRANSFER_INT)
            #undef EVE_USE_CMDB_METHOD
            #define EVE_USE_INTERRUPT_METHOD
        #endif // EVE_TRANSFER_INT
    #endif // defined(COPROCESSOR_TRANSFER)
#endif // defined(EVE_USE_CMDB_METHOD)

/** Interrupt management.
 *
 *   Define the macro to enable interrupt management of the REG_INT_FLAGS 
 *   register. If this is enabled then all testing of the REG_INT_FLAGS 
 *   register must be made through the EVE_LIB_GetInterrupt function.
 *   If EVE_USE_INTERRUPT_METHOD is defined then the feature is enabled.
 *   The function does not manage the INT# line (see EVE_LIB_Int function).
 */
#if defined(EVE_USE_INTERRUPT_METHOD) && !defined(EVE_MANANGE_INTERRUPTS)
#define EVE_MANANGE_INTERRUPTS
#endif

#if !defined(IS_ARDUINO_LIB) /* This block is not used in Arduino libraries */

/**
 * @brief Match display resolution to panel type
 */
//@{
#if defined(PANEL_TYPE) && (PANEL_TYPE != PANEL_TYPE_NONE)
#undef DISPLAY_RES

/** 
 * @brief Predefined selection of DISPLAY_RES by panel type.
 */
#if PANEL_TYPE == DP_0351_11A
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

#elif PANEL_TYPE == DP_0701_01A
// DP-0701-11A WVGA (Capacitive)
#define DISPLAY_RES WVGA

#elif PANEL_TYPE == DP_1011_01A
// DP-1011-01A WXGA (Capacitive)
#define DISPLAY_RES WXGA

#elif PANEL_TYPE == DP_1011_02A
// DP-1011-02A WXGA_NG (Capacitive)
#define DISPLAY_RES WXGA_NG

// Enable CUSTOM_TOUCH by default for this panel
#ifndef CUSTOM_TOUCH
#define CUSTOM_TOUCH
#endif

#elif PANEL_TYPE == DP_1012_01A
// DP-1012-01A WUXGA (Capacitive)
#define DISPLAY_RES WUXGA

#elif PANEL_TYPE == DP_1561_01A
// DP-1561-01A FullHD (Capacitive)
#define DISPLAY_RES FULLHD

#elif PANEL_TYPE == DP_1561_02A
// DP-1561-02A FullHD (Capacitive)
#define DISPLAY_RES FULLHD

#elif PANEL_TYPE == DP_IDM43A
// IDM204043A (Capacitive)
#define DISPLAY_RES WQVGA

#elif PANEL_TYPE == DP_IDM21R
// IDM204021R (Capacitive)
#define DISPLAY_RES WQVGAR  

#else

#error PANEL_TYPE must be configured.

#endif // PANEL_TYPE
#else
#undef PANEL_TYPE
#define PANEL_TYPE PANEL_TYPE_NONE
#endif // defined(PANEL_TYPE)

/**
 * @brief Setup default parameters for various displays.
 * @details These can be overridden for different display modules.
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

#elif DISPLAY_RES == WXGA_NG

#define EVE_DISP_WIDTH 1280 // Active width of LCD display
#define EVE_DISP_HEIGHT 800 // Active height of LCD display
#define EVE_DISP_HCYCLE 1440 // Total number of clocks per line
#define EVE_DISP_HOFFSET 158 // Start of active line
#define EVE_DISP_HSYNC0 72 // Start of horizontal sync pulse
#define EVE_DISP_HSYNC1 70 // End of horizontal sync pulse
#define EVE_DISP_VCYCLE 838 // Total number of lines per screen
#define EVE_DISP_VOFFSET 36 // Start of active screen
#define EVE_DISP_VSYNC0 12 // Start of vertical sync pulse
#define EVE_DISP_VSYNC1 14 // End of vertical sync pulse
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

#error DISPLAY_RES must be configured.

#endif // DISPLAY_RES
//@}

#endif // !defined(IS_ARDUINO_LIB)

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

#endif

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

#endif

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
 * @details Starts a co-processor list. Waits for the co-processor to be idle
 *      before asserting chip select.
 */
void EVE_LIB_BeginCoProList(void);

/**
 * @brief EVE API: End co-processor list
 * @details Ends a co-processor list. Deasserts chip select.
 */
void EVE_LIB_EndCoProList(void);

/**
 * @brief EVE API: Waits for co-processor list to end
 * @details Will poll the co-processor command list until it has been completed.
 * @returns 0 for successful completion, 0xff for co-processor exception.
 */
int EVE_LIB_AwaitCoProEmpty(void);

/**
 * @brief EVE API: Waits for co-processor list to end with a timeout value
 * @details Will poll the co-processor command list until it has been completed.
 * @param timeout - timeout in milliseconds (zero for NO timeout)
 * @returns 0 for successful completion, 0xff for co-processor exception, 0xfe for a timeout.
 */
int EVE_LIB_AwaitCoProEmptyTimeout(uint32_t timeout);

/**
 * @brief EVE API: Recovers the co-processor in the event of an exception
 * @details Will reset the co-processor after an exception is reported by EVE_LIB_AwaitCoProEmpty.
 */
void EVE_LIB_RecoverCoPro(void);

/**
 * @brief EVE API: Free space in of co-processor list 
 * @details Obtains the free space in the co-processor circular buffer. 
 *      This operation may have an effect on the performance of the device.
 * @returns The number of free instructions in the co-processor circular buffer.
 */
uint16_t EVE_LIB_GetCoProSpace(void);

#if defined(EVE_COPROC_PROFILE)
/**
 * @brief EVE API: Resets the co-processor list profiling length
 * @details Sets the profiling pointer to zero to restart profiling.
 */
void EVE_LIB_BeginCoProProfile(void);
#endif

#if defined(EVE_COPROC_PROFILE)
/**
 * @brief EVE API: Size of co-processor list since last reset
 * @details Obtains the current profiling pointer for the co-processor list.
 * @returns The number of instructions added to the co-processor list since 
 *      the last reset of the profiling pointer.
 */
uint16_t EVE_LIB_GetCoProProfile(void);
#endif

#if defined(EVE_COPROC_PROFILE)
/**
 * @brief EVE API: Size of display list
 * @details Obtains the current size of the display list.
 * @returns The number of instructions instructions currently in the display list.
 */
uint16_t EVE_LIB_GetDlProfile(void);
#endif

#if !defined (QUADSPI_ENABLE)
/**
 * @brief Test interrupt input line
 * @details This function will check the interrupt input INT# from
 *      the EVE device. If Quad SPI is enabled then the interrupt line
 *      is used as a data line for SPI and therefore cannot be used for
 *      an interrupt input.
 *      To prevent any unintended reads of this when Quad SPI is enabled
 *      the function is not compiled when Quad SPI is configured.
 * @returns zero if there is no interrupt, non-zero if the EVE device is
 *      asserting an interrupt.
 */
int EVE_LIB_Int(void);
#endif // defined (QUADSPI_ENABLE)

#if defined (EVE_MANANGE_INTERRUPTS)
/**
 * @brief EVE API: Test if an interrupt flag is set
 * @details Will read the interrupt flag register and add any newly pending to
 *      a status value. The flag register will clear any pending interrupt
 *      when read so the cumulative flagged bits are kept until they are
 *      cleared by the mask in this function.
 * @param mask - Bit mask of interrupts to query (and clear).
 * @returns 0 for no interrupts in the mask being set, if any interrupts are
 *      set then the return value will contain bits set from the mask parameter.
 */
uint8_t EVE_LIB_GetInterrupt(uint8_t mask);
#endif // defined (EVE_MANANGE_INTERRUPTS)

/**
 * @brief EVE API: Returns a result from the co-processor command buffer
 * @details Will return a result value from "offset" words back in the command buffer.
 *      If the value of offset is 1 then the previous value from the co-processor
 *      command buffer is returned.
 * @param offset - Number of 32-bit words to go back in the command buffer for
 *      the result.
 * @returns result of a previous co-processor command.
 */
uint32_t EVE_LIB_GetResult(int offset);

#if IS_EVE_API(3,4,5)
/**
 * @brief EVE API: Get co-processor exception description
 * @details Will query the co-processor exception description to a string.
 * @param desc - Buffer to receive the text of the exception description.
 * @returns Co-processor exception description. This is a pointer to a string
 *      and must be sufficient to hold 128 characters.
 */
void EVE_LIB_GetCoProException(char *desc);
#endif

/**
 * @brief EVE API: Write a buffer to memory mapped RAM
 * @details Writes a block of data via SPI to the EVE.
 * @param ImgData - Pointer to start of data buffer.
 * @param DataSize - Number of bytes in buffer.
 * @param DestAddress - 24-bit/32-bit memory mapped address on EVE.
 */
void EVE_LIB_WriteDataToRAMG(const uint8_t *ImgData, uint32_t DataSize, uint32_t DestAddress);

/**
 * @brief EVE API: Read a buffer from memory mapped RAM
 * @details Reads a block of data via SPI from the EVE.
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
 * @brief EVE API: Write a string the co-processor command memory
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
 * @brief EVE API: Get properties of an CMD_LOADIMAGE operation
 * @details Obtains the details of an image decoded by the CMD_LOADIMAGE
 *      co-processor command. The properties of the image are taken from
 *      the co-processor command list.
 * @param addr - Pointer to variable to receive the image start address.
 * @param width - Pointer to variable to receive the image width.
 * @param height - Pointer to variable to receive the image height.
 */
void EVE_LIB_GetProps(uint32_t *addr, uint32_t *width, uint32_t *height);

/**
 * @brief EVE API: Get current allocation pointer
 * @details Obtains the automatic allocation pointer of the last address
 *      used for certain co-processor operations.
 * @returns addr - Last allocation address rounded up to the next 32-bit 
 *      boundary.
 */
void EVE_LIB_GetPtr(uint32_t *addr);

/**
 * @brief EVE API: Get the touchscreen transformation matrix.
 * @details Obtains the transformation matrix from a CMD_CALIBRATE operation.
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
 * @param ptr - Start of memory area.
 * @param num - Number of bytes to CRC.
 * @param result - pointer to receive the CRC.
 */
void EVE_LIB_MemCrc(uint32_t ptr, uint32_t num, uint32_t *result);

#if IS_EVE_API(2, 3, 4, 5)
void EVE_LIB_BitmapTransform( int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, 
                              int32_t tx0, int32_t ty0, int32_t tx1, int32_t ty1, int32_t tx2, int32_t ty2,
                              uint32_t *result );
#endif
#if IS_EVE_API(4, 5)
/**
 * @brief EVE API: Get image properties.
 * @details From the last CMD_LOADIMAGE get the address, size, format and palette of the loaded image.
 * @param *addr - pointer to variable to receive the address the image was loaded to.
 * @param *fmt - pointer to variable to receive the format of the loaded image.
 * @param *width - pointer to variable to receive the width of the loaded image.
 * @param *height - pointer to variable to receive the height of the loaded image.
 * @param *palette - pointer to variable to receive the palette of the loaded image.
 */
void EVE_LIB_GetImage(uint32_t *addr, uint32_t *fmt, uint32_t *width, uint32_t *height, uint32_t *palette);
#endif
#if IS_EVE_API(5)
/**
 * @brief EVE API: Read a register.
 * @details Reads a register value.
 * @param addr - Address of register to read.
 * @param value - pointer to receive the contents of the register.
 */
void EVE_LIB_RegRead(uint32_t addr, uint32_t *value);
#endif

/**
 * @brief EVE API: Write a memory location.
 * @details Writes a memory location value.
 * @param addr - Address of register to read.
 * @param value - Value to write to memory.
 */
//@{
void EVE_LIB_MemWrite32(uint32_t addr, uint32_t value);
#if IS_EVE_API(1, 2, 3, 4) // Not supported on BT82x
void EVE_LIB_MemWrite16(uint32_t addr, uint16_t value);
void EVE_LIB_MemWrite8(uint32_t addr, uint8_t value);
#endif  // IS_EVE_API(1, 2, 3, 4)
//@}

/**
 * @brief EVE API: Read a memory location.
 * @details Reads a memory location value.
 * @param addr - Address of register to read.
 * @return - Value read from memory.
 */
//@{
uint32_t EVE_LIB_MemRead32(uint32_t address);
#if IS_EVE_API(1, 2, 3, 4) // Not supported on BT82x
uint16_t EVE_LIB_MemRead16(uint32_t address);
uint8_t EVE_LIB_MemRead8(uint32_t address);
#endif  // IS_EVE_API(1, 2, 3, 4)
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
#endif
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
#endif

#if IS_EVE_API(3, 4, 5)
void EVE_BITMAP_EXT_FORMAT(uint16_t fmt);
void EVE_BITMAP_SWIZZLE(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
#endif

#if IS_EVE_API(5) // BT82x extensions
void EVE_BITMAP_SOURCE_H(uint8_t addr);
void EVE_BITMAP_ZORDER(uint8_t o);
void EVE_PALLETE_SOURCE_H(uint8_t addr);
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
#else
void EVE_CMD_INFLATE(uint32_t ptr, uint32_t options);
void EVE_CMD_SETFONT(uint32_t font, uint32_t ptr, uint32_t firstchar);
#endif
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
#endif
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
#endif

#if IS_EVE_API(2, 3, 4) // FT80x, BT82x API change
void EVE_CMD_SETFONT2(uint32_t font, uint32_t ptr, uint32_t firstchar);
void EVE_CMD_SNAPSHOT2(uint32_t fmt, uint32_t ptr, int16_t x, int16_t y, int16_t w, int16_t h);
#endif

#if IS_EVE_API(3, 4)
void EVE_CMD_INFLATE2(uint32_t ptr, uint32_t options);
void EVE_CMD_CLEARCACHE(void);
void EVE_CMD_INTRAMSHARED(uint32_t ptr);
void EVE_CMD_VIDEOSTARTF(void);
#endif

#if IS_EVE_API(3, 4, 5)
void EVE_CMD_ANIMSTART(int32_t ch, uint32_t aoptr, uint32_t loop);
void EVE_CMD_ANIMSTOP(int32_t ch);
void EVE_CMD_ANIMXY(int32_t ch, int16_t x, int16_t y);
void EVE_CMD_ANIMDRAW(int32_t ch);
void EVE_CMD_ANIMFRAME(int16_t x, int16_t y, uint32_t aoptr, uint32_t frame);
void EVE_CMD_APPENDF(uint32_t ptr, uint32_t num);
#endif

#if IS_EVE_API(4)
void EVE_CMD_ANIMFRAMERAM(int16_t x, int16_t y, uint32_t aoptr, uint32_t frame );
void EVE_CMD_ANIMSTARTRAM(int32_t ch, uint32_t aoptr, uint32_t loop);
void EVE_CMD_APILEVEL(uint32_t level);
void EVE_CMD_FONTCACHE(uint32_t font, int32_t ptr, uint32_t num);
void EVE_CMD_FONTCACHEQUERY(uint32_t total, int32_t used);
void EVE_CMD_HSF(uint32_t w );
void EVE_CMD_PCLKFREQ(uint32_t ftarget, int32_t rounding, uint32_t factual);
#endif

#if IS_EVE_API(4, 5)
void EVE_CMD_RUNANIM(uint32_t waitmask, uint32_t play);
void EVE_CMD_TESTCARD(void);
void EVE_CMD_WAIT(uint32_t us);
void EVE_CMD_NEWLIST(uint32_t a);
void EVE_CMD_ENDLIST(void);
void EVE_CMD_CALLLIST(uint32_t a);
void EVE_CMD_RETURN(void);
#endif

#if IS_EVE_API(3, 4, 5)
void EVE_CMD_NOP(void);
void EVE_CMD_FILLWIDTH(uint32_t s); 
void EVE_CMD_ROTATEAROUND(int32_t x, int32_t y, int32_t a, int32_t s);
void EVE_CMD_RESETFONTS(void);
void EVE_CMD_GRADIENTA(int16_t x0, int16_t y0, uint32_t argb0, int16_t x1, int16_t y1, uint32_t argb1); //TODO
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
#endif

#if IS_EVE_API(4, 5)
void EVE_CMD_GETIMAGE(uint32_t source, uint32_t fmt, uint32_t w, uint32_t h, uint32_t palette);
void EVE_CMD_CALIBRATESUB(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t result);
#endif

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
#endif

/* EVE API END */

#if IS_EVE_API(5)
// Base patch for BT82x
#include <extensions/bt82x_patch.h>
#endif

#endif    /* EVE_HEADER_H */
