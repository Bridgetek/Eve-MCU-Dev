/**
 @file flightatt.c
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

/*
 * x,y - top left of seven segment graphic in pixels
 * radius - radius of widget
 * pitch - pitch in furmans
 * climb - climb in degrees
 * roll - roll in degrees
*/
void attwidget(int32_t x, int32_t y, uint16_t radius, int16_t pitch, int16_t climb, int16_t roll)
{
    // Dimensions of the widget are detemined by the radius
    int16_t radius_outer = radius;
    int16_t radius_b1 = (radius_outer * 39) / 40;
    int16_t radius_b2 = (radius_outer * 37) / 40;
    int16_t radius_inner = (radius_outer * 36) / 40;
    int16_t radius_roll = (radius_outer * 30) / 40;
    int16_t radius_bank = (radius_inner + radius_roll) / 2;
    // Attitude indicator line widths
    int16_t ai_ref_bold = radius_outer * 4 / 10;
    int16_t ai_ref_narrow = radius_outer * 2 / 10;
    // Overlay reference line widths
    int16_t ovl_bold = radius_outer * 4 / 10;
    int16_t ovl_narrow = radius_outer * 2 / 10;

    // Bezel colours
    uint32_t bezel_col = 0x505050;
    uint32_t bezel_col_bright = 0xaaaaaa;
    uint32_t bezel_col_dark = 0x333333;
    // Attitude indicator colours for roll and pitch areas
    uint32_t roll_col_ground = 0x444400;
    uint32_t roll_col_sky = 0x3060aa;
    uint32_t pitch_col_ground = 0x555500;
    uint32_t pitch_col_sky = 0x5070bb;
    uint32_t ai_reference = 0xffffff;
    // Overlay reference colours
    uint32_t ovl_reference = 0xffaa00;
    uint32_t ovl_reference_dark = 0x505050;

    int16_t dx, dy;
    int16_t dx1, dy1, dx2, dy2;
    int deg;
    int16_t pw;
    int i;

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

    // Bank/Roll Index: Outer circle
    EVE_SAVE_CONTEXT();
    EVE_CLEAR_STENCIL(0);
    EVE_CLEAR(0,1,0);
    EVE_STENCIL_OP(EVE_STENCIL_KEEP, EVE_STENCIL_INCR);
    // Draw ground and sky stencil
    EVE_BEGIN(EVE_BEGIN_POINTS);
    EVE_POINT_SIZE(radius_inner * 16);
    EVE_VERTEX2F(x, y);
    EVE_POINT_SIZE(radius_roll * 16);
    EVE_VERTEX2F(x, y);
    EVE_STENCIL_FUNC(EVE_TEST_EQUAL, 1, 255);
    EVE_BEGIN(EVE_BEGIN_LINES);
    EVE_LINE_WIDTH(radius_inner * 16 / 2);
    // Draw the roll sky and ground
    // These are rotated depending on the "roll"
    EVE_COLOR(roll_col_ground);
    dx = x + CIRC_X((radius_inner/2) + (ai_ref_bold/16), -roll);
    dy = y + CIRC_Y((radius_inner/2) + (ai_ref_bold/16), -roll);
    for (deg = -90; deg <= 90; deg += 180)
    {
        dx1 = dx + CIRC_X(radius_inner, -roll + DEG2FURMAN(deg));
        dy1 = dy + CIRC_Y(radius_inner, -roll + DEG2FURMAN(deg));
        EVE_VERTEX2F(dx1, dy1);
    }
    EVE_COLOR(roll_col_sky);
    dx = x - CIRC_X((radius_inner/2) + (ai_ref_bold/16), -roll);
    dy = y - CIRC_Y((radius_inner/2) + (ai_ref_bold/16), -roll);
    for (deg = -90; deg <= 90; deg += 180)
    {
        dx1 = dx + CIRC_X(radius_inner, -roll + DEG2FURMAN(deg));
        dy1 = dy + CIRC_Y(radius_inner, -roll + DEG2FURMAN(deg));
        EVE_VERTEX2F(dx1, dy1);
    }
    // Draw reference lines:
    EVE_COLOR(ai_reference);
    // These are rotated depending on the "roll"
    EVE_STENCIL_FUNC(EVE_TEST_EQUAL, 2, 255);
    EVE_BEGIN(EVE_BEGIN_LINES);
    // bold at 60 and 30 degrees
    EVE_LINE_WIDTH(ai_ref_bold);
    for (deg = -60; deg <= 60; deg += 30)
    {
        if (deg == 0) continue;
        dx = x + CIRC_X(radius_inner, DEG2FURMAN(deg) + roll);
        dy = y - CIRC_Y(radius_inner, DEG2FURMAN(deg) + roll);
        EVE_VERTEX2F(dx, dy);
        EVE_VERTEX2F(x, y);
    }
    // narrow at 10 and 20 degrees
    EVE_LINE_WIDTH(ai_ref_narrow);
    for (deg = -20; deg <= 20; deg += 10)
    {
        if (deg == 0) continue;
        dx = x + CIRC_X(radius_bank, DEG2FURMAN(deg) + roll);
        dy = y - CIRC_Y(radius_bank, DEG2FURMAN(deg) + roll);
        EVE_VERTEX2F(dx, dy);
        EVE_VERTEX2F(x, y);
    }
    // dot at 45 degrees
    EVE_BEGIN(EVE_BEGIN_POINTS);
    EVE_POINT_SIZE(ai_ref_narrow);
    for (deg = -45; deg <= 45; deg += 90)
    {
        dx = x + CIRC_X(radius_bank, DEG2FURMAN(deg) + roll);
        dy = y - CIRC_Y(radius_bank, DEG2FURMAN(deg) + roll);
        EVE_VERTEX2F(dx, dy);
    }
    EVE_STENCIL_FUNC(EVE_TEST_ALWAYS, 0, 255);
    EVE_RESTORE_CONTEXT();

    // Pitch Indicator: Inner circle
    EVE_SAVE_CONTEXT();
    EVE_CLEAR_STENCIL(0);
    EVE_CLEAR(0,1,0);
    EVE_STENCIL_OP(EVE_STENCIL_KEEP, EVE_STENCIL_INCR);
    // Draw ground and sky stencil
    EVE_BEGIN(EVE_BEGIN_POINTS);
    EVE_POINT_SIZE(radius_roll * 16);
    EVE_VERTEX2F(x, y);
    EVE_STENCIL_FUNC(EVE_TEST_GEQUAL, 1, 255);
    EVE_BEGIN(EVE_BEGIN_LINES);
    EVE_LINE_WIDTH(radius_roll * 16 / 4);
    // Draw the pitch sky and ground
    // These are rotated depending on the "pitch"
    // Offset of pitch radius_roll is pitch of +40, -radius_roll is -40
    EVE_COLOR(pitch_col_ground);
    for (i = 1; i < 8; i++)
    {
        pw = ((radius_roll * i) / 4) + (ai_ref_narrow/16) + ((radius_roll * climb) / DEG2FURMAN(40));
        dx = x + CIRC_X(pw, -pitch);
        dy = y + CIRC_Y(pw, -pitch);
        for (deg = -90; deg <= 90; deg += 180)
        {
            dx1 = dx + CIRC_X(radius_roll, -pitch + DEG2FURMAN(deg));
            dy1 = dy + CIRC_Y(radius_roll, -pitch + DEG2FURMAN(deg));
            EVE_VERTEX2F(dx1, dy1);
        }
    }
    EVE_COLOR(pitch_col_sky);
    for (i = 1; i < 8; i++)
    {
        pw = ((radius_roll * i) / 4) + (ai_ref_narrow/16) - ((radius_roll * climb) / DEG2FURMAN(40));
        dx = x - CIRC_X(pw, -pitch);
        dy = y - CIRC_Y(pw, -pitch);
        for (deg = -90; deg <= 90; deg += 180)
        {
            dx1 = dx + CIRC_X(radius_roll, -pitch + DEG2FURMAN(deg));
            dy1 = dy + CIRC_Y(radius_roll, -pitch + DEG2FURMAN(deg));
            EVE_VERTEX2F(dx1, dy1);
        }
    }

    // Draw the pitch and climb reference lines
    EVE_COLOR(ai_reference);
    EVE_LINE_WIDTH(ai_ref_narrow);
    // Major reference lines
    for (i = -20; i <= 20; i += 10)
    {
        if (i == 0) continue;
        pw = ((radius_roll * climb) / DEG2FURMAN(40)) + ((radius_roll * i) / 40);
        dx = x + CIRC_X(pw, -pitch);
        dy = y + CIRC_Y(pw, -pitch);
        for (deg = -90; deg <= 90; deg += 180)
        {
            dx1 = dx + CIRC_X(radius_roll * i / 50, -pitch + DEG2FURMAN(deg));
            dy1 = dy + CIRC_Y(radius_roll * i / 50, -pitch + DEG2FURMAN(deg));
            EVE_VERTEX2F(dx1, dy1);
        }
    }
    // Minor reference lines
    for (i = -15; i <= 15; i += 5)
    {
        if (i == 0) continue;
        pw = ((radius_roll * climb) / DEG2FURMAN(40)) + ((radius_roll * i) / 40);
        dx = x + CIRC_X(pw, -pitch);
        dy = y + CIRC_Y(pw, -pitch);
        for (deg = -90; deg <= 90; deg += 180)
        {
            dx1 = dx + CIRC_X(radius_roll / 10, -pitch + DEG2FURMAN(deg));
            dy1 = dy + CIRC_Y(radius_roll / 10, -pitch + DEG2FURMAN(deg));
            EVE_VERTEX2F(dx1, dy1);
        }
    }
    EVE_RESTORE_CONTEXT();

    // Triangle for 0 in Pitch Indicator
    EVE_SAVE_CONTEXT();
    EVE_BEGIN(EVE_BEGIN_LINE_STRIP);
    EVE_LINE_WIDTH(ai_ref_narrow);
    EVE_COLOR(ai_reference);
    dx = x + CIRC_X(radius_roll - (ai_ref_bold/8), roll);
    dy = y - CIRC_Y(radius_roll - (ai_ref_bold/8), roll);
    EVE_VERTEX2F(dx, dy);
    dx2 = x + CIRC_X(radius_inner - (ai_ref_bold/16), roll + DEG2FURMAN(5));
    dy2 = y - CIRC_Y(radius_inner - (ai_ref_bold/16), roll + DEG2FURMAN(5));
    EVE_VERTEX2F(dx2, dy2);
    dx1 = x + CIRC_X(radius_inner - (ai_ref_bold/16), roll - DEG2FURMAN(5));
    dy1 = y - CIRC_Y(radius_inner - (ai_ref_bold/16), roll - DEG2FURMAN(5));
    EVE_VERTEX2F(dx1, dy1);
    EVE_VERTEX2F(dx, dy);
    EVE_END();
    EVE_RESTORE_CONTEXT();

    // Reference Overlay
    // Triangle for 0 in reference
    EVE_SAVE_CONTEXT();
    EVE_BEGIN(EVE_BEGIN_LINE_STRIP);
    EVE_LINE_WIDTH(ovl_narrow);
    EVE_COLOR(ovl_reference);
    dx = x;
    dy = y - radius_roll;
    EVE_VERTEX2F(dx, dy);
    dx2 = x - (radius_roll * 2 / 10);
    dy2 = y - (radius_roll * 8 / 10);
    EVE_VERTEX2F(dx2, dy2);
    dx1 = x + (radius_roll * 2 / 10);
    dy1 = y - (radius_roll * 8 / 10);
    EVE_VERTEX2F(dx1, dy1);
    EVE_VERTEX2F(dx, dy);
    EVE_END();
    // Centre target for reference
    EVE_BEGIN(EVE_BEGIN_LINE_STRIP);
    EVE_LINE_WIDTH(ovl_bold);
    dx = x - ((radius_roll * 3) / 4);
    dy = y;
    EVE_VERTEX2F(dx, dy);
    dx = x - (radius_roll / 4);
    EVE_VERTEX2F(dx, dy);
    dx = x - (radius_roll / 5);
    dy = y + (radius_roll / 10);
    EVE_VERTEX2F(dx, dy);
    EVE_END();
    EVE_BEGIN(EVE_BEGIN_LINE_STRIP);
    dx = x + (radius_roll / 5);
    dy = y + (radius_roll / 10);
    EVE_VERTEX2F(dx, dy);
    dx = x + (radius_roll / 4);
    dy = y;
    EVE_VERTEX2F(dx, dy);
    dx = x + ((radius_roll * 3) / 4);
    dy = y;
    EVE_VERTEX2F(dx, dy);
    EVE_END();
    // dot at 45 degrees
    EVE_BEGIN(EVE_BEGIN_POINTS);
    EVE_POINT_SIZE(ovl_bold);
    dx = x;
    dy = y;
    EVE_VERTEX2F(dx, dy);
    EVE_RESTORE_CONTEXT();
}
