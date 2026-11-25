/**
 * @file eve_example.h
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

#ifndef _EVE_EXAMPLE_H
#define _EVE_EXAMPLE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Include EVE-MCU-Dev library API layer */
#include <EVE.h> 
/* Include the EVE debug-output macros */
#include <EVE_debug.h>

/* Include snippets */
#include <touch.h>

/* At the end of the first pass of the main loop return to prevent the simulator 
 * running continuously */

#if defined(SOFTWARE_SIMULATOR)
#define LOOP 0
#else
#define LOOP 1
#endif

/* Supported colour picker examples */
#define EXAMPLE_COLOURPICKER_WHEEL 1
#define EXAMPLE_COLOURPICKER_MULTI 2
#define EXAMPLE_COLOURPICKER_PALETTED_RECT 3
#define EXAMPLE_COLOURPICKER_PALETTED_IMAGE 4

/* Choose the colour picker example to display */
#ifndef EXAMPLE_COLOURPICKER 
#define EXAMPLE_COLOURPICKER EXAMPLE_COLOURPICKER_WHEEL
#endif // EXAMPLE_COLOURPICKER 

/* Entry point to the example code */
void eve_example(void);

/* Helper functions */
uint16_t rgb888_to_rgb565(uint8_t r, uint8_t g, uint8_t b);
uint16_t rgb888_to_argb1555(uint8_t r, uint8_t g, uint8_t b);
uint8_t rgb888_to_rgb332(uint8_t r, uint8_t g, uint8_t b);
void rgb565_to_rgb(uint16_t rgb565, uint8_t *r, uint8_t *g, uint8_t *b);
void argb1555_to_rgb(uint16_t argb1555, uint8_t *r, uint8_t *g, uint8_t *b);
void rgb332_to_rgb(uint8_t rgb565, uint8_t *r, uint8_t *g, uint8_t *b);
void hsv_to_rgb(float h, float s, float v, uint8_t* r, uint8_t* g, uint8_t* b);

/* Entry point to each example code opertation */
void eve_display_wheel(void);
void eve_display_argb1555(void);
void eve_display_paletted8(void);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* _EVE_EXAMPLE_H */
