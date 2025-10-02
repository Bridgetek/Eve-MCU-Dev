/**
 @file flightalt.c
 */
/*
 * ============================================================================
 * History
 * =======
 * Nov 2019        Initial beta for FT81x and FT80x
 * Mar 2020        Updated beta - added BT815/6 commands
 * Mar 2021        Beta with BT817/8 support added
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
#include <stdint.h>
#include <stdlib.h>
#include "EVE.h"
#include "../include/HAL.h"
#include "MCU.h"

#include "trig_furman.h"

#ifndef min
#define min(X,Y) ((X) < (Y) ? (X) : (Y))
#endif
#ifndef max
#define max(X,Y) ((X) > (Y) ? (X) : (Y))
#endif

/*
 * x,y - top left of seven segment graphic in pixels
 * radius - radius of widget
 * alt - altitude reading
*/
void altwidget(int32_t x, int32_t y, uint16_t radius, int alt)
{
    // Like a real altitude instrument it will never go beyond the limits
    alt = max(alt, 0);
    alt = min(alt, 10000);

    // Dimensions of the widget are detemined by the radius
    int16_t radius_outer = radius;
    int16_t radius_b1 = (radius_outer * 39) / 40;
    int16_t radius_b2 = (radius_outer * 37) / 40;
    int16_t radius_inner = (radius_outer * 36) / 40;
    int16_t radius_major = (radius_outer * 30) / 40;
    int16_t radius_minor = (radius_inner + radius_major) / 2;
    // Altitude indicator line widths
    int16_t alt_ref_bold = radius_outer * 2 / 10;
    int16_t alt_ref_narrow = radius_outer * 1 / 10;
    int16_t alt_needle = radius_outer * 5 / 10;

    // Bezel colours
    uint32_t bezel_col = 0x505050;
    uint32_t bezel_col_bright = 0xaaaaaa;
    uint32_t bezel_col_dark = 0x333333;

    uint32_t alt_reference = 0xffffff;
    uint32_t alt_background = 0x000000;
    uint32_t alt_covering = 0x202020;

    int16_t dx, dy;
    int degthou, deghund;
    int deg;
    uint16_t dx1, dy1, dx2, dy2;
    int n;
    uint16_t dxt1, dyt1, dxt2, dyt2, dxt3, dyt3, dxh, dyh;

    EVE_VERTEX_FORMAT(0);

    // Draw bezel
    EVE_SAVE_CONTEXT();
    EVE_CLEAR(0,1,0);
    EVE_COLOR(bezel_col);
    // Draw the circles to make the bezel in three levels in the stencil buffer
    EVE_STENCIL_OP(EVE_STENCIL_INCR, EVE_STENCIL_INCR);	// Set the stencil to increment
    EVE_BEGIN(EVE_BEGIN_POINTS);
    EVE_POINT_SIZE(radius_outer * 16);
    EVE_VERTEX2F(x, y);
    EVE_POINT_SIZE(radius_b1 * 16);
    EVE_VERTEX2F(x, y);
    EVE_POINT_SIZE(radius_b2 * 16);
    EVE_VERTEX2F(x, y);
    EVE_POINT_SIZE(radius_inner * 16);
    EVE_VERTEX2F(x, y);
    EVE_END();
    EVE_STENCIL_OP(EVE_STENCIL_KEEP, EVE_STENCIL_KEEP); // Stop the stencil INCR
    // Gradient (light at top) for outer of bezel
    EVE_STENCIL_FUNC(EVE_TEST_EQUAL, 1, 255);
    EVE_CMD_GRADIENT(x - radius_outer, y + radius_outer, bezel_col_dark, x - radius_outer, y - radius_outer, bezel_col_bright);
    // Flat colour for centre of bezel
    EVE_STENCIL_FUNC(EVE_TEST_EQUAL, 2, 255);
    EVE_POINT_SIZE(radius_outer * 16);
    EVE_VERTEX2F(x, y);
    // Gradient (dark at top) for inner of bezel
    EVE_STENCIL_FUNC(EVE_TEST_EQUAL, 3, 255);
    EVE_CMD_GRADIENT(x - radius_outer, y + radius_outer, bezel_col_bright, x - radius_outer, y - radius_outer, bezel_col_dark);
    EVE_RESTORE_CONTEXT();

    // Altitude graduations
    EVE_SAVE_CONTEXT();
    EVE_CLEAR_STENCIL(0);
    EVE_CLEAR(0,1,0);
    EVE_COLOR(alt_covering);
    EVE_STENCIL_OP(EVE_STENCIL_KEEP, EVE_STENCIL_INCR);
    // Draw ground and sky stencil
    EVE_BEGIN(EVE_BEGIN_POINTS);
    EVE_POINT_SIZE(radius_inner * 16);
    EVE_VERTEX2F(x, y);
    EVE_POINT_SIZE(radius_minor * 16);
    EVE_VERTEX2F(x, y);
    EVE_POINT_SIZE(radius_major * 16);
    EVE_VERTEX2F(x, y);
    EVE_BEGIN(EVE_BEGIN_LINES);
    // bold at 1000ft intervals
    EVE_STENCIL_FUNC(EVE_TEST_NOTEQUAL, 0, 255);
    EVE_LINE_WIDTH(alt_ref_bold);
    EVE_COLOR(alt_reference);
    for (deg = 0; deg < 10; deg++)
    {
        dx1 = x + CIRC_X(radius_inner, DEG2FURMAN(deg * 36));
        dy1 = y - CIRC_Y(radius_inner, DEG2FURMAN(deg * 36));
        dx2 = x + CIRC_X(radius_major, DEG2FURMAN(deg * 36));
        dy2 = y - CIRC_Y(radius_major, DEG2FURMAN(deg * 36));
        EVE_VERTEX2F(dx1, dy1);
        EVE_VERTEX2F(dx2, dy2);
    }
    // narrow at 10 and 20 degrees
    EVE_LINE_WIDTH(alt_ref_narrow);
    for (deg = 0; deg < 100; deg+=2)
    {
        dx1 = x + CIRC_X(radius_inner, DEG2FURMAN(deg * 36 / 10));
        dy1 = y - CIRC_Y(radius_inner, DEG2FURMAN(deg * 36 / 10));
        dx2 = x + CIRC_X(radius_minor, DEG2FURMAN(deg * 36 / 10));
        dy2 = y - CIRC_Y(radius_minor, DEG2FURMAN(deg * 36 / 10));
        EVE_VERTEX2F(dx1, dy1);
        EVE_VERTEX2F(dx2, dy2);
    }
    EVE_STENCIL_FUNC(EVE_TEST_ALWAYS, 0, 255);
    EVE_COLOR(alt_covering);
    EVE_BEGIN(EVE_BEGIN_POINTS);
    EVE_POINT_SIZE(radius_major * 16);
    EVE_VERTEX2F(x, y);
    EVE_RESTORE_CONTEXT();

    // Indicators choose a font which is about 1/5th of the radius
    uint8_t fontheights[] = EVE_ROMFONT_HEIGHTS;
    int8_t font = sizeof(fontheights) / sizeof(fontheights[0]);
    for (n = sizeof(fontheights) / sizeof(fontheights[0]) - 1; n >= 0; n--)
    {
        if (fontheights[n] > 0)
        {
            if (fontheights[n] < (radius_inner / 5))
            {
                break;
            }
        }
        font -= 1;
    }
    if (font)
    {
        font = min(font, sizeof(fontheights) / sizeof(fontheights[0]) - 1);
        for (deg = 0; deg < 10; deg++)
        {
            dx = x + CIRC_X(radius_inner * 7 / 10, DEG2FURMAN(deg * 36));
            dy = y - CIRC_Y(radius_inner * 7 / 10, DEG2FURMAN(deg * 36));
            EVE_CMD_NUMBER(dx, dy, font, EVE_OPT_CENTER, deg);
        }
        EVE_CMD_TEXT(x, y - (radius_inner * 3 / 10), font, EVE_OPT_CENTER, "Altitude");
        EVE_CMD_TEXT(x, y + (radius_inner * 3 / 10), font, EVE_OPT_CENTER, "10000ft");
    }
    // Altitude needle
    EVE_SAVE_CONTEXT();
    EVE_CLEAR(0,1,0);
    EVE_COLOR(alt_reference);
    EVE_BEGIN(EVE_BEGIN_LINES);
    degthou = alt * 0x10000 / 10000;
    deghund = (alt % 1000) * 0x10000 / 1000;
    dx = x;
    dy = y;
    dxt1 = x + CIRC_X(radius_major, degthou);
    dyt1 = y - CIRC_Y(radius_major, degthou);
    dxt2 = x + CIRC_X((radius_major + radius_minor) / 2, degthou);
    dyt2 = y - CIRC_Y((radius_major + radius_minor) / 2, degthou);
    dxt3 = x + CIRC_X(radius_minor, degthou);
    dyt3 = y - CIRC_Y(radius_minor, degthou);
    dxh = x + CIRC_X(radius_major, deghund);
    dyh = y - CIRC_Y(radius_major, deghund);
    for (n = 0; n < 2; n++)
    {
        EVE_COLOR(n==0?alt_background:alt_reference);
        EVE_LINE_WIDTH(alt_needle/2 + (n==0?32:0));
        EVE_VERTEX2F(dx, dy);
        EVE_VERTEX2F(dxt3, dyt3);
        EVE_VERTEX2F(dx, dy);
        EVE_VERTEX2F(dxh, dyh);
        EVE_LINE_WIDTH(alt_needle*2/3 + (n==0?32:0));
        EVE_VERTEX2F(dx, dy);
        EVE_VERTEX2F(dxt2, dyt2);
        EVE_LINE_WIDTH(alt_needle + (n==0?32:0));
        EVE_VERTEX2F(dx, dy);
        EVE_VERTEX2F(dxt1, dyt1);
    }
    EVE_COLOR(alt_background);
    EVE_BEGIN(EVE_BEGIN_POINTS);
    EVE_POINT_SIZE((radius_inner / 5) * 16);
    EVE_VERTEX2F(x, y);
    EVE_RESTORE_CONTEXT();
}
