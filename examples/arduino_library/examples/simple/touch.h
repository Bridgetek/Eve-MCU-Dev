/**
    @file touch.h
 **/
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

#ifndef EVE_TOUCH_H
#define EVE_TOUCH_H

#include <stdint.h>

/**
 @brief Key for identifying if touchscreen calibration values are programmed correctly.
 @details This is used to determine if the stored calibration information on a 
    platform is correct. If the key is not correct then the calibration will be
    redone. If one MCU is used with different panel types (or display resolutions)
    then the following modification will ensure that calibration is rerun each time
    there is a change of panel in EVE_config.h.
        #define VALID_KEY_TOUCHSCREEN ((0xd72f91a3 ^ PANEL_TYPE) ^ DISPLAY_RES)
 */
#define VALID_KEY_TOUCHSCREEN 0xd72f91a3

/**
 @brief Structure to hold touchscreen calibration settings.
 @details This is used to store the touchscreen calibration settings persistently
 in Flash and identify if the calibration needs to be re-performed.
 */
struct touchscreen_calibration {
    uint32_t key; // VALID_KEY_TOUCHSCREEN
    uint32_t transform[6];
};

/**
 @brief Calibration Function
 */
int eve_calibrate(void);

/**
 @brief Simple Touch Detect Function
 */
int eve_key_detect(void);

/**
 @brief Touch Detect Function returning Tag Value
 */
int eve_read_tag(uint8_t *key);

/**
 @brief Functions called from eve_example code to platform specific code 
 @details These will not be called if there is a preconfigured touchscreen
 setting for a panel. e.g. DP_1012_01A and DP_1561_01A.
 */
//@{

/** @brief Initialise method used to store touchscreen settings
 * @returns zero if storing touchscreen settings is supported
 *          non-zero if the program does not store touchscreen settings
 */
int8_t platform_calib_init(void);

/** @brief Write touchscreen settings
 * @returns zero if touchscreen settings successfully written
 *          non-zero if the program could not store touchscreen settings
 */
int8_t platform_calib_write(struct touchscreen_calibration *calib);

/** @brief Read touchscreen settings
 * @returns zero if touchscreen settings successfully read
 *          non-zero if the program could not read touchscreen settings
 */
int8_t platform_calib_read(struct touchscreen_calibration *calib);
//@}

#endif /* EVE_TOUCH_H */
