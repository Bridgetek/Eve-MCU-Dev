/**
 @file compass_binnacle.c
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
#include "compass_controls.h"

static inline void draw_vertex(int16_t x, int16_t y)
{
#if IS_EVE_API(1)
    EVE_VERTEX2F(x * 16, y * 16);
#else
    EVE_VERTEX2F(x, y);
#endif
}

/*
 * x,y - top left of widget in pixels
 * radius - radius of widget
 * heading - compass heading in degrees
*/
void compass_binnacle(int32_t x, int32_t y, uint16_t radius, uint16_t options, int16_t degrees)
{
    int heading = DEG2FURMAN(degrees);

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
    int16_t dx, dy;
    int16_t dx1, dy1, dx2, dy2;
    int deg;

    if (options & OPT_COMPASS_BEZEL)
    {
        // Bezel border size
        int16_t border = (radius * 1) / 40;
        // Dimensions of the widget are detemined by the radius
        int16_t radius_outer = radius;
        int16_t radius_b1 = (radius_outer - (border));
        int16_t radius_b2 = (radius_outer - (2 * border));
        int16_t radius_inner = (radius_outer - (3 * border));

        // Draw bezel
        EVE_SAVE_CONTEXT();
#if !IS_EVE_API(1)
        EVE_VERTEX_FORMAT(0);
#endif
        EVE_CLEAR_STENCIL(0);
        EVE_CLEAR(0,1,0);
        EVE_COLOR_MASK(0,0,0,0);
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
        EVE_COLOR_MASK(1,1,1,1);
        EVE_END();
        
        EVE_STENCIL_OP(EVE_STENCIL_KEEP, EVE_STENCIL_KEEP); // Stop the stencil INCR
        // Gradient (light at top) for outer of bezel
        EVE_STENCIL_FUNC(EVE_TEST_EQUAL, 1, 255);
        EVE_CMD_GRADIENT(x - radius_outer, y + radius_outer, bezel_col_dark, x - radius_outer, y - radius_outer, bezel_col_bright);
        // Flat colour for centre of bezel
        EVE_BEGIN(EVE_BEGIN_POINTS);
        EVE_STENCIL_FUNC(EVE_TEST_EQUAL, 2, 255);
        EVE_POINT_SIZE(radius_outer * 16);
        draw_vertex(x, y);
        // Gradient (dark at top) for inner of bezel
        EVE_STENCIL_FUNC(EVE_TEST_EQUAL, 3, 255);
        EVE_CMD_GRADIENT(x - radius_outer, y + radius_outer, bezel_col_bright, x - radius_outer, y - radius_outer, bezel_col_dark);
        EVE_RESTORE_CONTEXT();

        radius = radius_inner;
    }

    // Inner of degree graduations
    int16_t radius_heading_inner = (radius * 30) / 40;
    // Centre of degree graduations
    int16_t radius_heading_centre = (radius + radius_heading_inner) / 2;
    // Degree graduation line widths
    int16_t reference_bold = radius * 2 / 10;
    int16_t reference_narrow = radius * 1 / 10;
    // Radius of N pointer arrow
    int16_t radius_pointer_arrow = (radius_heading_inner * 35) / 40;
    // Radius of N pointer arrow head
    int16_t radius_pointer_head = (radius_heading_inner * 25) / 40;
    // Width of N and S pointer arrow
    int16_t radius_pointer_width = (radius_heading_inner * 8) / 40;
    int16_t radius_text_height = (radius_heading_inner * 5) / 40;
    
    // Compass Degree Markings
    EVE_SAVE_CONTEXT();
#if !IS_EVE_API(1)
    EVE_VERTEX_FORMAT(0);
#endif
    EVE_CLEAR_STENCIL(0);
    EVE_CLEAR(0,1,0);
    EVE_COLOR_MASK(0,0,0,0);
    EVE_STENCIL_OP(EVE_STENCIL_KEEP, EVE_STENCIL_INCR);

    // Stencils for degree markings
    EVE_BEGIN(EVE_BEGIN_POINTS);
    EVE_POINT_SIZE(radius * 16);
    draw_vertex(x, y); // stencil 1
    EVE_POINT_SIZE(radius_heading_centre * 16);
    draw_vertex(x, y); // stencil 2
    EVE_POINT_SIZE(radius_heading_inner * 16);
    draw_vertex(x, y); // stencil 3

    EVE_COLOR_MASK(1,1,1,1);
    EVE_STENCIL_OP(EVE_STENCIL_KEEP, EVE_STENCIL_KEEP);

    if (!(options & OPT_COMPASS_TRANSPARENT))
    {
        // Gradient (dark at bottom) for heading gradients
        EVE_STENCIL_FUNC(EVE_TEST_GEQUAL, 1, 255);
        EVE_CMD_GRADIENT(x - radius, y + radius, bezel_col_dark, x - radius, y - radius, bezel_col_bright);
        // Flat fill for centre
        EVE_COLOR(bezel_col);
        EVE_STENCIL_FUNC(EVE_TEST_GEQUAL, 2, 255);
        EVE_BEGIN(EVE_BEGIN_POINTS);
        EVE_POINT_SIZE(radius * 16);
        draw_vertex(x, y); 
    }

    // Draw gradient lines
    EVE_BEGIN(EVE_BEGIN_LINES);
    EVE_COLOR(reference_col);
    
    // Narrow degree gradients at 5 degree intervals
    EVE_STENCIL_FUNC(EVE_TEST_LEQUAL, 1, 255);
    EVE_LINE_WIDTH(reference_narrow);
    for (deg = 5; deg < 360; deg += 5)
    {
        if ((deg % 30) == 0) continue;
        dx = x + CIRC_X(radius, DEG2FURMAN(deg) + heading);
        dy = y - CIRC_Y(radius, DEG2FURMAN(deg) + heading);
        draw_vertex(dx, dy);
        draw_vertex(x, y);
    }

    // Bold degree gradients at 30 degree intervals
    EVE_STENCIL_FUNC(EVE_TEST_LEQUAL, 2, 255);
    EVE_LINE_WIDTH(reference_bold);
    for (deg = 0; deg < 360; deg += 30)
    {
        dx = x + CIRC_X(radius, DEG2FURMAN(deg) + heading);
        dy = y - CIRC_Y(radius, DEG2FURMAN(deg) + heading);
        draw_vertex(dx, dy);
        draw_vertex(x, y);
    }
    EVE_STENCIL_FUNC(EVE_TEST_ALWAYS, 0, 255);
    EVE_RESTORE_CONTEXT();

    // Reference markings
    EVE_SAVE_CONTEXT();
#if !IS_EVE_API(1)
    EVE_VERTEX_FORMAT(0);
#endif
    // North pointing arrow
    EVE_BEGIN(EVE_BEGIN_LINE_STRIP);
    EVE_LINE_WIDTH(reference_bold);
    EVE_COLOR(reference_col);
    dx = CIRC_X(radius_pointer_width, heading + DEG2FURMAN(90));
    dy = CIRC_Y(radius_pointer_width, heading + DEG2FURMAN(90));
    // Start of the arrow
    dx1 = x - CIRC_X(radius_pointer_arrow, heading) + dx;
    dy1 = y + CIRC_Y(radius_pointer_arrow, heading) - dy;
    draw_vertex(dx1, dy1);
    // Inner point of arrow head
    dx1 = x + CIRC_X(radius_pointer_head, heading) + dx;
    dy1 = y - CIRC_Y(radius_pointer_head, heading) - dy;
    draw_vertex(dx1, dy1);
    // Outer point of arrow head
    dx1 = x + CIRC_X(radius_pointer_head, heading) + ((dx * 4) / 3);
    dy1 = y - CIRC_Y(radius_pointer_head, heading) - ((dy * 4) / 3);
    draw_vertex(dx1, dy1);
    // Tip of the arrow
    dx1 = x + CIRC_X(radius_pointer_arrow, heading);
    dy1 = y - CIRC_Y(radius_pointer_arrow, heading);
    draw_vertex(dx1, dy1);
    // Outer point of arrow head
    dx1 = x + CIRC_X(radius_pointer_head, heading) - ((dx * 4) / 3);
    dy1 = y - CIRC_Y(radius_pointer_head, heading) + ((dy * 4) / 3);
    draw_vertex(dx1, dy1);
    // Inner point of arrow head
    dx1 = x + CIRC_X(radius_pointer_head, heading) - dx;
    dy1 = y - CIRC_Y(radius_pointer_head, heading) + dy;
    draw_vertex(dx1, dy1);
    // End of the arrow
    dx1 = x - CIRC_X(radius_pointer_arrow, heading) - dx;
    dy1 = y + CIRC_Y(radius_pointer_arrow, heading) + dy;
    draw_vertex(dx1, dy1);
    
    // Other markings
    EVE_BEGIN(EVE_BEGIN_LINES);
    // E/W bars
    dx1 = CIRC_X(radius_pointer_head + radius_text_height, heading + DEG2FURMAN(90));
    dy1 = CIRC_Y(radius_pointer_head + radius_text_height, heading + DEG2FURMAN(90));
    dx2 = CIRC_X(radius_pointer_arrow, heading + DEG2FURMAN(90));
    dy2 = CIRC_Y(radius_pointer_arrow, heading + DEG2FURMAN(90));
    draw_vertex(x + dx1, y - dy1);
    draw_vertex(x + dx2, y - dy2);
    draw_vertex(x - dx1, y + dy1);
    draw_vertex(x - dx2, y + dy2);
    // 45 degree bars
    EVE_LINE_WIDTH(reference_narrow);
    dx1 = CIRC_X((radius_pointer_arrow * 2) / 4, heading + DEG2FURMAN(45));
    dy1 = CIRC_Y((radius_pointer_arrow * 2) / 4, heading + DEG2FURMAN(45));
    dx2 = CIRC_X((radius_pointer_arrow * 8) / 8, heading + DEG2FURMAN(45));
    dy2 = CIRC_Y((radius_pointer_arrow * 8) / 8, heading + DEG2FURMAN(45));
    draw_vertex(x + dx1, y - dy1);
    draw_vertex(x + dx2, y - dy2);
    draw_vertex(x - dx1, y + dy1);
    draw_vertex(x - dx2, y + dy2);
    dx1 = CIRC_X((radius_pointer_arrow * 2) / 4, heading - DEG2FURMAN(45));
    dy1 = CIRC_Y((radius_pointer_arrow * 2) / 4, heading - DEG2FURMAN(45));
    dx2 = CIRC_X((radius_pointer_arrow * 8) / 8, heading - DEG2FURMAN(45));
    dy2 = CIRC_Y((radius_pointer_arrow * 8) / 8, heading - DEG2FURMAN(45));
    draw_vertex(x + dx1, y - dy1);
    draw_vertex(x + dx2, y - dy2);
    draw_vertex(x - dx1, y + dy1);
    draw_vertex(x - dx2, y + dy2);

    // Draw with highlighted colour
    EVE_LINE_WIDTH(reference_bold);
    EVE_COLOR(north_col);

    // Draw a letter N
    EVE_BEGIN(EVE_BEGIN_LINE_STRIP);
    dx = CIRC_X(radius_text_height / 2, heading + DEG2FURMAN(90));
    dy = CIRC_Y(radius_text_height / 2, heading + DEG2FURMAN(90));
    // Start of the letter N (bottom left)
    dx1 = x + CIRC_X(radius_pointer_head - radius_text_height, heading) - dx;
    dy1 = y - CIRC_Y(radius_pointer_head - radius_text_height, heading) + dy;
    draw_vertex(dx1, dy1);
    // (top left)
    dx1 = x + CIRC_X(radius_pointer_head + radius_text_height, heading) - dx;
    dy1 = y - CIRC_Y(radius_pointer_head + radius_text_height, heading) + dy;
    draw_vertex(dx1, dy1);
    // (bottom right)
    dx1 = x + CIRC_X(radius_pointer_head - radius_text_height, heading) + dx;
    dy1 = y - CIRC_Y(radius_pointer_head - radius_text_height, heading) - dy;
    draw_vertex(dx1, dy1);
    // (top right)
    dx1 = x + CIRC_X(radius_pointer_head + radius_text_height, heading) + dx;
    dy1 = y - CIRC_Y(radius_pointer_head + radius_text_height, heading) - dy;
    draw_vertex(dx1, dy1);
    
    // Draw with reference colour
    EVE_LINE_WIDTH(reference_bold);
    EVE_COLOR(ovl_reference_col);

    // Draw a letter S at 180 degrees
    EVE_BEGIN(EVE_BEGIN_LINE_STRIP);
    dx = CIRC_X(radius_text_height / 2, heading + DEG2FURMAN(90));
    dy = CIRC_Y(radius_text_height / 2, heading + DEG2FURMAN(90));
    // Start of the letter S (bottom left)
    dx1 = x + CIRC_X(radius_pointer_arrow - ((radius_text_height * 1) / 4), heading + DEG2FURMAN(180)) - dx;
    dy1 = y - CIRC_Y(radius_pointer_arrow - ((radius_text_height * 1) / 4), heading + DEG2FURMAN(180)) + dy;
    draw_vertex(dx1, dy1);
    dx1 = x + CIRC_X(radius_pointer_arrow, heading + DEG2FURMAN(180)) - (dx / 2);
    dy1 = y - CIRC_Y(radius_pointer_arrow, heading + DEG2FURMAN(180)) + (dy / 2);
    draw_vertex(dx1, dy1);
    dx1 = x + CIRC_X(radius_pointer_arrow, heading + DEG2FURMAN(180)) + (dx / 2);
    dy1 = y - CIRC_Y(radius_pointer_arrow, heading + DEG2FURMAN(180)) - (dy / 2);
    draw_vertex(dx1, dy1);
    dx1 = x + CIRC_X(radius_pointer_arrow - ((radius_text_height * 1) / 4), heading + DEG2FURMAN(180)) + dx;
    dy1 = y - CIRC_Y(radius_pointer_arrow - ((radius_text_height * 1) / 4), heading + DEG2FURMAN(180)) - dy;
    draw_vertex(dx1, dy1);
    dx1 = x + CIRC_X(radius_pointer_arrow - ((radius_text_height * 3) / 4), heading + DEG2FURMAN(180)) + dx;
    dy1 = y - CIRC_Y(radius_pointer_arrow - ((radius_text_height * 3) / 4), heading + DEG2FURMAN(180)) - dy;
    draw_vertex(dx1, dy1);
    dx1 = x + CIRC_X(radius_pointer_arrow - ((radius_text_height * 4) / 4), heading + DEG2FURMAN(180)) + (dx / 2);
    dy1 = y - CIRC_Y(radius_pointer_arrow - ((radius_text_height * 4) / 4), heading + DEG2FURMAN(180)) - (dy / 2);
    draw_vertex(dx1, dy1);
    dx1 = x + CIRC_X(radius_pointer_arrow - ((radius_text_height * 4) / 4), heading + DEG2FURMAN(180)) - (dx / 2);
    dy1 = y - CIRC_Y(radius_pointer_arrow - ((radius_text_height * 4) / 4), heading + DEG2FURMAN(180)) + (dy / 2);
    draw_vertex(dx1, dy1);
    dx1 = x + CIRC_X(radius_pointer_arrow - ((radius_text_height * 5) / 4), heading + DEG2FURMAN(180)) - dx;
    dy1 = y - CIRC_Y(radius_pointer_arrow - ((radius_text_height * 5) / 4), heading + DEG2FURMAN(180)) + dy;
    draw_vertex(dx1, dy1);
    dx1 = x + CIRC_X(radius_pointer_arrow - ((radius_text_height * 7) / 4), heading + DEG2FURMAN(180)) - dx;
    dy1 = y - CIRC_Y(radius_pointer_arrow - ((radius_text_height * 7) / 4), heading + DEG2FURMAN(180)) + dy;
    draw_vertex(dx1, dy1);
    dx1 = x + CIRC_X(radius_pointer_arrow - ((radius_text_height * 8) / 4), heading + DEG2FURMAN(180)) - (dx / 2);
    dy1 = y - CIRC_Y(radius_pointer_arrow - ((radius_text_height * 8) / 4), heading + DEG2FURMAN(180)) + (dy / 2);
    draw_vertex(dx1, dy1);
    dx1 = x + CIRC_X(radius_pointer_arrow - ((radius_text_height * 8) / 4), heading + DEG2FURMAN(180)) + (dx / 2);
    dy1 = y - CIRC_Y(radius_pointer_arrow - ((radius_text_height * 8) / 4), heading + DEG2FURMAN(180)) - (dy / 2);
    draw_vertex(dx1, dy1);
    dx1 = x + CIRC_X(radius_pointer_arrow - ((radius_text_height * 7) / 4), heading + DEG2FURMAN(180)) + dx;
    dy1 = y - CIRC_Y(radius_pointer_arrow - ((radius_text_height * 7) / 4), heading + DEG2FURMAN(180)) - dy;
    draw_vertex(dx1, dy1);

    // Draw a letter E at 90 degrees
    EVE_BEGIN(EVE_BEGIN_LINE_STRIP);
    dx = CIRC_X(radius_text_height / 2, heading + DEG2FURMAN(180));
    dy = CIRC_Y(radius_text_height / 2, heading + DEG2FURMAN(180));
    // Start of the letter E (top right)
    dx1 = x + CIRC_X(radius_pointer_head + (radius_text_height / 2), heading + DEG2FURMAN(90)) + dx;
    dy1 = y - CIRC_Y(radius_pointer_head + (radius_text_height / 2), heading + DEG2FURMAN(90)) - dy;
    draw_vertex(dx1, dy1);
    dx1 = x + CIRC_X(radius_pointer_head + (radius_text_height / 2), heading + DEG2FURMAN(90)) - dx;
    dy1 = y - CIRC_Y(radius_pointer_head + (radius_text_height / 2), heading + DEG2FURMAN(90)) + dy;
    draw_vertex(dx1, dy1);
    dx1 = x + CIRC_X(radius_pointer_head - (radius_text_height / 4), heading + DEG2FURMAN(90)) - dx;
    dy1 = y - CIRC_Y(radius_pointer_head - (radius_text_height / 4), heading + DEG2FURMAN(90)) + dy;
    draw_vertex(dx1, dy1);
    dx1 = x + CIRC_X(radius_pointer_head - (radius_text_height / 4), heading + DEG2FURMAN(90)) + dx;
    dy1 = y - CIRC_Y(radius_pointer_head - (radius_text_height / 4), heading + DEG2FURMAN(90)) - dy;
    draw_vertex(dx1, dy1);
    dx1 = x + CIRC_X(radius_pointer_head - (radius_text_height / 4), heading + DEG2FURMAN(90)) - dx;
    dy1 = y - CIRC_Y(radius_pointer_head - (radius_text_height / 4), heading + DEG2FURMAN(90)) + dy;
    draw_vertex(dx1, dy1);
    dx1 = x + CIRC_X(radius_pointer_head - radius_text_height, heading + DEG2FURMAN(90)) - dx;
    dy1 = y - CIRC_Y(radius_pointer_head - radius_text_height, heading + DEG2FURMAN(90)) + dy;
    draw_vertex(dx1, dy1);
    dx1 = x + CIRC_X(radius_pointer_head - radius_text_height, heading + DEG2FURMAN(90)) + dx;
    dy1 = y - CIRC_Y(radius_pointer_head - radius_text_height, heading + DEG2FURMAN(90)) - dy;
    draw_vertex(dx1, dy1);

    // Draw a letter W at 270 degrees
    EVE_BEGIN(EVE_BEGIN_LINE_STRIP);
    dx = CIRC_X(radius_text_height / 2, heading);
    dy = CIRC_Y(radius_text_height / 2, heading);
    // Start of the letter W (top left)
    dx1 = x + CIRC_X(radius_pointer_head + (radius_text_height / 2), heading + DEG2FURMAN(270)) - ((dx * 4) / 3);
    dy1 = y - CIRC_Y(radius_pointer_head + (radius_text_height / 2), heading + DEG2FURMAN(270)) + ((dy * 4) / 3);
    draw_vertex(dx1, dy1);
    dx1 = x + CIRC_X(radius_pointer_head - radius_text_height, heading + DEG2FURMAN(270)) - ((dx * 2) / 3);
    dy1 = y - CIRC_Y(radius_pointer_head - radius_text_height, heading + DEG2FURMAN(270)) + ((dy * 2) / 3);
    draw_vertex(dx1, dy1);
    dx1 = x + CIRC_X(radius_pointer_head + (radius_text_height / 2), heading + DEG2FURMAN(270));
    dy1 = y - CIRC_Y(radius_pointer_head + (radius_text_height / 2), heading + DEG2FURMAN(270));
    draw_vertex(dx1, dy1);
    dx1 = x + CIRC_X(radius_pointer_head - radius_text_height, heading + DEG2FURMAN(270)) + ((dx * 2) / 3);
    dy1 = y - CIRC_Y(radius_pointer_head - radius_text_height, heading + DEG2FURMAN(270)) - ((dy * 2) / 3);
    draw_vertex(dx1, dy1);
    dx1 = x + CIRC_X(radius_pointer_head + (radius_text_height / 2), heading + DEG2FURMAN(270)) + ((dx * 4) / 3);
    dy1 = y - CIRC_Y(radius_pointer_head + (radius_text_height / 2), heading + DEG2FURMAN(270)) - ((dy * 4) / 3);
    draw_vertex(dx1, dy1);

    // Fixed reference point for heading
    EVE_BEGIN(EVE_BEGIN_LINES);
    EVE_LINE_WIDTH(reference_bold);
    EVE_COLOR(ovl_reference_col);
    draw_vertex(x, y - radius_pointer_head);
    draw_vertex(x, y - radius);
    
    EVE_END();
    EVE_RESTORE_CONTEXT();
}
