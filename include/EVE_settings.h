/**
 * @file EVE_settings.h
 * @brief Header file to include to library settings and macros derived from the configuration file (EVE_config.h).
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

#ifndef _EVE_SETTINGS_H
#define _EVE_SETTINGS_H

/*
 * Include the user-supplied EVE configuration from the configured
 * include path to select the EVE API and library options.
 * EVE_config.h includes EVE_defs.h from the same include search path,
 * so a compatible pair of headers must be provided.
 */
#include <EVE_config.h>

/*
 * Support deprecated config items by overriding the replacement
 * macros with the deprecated values.
 * The presence of the deprecated macros is reported in EVE_API.c.
 */
#define EVEIFY(y) EVE_ ## y
#define EVEUPDATE(y) EVEIFY(y)

#if defined(FT8XX_TYPE)
#undef EVE_DEVICE
#define EVE_DEVICE EVEUPDATE(FT8XX_TYPE)
#endif // defined(FT8XX_TYPE)

#if defined(DISPLAY_RES)
#undef EVE_DISPLAY_RES
#define EVE_DISPLAY_RES EVEUPDATE(DISPLAY_RES)
#endif // defined(DISPLAY_RES)

#if defined(MODULE_TYPE)
#undef EVE_MODULE
#define EVE_MODULE EVEUPDATE(MODULE_TYPE)
#endif // defined(MODULE_TYPE)

#if defined(PANEL_TYPE)
#undef EVE_PANEL
#define EVE_PANEL EVEUPDATE(PANEL_TYPE)
#endif // defined(PANEL_TYPE)

#if defined(QUADSPI_ENABLE)
#define EVE_QSPI_ENABLE
#endif // defined(QUADSPI_ENABLE)

/* EVE SETTINGS */

/**
 * @brief EVE library settings derived from the target configuration.
 * @details The settings and macros in this file are derived from the
 *      configuration supplied by EVE_config.h. This includes EVE API
 *      selection, module and panel settings, display parameters and
 *      device-specific feature support.
 *
 *      Application-specific configuration should be made in EVE_config.h
 *      rather than directly in this file.
 */

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
 *      The EVE_DEVICE and the display settings must be configured before calling EVE.h.
 *      "#define EVE_DEVICE EVE_BT817" is equivalent to having "#define EVE_API 4".
 *      Note the use of EVEx_ENABLE is deprecated but the macro is still defined.
 *      The EVE_DEVICE and EVE_PANEL macros must not be expanded until their
 *      allowable values from EVE_defs.h are available.
 */
//@{
#if defined(EVE_MODULE) && (EVE_MODULE != EVE_NO_MODULE)
#undef EVE_DEVICE
#undef EVE_PANEL

/**
 * @brief Predefined Bridgetek module displays
 */
#if EVE_MODULE == EVE_VM800B
// VM800B35A-BK with 3.5 inch display
#define EVE_DEVICE EVE_FT800
#define EVE_PANEL EVE_DP_0351_11A

#elif EVE_MODULE == EVE_VM800C35A
// VM800C35A-D with 3.5 inch display
#define EVE_DEVICE EVE_FT800
#define EVE_PANEL EVE_DP_0351_11A

#elif EVE_MODULE == EVE_VM800C43A
// VM800C43A-D with 4.3 inch display
#define EVE_DEVICE EVE_FT800
#define EVE_PANEL EVE_DP_0431_11A

#elif EVE_MODULE == EVE_VM800C50A
// VM800C50A-D with 5 inch display
#define EVE_DEVICE EVE_FT800
#define EVE_PANEL EVE_DP_0502_11A

#elif EVE_MODULE == EVE_VM810C
// VM810C50A-D with 5 inch display
#define EVE_DEVICE EVE_FT810
#define EVE_PANEL EVE_DP_0501_11A

#if defined(EVE_QSPI_ENABLE)
// disable QSPI for this module (pins not available)
#undef EVE_QSPI_ENABLE
#endif // EVE_QPSI_ENABLE

#elif EVE_MODULE == EVE_ME812A
// ME812A-WH50R, ME812AU-WH50R with 5 inch display
#define EVE_DEVICE EVE_FT812
#define EVE_PANEL EVE_DP_0501_11A

#elif EVE_MODULE == EVE_ME813A
// ME813A-WH50C with 5 inch display
#define EVE_DEVICE EVE_FT813
#define EVE_PANEL EVE_DP_0501_01A

#elif EVE_MODULE == EVE_VM816C
// VM816C50A-D, VM816CU50A-D with 5 inch display
#define EVE_DEVICE EVE_BT816
#define EVE_PANEL EVE_DP_0501_11A

