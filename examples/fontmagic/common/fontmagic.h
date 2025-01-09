/**
    @file eve_example.h
 **/
/*
 * ============================================================================
 * History
 * =======
 * Nov 2019		Initial beta for FT81x and FT80x
 * Mar 2020		Updated beta - added BT815/6 commands
 * Mar 2021		Beta with BT817/8 support added
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

#ifndef _FONTMAGIC_H
#define _FONTMAGIC_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 @brief Definitions of handles for custom fonts and bitmaps.
 */
//@{
#define FONT_ROM 30
#define FONT_CUSTOM 8
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

/**
 @brief Globals available within the example code.
 */
//{
extern const uint8_t font0[];
extern const uint32_t font0_offset;
extern const uint8_t font0_first;
extern const uint32_t font0_size;
//}

/**
 @brief Ratios in 16.16 format for zooming in and out. 
 */
//{
#define ZOOM_1 (1 << 16)
#define ZOOM_IN_2 (2 * (1 << 16))
#define ZOOM_IN_4 (4 * (1 << 16))
#define ZOOM_OUT_2 ((1 << 16) / 2)
#define ZOOM_OUT_3_4 ((1 << 16) * 3 / 4)
//}

/**
 @brief Maximum number of characters to cache form a font.
 */
#define MAX_CHARACTERS 128

/**
 @brief Structure to hold cache of font settings.
 @details This is used to store the font parameters to accelerate drawing
 the glyphs during the application.
 */
struct eve_font_cache {
    uint8_t handle;
    uint8_t legacy;
    uint8_t widths[MAX_CHARACTERS];
    uint32_t glyphs[MAX_CHARACTERS];
    uint16_t height;
    uint16_t width;
    uint8_t first;
};


/**
 @brief Functions called within the eve_example code. 
 */
//{
void eve_calibrate(void);
uint32_t eve_init_fonts(uint8_t fontnumber);
uint8_t eve_read_tag(uint8_t *key);
//}

/**
 @brief Functions called from example code to platform specific code. 
 */
//{
int8_t platform_calib_init(void);
int8_t platform_calib_write(struct touchscreen_calibration *calib);
int8_t platform_calib_read(struct touchscreen_calibration *calib);
//}

/**
 @brief Entry point to the example code .
 */
void fontmagic(void);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* _FONTMAGIC_H */
