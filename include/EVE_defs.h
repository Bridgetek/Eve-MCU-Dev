/**
 * @file EVE_defs.h
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

#ifndef _EVE_DEFS_H_
#define _EVE_DEFS_H_

/**
 * @note IMPORTANT This header file must be included before EVE.h.
 * @details The macro FT8XX_TYPE and the panel display settings (EVE_DISP_*) must 
 *      be configured in this file. For BT82x the EVE_RAM_G_CONFIG_SIZE macro must
 *      also be configured.
 *      Values from the macros defined in this file can be used in code based on this library. 
 *      To make a custom configuration file, edit this file as required as long as the macros
 *      listed above are correctly defined.
 */

/**
 * @brief Definitions used for target device selection.
 * @details The following options are defined for the FT8XX_TYPE macro.
 */
#define FT800 800
#define FT801 801
#define FT810 810
#define FT811 811
#define FT812 812
#define FT813 813
#define BT880 880
#define BT881 881
#define BT882 882
#define BT883 883
#define BT815 815
#define BT816 816
#define BT817 817
#define BT818 818
#define BT820 820

/**
 * @brief Select Bridgetek EVE Module or Development Kit Types.
 * @details The following options are defined for the MODULE_TYPE macro:
 *      VM800B           - VM800B35A-BK with 3.5 inch display (FT800 with DP-0351-11A)
 *      VM800C35A        - VM800C35A-D with 3.5 inch display (FT800 with DP-0351-11A)
 *      VM800C43A        - VM800C43A-D with 4.3 inch display (FT800 with DP-0431-11A)
 *      VM800C50A        - VM800C50A-D with 5 inch display (FT800 with DP-0502-11A)
 *      VM810C           - VM810C50A-D with 5 inch display (FT810 with DP-0501-11A)
 *      ME812A           - ME812A-WH50R, ME812AU-WH50R with 5 inch display (FT812 with DP-0501-11A)
 *      ME813A           - ME813A-WH50C with 5 inch display (FT813 with DP-0501-01A)
 *      VM816C           - VM816C50A-D, VM816CU50A-D with 5 inch display (BT816 with DP-0501-11A)
 *      VM880C           - VM880C with assumed 4.3 inch display (BT880 with DP-0431-11A)
 *      IDM204021R       - IDM2040-21R (FT800 with 2.1 inch display)
 *      IM204043A        - IDM2040-43A (BT883 with DP-0431-11A)
 *      IDM20407A        - IDM2040-7A (BT817 with DP-0701-01A)
 *      VM820B10A        - VM820B10A with 10.1 inch display (BT820 with DP-1011-02A)
 *      VM820B15A        - VM820B15A with 15.6 inch display (BT820 with DP-1561-02A)
 *      IDK_FT810_43A    - FT810 IC Development Kit with 4.3-inch display (FT810 with DP-0431-11A)
 *      IDK_BT816_50A    - BT816 IC Development Kit with 5.0-inch display (BT816 with DP-0501-11A)
 *      IDK_BT817_70A    - BT817 IC Development Kit with 7.0-inch display (BT817 with DP-0701-01A)
 *      IDK_BT817_101A   - BT817 IC Development Kit with 10.1 inch display (BT817 with DP-1011-02A)
 *      IDK_BT820_101A   - BT820 IC Development Kit with 10.1-inch display (BT820 with DP-1012-01A)
 * 
 * Setting MODULE_TYPE will select the correct FT8XX_TYPE, PANEL_TYPE and DISPLAY_RES.
 */
//@{
#define VM800B          800     // VM800B35A-BK 
#define VM800C35A       835     // VM800C35A-D 
#define VM800C43A       843     // VM800C43A-D 
#define VM800C50A       850     // VM800C50A-D 
#define VM810C          810     // VM810C50A-D 
#define ME812A          812     // ME812A-WH50R, ME812AU-WH50R 
#define ME813A          813     // ME813A-WH50C 
#define VM816C          816     // VM816C50A-D, VM816CU50A-D 
#define VM880C          880     // VM880C 
#define IDM204021R      204021  // IDM2040-21R 
#define IDM204043A      204043  // IDM2040-43A 
#define IDM20407A       20407   // IDM2040-7A 
#define VM820B10A       82010   // VM820B10A 
#define VM820B15A       82015   // VM820B15A 
#define IDK_FT810_43A   81043   // IDK-FT810-43A 
#define IDK_BT816_50A   81650   // IDK-BT816-50A 
#define IDK_BT817_70A   81770   // IDK-BT817-70A 
#define IDK_BT817_101A  817101  // IDK-BT817-101A  
#define IDK_BT820_101A  820101  // IDK-BT820-101A
#define MODULE_TYPE_NONE        0
//@}

/**
 * @brief Define Bridgetek Panel Types
 * @details The following options are defined for the PANEL_TYPE macro.
 */
