/**
 @file eve_example.c
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
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdint.h>
#include "EVE.h"
#include "../include/HAL.h"
#include "MCU.h"

#include "eve_helper.h"
#include "sevenseg.h"

#if IS_EVE_API(1)
#error This example requires EVE API 2 or above.
#endif

#if EVE_DISP_WIDTH < 480
#error This example needs a screen width of 480 pixels or above.
#endif

void ledbox(uint16_t x, uint16_t y, uint16_t count, uint16_t segsize)
{
    EVE_VERTEX_FORMAT(2);
    EVE_BEGIN(EVE_BEGIN_RECTS);
    EVE_VERTEX_TRANSLATE_X((x - (segsize/3)) * 16);
    EVE_VERTEX_TRANSLATE_Y((y - (segsize/3)) * 16);
    EVE_VERTEX2F(0, 0);
    EVE_VERTEX2F(((count * (segsize + (segsize/3))) + (segsize/3)) * 4, 
                 ((2 * segsize + (segsize/3)) + (segsize/3)) * 4);
    EVE_VERTEX_TRANSLATE_X(0);
    EVE_VERTEX_TRANSLATE_Y(0);
}

void lednumber(uint16_t x, uint16_t y, uint16_t count, uint16_t segsize, uint32_t value, uint32_t fg, uint32_t bg)
{
    int i;

    for (i = 0; i < count; i++)
    {
        sevensegment(x + ((count - i - 1) * (segsize + (segsize/3))), y, segsize, value % 10, fg, bg);
        value = value / 10;
    }
}

void tapebox(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    EVE_BEGIN(EVE_BEGIN_RECTS);
    EVE_VERTEX_TRANSLATE_X(x * 16);
    EVE_VERTEX_TRANSLATE_Y(y * 16);
    EVE_VERTEX2F(0, 0);
    EVE_VERTEX_TRANSLATE_X((x + w) * 16);
    EVE_VERTEX_TRANSLATE_Y((y + h) * 16);
    EVE_VERTEX2F(0, 0);
    EVE_VERTEX_TRANSLATE_X(0);
    EVE_VERTEX_TRANSLATE_Y(0);
}

void gradbox(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    EVE_SCISSOR_XY(x, y);
    EVE_SCISSOR_SIZE(w, h);
    EVE_CMD_GRADIENT(x, y, 0x808080, x, y+h, 0x404040);
}

void eve_display(void)
{
    time_t ptime;
    struct tm destination;
    struct tm present;
    struct tm lasttime;

    //destination. = datetime.datetime(1985, 10, 26, 1, 21);
    memset(&destination, 0, sizeof(destination));
    destination.tm_year = 1985 - 1900;
    destination.tm_mon = 10 - 1;
    destination.tm_mday = 26;
    destination.tm_hour = 1 - 1;
    destination.tm_min = 21 - 1;
    ptime = time(NULL);
    gmtime_s(&present, &ptime);
    gmtime_s(&lasttime, &ptime);

    while (1)
    {
        /*
        printf("The date time is: %d month %d day %d year %d hour %d minutes\n",
              present.tm_mon + 1,
              present.tm_mday,
              present.tm_year + 1900,
              present.tm_hour,
              present.tm_min
              );
              */
        EVE_LIB_BeginCoProList();
        EVE_CMD_DLSTART();
        EVE_CLEAR_COLOR_RGB(0x0, 0x0, 0x0);
        EVE_CLEAR(1,1,1);

        // Screen sizing sets the seven segment size
        uint16_t segsize = EVE_DISP_WIDTH / 24;
        // Starting point
        uint16_t x = segsize * 2;
        uint16_t y = (segsize * 2) / 2;
        uint16_t cx = x + (segsize * 9);
        uint8_t font = 31;
        if (segsize < 50) // < 1200 pixels wide
            font -= 1;
        if (segsize < 42) // < 1024 pixels wide
            font -= 1;
        if (segsize < 33) // < 800 pixels wide
            font -= 1;
        uint16_t w = segsize * 10;
        uint16_t dx = x;
        uint16_t dy = y;
        EVE_COLOR_RGB(0x80, 0x80, 0x80);
        EVE_SAVE_CONTEXT();
        gradbox(dx - segsize / 2, dy - segsize / 2, 20 * segsize, segsize * 4);
        dy = dy + segsize * 4;
        gradbox(dx - segsize / 2, dy - segsize / 2, 20 * segsize, segsize * 4);
        dy = dy + segsize * 4;
        gradbox(dx - segsize / 2, dy - segsize / 2, 20 * segsize, segsize * 4);
        EVE_RESTORE_CONTEXT();

        dy = y;
        EVE_COLOR_RGB(0, 0, 0);
        tapebox(cx - w/2, dy + 5 * segsize / 2, w, (eve_romfont_height(font) * 4) / 3);
        dy = dy + segsize * 4;
        tapebox(cx - w/2, dy + 5 * segsize / 2, w, (eve_romfont_height(font) * 4) / 3);
        dy = dy + segsize * 4;
        tapebox(cx - w/2, dy + 5 * segsize / 2, w, (eve_romfont_height(font) * 4) / 3);

        dy = y;
        EVE_COLOR_RGB(255, 255, 255);
        EVE_CMD_TEXT(cx, dy + 5 * segsize / 2 + (eve_romfont_height(font) * 2) / 3, font, EVE_OPT_CENTER, "DESTINATION TIME");
        dy = dy + segsize * 4;
        EVE_CMD_TEXT(cx, dy + 5 * segsize / 2 + (eve_romfont_height(font) * 2) / 3, font, EVE_OPT_CENTER, "PRESENT TIME");
        dy = dy + segsize * 4;
        EVE_CMD_TEXT(cx, dy + 5 * segsize / 2 + (eve_romfont_height(font) * 2) / 3, font, EVE_OPT_CENTER, "LAST TIME DEPARTED");
        dy = dy + segsize * 4;

        // Bacground colour of non-active LED segments
        uint32_t bg = 0x100000;
        EVE_CMD_BGCOLOR(bg);

        // Red LEDs
        uint32_t fg = 0xff0000;
        EVE_CMD_FGCOLOR(fg);

        dx = x;
        dy = y;
        EVE_COLOR_RGB(0, 0, 0);
        ledbox(dx, dy, 2, segsize);
        lednumber(dx, dy, 2, segsize, destination.tm_mon + 1, fg, bg);
        
        dx = dx + (7 * segsize / 2);
        EVE_COLOR_RGB(0, 0, 0);
        ledbox(dx, dy, 2, segsize);
        lednumber(dx, dy, 2, segsize, destination.tm_mday, fg, bg);

        dx = dx + (7 * segsize / 2);
        EVE_COLOR_RGB(0, 0, 0);
        ledbox(dx, dy, 4, segsize);
        lednumber(dx, dy, 4, segsize, destination.tm_year + 1900, fg, bg);

        dx = dx + (25 * segsize / 4);
        EVE_COLOR_RGB(0, 0, 0);
        ledbox(dx, dy, 2, segsize);
        lednumber(dx, dy, 2, segsize, destination.tm_hour, fg, bg);

        dx = dx + (7 * segsize / 2);
        EVE_COLOR_RGB(0, 0, 0);
        ledbox(dx, dy, 2, segsize);
        lednumber(dx, dy, 2, segsize, destination.tm_min, fg, bg);

        // Green LEDs
        fg = 0x00ff00;
        EVE_CMD_FGCOLOR(fg);

        dx = x;
        dy = dy + segsize * 4;

        EVE_COLOR_RGB(0, 0, 0);
        ledbox(dx, dy, 2, segsize);
        lednumber(dx, dy, 2, segsize, present.tm_mon + 1, fg, bg);
        
        dx = dx + (7 * segsize / 2);
        EVE_COLOR_RGB(0, 0, 0);
        ledbox(dx, dy, 2, segsize);
        lednumber(dx, dy, 2, segsize, present.tm_mday, fg, bg);

        dx = dx + (7 * segsize / 2);
        EVE_COLOR_RGB(0, 0, 0);
        ledbox(dx, dy, 4, segsize);
        lednumber(dx, dy, 4, segsize, present.tm_year + 1900, fg, bg);

        dx = dx + (25 * segsize / 4);
        EVE_COLOR_RGB(0, 0, 0);
        ledbox(dx, dy, 2, segsize);
        lednumber(dx, dy, 2, segsize, present.tm_hour, fg, bg);

        dx = dx + (7 * segsize / 2);
        EVE_COLOR_RGB(0, 0, 0);
        ledbox(dx, dy, 2, segsize);
        lednumber(dx, dy, 2, segsize, 88, fg, bg);

        // Amber LEDs
        fg = 0xffff00;
        EVE_CMD_FGCOLOR(fg);
        
        dx = x;
        dy = dy + segsize * 4;

        EVE_COLOR_RGB(0, 0, 0);
        EVE_COLOR_RGB(0, 0, 0);
        ledbox(dx, dy, 2, segsize);
        lednumber(dx, dy, 2, segsize, lasttime.tm_mon + 1, fg, bg);
        
        dx = dx + (7 * segsize / 2);
        EVE_COLOR_RGB(0, 0, 0);
        ledbox(dx, dy, 2, segsize);
        lednumber(dx, dy, 2, segsize, lasttime.tm_mday, fg, bg);

        dx = dx + (7 * segsize / 2);
        EVE_COLOR_RGB(0, 0, 0);
        ledbox(dx, dy, 4, segsize);
        lednumber(dx, dy, 4, segsize, lasttime.tm_year + 1900, fg, bg);

        dx = dx + (25 * segsize / 4);
        EVE_COLOR_RGB(0, 0, 0);
        ledbox(dx, dy, 2, segsize);
        lednumber(dx, dy, 2, segsize, lasttime.tm_hour, fg, bg);

        dx = dx + (7 * segsize / 2);
        EVE_COLOR_RGB(0, 0, 0);
        ledbox(dx, dy, 2, segsize);
        lednumber(dx, dy, 2, segsize, lasttime.tm_min, fg, bg);

        EVE_DISPLAY();
        EVE_CMD_SWAP();
        EVE_LIB_EndCoProList();
        EVE_LIB_AwaitCoProEmpty();

        present.tm_hour = present.tm_hour - 1;
        mktime(&present);
        ptime = time(NULL);
        gmtime_s(&lasttime, &ptime);
    }
}

void eve_example(void)
{
    // Initialise the display
    EVE_Init();

    // Calibrate the display
    printf("Calibrating display...\n");
    eve_calibrate();

    // Start example code
    printf("Starting demo:\n");
    eve_display();
}