#elif EVE_MODULE == EVE_VM880C
// VM880C with 4.3 inch display
#define EVE_DEVICE EVE_BT880
#define EVE_PANEL EVE_DP_0431_11A

#if defined(EVE_QSPI_ENABLE)
// disable QSPI for this module (pins not available)
#undef EVE_QSPI_ENABLE
#endif // EVE_QPSI_ENABLE

#elif EVE_MODULE == EVE_IDM204021R
// IDM2040-21R
#define EVE_DEVICE EVE_FT800
#define EVE_PANEL EVE_DP_IDM21R

// Enable EVE_CUSTOM_TOUCH by default for this module
#ifndef EVE_CUSTOM_TOUCH
#define EVE_CUSTOM_TOUCH
#endif

// Enable EVE_LCD_INIT by default for this module
#ifndef EVE_LCD_INIT
#define EVE_LCD_INIT
#endif

#elif EVE_MODULE == EVE_IDM204043A
// IDM2040-43A
#define EVE_DEVICE EVE_BT883
#define EVE_PANEL EVE_DP_0431_11A

#elif EVE_MODULE == EVE_IDM20407A
// IDM2040-7A
#define EVE_DEVICE EVE_BT817
#define EVE_PANEL EVE_DP_0701_01A

#elif EVE_MODULE == EVE_VM820B10A
// VM820B10A with 10.1 inch display (BT820 with DP-1011-02A)
#define EVE_DEVICE EVE_BT820
#define EVE_PANEL EVE_DP_1011_02A

#elif EVE_MODULE == EVE_VM820B15A
// VM820B15A with 15.6 inch display (BT820 with DP-1561-02A)
#define EVE_DEVICE EVE_BT820
#define EVE_PANEL EVE_DP_1561_02A

#elif EVE_MODULE == EVE_IDK_FT810_43A
// FT810 IC Development Kit with 4.3 - inch display (FT810 with DP-0431-11A)
#define EVE_DEVICE EVE_FT810
#define EVE_PANEL EVE_DP_0431_11A

#elif EVE_MODULE == EVE_IDK_BT816_50A
// BT816 IC Development Kit with 5.0-inch display (BT816 with DP-0501-11A)
#define EVE_DEVICE EVE_BT816
#define EVE_PANEL EVE_DP_0501_11A

#elif EVE_MODULE == EVE_IDK_BT817_70A
// BT817 IC Development Kit with 7.0-inch display (BT817 with DP-0701-01A)
#define EVE_DEVICE EVE_BT817
#define EVE_PANEL EVE_DP_0701_01A

#elif EVE_MODULE == EVE_IDK_BT817_101A
// BT817 IC Development Kit with 10.1-inch display (BT817 with DP-1011-02A)
#define EVE_DEVICE EVE_BT817
#define EVE_PANEL EVE_DP_1011_02A

#elif EVE_MODULE == EVE_IDK_BT820_101A
// BT820B IC Development Kit with 10.1-inch display (BT820 with DP-1012-01A)
#define EVE_DEVICE EVE_BT820
#define EVE_PANEL EVE_DP_1012_01A

#else

#error EVE_MODULE must be configured.

#endif // EVE_MODULE
#endif // defined(EVE_MODULE)
//@}

#endif // !defined(IS_ARDUINO_LIB) 

#ifndef EVE_API
#undef EVE_SUB_API

#if (EVE_DEVICE == EVE_FT800)
#define EVE_API 1

#elif (EVE_DEVICE == EVE_FT801)
#define EVE_API 1

#elif (EVE_DEVICE == EVE_FT810)
#define EVE_API 2
#define EVE_SUB_API 1

#elif (EVE_DEVICE == EVE_FT811)
#define EVE_API 2
#define EVE_SUB_API 1

#elif (EVE_DEVICE == EVE_FT812)
#define EVE_API 2
#define EVE_SUB_API 1

#elif (EVE_DEVICE == EVE_FT813)
#define EVE_API 2
#define EVE_SUB_API 1

#elif (EVE_DEVICE == EVE_BT880)
#define EVE_API 2
#define EVE_SUB_API 2

#elif (EVE_DEVICE == EVE_BT881)
#define EVE_API 2
#define EVE_SUB_API 2

#elif (EVE_DEVICE == EVE_BT882)
#define EVE_API 2
#define EVE_SUB_API 2

#elif (EVE_DEVICE == EVE_BT883)
#define EVE_API 2
#define EVE_SUB_API 2

#elif (EVE_DEVICE == EVE_BT815)
#define EVE_API 3

