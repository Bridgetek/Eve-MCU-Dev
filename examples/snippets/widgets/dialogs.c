/**
 @file dialogs.c
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

#include "dialogs.h"

/* Array containing the bitmap sizes of ROM fonts. */

// Note: this is for ROM fonts only. To get a more accurate size
//  use the snippet in "controls/fonts.c". Use font_getheight and
//  font_getcharwidth to get more accurate text box sizing.

static const uint8_t rom_font_widths[] = EVE_ROMFONT_WIDTHS;
static const uint8_t rom_font_heights[] = EVE_ROMFONT_HEIGHTS;

static uint8_t romfont_width(uint8_t font)
{
    return rom_font_widths[font];
}

static uint8_t romfont_height(uint8_t font)
{
    return rom_font_heights[font];
}

/*
 * font - Font used for measuring the text.
 * options - Note that the OPT_CENTER, OPT_BASELINE and OPT_RIGHTX options are valid for
 *           CMD_TEXTSIZE. The width and height of the text is not affected by the alignment settings.
 * message - Text string to measure. 
 * w - Return the maximum width of the text in all lines, in pixels.
 * h - Return the height of the text, in pixels.
 */
void textsize(uint16_t font, uint16_t options, const char *message, uint16_t *w, uint16_t *h)
{
    // TODO: check 16 <= font <= EVE_ROMFONT_MAX
    (void)options;
    const char *pmes = message;
    uint16_t maxw = 0;
    uint16_t linew = 0;
    uint16_t maxh = 1;
    while (*pmes)
    {
        linew++;
        if (*pmes++ == '\n')
        {
            maxh++;
            linew = 0;
        }
        if (linew > maxw)
        {
            maxw = linew;
        }
    }
    *w = (maxw * romfont_width((uint8_t)font));
    *h = ((maxh + 1) * romfont_height((uint8_t)font));
}

/*
 * font - Font used for the text. 
 * options - By default, the message box is drawn with a 3D effect unless the OPT_FLAT option is set.
 *           The OPT_MSGTOP will draw the message box towards the top of the screen to allow for
 *           subsequent graphics elements to be drawn below. Similarly, the OPT_MSGBOTTOM will draw
 *           in the bottom part of the screen. If neither are specified, then the message box will be drawn
 *           in the centre of the screen. If OPT_MSGEDGE is set, then the message box will be drawn at
 *           to top edge for OPT_MSGTOP and the bottom edge for OPT_MSGBOTTOM. This allows for
 *           five different positions on the screen for the dialog box.
 * message - Text string to display.
 */
void messagebox(int16_t font, uint16_t options, const char *message)
{
    int16_t x,y;
    uint16_t w,h;
    textsize(font, options, message, &w, &h);
    x = (EVE_DISP_WIDTH / 2);
    if (options & EVE_OPT_MSGTOP)
    {
        if (options & EVE_OPT_MSGEDGE)
        {
            y = (h / 2);
        }
        else
        {
            y = EVE_DISP_HEIGHT / 3;
        }
    }
    else if (options & EVE_OPT_MSGBOTTOM)
    {
        if (options & EVE_OPT_MSGEDGE)
        {
            y = EVE_DISP_HEIGHT - (h / 2);
        }
        else
        {
            y = ((EVE_DISP_HEIGHT * 2) / 3);
        }
    }
    else
    {
        y = EVE_DISP_HEIGHT / 2;
    }
    EVE_CMD_BUTTON(x - (w / 2), y - (h / 2), w, h, font, (options & EVE_OPT_FLAT), message);
}

/*
 * x - x-coordinate of tooltip box centre point, in pixels.
 * y - y-coordinate of tooltip box centre point, in pixels.
 * font - Font used for the text. 
 * options - By default, the message box is drawn with a 3D effect unless the OPT_FLAT option is set.
 *           The OPT_MSGTOP will draw the tooltip box above the centre point in x,y. Similarly, the
 *           OPT_MSGBOTTOM will draw the tooltip below. If neither are specified then the tooltip box
 *           will be drawn centred.
 * message - Text string to display.
 */
void tooltip(int16_t x, int16_t y, int16_t font, uint16_t options, const char *message)
{
    uint16_t w,h;
    textsize(font, options, message, &w, &h);
    if (options & EVE_OPT_MSGTOP)
    {
        y -= h;
        if (y < 0)
        {
            y = 0;
        }
    }
    else if (options & EVE_OPT_MSGBOTTOM)
    {
        y += h;
        if (y > EVE_DISP_HEIGHT)
        {
            y = EVE_DISP_HEIGHT;
        }
    }
    x = x - w/2;
    if (x < 0)
    {
        x = 0;
    }
    else if ((x + w) > EVE_DISP_WIDTH)
    {
        x =  EVE_DISP_WIDTH - w;
    }
    EVE_CMD_BUTTON(x, y, w, h, font, (options & EVE_OPT_FLAT), message);
}
