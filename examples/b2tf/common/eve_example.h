/**
    @file eve_example.h
 **/
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

#ifndef _EVE_EXAMPLE_H
#define _EVE_EXAMPLE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 @brief Definitions of handles for custom fonts and bitmaps.
 */
//@{
#define FONT_CUSTOM 8
#define BITMAP_BRIDGETEK_LOGO 7
//@}

/**
 @brief Key for identifying if touchscreen calibration values are programmed correctly.
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

/* Globals available within the eve_example code */
extern uint32_t eve_img_bridgetek_logo_width;
extern uint32_t eve_img_bridgetek_logo_height;

/* Functions called within the eve_example code */
void eve_calibrate(void);
uint8_t eve_read_tag(uint8_t *key);
uint8_t eve_romfont_width(uint8_t font);
uint8_t eve_romfont_height(uint8_t font);

/* Functions called from eve_example code to platform specific code */
int8_t platform_calib_init(void);
int8_t platform_calib_write(struct touchscreen_calibration *calib);
int8_t platform_calib_read(struct touchscreen_calibration *calib);

/* Entry point to the example code */
void eve_example(void);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* _EVE_EXAMPLE_H */