//@{
#define DP_0351_11A 0351    // DP-0351-11A QVGA (Resistive)
#define DP_0431_11A 0431    // DP-0431-11A WQVGA (Resistive)
#define DP_0501_01A 050101  // DP-0501-01A WVGA (Capacitive)
#define DP_0501_11A 050111  // DP-0501-11A WVGA (Resistive)
#define DP_0502_11A 0502    // DP-0502-11A WQVGA (Resistive)
#define DP_0701_01A 0701    // DP-0701-11A WVGA (Capacitive)
#define DP_1011_01A 10111   // DP-1011-01A WXGA (Capacitive)
#define DP_1011_02A 10112   // DP-1011-02A WXGA_NG (Capacitive)
#define DP_1012_01A 1012    // DP-1012-01A WUXGA (Capacitive)
#define DP_1561_01A 156101  // DP-1561-01A FullHD (Capacitive)
#define DP_1561_02A 156102  // DP-1561-02A FullHD (Capacitive)
#define DP_IDM43A   43      // IDM204043A (Capacitive)
#define DP_IDM21R   21      // IDM204021R (Capacitive)
#define PANEL_TYPE_NONE     0
//@}

/** 
 * @brief Select Display Resolutions for Common Panels
 * @details The following options are defined for the DISPLAY_RES macro:
 *      QVGA      - 320x240   e.g. DP-0351-11A
 *      WQVGA     - 480x272   e.g. DP-0431-11A, DP-0502-11A
 *      WQVGAR    - 480x480   e.g. IDM2040-21R with 2.1 inch round display
 *      WVGA      - 800x480   e.g. DP-0501-01A, DP-0501-11A, DP-0701-11A
 *      WSVGA     - 1024x600  e.g. BT817 with 7 inch display
 *      WXGA      - 1280x800  e.g. DP-1011-01A
 *      WXGA_NG   - 1280x800  e.g. DP-1011-02A
 *      FULLHD    - 1920x1080 e.g. DP-1561-01A, DP-1561-02A
 *      WUXGA     - 1920x1200 e.g. DP-1012-01A
 */
//@{
#define QVGA    320        // 320x240   e.g. DP-0351-11A
#define WQVGA   480        // 480x272   e.g. DP-0431-11A, DP-0502-11A
#define WQVGAR  480480     // 480x480   e.g. IDM2040-21R with 2.1 inch round display
#define WVGA    800        // 800x480   e.g. DP-0501-01A, DP-0501-11A, DP-0701-11A
#define WSVGA   1024       // 1024x600  e.g. BT817 with 7 inch display
#define WXGA    12801      // 1280x800  e.g. DP-1011-01A
#define WXGA_NG 12802      // 1280x800  e.g. DP-1011-02A
#define FULLHD  1920       // 1920x1080 e.g. DP-1561-01A, DP-1561-02A
#define WUXGA   19201200   // 1920x1200 e.g. DP-1012-01A
//@}

/**
 * @brief RAM_G size options for BT82X only
 * @details Available options are in Gigabits: 0.03Gb, 0.06Gb, 0.12Gb, 0.25Gb, 0.5Gb, 1Gb, 2Gb, 4Gb, or 8Gb
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

/**
 * @brief Definitions used for capacitive touch controller i2c address and type (type is BT82X only).
 *
 * NOTE: For FT81X/BT88X/BT81X only TOUCH_ADDR_FOCALTECH & TOUCH_ADDR_GOODIX are applicale.
 */
//@{
#define TOUCH_ADDR_FOCALTECH 0x38 // Focaltech (e.g. FT5206)
#define TOUCH_TYPE_FOCALTECH 1
#define TOUCH_ADDR_GOODIX 0x5d // Goodix (e.g. GT911)
#define TOUCH_TYPE_GOODIX 2
#define TOUCH_ADDR_ILITEK 0x41 // Ilitek (e.g. ILI2511, ILI2130)
#define TOUCH_TYPE_ILITEK 4
#define TOUCH_ADDR_SITRONIX 0x55 // Sitronix (e.g. ST1633i)
#define TOUCH_TYPE_SITRONIX 6
#define TOUCH_ADDR_EETI 0x55 // Eeti (e.g. EXC80W46)
#define TOUCH_TYPE_EETI 7
//@}

/**
 * @brief Defintions for transfer methods for co-processor lists.
 * @details These can enable the use of REG_CMDB_WRITE on EVE2 onwards
 *      or enable the INT# line to be used when REG_CMD_WRITE method
 *      is being used. See the "Command FIFO" section in the Programming
 *      Guides for details of the differences. The "Interrupts" section
 *      in the Data Sheets explains the use of the INT# line.
 *      The EVE_TRANSFER_INT option is not permissible with the
 *      EVE_CMDB_WRITE setting and will be ignored.
 */
//@{
// Add to the co-processor circular buffer through REG_CMDB_WRITE if it
// is supported.
#define EVE_TRANSFER_CMDB_WRITE     0
// Add to the co-processor circular buffer directly then update REG_CMD_WRITE.
#define EVE_TRANSFER_CMD_WRITE      1
// Modifier for EVE_TRANSFER_CMD_WRITE to use the INT# line when detecting
// the co-processor reaching the end of the list.
#define EVE_TRANSFER_INT            2
//@}
#endif /* _EVE_DEFS_H_ */