#elif (EVE_DEVICE == EVE_BT816)
#define EVE_API 3

#elif (EVE_DEVICE == EVE_BT817)
#define EVE_API 4

#elif (EVE_DEVICE == EVE_BT818)
#define EVE_API 4

#elif (EVE_DEVICE == EVE_BT820)
#define EVE_API 5

#else
#ifndef EVE_API
#error EVE_DEVICE definition not recognised.
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

/** Co-processor transfer method.
 *
 *   EVE1 always writes directly to the co-processor ring buffer then
 *   REG_CMD_WRITE is updated.
 *   EVE2+ can write directly to the co-processor ring buffer via the
 *   REG_CMDB_WRITE register.
 *
 *   If EVE_COPRO_METHOD is undefined then EVE_COPRO_CMD_WRITE is
 *   used for EVE1 and EVE_COPRO_CMDB_WRITE is used for EVE2 onwards.
 *   If EVE_COPRO_METHOD is set for EVE_COPRO_CMDB_WRITE on EVE1
 *   then the setting will be modified to EVE_COPRO_CMD_WRITE.
 *   The EVE_COPRO_INT flag can be used to modify the
 *   EVE_COPRO_CMD_WRITE method to use the hardware INT# line as well.
 *   This requires support from the port in the MCU layer.
 */
#if defined(EVE_COPRO_METHOD)
    // Validate the selected co-processor transfer method
    #if (EVE_COPRO_METHOD != EVE_COPRO_CMDB_WRITE) && \
            (EVE_COPRO_METHOD != EVE_COPRO_CMD_WRITE)  && \
            (EVE_COPRO_METHOD != EVE_COPRO_INT)
        #error "Invalid EVE_COPRO_METHOD value"
    #endif

    // Select the requested method.
    #if (EVE_COPRO_METHOD == EVE_COPRO_CMDB_WRITE)
        #if IS_EVE_API(1)
            // CMDB write method is only supported on EVE2 onwards, undef to use CMD 
            #undef EVE_USE_CMDB_METHOD
        #else
            #define EVE_USE_CMDB_METHOD
        #endif // IS_EVE_API(1)
    #elif (EVE_COPRO_METHOD == EVE_COPRO_INT)
        // INT implies use of the CMD write method.
        #define EVE_USE_INTERRUPT_METHOD
    #endif
#else // else defined(EVE_COPRO_METHOD)
    #if IS_EVE_API(2,3,4,5)
        // Default to CMDB on EVE2 onwards.
        #define EVE_USE_CMDB_METHOD
    #endif // IS_EVE_API(2,3,4,5)
#endif // defined(EVE_COPRO_METHOD)

/** Interrupt management.
 *
 *   Define the macro to enable interrupt management of the REG_INT_FLAGS
 *   register. If this is enabled then all testing of the REG_INT_FLAGS
 *   register must be made through the EVE_LIB_GetInterrupt function.
 *   If EVE_USE_INTERRUPT_METHOD is defined then the feature is enabled.
 *   The function does not manage the INT# line (see EVE_LIB_Int function).
 */
#if defined(EVE_USE_INTERRUPT_METHOD) && !defined(EVE_MANAGE_INTERRUPTS)
#define EVE_MANAGE_INTERRUPTS
#endif

/** EVE1 does not support QSPI, disable this if it has been defined */
#if IS_EVE_API(1)
    #if defined(EVE_QSPI_ENABLE)
        #undef EVE_QSPI_ENABLE
    #endif // EVE_QSPI_ENABLE
#endif // IS_EVE_API(2,3,4,5)


/** Custom touch firmware is supported by EVE API 2-4.
 *
 * @note Custom touch firmware on EVE5 is supported via patch extensions,
 *       so the code guarded by this define is not required.
 */
#if IS_EVE_API(2,3,4) 
#define EVE_SUPPORT_CUSTOM_TOUCH
#endif
/** IDM2040-21R is an EVE API 1 exception which requires a
 * custom touch firmware patch.
 */
#if defined(EVE_MODULE) && (EVE_MODULE == EVE_IDM204021R)
#define EVE_SUPPORT_CUSTOM_TOUCH
#endif


#if !defined(IS_ARDUINO_LIB) /* This block is not used in Arduino libraries */

/**
 * @brief Match display resolution to panel type
 */
//@{
#if defined(EVE_PANEL) && (EVE_PANEL != EVE_NO_PANEL)
#undef EVE_DISPLAY_RES

/**
 * @brief Predefined selection of EVE_DISPLAY_RES by panel type.
 */
#if EVE_PANEL == EVE_DP_0351_11A
// DP-0351-11A QVGA (Resistive)
#define EVE_DISPLAY_RES EVE_QVGA

