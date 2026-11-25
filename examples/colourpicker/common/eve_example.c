/**
 * @file eve_example.c
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

/* INCLUDES ************************************************************************/

#include <stdint.h>
#include <math.h>

/* Include EVE-MCU-Dev library API layer and other required headers. */
#include <eve_example.h>

/* FUNCTIONS ***********************************************************************/

// Convert RGB8 to RGB565.
uint16_t rgb888_to_rgb565(uint8_t r, uint8_t g, uint8_t b)
{
    return ((r & 0xF8) << 8) |
        ((g & 0xFC) << 3) |
        (b >> 3);
}

// Convert RGB8 to ARG1555.
uint16_t rgb888_to_argb1555(uint8_t r, uint8_t g, uint8_t b)
{
    uint16_t a = 0;
    if (r + g + b > 0) a = 1 << 15;

    return a | ((r & 0xF8) << 7) |
        ((g & 0xF8) << 2) |
        (b >> 3);
}

// Convert RGB8 to RGB332.
uint8_t rgb888_to_rgb332(uint8_t r, uint8_t g, uint8_t b)
{
    return ((r & 0xe0)) |
        ((g & 0xe0) >> 3) |
        ((b & 0xc0) >> 6);
}

// Convert RGB565 to RGB8.
void rgb565_to_rgb(uint16_t rgb565, uint8_t *r, uint8_t *g, uint8_t *b)
{
    *r = (rgb565 >> 8) & 0xf8;
    *g = (rgb565 >> 3) & 0xfc;
    *b = (rgb565 << 3) & 0xf8;
}

// Convert ARGB1555 to RGB8.
void argb1555_to_rgb(uint16_t argb1555, uint8_t *r, uint8_t *g, uint8_t *b)
{
    *r = (argb1555 >> 7) & 0xf8;
    *g = (argb1555 >> 2) & 0xf8;
    *b = (argb1555 << 3) & 0xf8;
}

// Convert RGB332 to RGB8.
void rgb332_to_rgb(uint8_t rgb565, uint8_t *r, uint8_t *g, uint8_t *b)
{
    *r = (rgb565) & 0xe0;
    *g = (rgb565 << 3) & 0xe0;
    *b = (rgb565 << 6) & 0xc0;
}

// Hue/Saturation/Brightness (HSV colour space) to RGB8 conversion.
void hsv_to_rgb(float h, float s, float v, uint8_t* r, uint8_t* g, uint8_t* b)
{
    float c = v * s;
    float x = c * (1 - fabsf(fmodf(h * 6.0f, 2.0f) - 1));
    float m = v - c;
 
    float rp = 0, gp = 0, bp = 0;
 
    if (h < 1.0f / 6.0f) { rp = c; gp = x; }
    else if (h < 2.0f / 6.0f) { rp = x; gp = c; }
    else if (h < 3.0f / 6.0f) { gp = c; bp = x; }
    else if (h < 4.0f / 6.0f) { gp = x; bp = c; }
    else if (h < 5.0f / 6.0f) { rp = x; bp = c; }
    else { rp = c; bp = x; }
 
    *r = (uint8_t)((rp + m) * 255);
    *g = (uint8_t)((gp + m) * 255);
    *b = (uint8_t)((bp + m) * 255);
}

void eve_example(void)
{
    // Initialise the display
    EVE_DEBUG_PRINTF("Initialising display...\n");
    if (EVE_Init() != 0)
    {
        EVE_DEBUG_ERROR("ERROR: EVE_Init() failed.\n");
        return;
    }

#if IS_EVE_API(5)
    // On BT82x EVE5 onwards put the touch controller in extended mode
    EVE_LIB_MemWrite32(EVE_REG_CPURESET, 2);
    EVE_LIB_MemWrite32(EVE_REG_CTOUCH_EXTENDED, EVE_CTOUCH_MODE_EXTENDED);
    EVE_LIB_MemWrite32(EVE_REG_CPURESET, 0);
#endif //IS_EVE_API(5)

    // Calibrate the display
    EVE_DEBUG_PRINTF("Calibrating display...\n");
    if (eve_calibrate() != 0)
    {
        EVE_DEBUG_ERROR("ERROR: eve_calibrate() failed.\n");
        return;
    }

    // Start example code
    EVE_DEBUG_PRINTF("Starting demo:\n");
    // Run Application - choose one of the following modes
#if (EXAMPLE_COLOURPICKER == EXAMPLE_COLOURPICKER_WHEEL)
    eve_display_wheel();
#elif (EXAMPLE_COLOURPICKER == EXAMPLE_COLOURPICKER_MULTI)
    eve_display_argb1555();
#elif (EXAMPLE_COLOURPICKER == EXAMPLE_COLOURPICKER_PALETTED_RECT) || \
    (EXAMPLE_COLOURPICKER == EXAMPLE_COLOURPICKER_PALETTED_IMAGE)
    eve_display_paletted8();
#else
    EVE_DEBUG_PRINTF("Nothing to do!\n");
    #error EXAMPLE_COLOURPICKER invalid value
#endif
}
