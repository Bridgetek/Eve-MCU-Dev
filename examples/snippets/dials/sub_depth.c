/**
 @file sub_depth.c
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
#include "sub_controls.h"

static inline void draw_vertex(int16_t x, int16_t y)
{
#if IS_EVE_API(1)
    EVE_VERTEX2F(x * 16, y * 16);
#else
    EVE_VERTEX2F(x, y);
#endif
}

static void draw_char_font(int16_t x, int16_t y, uint16_t furmans, int16_t width, int val)
{
    int16_t height;

    // Indicators choose a font which is about 1/5th of the radius
    static int16_t fontwidths[] = EVE_ROMFONT_WIDTHS;
    int8_t n;
    // Default font - use the smallest defined ROM font
    int8_t font = 20;
    // Make zero the same width as 10
    int len = 2;
    int tmp = val / 10;
    // Get number of digits in the number
    while (tmp /= 10)
    {
        len++;
    }
    for (n = sizeof(fontwidths) / sizeof(fontwidths[0]) - 1; n > 0; n--)
    {
        if (fontwidths[n] > 0)
        {
            // Fudge for a comfortable width
            if (((fontwidths[n] * len * 2) / 3) <= width)
            {
                font = n;
                break;
            }
        }
    }

    static int16_t fontheights[] = EVE_ROMFONT_HEIGHTS;
    int16_t charheight = fontheights[font];

    height = CIRC_X(charheight, furmans);
    if (height < 0) height = -height;

    EVE_CMD_LOADIDENTITY();
    EVE_CMD_SCALE(0x10000, (0x10000 * height) / charheight);
    EVE_CMD_SETMATRIX(); 
    EVE_CMD_NUMBER(x, y - (height / 2), font, EVE_OPT_RIGHTX, val);
    EVE_CMD_LOADIDENTITY();
    EVE_CMD_SETMATRIX(); 
}

/*
 * x,y - top left of widget in pixels
 * radius - radius of widget
 * heading - compass heading in depth
*/
void sub_depth(int32_t x, int32_t y, uint16_t width, uint16_t height, uint16_t options, int16_t depth, int16_t visible)
{
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
    // Stepping for depth
    int32_t fur;

    if (options & OPT_SUB_BEZEL)
    {
        // Dimensions of the widget are detemined by the height and width
        int16_t border = (height * 1) / 80;
        int16_t height_b1 = height - (2 * border);
        int16_t height_b2 = height - (4 * border);
        int16_t height_inner = height - (6 * border);
        int16_t width_b1 = width - (2 * border);
        int16_t width_b2 = width - (4 * border);
        int16_t width_inner = width - (6 * border);
        
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

        EVE_BEGIN(EVE_BEGIN_RECTS);
        EVE_LINE_WIDTH(1 * 16);
        draw_vertex(x, y); // stencil 1
        draw_vertex(x + width, y + height); // stencil 1
        draw_vertex(x + (1 * border), y + (1 * border)); // stencil 2
        draw_vertex(x + (1 * border) + width_b1, y + (1 * border) + height_b1); // stencil 2
        draw_vertex(x + (2 * border), y + (2 * border)); // stencil 3
        draw_vertex(x + (2 * border) + width_b2, y + + (2 * border) + height_b2); // stencil 3
        draw_vertex(x + (3 * border), y + (3 * border)); // stencil 4
        draw_vertex(x + (3 * border) + width_inner, y + (3 * border) + height_inner); // stencil 4
        EVE_COLOR_MASK(1,1,1,1);
        EVE_END();
        
        EVE_STENCIL_OP(EVE_STENCIL_KEEP, EVE_STENCIL_KEEP); // Stop the stencil INCR
        // Gradient (light at top) for outer of bezel
        EVE_STENCIL_FUNC(EVE_TEST_EQUAL, 1, 255);
        EVE_CMD_GRADIENT(x, y, bezel_col_dark, x + width, y + height, bezel_col_bright);
        // Flat colour for centre of bezel
        EVE_STENCIL_FUNC(EVE_TEST_EQUAL, 2, 255);
        EVE_BEGIN(EVE_BEGIN_RECTS);
        draw_vertex(x, y);
        draw_vertex(x + width, y + height);
        // Gradient (dark at top) for inner of bezel
        EVE_STENCIL_FUNC(EVE_TEST_EQUAL, 3, 255);
        EVE_CMD_GRADIENT(x, y, bezel_col_bright, x + width, y + height, bezel_col_dark);
        EVE_RESTORE_CONTEXT();

        height -= (6 * border);
        width -= (6 * border);
        x += (3 * border);
        y += (3 * border);
    }

    // Gap between graduations
    int16_t height_grad = (height * 1) / ((visible * 2) / SUB_UNITS_SCALE);
    // Width of graduations
    int16_t width_depth = (width * 1) / 2;
    // Degree graduation line widths
    int16_t reference_bold = height * 3 / (visible / SUB_UNITS_SCALE);
    int16_t reference_narrow = height * 2 / (visible / SUB_UNITS_SCALE);
    int16_t reference_width;
    // Inner of degree graduations
    int16_t text_size = (height * 7) / 10;
    
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

    // Stencils for depth markings
    EVE_BEGIN(EVE_BEGIN_RECTS);
    EVE_LINE_WIDTH(1 * 16);
    draw_vertex(x, y); // stencil 1
    draw_vertex(x + width, y + height); // stencil 1
    draw_vertex(x, y); // stencil 2
    draw_vertex(x + (width / 2), y + height); // stencil 2
    draw_vertex(x, y); // stencil 3
    draw_vertex(x + (width / 4), y + height); // stencil 3
    EVE_COLOR_MASK(1,1,1,1);

    EVE_STENCIL_OP(EVE_STENCIL_KEEP, EVE_STENCIL_KEEP);
    if (!(options & OPT_SUB_TRANSPARENT))
    {    
        // Gradient fill for background
        EVE_STENCIL_FUNC(EVE_TEST_GEQUAL, 1, 255);
        // Gradient (dark at bottom) for heading gradients rotator
        EVE_CMD_GRADIENT(x, y, bezel_col_dark, x + width, y + height, bezel_col_bright);
    }
    
    // Draw gradient lines
    EVE_BEGIN(EVE_BEGIN_LINES);
    EVE_COLOR(reference_col);

    int16_t offset;
    // Draw depth gradients at SUB_UNITS_SCALE division intervals
    for (offset = depth - (visible / 2); offset < depth + (visible / 2); offset += SUB_UNITS_SCALE)
    {
        if (offset >= 0)
        {
            // Can't do a modulus on a negative number
            int16_t step = (offset);
            if (step < 0) step = -step;

            // Work out apparent viewing angle of reference lines
            fur = ((((offset / SUB_UNITS_SCALE) * SUB_UNITS_SCALE) - depth + (visible / 2)) * 0x8000) / (visible);

            // Calculate offset to reference lines
            int16_t dy = y + ((int16_t)height / 2) - (CIRC_X(height, fur + 0x4000) / 2);
            // If a thick (10 subdivisions) is drawn with the depth
            if ((offset % (SUB_UNITS_SCALE * 10)) < SUB_UNITS_SCALE)
            {
                EVE_STENCIL_FUNC(EVE_TEST_GEQUAL, 1, 255);
                draw_char_font(x + width, dy, fur, width / 2, offset / SUB_UNITS_SCALE);
                EVE_BEGIN(EVE_BEGIN_LINES);
                reference_width = CIRC_X(reference_bold, fur);
                if (reference_width < 0) reference_width = -reference_width;
                EVE_LINE_WIDTH(reference_width);
                EVE_STENCIL_FUNC(EVE_TEST_GEQUAL, 2, 255);
            }
            // If a thin (1 subdivision) is drawn
            else
            {
                reference_width = CIRC_X(reference_narrow, fur);
                if (reference_width < 0) reference_width = -reference_width;
                EVE_LINE_WIDTH(reference_width);
                EVE_STENCIL_FUNC(EVE_TEST_GEQUAL, 3, 255);
            }
            draw_vertex(x, dy);
            draw_vertex(x + width, dy);
        }
    }

    // Fixed reference point for depth
    EVE_STENCIL_FUNC(EVE_TEST_GEQUAL, 1, 255);
    EVE_BEGIN(EVE_BEGIN_LINES);
    EVE_LINE_WIDTH(reference_bold);
    EVE_COLOR(ovl_reference_col);
    draw_vertex(x, y + (height / 2));
    draw_vertex(x + width, y + (height / 2));

    EVE_END();
    EVE_STENCIL_FUNC(EVE_TEST_ALWAYS, 0, 255);

    EVE_RESTORE_CONTEXT();
}
