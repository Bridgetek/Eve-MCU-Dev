/**
 @file arcs.c
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
#include <stdint.h>

#include <EVE.h>

#include "arcs.h"

// Set pixel precision based on EVE version
#if IS_EVE_API(2,3,4,5)
// 1/8 pixel on EVE2,3,4,5
#define PIX_PRECISION 8
#elif IS_EVE_API(1)
// 1/16 pixel on EVE1
#define PIX_PRECISION 16
#endif

// Options for the 
#define OPT_NONE 0
#define OPT_INDICATOR 1

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

/**
 @brief Function to draw a simple arc gauge, using an optional indicator point.
 */
static void arc_simple_gauge_impl(int16_t x, int16_t y, 
    uint16_t r0, uint16_t r1,
    uint16_t a0, uint16_t a1,
    uint8_t opt,
    uint16_t val
    )
{
    // Draw in reverse if end > start
    int8_t reverse = 0;
    // Calculate arc range and indication value based on input user value
    uint16_t range = (a1 - a0) & 0xffff;
    if (range & 0x8000)
    {
        // Negative range end > start
        uint16_t temp = a1;
        a1 = a0;
        a0 = temp;
        reverse = 1;
    }
    else if (range < 0x10)
    {
        // Special case for end angle equal to start angle
        range = 0xffff;
        reverse = 1;
    }

    // The value to grow the shadow around the indicator point
    uint8_t indicator_size = (r1 - r0) / 4;
    int16_t indicator_x;
    int16_t indicator_y;

    // Ensure the value is within limits
    if (opt & OPT_INDICATOR)
    {
        int16_t arc_value;
        if (reverse)
        {
            arc_value = a1;
        }
        else
        {
            arc_value = a0;
        }
        arc_value += (((uint32_t)val * (uint32_t)range) / 65536);

        // Points for the indicator

        indicator_x = CIRC_X(((r0 + r1) * PIX_PRECISION) / 2, arc_value);
        indicator_y = CIRC_Y(((r0 + r1) * PIX_PRECISION) / 2, arc_value);
    }

    // Calculate the coordinates of the points used to draw the arc
    
    // Points for the starting angle 
    int16_t arc_start_x = CIRC_X(r1 * 2, a0);
    int16_t arc_start_y = CIRC_Y(r1 * 2, a0);
 
    // Points for the finishing angle 
    int16_t arc_end_x = CIRC_X(r1 * 2, a1);
    int16_t arc_end_y = CIRC_Y(r1 * 2, a1);

    // Points for the intermediate stretcher point
    int16_t arc_int_x = CIRC_X(r1 * 2, a0 + 0x4000);
    int16_t arc_int_y = CIRC_Y(r1 * 2, a0 + 0x4000);

    // Points for the rounded ends
    // multiply by PIX_PRECISION here to add these values directly into VERTEX2F calls
    int16_t point_start_x = CIRC_X(((r0 + r1) * PIX_PRECISION) / 2, a0);
    int16_t point_start_y = CIRC_Y(((r0 + r1) * PIX_PRECISION) / 2, a0);
    int16_t point_end_x = CIRC_X(((r0 + r1) * PIX_PRECISION) / 2, a1);
    int16_t point_end_y = CIRC_Y(((r0 + r1) * PIX_PRECISION) / 2, a1);

    // Save current graphics context
    EVE_SAVE_CONTEXT();

    #if IS_EVE_API(2,3,4,5) // PIX_PRECISION = 1/8th if API level is 2,3,4,5, so we need to insert a VERTEX_FORMAT command
    // Set desired vertex format for the example
    EVE_VERTEX_FORMAT(3);
    #endif
    // Stencils preclude using alpha
    EVE_COLOR_A(255);

    EVE_COLOR_MASK(0, 0, 0, 1);
    // Scissor for the size of the arc we wish to draw
    EVE_SCISSOR_SIZE((r1 * 2) + 1, (r1 * 2) + 1);
    EVE_SCISSOR_XY((x - r1), (y - r1));

    EVE_CLEAR_COLOR_A(0);
    EVE_CLEAR_STENCIL(reverse);
    EVE_CLEAR(1, 1, 0);
    EVE_COLOR_MASK(0, 0, 0, 0);
    EVE_STENCIL_MASK(1); 
    EVE_STENCIL_OP(EVE_STENCIL_INCR, EVE_STENCIL_INCR);

    if (range != 0xffff)
    {
        // Stencil cut-out from the circle for the arc
        EVE_BEGIN(EVE_BEGIN_EDGE_STRIP_R);
        EVE_VERTEX2F((x - arc_start_x) * PIX_PRECISION, (y + arc_start_y) * PIX_PRECISION);
        EVE_VERTEX2F(x * PIX_PRECISION, y * PIX_PRECISION);
        EVE_VERTEX2F((x - arc_end_x) * PIX_PRECISION, (y + arc_end_y) * PIX_PRECISION);
        if (range > 0x4000)
        {
            EVE_VERTEX2F((x - arc_int_x) * PIX_PRECISION, (y + arc_int_y) * PIX_PRECISION);
        }
        EVE_VERTEX2F((x - arc_start_x) * PIX_PRECISION, (y + arc_start_y) * PIX_PRECISION);
    }

    EVE_COLOR_MASK(0, 0, 0, 1);
    EVE_STENCIL_MASK(0);
    EVE_STENCIL_FUNC(EVE_TEST_EQUAL, 1, 1);

    EVE_BEGIN(EVE_BEGIN_POINTS);
    // Add outer circle to alpha buffer
    EVE_BLEND_FUNC(EVE_BLEND_ONE, EVE_BLEND_ONE_MINUS_SRC_ALPHA);
    EVE_POINT_SIZE(r1 * 16);
    EVE_VERTEX2F((x * PIX_PRECISION), (y * PIX_PRECISION));
 
    // Remove inner circle from alpha buffer
    EVE_BLEND_FUNC(EVE_BLEND_ZERO, EVE_BLEND_ONE_MINUS_SRC_ALPHA);
    EVE_POINT_SIZE(r0 * 16);
    EVE_VERTEX2F((x * PIX_PRECISION), (y * PIX_PRECISION));
    
    // add points to the end of the arc (only if requried)
    if(range != 0xffff)
    {
        EVE_STENCIL_FUNC(EVE_TEST_ALWAYS, 1, 1);
        EVE_POINT_SIZE(((r1 - r0) * 16) / 2 );
        EVE_BLEND_FUNC(EVE_BLEND_ONE, EVE_BLEND_ONE_MINUS_SRC_ALPHA);
        // Start point
        EVE_VERTEX2F((x * PIX_PRECISION) - point_start_x, (y * PIX_PRECISION) + point_start_y);
        // End point
        EVE_VERTEX2F((x * PIX_PRECISION) - point_end_x, (y * PIX_PRECISION) + point_end_y);
    }

    // Draw the indicator
    if (opt & OPT_INDICATOR)
    {
        // Remove outer of indicator from the alpha buffer
        EVE_BLEND_FUNC(EVE_BLEND_ZERO, EVE_BLEND_ONE_MINUS_SRC_ALPHA);
        EVE_POINT_SIZE((((r1 - r0) / 2) + indicator_size) * 16);
    
        // Draw point based on current input value for the indicator
        EVE_VERTEX2F((x * PIX_PRECISION) - indicator_x, (y * PIX_PRECISION) + indicator_y);
    
        // Add inner of the indicator to alpha buffer
        EVE_BLEND_FUNC(EVE_BLEND_ONE, EVE_BLEND_ONE_MINUS_SRC_ALPHA);
        EVE_POINT_SIZE(((r1 - r0) / 2) * 16); 
    
        // Draw point based on current input value for the indicator
        EVE_VERTEX2F((x * PIX_PRECISION) - indicator_x, (y * PIX_PRECISION) + indicator_y);
    }

    // Draw a circle which will fill the arc with the colour set in the graphics context before the funciton call
    EVE_COLOR_MASK(1, 1, 1, 0);
    EVE_BLEND_FUNC(EVE_BLEND_DST_ALPHA, EVE_BLEND_ONE_MINUS_DST_ALPHA);
    EVE_POINT_SIZE(r1 * 16);
    EVE_VERTEX2F((x * PIX_PRECISION), (y * PIX_PRECISION));
 
    // End drawing
    //not needed EVE_END();

    // Restore previous graphics context
    EVE_RESTORE_CONTEXT();
}

void arc_simple(int16_t x, int16_t y, 
    uint16_t r0, uint16_t r1,
    uint16_t a0, uint16_t a1
)
{
    arc_simple_gauge_impl(x, y, r0, r1, a0, a1, OPT_NONE, 0);
}

void arc_simple_gauge(int16_t x, int16_t y, 
    uint16_t r0, uint16_t r1,
    uint16_t a0, uint16_t a1,
    uint16_t val
)
{
    arc_simple_gauge_impl(x, y, r0, r1, a0, a1, OPT_INDICATOR, val);
}
