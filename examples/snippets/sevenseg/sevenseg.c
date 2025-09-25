/**
 @file sevenseg.c
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
#include "EVE.h"
#include "../include/HAL.h"
#include "MCU.h"

/*
 * x,y - top left of seven segment graphic in pixels
 * size - width/height of each segment in pixels
 * digit - numeral to display in segment (0-9)
 * fgcolor - colour of on/highlighted segment
 * bgcolor - colour of off segment
*/
void sevensegment(int32_t x, int32_t y, uint16_t size, char digit, uint32_t fgcolour, uint32_t bgcolour)
{
#if IS_EVE_API(2, 3, 4, 5)
    const int32_t vertex = 4;
#else
    const int32_t vertex = 16;
#endif

    const uint8_t map[][7] = {{1,1,1,0,1,1,1}, // 0
            {0,0,1,0,0,1,0}, // 1
            {1,0,1,1,1,0,1}, // 2
            {1,0,1,1,0,1,1}, // 3
            {0,1,1,1,0,1,0}, // 4
            {1,1,0,1,0,1,1}, // 5
            {1,1,0,1,1,1,1}, // 6
            {1,1,1,0,0,1,0}, // 7
            {1,1,1,1,1,1,1}, // 8
            {1,1,1,1,0,1,1}, // 9
            {1,1,1,1,1,1,0}, // 10 - A
            {0,1,0,1,1,1,1}, // 11 - b
            {1,1,0,1,1,0,1}, // 12 - C
            {0,0,1,1,1,1,1}, // 13 - d
            {1,1,0,1,1,0,1}, // 14 - E
            {1,1,0,1,1,0,0}, // 15 - F
            {0,0,0,1,0,0,0}, // 16 - dash
            };

    int32_t top = (y) * vertex;
    int32_t centre = (y + size) * vertex;
    int32_t bottom = (y + (size * 2)) * vertex;
    int32_t left = (x) * vertex;
    int32_t right = (x + size) * vertex;

    int32_t pt0lx = (x - (size / 2)) * vertex;
    int32_t pt0ly = (y - (size / 2)) * vertex;

    int32_t pt1lx = (x + (size / 2)) * vertex;
    int32_t pt1ly = (y + (size / 2)) * vertex;

    int32_t pt2lx = (x + (size * 3 / 2)) * vertex;
    int32_t pt2ly = (y + (size * 3 / 2)) * vertex;

    int32_t pt3ly = (y + (size * 5 / 2)) * vertex;

    int32_t width = (((size * 2)/ 3) / 8) * 16;

    EVE_SAVE_CONTEXT();
#if IS_EVE_API(2, 3, 4, 5)
    EVE_VERTEX_FORMAT(2);
#endif
    EVE_COLOR_MASK(0, 0, 0, 1);
    EVE_BLEND_FUNC(EVE_BLEND_ONE, EVE_BLEND_ONE_MINUS_SRC_ALPHA);
    EVE_BEGIN(EVE_BEGIN_LINES);
    EVE_LINE_WIDTH(width);
    // Top segment
    EVE_VERTEX2F(left, top);
    EVE_VERTEX2F(right, top);
    // Top left segment
    EVE_VERTEX2F(left, top);
    EVE_VERTEX2F(left, centre);
    // Top right segment
    EVE_VERTEX2F(right, top);
    EVE_VERTEX2F(right, centre);
    // Centre segment
    EVE_VERTEX2F(left, centre);
    EVE_VERTEX2F(right, centre);
    // Bottom left segment
    EVE_VERTEX2F(left, centre);
    EVE_VERTEX2F(left, bottom);
    // Bottom right segment
    EVE_VERTEX2F(right, centre);
    EVE_VERTEX2F(right, bottom);
    // Bottom segment
    EVE_VERTEX2F(left, bottom);
    EVE_VERTEX2F(right, bottom);

    // Draw mesh frame for segments
    EVE_BLEND_FUNC(EVE_BLEND_ZERO, EVE_BLEND_ONE_MINUS_SRC_ALPHA);
    EVE_LINE_WIDTH((width * 3) / 4);
    EVE_BEGIN(EVE_BEGIN_LINE_STRIP);
    EVE_VERTEX2F(pt0lx, pt0ly);
    EVE_VERTEX2F(pt2lx, pt2ly);
    EVE_VERTEX2F(pt1lx, pt3ly);
    EVE_VERTEX2F(pt0lx, pt2ly);
    EVE_VERTEX2F(pt2lx, pt0ly);
    EVE_BEGIN(EVE_BEGIN_LINE_STRIP);
    EVE_VERTEX2F(pt0lx, pt3ly);
    EVE_VERTEX2F(pt2lx, pt1ly);
    EVE_VERTEX2F(pt1lx, pt0ly);
    EVE_VERTEX2F(pt0lx, pt1ly);
    EVE_VERTEX2F(pt2lx, pt3ly);

    EVE_COLOR_MASK(1, 1, 1, 0);
    EVE_BLEND_FUNC(EVE_BLEND_DST_ALPHA, EVE_BLEND_ONE_MINUS_DST_ALPHA);
    EVE_BEGIN(EVE_BEGIN_LINES);
    EVE_LINE_WIDTH(width);
    // Top segment
    EVE_COLOR(map[(int)digit][0]?fgcolour:bgcolour);
    EVE_VERTEX2F(left, top);
    EVE_VERTEX2F(right, top);
    // Top left segment
    EVE_COLOR(map[(int)digit][1]?fgcolour:bgcolour);
    EVE_VERTEX2F(left, top);
    EVE_VERTEX2F(left, centre);
    // Top right segment
    EVE_COLOR(map[(int)digit][2]?fgcolour:bgcolour);
    EVE_VERTEX2F(right, top);
    EVE_VERTEX2F(right, centre);
    // Centre segment
    EVE_COLOR(map[(int)digit][3]?fgcolour:bgcolour);
    EVE_VERTEX2F(left, centre);
    EVE_VERTEX2F(right, centre);
    // Bottom left segment
    EVE_COLOR(map[(int)digit][4]?fgcolour:bgcolour);
    EVE_VERTEX2F(left, centre);
    EVE_VERTEX2F(left, bottom);
    // Bottom right segment
    EVE_COLOR(map[(int)digit][5]?fgcolour:bgcolour);
    EVE_VERTEX2F(right, centre);
    EVE_VERTEX2F(right, bottom);
    // Bottom segment
    EVE_COLOR(map[(int)digit][6]?fgcolour:bgcolour);
    EVE_VERTEX2F(left, bottom);
    EVE_VERTEX2F(right, bottom);
    EVE_BLEND_FUNC(EVE_BLEND_ONE, EVE_BLEND_ONE_MINUS_SRC_ALPHA);
    EVE_RESTORE_CONTEXT();
}
