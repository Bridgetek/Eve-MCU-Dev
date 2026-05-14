/**
 @file compass_bulkhead.c
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
#include <stdlib.h>

#include <EVE.h>

#include "maths/trig_furman.h"

static inline void draw_vertex(int16_t x, int16_t y)
{
#if IS_EVE_API(1)
    EVE_VERTEX2F(x * 16, y * 16);
#else
    EVE_VERTEX2F(x, y);
#endif
}

static void draw_n(int16_t x, int16_t y, int16_t offset, int16_t width, int16_t height)
{
    int16_t dx1, dy1;

    EVE_BEGIN(EVE_BEGIN_LINE_STRIP);
    width = (width * 3) / 4;
    // Start of the letter N (bottom left)
    dx1 = x - offset - width;
    dy1 = y + (height / 8) + height;
    draw_vertex(dx1, dy1);
    // (top left)
    dx1 = x - offset - width;
    dy1 = y + (height / 8);
    draw_vertex(dx1, dy1);
    // (bottom right)
    dx1 = x - offset + width;
    dy1 = y + (height / 8) + height;
    draw_vertex(dx1, dy1);
    // (top right)
    dx1 = x - offset + width;
    dy1 = y + (height / 8);
    draw_vertex(dx1, dy1);
}

static void draw_s(int16_t x, int16_t y, int16_t offset, int16_t width, int16_t height)
{
    int16_t dx1, dy1;

    EVE_BEGIN(EVE_BEGIN_LINE_STRIP);
    width = (width * 3) / 4;
    // Start of the letter S (top right)
    dx1 = x - offset + width;
    dy1 = y + (height / 8) + ((height * 1) / 8);
    draw_vertex(dx1, dy1);
    dx1 = x - offset + (width / 2);
    dy1 = y + (height / 8) + ((height * 0) / 8);
    draw_vertex(dx1, dy1);
    dx1 = x - offset - (width / 2);
    dy1 = y + (height / 8) + ((height * 0) / 8);
    draw_vertex(dx1, dy1);
    dx1 = x - offset - width;
    dy1 = y + (height / 8) + ((height * 1) / 8);
    draw_vertex(dx1, dy1);
    dx1 = x - offset - width;
    dy1 = y + (height / 8) + ((height * 3) / 8);
    draw_vertex(dx1, dy1);
    dx1 = x - offset - (width / 2);
    dy1 = y + (height / 8) + ((height * 4) / 8);
    draw_vertex(dx1, dy1);
    dx1 = x - offset + (width / 2);
    dy1 = y + (height / 8) + ((height * 4) / 8);
    draw_vertex(dx1, dy1);
    dx1 = x - offset + width;
    dy1 = y + (height / 8) + ((height * 5) / 8);
    draw_vertex(dx1, dy1);
    dx1 = x - offset + width;
    dy1 = y + (height / 8) + ((height * 7) / 8);
    draw_vertex(dx1, dy1);
    dx1 = x - offset + (width / 2);
    dy1 = y + (height / 8) + ((height * 8) / 8);
    draw_vertex(dx1, dy1);
    dx1 = x - offset - (width / 2);
    dy1 = y + (height / 8) + ((height * 8) / 8);
    draw_vertex(dx1, dy1);
    dx1 = x - offset - width;
    dy1 = y + (height / 8) + ((height * 7) / 8);
    draw_vertex(dx1, dy1);
}

static void draw_w(int16_t x, int16_t y, int16_t offset, int16_t width, int16_t height)
{
    int16_t dx1, dy1;

    EVE_BEGIN(EVE_BEGIN_LINE_STRIP);
    // Start of the letter W (top right)
    dx1 = x - offset + width;
    dy1 = y + (height / 8) + ((height * 0) / 8);
    draw_vertex(dx1, dy1);
    dx1 = x - offset + (width / 2);
    dy1 = y + (height / 8) + ((height * 8) / 8);
    draw_vertex(dx1, dy1);
    dx1 = x - offset;
    dy1 = y + (height / 8) + ((height * 0) / 8);
    draw_vertex(dx1, dy1);
    dx1 = x - offset - (width / 2);
    dy1 = y + (height / 8) + ((height * 8) / 8);
    draw_vertex(dx1, dy1);
    dx1 = x - offset - width;
    dy1 = y + (height / 8) + ((height * 0) / 8);
    draw_vertex(dx1, dy1);
}

static void draw_e(int16_t x, int16_t y, int16_t offset, int16_t width, int16_t height)
{
    int16_t dx1, dy1;

    EVE_BEGIN(EVE_BEGIN_LINE_STRIP);
    width = (width * 3) / 4;
    // Start of the letter E (top right)
    dx1 = x - offset + width;
    dy1 = y + (height / 8) + ((height * 0) / 8);
    draw_vertex(dx1, dy1);
    dx1 = x - offset - width;
    dy1 = y + (height / 8) + ((height * 0) / 8);
    draw_vertex(dx1, dy1);
    dx1 = x - offset - width;
    dy1 = y + (height / 8) + ((height * 4) / 8);
    draw_vertex(dx1, dy1);
    dx1 = x - offset + width;
    dy1 = y + (height / 8) + ((height * 4) / 8);
    draw_vertex(dx1, dy1);
    dx1 = x - offset - width;
    dy1 = y + (height / 8) + ((height * 4) / 8);
    draw_vertex(dx1, dy1);
    dx1 = x - offset - width;
    dy1 = y + (height / 8) + ((height * 8) / 8);
    draw_vertex(dx1, dy1);
    dx1 = x - offset + width;
    dy1 = y + (height / 8) + ((height * 8) / 8);
    draw_vertex(dx1, dy1);
}

typedef void draw_char(int16_t x, int16_t y, int16_t offset, int16_t width, int16_t height);

static void draw_compass_point(int16_t x, int16_t y, int16_t radius, int16_t line_width, int16_t text_size, int heading, int degrees, draw_char *l1, draw_char *l2)
{
    int16_t width;
    int16_t offset;

    // Show only when heading is +/-90 from the degrees value
    if ((uint16_t)((DEG2FURMAN(degrees + 90) - heading) & 0xffff) < DEG2FURMAN(180))
    {
        // Get the scaled width of the lines for the letter
        // This cannot be negative (even though we use a sine)
        width = CIRC_X(line_width, DEG2FURMAN(degrees + 90) - heading);
        if (width < 0) width = -width;
        EVE_LINE_WIDTH(width);

        // Draw a letter N
        offset = ~CIRC_X(radius, DEG2FURMAN(degrees) - heading);
        width = CIRC_X(text_size, DEG2FURMAN(degrees + 90) - heading);
        if (width < 0) width = -width;
        width /= 3;

        if (l2)
        {
            l1(x, y, offset + width, width, text_size);
            l2(x, y, offset - width, width, text_size);
        }
        else
        {
            l1(x, y, offset, width, text_size);
        }
    }
}

/*
 * x,y - top left of widget in pixels
 * radius - radius of widget
 * heading - compass heading in degrees
*/
void compass_bulkhead(int32_t x, int32_t y, uint16_t radius, int16_t degrees)
{
    int heading = DEG2FURMAN(degrees);
    // Dimensions of the widget are detemined by the radius
    int16_t radius_outer = radius;
    int16_t radius_b1 = (radius_outer * 39) / 40;
    int16_t radius_b2 = (radius_outer * 37) / 40;
    int16_t radius_inner = (radius_outer * 36) / 40;
    // Inner of degree graduations
    int16_t height_heading = (radius_outer * 10) / 40;
    // Centre of degree graduations
    int16_t radius_heading_centre = (radius_inner + height_heading) / 2;
    // Degree graduation line widths
    int16_t reference_bold = radius_outer * 2 / 10;
    int16_t reference_narrow = radius_outer * 1 / 10;
    // Radius of N pointer arrow
    int16_t radius_pointer_arrow = (height_heading * 35) / 40;
    // Radius of N pointer arrow head
    int16_t radius_pointer_head = (height_heading * 25) / 40;
    // Width of N and S pointer arrow
    int16_t radius_pointer_width = (height_heading * 8) / 40;
    int16_t radius_text_height = (height_heading * 5) / 40;
    // Inner of degree graduations
    int16_t text_size = (radius_outer * 8) / 40;
    
    // Bezel colours
    uint32_t bezel_col = 0x505050;
    uint32_t bezel_col_bright = 0xaaaaaa;
    uint32_t bezel_col_dark = 0x333333;
    // Reference line colour
    uint32_t reference_col = 0xffffff;
    // Overlay reference colours
    uint32_t ovl_reference_col = 0xffaa00;
    // North N colour
    uint32_t north_col = 0xFF0000;
    int deg;

    // Draw bezel
    EVE_SAVE_CONTEXT();
#if !IS_EVE_API(1)
    EVE_VERTEX_FORMAT(0);
#endif
    EVE_CLEAR(0,1,0);
    EVE_COLOR(bezel_col);
    // Draw the circles to make the bezel in three levels in the stencil buffer
    EVE_STENCIL_OP(EVE_STENCIL_KEEP, EVE_STENCIL_INCR);	// Set the stencil to increment
    EVE_BEGIN(EVE_BEGIN_POINTS);
    EVE_POINT_SIZE(radius_outer * 16);
    draw_vertex(x, y); // stencil 1
    EVE_POINT_SIZE(radius_b1 * 16);
    draw_vertex(x, y); // stencil 2
    EVE_POINT_SIZE(radius_b2 * 16);
    draw_vertex(x, y); // stencil 3
    EVE_POINT_SIZE(radius_inner * 16);
    draw_vertex(x, y); // stencil 4
    EVE_END();
    
    EVE_STENCIL_OP(EVE_STENCIL_KEEP, EVE_STENCIL_KEEP); // Stop the stencil INCR
    // Gradient (light at top) for outer of bezel
    EVE_STENCIL_FUNC(EVE_TEST_EQUAL, 1, 255);
    EVE_CMD_GRADIENT(x - radius_outer, y + radius_outer, bezel_col_dark, x - radius_outer, y - radius_outer, bezel_col_bright);
    // Flat colour for centre of bezel
    EVE_STENCIL_FUNC(EVE_TEST_EQUAL, 2, 255);
    EVE_POINT_SIZE(radius_outer * 16);
    draw_vertex(x, y);
    // Gradient (dark at top) for inner of bezel
    EVE_STENCIL_FUNC(EVE_TEST_EQUAL, 3, 255);
    EVE_CMD_GRADIENT(x - radius_outer, y + radius_outer, bezel_col_bright, x - radius_outer, y - radius_outer, bezel_col_dark);
    EVE_RESTORE_CONTEXT();

    // Compass Degree Markings
    EVE_SAVE_CONTEXT();
#if !IS_EVE_API(1)
    EVE_VERTEX_FORMAT(0);
#endif
    EVE_CLEAR_STENCIL(0);
    EVE_CLEAR(0,1,0);
    EVE_COLOR_MASK(0,0,0,0);
    EVE_COLOR(bezel_col);
    EVE_STENCIL_OP(EVE_STENCIL_KEEP, EVE_STENCIL_INCR);
    // Stencils for degree markings
    EVE_BEGIN(EVE_BEGIN_POINTS);
    EVE_POINT_SIZE(radius_inner * 16);
    draw_vertex(x, y); // stencil 1
    EVE_BEGIN(EVE_BEGIN_LINES);
    EVE_LINE_WIDTH(height_heading * 16);
    draw_vertex(x - radius_inner, y); // stencil 2
    draw_vertex(x + radius_inner, y); // stencil 2
    EVE_COLOR_MASK(1,1,1,1);

    EVE_STENCIL_OP(EVE_STENCIL_KEEP, EVE_STENCIL_KEEP);
    // Flat fill for centre
    EVE_STENCIL_FUNC(EVE_TEST_EQUAL, 1, 255);
    EVE_BEGIN(EVE_BEGIN_POINTS);
    EVE_POINT_SIZE(radius_inner * 16);
    draw_vertex(x, y); 
    // Gradient (dark at bottom) for heading gradients rotator
    EVE_STENCIL_FUNC(EVE_TEST_EQUAL, 2, 255);
    EVE_COLOR(ovl_reference_col);
    EVE_CMD_GRADIENT(x - radius_inner, y - height_heading, bezel_col_dark, x + radius_inner, y + height_heading, bezel_col_bright);

    // Draw gradient lines
    EVE_BEGIN(EVE_BEGIN_LINES);
    EVE_COLOR(reference_col);
    
    // Narrow degree gradients at 5 degree intervals
    EVE_STENCIL_FUNC(EVE_TEST_GEQUAL, 2, 255);
    EVE_LINE_WIDTH(reference_narrow);
    for (deg = -180; deg < +180; deg += 5)
    {
        if ((deg % 30) == 0) continue;
        if (((DEG2FURMAN(deg) + heading + 0x4000) & 0xffff) < 0x8000)
        {
            int16_t dx = x - CIRC_X(radius_inner, DEG2FURMAN(deg) + heading);
            draw_vertex(dx, y - height_heading);
            draw_vertex(dx, y - (height_heading / 2));
        }
    }

    // Bold degree gradients at 30 degree intervals
    EVE_STENCIL_FUNC(EVE_TEST_EQUAL, 2, 255);
    EVE_LINE_WIDTH(reference_bold);
    for (deg = -180; deg < +180; deg += 30)
    {
        if (((DEG2FURMAN(deg) + heading + 0x4000) & 0xffff) < 0x8000)
        {
            int16_t dx = x - CIRC_X(radius_inner, DEG2FURMAN(deg) + heading);
            draw_vertex(dx, y - height_heading);
            draw_vertex(dx, y);
        }
    }
    
    // Draw with highlighted colour
    EVE_COLOR(north_col);
    // Letter visible at 0 degrees 
    draw_compass_point(x, y, radius_inner, reference_bold, text_size, heading, 0, draw_n, NULL);

    // Draw with reference colour
    EVE_COLOR(ovl_reference_col);
    // Letters visible at 45 degrees 
    draw_compass_point(x, y, radius_inner, reference_bold, text_size, heading, 45, draw_n, draw_w);
    // Letter visible at 90 degrees 
    draw_compass_point(x, y, radius_inner, reference_bold, text_size, heading, 90, draw_w, NULL);
    // Letters visible at 135 degrees 
    draw_compass_point(x, y, radius_inner, reference_bold, text_size, heading, 135, draw_s, draw_w);
    // Letter visible at 180 degrees 
    draw_compass_point(x, y, radius_inner, reference_bold, text_size, heading, 180, draw_s, NULL);
    // Letters visible at 225 degrees 
    draw_compass_point(x, y, radius_inner, reference_bold, text_size, heading, 225, draw_s, draw_e);
    // Letter visible at 270 degrees 
    draw_compass_point(x, y, radius_inner, reference_bold, text_size, heading, 270, draw_e, NULL);
    // Letters visible at 315 degrees 
    draw_compass_point(x, y, radius_inner, reference_bold, text_size, heading, 315, draw_n, draw_e);

    // Fixed reference point for heading
    EVE_STENCIL_FUNC(EVE_TEST_GEQUAL, 1, 255);
    EVE_BEGIN(EVE_BEGIN_LINES);
    EVE_LINE_WIDTH(reference_bold);
    EVE_COLOR(ovl_reference_col);
    draw_vertex(x, y - height_heading);
    draw_vertex(x, y + height_heading);

    EVE_END();
    EVE_STENCIL_FUNC(EVE_TEST_ALWAYS, 0, 255);
    EVE_RESTORE_CONTEXT();

}
