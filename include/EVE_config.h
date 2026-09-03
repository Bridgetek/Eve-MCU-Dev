/**
 * @file EVE_config.h
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

#ifndef _EVE_CONFIG_H
#define _EVE_CONFIG_H

/**
 * @brief EVE device and screen configuration file.
 * @note IMPORTANT This header file must be included by EVE.h.
 * @details The macro EVE_DEVICE and the panel display settings (EVE_DISP_*) 
 *      must be configured in this file. 
 *      For BT82x the EVE_RAM_G_CONFIG_SIZE macro must also be configured.
 *      Values from the macros defined in this file can be used in code 
 *      based on this library. 
 *      To make a custom configuration file, edit this file as required as 
 *      long as the macros listed above are correctly defined.
 *      This file is included with angle brackets in EVE.h and can therefore 
 *      be copied to anywhere in the include file search path to override this 
 *      copy.
 */

/** 
 * @brief Module selection.
 * @details  If this is set to a valid module type then the EVE_DEVICE and 
 *      EVE_DISPLAY_RES values will be set accordingly.
 *      This selection will override any EVE_PANEL, EVE_DEVICE and EVE_DISPLAY_RES
 *      settings.
  * @note The MODULE_TYPE macro has been deprecated in favour of EVE_MODULE.
 *      Please update references accordingly.
*/
//@{
#ifndef EVE_MODULE
#define EVE_MODULE EVE_NO_MODULE
#endif
//@}

/**
 * @brief Select the EVE controller type.
 * @details In EVE.h the EVE controller type will lead to the selection of 
 *      the EVE Programming support methods via macros "EVE_API" where the 
 *      value depends on the level of the EVE device support. 
 *      Alternatively, to override this directly set the EVE_API and 
 *      EVE_SUB_API macro as required.
 * @note The FT8XX_TYPE macro has been deprecated in favour of EVE_DEVICE.
 *      Please update references accordingly.
 */
//@{
#ifndef EVE_DEVICE
#define EVE_DEVICE EVE_BT817
#endif
//@}

/**
 * @brief Select the EVE panel type.
 * @details Setting the EVE panel type to a valid value will lead to the 
 *      selection of the EVE panel which in turn will select a valid 
 *      EVE_DISPLAY_RES setting.
 *      This setting will override the EVE_DISPLAY_RES setting.
 * @note The PANEL_TYPE macro has been deprecated in favour of EVE_PANEL.
 *      Please update references accordingly.
 */
//@{
#ifndef EVE_PANEL
#define EVE_PANEL EVE_NO_PANEL
#endif
//@}

/**
 * @brief Match display resolution to panel type.
 * @details The distribution default is WVGA (800x480).
 * @note The DISPLAY_RES macro has been deprecated in favour of EVE_DISPLAY_RES.
 *      Please update references accordingly.
 */
//@{
#ifndef EVE_DISPLAY_RES
#define EVE_DISPLAY_RES EVE_WVGA
#endif
//@}

/**
 * @brief Enable or Disable QuadSPI.
 * @details If the macro is set then the platform port may only enable QSPI
 *      on the EVE device (using HAL_SetSPIMode) if  is supported by the platform.
 *      EVE_QSPI_ENABLE may be defined externally to enable QSPI, otherwise it
 *      remains undefined.
 * @note QSPI is only supported on devices from EVE API 2 onwards. It is not
 *      supported on FT80x devices. For default set this to disabled.
 * @note The QUADSPI_ENABLE macro has been deprecated in favour of EVE_QSPI_ENABLE.
 *      Please update references accordingly.
 */
//@{
#ifndef EVE_QSPI_ENABLE
#undef EVE_QSPI_ENABLE
#endif
//@}

 /**
  * @brief Setup RAM_G size for BT82X only
  */
//@{
#ifndef EVE_RAM_G_CONFIG_SIZE
#define EVE_RAM_G_CONFIG_SIZE EVE_RAM_G_1_GBIT
#endif
//@}

/**
 * @brief Select the touchscreen automatically for BT82X or use the EVE_REG_TOUCH_CONFIG.
 *      default for FT81X/BT88X/BT81X (#undef). Assign the desired i2c address or type (BT82X)
 *      by defining the value (#define). 
 */
//@{
#undef EVE_TOUCH_ADDR
#undef EVE_TOUCH_TYPE
//@}

/**
  * @brief Enable or Disable custom couch FW load.
  * @details If the macro is set then custom touch FW will be loaded during IC
  *			  initialisation from the binary data array in the "custom_touch_fw.c" file.
  *			  Applicable for FT81X/BT88X/BT81X devices only.
  *
  *       EVE_CUSTOM_TOUCH may be defined externally to enable custom touch.
  *       It is also enabled automatically for panels which require it.
  *       Otherwise it remains undefined.
  *
  * NOTE: EVE_TOUCH_ADDR settings will be overridden when using EVE_CUSTOM_TOUCH.
  * NOTE: Custom touch for the BT82X series is implemented in extension patches.
  *
  */
//@{
#ifndef EVE_CUSTOM_TOUCH
#undef EVE_CUSTOM_TOUCH
#endif
//@}

/**
  * @brief Specify method to write to the co-processor circular buffer.
  * @details If the macro is set then the appropriate method of writing to
  *   the co-processor is used.
  *   If this is undefined then EVE_COPRO_CMD_WRITE is used for EVE1
  *   and EVE_COPRO_CMDB_WRITE is used for EVE2 onwards.
  *   If this is set for EVE_COPRO_CMDB_WRITE on EVE1 then the setting
  *   will be modified to EVE_COPRO_CMD_WRITE.
  *   The EVE_COPRO_INT can be used to modify the EVE_COPRO_CMD_WRITE
  *   method to use the hardware INT# line as well. This requires support
  *   from the port in the MCU layer.
  */
//@{
#ifndef EVE_COPRO_METHOD
#define EVE_COPRO_METHOD EVE_COPRO_CMDB_WRITE
#endif
//@}


#endif /* _EVE_CONFIG_H */