#elif EVE_PANEL == EVE_DP_0431_11A
// DP-0431-11A WQVGA (Resistive)
#define EVE_DISPLAY_RES EVE_WQVGA   

#elif EVE_PANEL == EVE_DP_0501_01A
// DP-0501-01A WVGA (Capacitive)
#define EVE_DISPLAY_RES EVE_WVGA    

#elif EVE_PANEL == EVE_DP_0501_11A
// DP-0351-11A WVGA (Resistive)
#define EVE_DISPLAY_RES EVE_WVGA    

#elif EVE_PANEL == EVE_DP_0502_11A
// DP-0502-11A WQVGA (Resistive)
#define EVE_DISPLAY_RES EVE_WQVGA   

#elif EVE_PANEL == EVE_DP_0701_01A
// DP-0701-01A WVGA (Capacitive)
#define EVE_DISPLAY_RES EVE_WVGA

#elif EVE_PANEL == EVE_DP_1011_01A
// DP-1011-01A WXGA (Capacitive)
#define EVE_DISPLAY_RES EVE_WXGA

#elif EVE_PANEL == EVE_DP_1011_02A
// DP-1011-02A WXGA_NG (Capacitive)
#define EVE_DISPLAY_RES EVE_WXGA_NG

// Enable EVE_CUSTOM_TOUCH by default for this panel
#ifndef EVE_CUSTOM_TOUCH
#define EVE_CUSTOM_TOUCH
#endif

#elif EVE_PANEL == EVE_DP_1012_01A
// DP-1012-01A WUXGA (Capacitive)
#define EVE_DISPLAY_RES EVE_WUXGA

#elif EVE_PANEL == EVE_DP_1561_01A
// DP-1561-01A FullHD (Capacitive)
#define EVE_DISPLAY_RES EVE_FULLHD

#elif EVE_PANEL == EVE_DP_1561_02A
// DP-1561-02A FullHD (Capacitive)
#define EVE_DISPLAY_RES EVE_FULLHD

#elif EVE_PANEL == EVE_DP_IDM21R
// IDM204021R (Capacitive)
#define EVE_DISPLAY_RES EVE_WQVGAR 

#else

#error EVE_PANEL must be configured.

#endif // EVE_PANEL
#else
#undef EVE_PANEL
#define EVE_PANEL EVE_NO_PANEL
#endif // defined(EVE_PANEL)
//@}

/**
 * @brief Setup default parameters for various displays.
 * @details These can be overridden for different display modules.
 */
 //@{
#undef EVE_SET_PCLK_FREQ

#if EVE_DISPLAY_RES == EVE_QVGA

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

#elif EVE_DISPLAY_RES == EVE_WQVGA

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

#elif EVE_DISPLAY_RES == EVE_WVGA

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

#elif EVE_DISPLAY_RES == EVE_WSVGA

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
#define EVE_SET_PCLK_FREQ
#define EVE_DISP_PCLK_FREQ  0xD12    // set 51MHz (must also define EVE_SET_PCLK_FREQ in line above to use this)
// BT82x settings
#define EVE_DISP_LVDSTXCLKDIV 3
#define EVE_DISP_LVDSTXFORMAT EVE_FORMAT_RGB6

#elif EVE_DISPLAY_RES == EVE_WXGA

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
#define EVE_SET_PCLK_FREQ
#define EVE_DISP_PCLK_FREQ  0x8B1    // set 51MHz (must also define EVE_SET_PCLK_FREQ in line above to use this)
// BT82x settings
#define EVE_DISP_LVDSTXCLKDIV 3
#define EVE_DISP_LVDSTXFORMAT EVE_FORMAT_RGB6

#elif EVE_DISPLAY_RES == EVE_WXGA_NG

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
#define EVE_SET_PCLK_FREQ
#define EVE_DISP_PCLK_FREQ  0x8B1    // set 51MHz (must also define EVE_SET_PCLK_FREQ in line above to use this)
// BT82x settings
#define EVE_DISP_LVDSTXCLKDIV 3
#define EVE_DISP_LVDSTXFORMAT EVE_FORMAT_RGB6

#elif EVE_DISPLAY_RES == EVE_FULLHD

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

#elif EVE_DISPLAY_RES == EVE_WUXGA

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

#elif EVE_DISPLAY_RES == EVE_WQVGAR

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

#error EVE_DISPLAY_RES must be configured.

#endif // EVE_DISPLAY_RES
//@}

#endif // !defined(IS_ARDUINO_LIB)

#endif /* _EVE_SETTINGS_H */
