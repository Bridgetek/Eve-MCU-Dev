/**
 @file eve_example.ino
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

#include "eve_example.h"

/**
 @brief EVE library handle.
 @details This is the one instance of the EVE library. Available as a global to other files.
 */
### EVE CLASS ### eve;

#ifndef MAX
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#endif
#ifndef MIN
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#endif

// Bitmap handle to use for scaled fonts
#define SCALED_FONT 14

// Speed of simulation... milliseconds per tick
const uint32_t tick = 1000;

const uint16_t furmans_top = 0x8000;

#define ARC_CLOCK_WIDTH (eve.DISP_HEIGHT() / 32)
#define ARC_CYCLE_WIDTH (eve.DISP_HEIGHT() / 32)

#define ARC_GAP(A) ((A) * 8 / 10)

// Colours in hexadecimal RGB
const uint32_t col_timer_active = 0x6666ff;
const uint32_t col_rest_active = 0x444466;

const uint32_t col_interval_active = 0x00ff00;
const uint32_t col_interval_inactive = 0x446644;

const uint32_t col_cycle_active = 0xff0000;
const uint32_t col_cycle_inactive = 0x664444;

const uint32_t col_clock = 0xffffff;

struct eve_font_cache clockfont;

const int cycle_count_max = 99;
const int interval_count_max = 99;
const int timer_max = (59*60) + 59;

const int key_debounce_filter = 3;

// Turn an unsigned integer into a string.
static void uint2str(uint8_t val, char *str)
{
    if (val < 10)
    {
        str[0] = '0' + val % 10;
        str[1] = 0;
    }
    else if (val < 100)
    {
        str[0] = '0' + val / 10;
        str[1] = '0' + val % 10;
        str[2] = 0;
    }
    else
    {
        str[0] = '0' + val / 100;
        str[1] = '0' + val / 10;
        str[2] = '0' + val % 10;
        str[3] = 0;
    }
}

// Turn a number of seconds (val) into a time string.
static void time2str(uint16_t val, char *str)
{
    int i = 0;
    if (val >= 3600)
    {
        if (val >= 36000)
        {
            str[i++] = '0' + ((val / 36000) % 10);
        }
        str[i++] = '0' + ((val / 3600) % 10);
        str[i++] = '.';
        val = (val % 3600);
    }
    str[i++] = '0' + ((val / 600) % 10);
    str[i++] = '0' + ((val / 60) % 10);
    str[i++] = ':';
    str[i++] = '0' + ((val % 60) / 10);
    str[i++] = '0' + (val % 10);
    str[i++] = '\0';
}

static void chardraw(struct eve_font_cache *cache, int16_t x, int16_t y, char ch)
{
    if (cache->legacy)
    {
        // This is a legacy font.
        if ((y > 511) || (x > 511))
        {
            /* ### BEGIN API > 1 ### */
            eve.VERTEX_TRANSLATE_X(x * 16);
            eve.VERTEX_TRANSLATE_Y(y * 16);
            /* ### END API ### */
            eve.VERTEX2II(0, 0, cache->handle, ch);
        }
        else
        {
            eve.VERTEX2II(x, y, cache->handle, ch);
        }
    }
    else
    {
        // This is a RAM font.
        /* ### BEGIN API >= 5 ### */
        eve.BITMAP_SOURCE_H(cache->glyphs[(int)ch] >> 24);
        /* ### END API ### */
        eve.BITMAP_SOURCE(cache->glyphs[(int)ch]);
        if ((y > 511) || (x > 511))
        {
            /* ### BEGIN API > 1 ### */
            eve.VERTEX_TRANSLATE_X(x * 16);
            eve.VERTEX_TRANSLATE_Y(y * 16);
            /* ### END API ### */
            eve.VERTEX2F(0, 0);
        }
        else
        {
            eve.VERTEX2F(x * 16, y * 16);
        }
    }

    /* ### BEGIN API > 1 ### */
    eve.VERTEX_TRANSLATE_X(0);
    eve.VERTEX_TRANSLATE_Y(0);
    /* ### END API ### */
}

static void setupzoom(struct eve_font_cache *cache, uint32_t zoom)
{
    eve.BEGIN(eve.BEGIN_BITMAPS);
    // Manipulate the font display.
    eve.CMD_LOADIDENTITY();
    // Select the handle for the font.
    eve.BITMAP_HANDLE(cache->handle);
    /* ### BEGIN API > 1 ### */
    eve.BITMAP_SIZE_H((((uint32_t)font_getwidth(cache) * zoom) / 0x10000) >> 9, 
        (((uint32_t)font_getheight(cache) * zoom) / 0x10000) >> 9);
    /* ### END API ### */
    eve.BITMAP_SIZE(eve.FILTER_NEAREST, eve.WRAP_BORDER, eve.WRAP_BORDER, 
        ((uint32_t)font_getwidth(cache) * zoom) / 0x10000, 
        ((uint32_t)font_getheight(cache) * zoom) / 0x10000);
    eve.CMD_SCALE(zoom, zoom);
    eve.CMD_SETMATRIX();
}

static void stringdraw(struct eve_font_cache *cache, uint32_t zoom, int16_t x, int16_t y, const char *str)
{
    const char *ch = str;

    while (*ch)
    {
        chardraw(cache, x, y, *ch);
    
        // Move the cursor in the x-axis.
        x = x + (((uint32_t)font_getcharwidth(cache, *ch) * zoom) / 0x10000);
        ch++;
    }
}

static uint16_t stringwidth(struct eve_font_cache *cache, uint32_t zoom, const char *str)
{
    uint16_t x = 0;
    int i = 0;
    const char *ch = str;

    while (*ch)
    {
        // Move the cursor in the x-axis.
        x = x + (((uint32_t)font_getcharwidth(cache, *ch) * zoom) / 0x10000);
        ch++;
    }

    return x;
}

void timer_page(int cycle_count, int cycle_rest_count, int interval_count, int interval_rest_count, int timer_count)
{
    // Radius of timerclock arc
    const uint16_t r1_timer = (eve.DISP_WIDTH() / 4);
    const uint16_t r0_timer = (eve.DISP_WIDTH() / 4) - ((ARC_CLOCK_WIDTH * 3) / 2);

    // Radius of interval counter arc
    const uint16_t r1_interval = ARC_GAP(eve.DISP_WIDTH() / 4);
    const uint16_t r0_interval = ARC_GAP((eve.DISP_WIDTH() / 4) - ARC_CLOCK_WIDTH);

    // Radius of cycle counter arc
    const uint16_t r1_cycle = ARC_GAP(ARC_GAP(eve.DISP_WIDTH() / 4));
    const uint16_t r0_cycle = ARC_GAP(ARC_GAP((eve.DISP_WIDTH() / 4) - ARC_CYCLE_WIDTH));

    // Full extents of clock image arcs
    const uint16_t r_inner = (eve.DISP_WIDTH() / 6);
    const uint16_t r_outer = (eve.DISP_WIDTH() / 4) + ARC_CLOCK_WIDTH;

    // Centre point for clock arcs
    const int16_t centre_x = eve.DISP_WIDTH() / 2;
    const int16_t centre_y = eve.DISP_HEIGHT() / 2;

    int i = 0;
    
    int cycle = cycle_count;
    int interval = interval_count;
    int timer = timer_count;
    int quit_rest_count = 10;

    int rest = 0;
    int rest_max;
    int skip = 0;
    int pause = 0;

    // Touchscreen debounce
    int key_debounce = 0;
    uint8_t key_prev =0;

    uint32_t curr_time = platform_get_time();
    uint32_t prev_time = curr_time;
    uint32_t key_time = curr_time;

    // Zoom for buttons
    uint32_t button_zoom = (0x10000 * (centre_x - (r_outer / 2))) / (font_getwidth(&clockfont) * 8);
    int16_t button_x1_zoom = (centre_x - r_outer) - ((4 * font_getwidth(&clockfont) * button_zoom) / 0x10000);
    int16_t button_x2_zoom = (centre_x + r_outer) + ((font_getwidth(&clockfont) * button_zoom) / 0x10000);
    int16_t button_y_zoom = centre_y - (((font_getheight(&clockfont) * button_zoom) / 0x10000) / 2);
    int16_t button_y1_zoom = (centre_y / 2) - (((font_getheight(&clockfont) * button_zoom) / 0x10000) / 2);
    int16_t button_y2_zoom = ((centre_y * 3) / 2) - (((font_getheight(&clockfont) * button_zoom) / 0x10000) / 2);
    int16_t button_w_zoom = (4 * font_getwidth(&clockfont) * button_zoom) / 0x10000;
    int16_t button_h_zoom = (font_getheight(&clockfont) * button_zoom) / 0x10000;

    // Initial message... motivate
    const char *msg_text = "Start!";
    uint8_t msg_timer = 0;
    uint8_t msg_alpha = 0;

    while ((cycle > 0) || (rest > 0))
    {
        curr_time = platform_get_time();

        if (curr_time >= prev_time + tick)
        {
            // Ensure that timing is accurate
            prev_time = prev_time + tick;

            // Flash on/off display when paused
            if (pause == 1)
            {
                pause = 2;
            }
            else if (pause == 2)
            {
                pause = 1;
            }
            else
            {
                // Update cycle, interval and timer values
                if (rest == 0)
                {
                    if (timer == 0)
                    {
                        timer = timer_count;
                        if (interval == 0)
                        {
                            interval = interval_count;
                        }
                    }
                    else
                    {
                        timer--;
                        if ((timer < 4) && (timer > 0))
                        {
                            playClick();
                        }

                        if ((timer == 0) || (skip))
                        {
                            interval--;
                            if (interval > 0)
                            {
                                rest = interval_rest_count;
                                rest_max = interval_rest_count;

                                if (skip == 0)
                                {
                                    msg_text = "Interval Complete";
                                    playChimes(NOTE_C3);
                                }
                            }
                            else if (cycle > 0)
                            {
                                cycle--;
                                if (cycle > 0)
                                {
                                    // Finished all cycles
                                    rest = cycle_rest_count;
                                    rest_max = cycle_rest_count;

                                    msg_text = "Cycle Complete";
                                    playChimes(NOTE_C3);
                                }
                                else
                                {
                                    rest = cycle_rest_count;
                                    rest_max = cycle_rest_count;

                                    msg_text = "Workout Complete";
                                    playChimes(NOTE_C3);
                                }
                            }
                            if (skip)
                            {
                                timer = timer_count;
                            }
                        }
                    }
                }
                else if (rest > 0)
                {
                    rest--;
                    if ((rest < 4) && (rest > 0))
                    {
                        playPip(NOTE_C3);
                    }
                    else if (rest == 0)
                    {
                        playBell(NOTE_C3);
                    }

                    if (skip)
                    {
                        rest = 0;
                    }
                }
        
                skip = 0;
            }

            if (msg_timer > 0)
            {
                msg_timer--;
            }
        }

        // Clock string to show in countdown
        char clock[8];

        // Use the rest timer if we are resting or the countdown timer
        int clock_val;
        if (rest == 0)
        {
            clock_val = timer;
        }
        else
        {
            clock_val = rest;
        }

        // Number of digits to show inside the smaller ring
        int digits = 1;            // 1 digit "9"
        if (clock_val > 9) digits++;   // 2 digits "21"
        if (clock_val > 59) digits++;  // 3 digits "1:23"
        if (clock_val > 599) digits++; // 4 digits "12:56"

        if (digits > 2)
        {
            time2str(clock_val, clock);
        }
        else
        {
            uint2str(clock_val, clock);
        }

        // Work out maximum zoom for the clock string
        uint32_t clock_zoom = 0x10000;
        
        if (clock_val < 60)
        {
            // Magnify based on height e.g. "23"
            clock_zoom = (clock_zoom * r_inner) / font_getheight(&clockfont);
        }
        else
        {
            // Magnify based on 4 times the width e.g. "45:15"
            clock_zoom = (clock_zoom * r_inner * 2) / (4 * font_getwidth(&clockfont));
        }

        // Calculate origin for zoomed text
        int16_t clock_x_zoom = centre_x - (stringwidth(&clockfont, clock_zoom, clock) / 2);
        int16_t clock_y_zoom = centre_y - (((font_getheight(&clockfont) * clock_zoom) / 0x10000) / 2);
            
        eve.LIB_BeginCoProList();
        eve.CMD_DLSTART();
        eve.CLEAR_COLOR_RGB(0, 0, 0);
        eve.CLEAR(1,1,1);

        eve.TAG_MASK(1);
        eve.BEGIN(eve.BEGIN_RECTS);
        // Don't show on screen
        eve.COLOR_MASK(0, 0, 0, 0);
        eve.TAG(1);
        // Finish active area
        /* ### BEGIN API > 1 ### */
        eve.VERTEX_TRANSLATE_X(button_x1_zoom * 16);
        eve.VERTEX_TRANSLATE_Y(button_y_zoom * 16);
        eve.VERTEX2F(0, 0);
        eve.VERTEX_TRANSLATE_X((button_x1_zoom + button_w_zoom) * 16);
        eve.VERTEX_TRANSLATE_Y((button_y_zoom + button_h_zoom) * 16);
        eve.VERTEX2F(0, 0);
        /* ### END API ### */
        /* ### BEGIN API == 1 ### */
        eve.VERTEX2F(button_x1_zoom, button_y_zoom);
        eve.VERTEX2F(button_x1_zoom + button_w_zoom, button_y_zoom + button_h_zoom);
        /* ### END API ### */
        eve.TAG(2);
        // Skip active area
        /* ### BEGIN API > 1 ### */
        eve.VERTEX_TRANSLATE_X(button_x2_zoom * 16);
        eve.VERTEX_TRANSLATE_Y(button_y1_zoom * 16);
        eve.VERTEX2F(0, 0);
        eve.VERTEX_TRANSLATE_X((button_x2_zoom + button_w_zoom) * 16);
        eve.VERTEX_TRANSLATE_Y((button_y1_zoom + button_h_zoom) * 16);
        eve.VERTEX2F(0, 0);
        /* ### END API ### */
        /* ### BEGIN API == 1 ### */
        eve.VERTEX2F(button_x2_zoom, button_y1_zoom);
        eve.VERTEX2F(button_x2_zoom + button_w_zoom, button_y1_zoom + button_h_zoom);
        /* ### END API ### */
        eve.TAG(3);
        // Pause active area
        /* ### BEGIN API > 1 ### */
        eve.VERTEX_TRANSLATE_X(button_x2_zoom * 16);
        eve.VERTEX_TRANSLATE_Y(button_y2_zoom * 16);
        eve.VERTEX2F(0, 0);
        eve.VERTEX_TRANSLATE_X((button_x2_zoom + button_w_zoom) * 16);
        eve.VERTEX_TRANSLATE_Y((button_y2_zoom + button_h_zoom) * 16);
        eve.VERTEX2F(0, 0);
        /* ### END API ### */
        /* ### BEGIN API == 1 ### */
        eve.VERTEX2F(button_x2_zoom, button_y2_zoom);
        eve.VERTEX2F(button_x2_zoom + button_w_zoom, button_y2_zoom + button_h_zoom);
        /* ### END API ### */
        eve.COLOR_MASK(1, 1, 1, 1);
        eve.TAG_MASK(0);

        // Draw countdown
        eve.COLOR(col_clock);

        // Flash remaining time if paused
        if (pause < 2)
        {
            setupzoom(&clockfont, clock_zoom);
            stringdraw(&clockfont, clock_zoom, clock_x_zoom, clock_y_zoom, clock);
        }

        eve.TAG_MASK(1);
        setupzoom(&clockfont, button_zoom);
        eve.TAG(1);
        stringdraw(&clockfont, button_zoom, 
            button_x1_zoom, 
            button_y_zoom, "FINISH");
        eve.TAG(2);
        stringdraw(&clockfont, button_zoom, 
            button_x2_zoom, 
            button_y1_zoom, "SKIP");
        eve.TAG(3);
        if (pause == 0)
        {
            stringdraw(&clockfont, button_zoom, 
                button_x2_zoom, 
                button_y2_zoom, "PAUSE");
        }
        else
        {
            stringdraw(&clockfont, button_zoom, 
                button_x2_zoom, 
                button_y2_zoom, "GO!");
        }
        eve.TAG_MASK(0);

        if (cycle_count > 1)
        {
            // Draw segments around the circle - Cycles remaining
            for (i = 0; i < cycle_count; i++)
            {
                if (i < cycle)
                {
                    eve.COLOR(col_cycle_active);
                }
                else
                {
                    eve.COLOR(col_cycle_inactive);
                }
                arc_simple(centre_x, centre_y, 
                    r0_cycle, r1_cycle, 
                    furmans_top + ((i * 0x10000) / cycle_count) + (0x10000 / (r1_cycle * 8 / ARC_CYCLE_WIDTH)), 
                    furmans_top + (((i + 1) * 0x10000) / cycle_count) - (0x10000 / (r1_cycle * 8 / ARC_CYCLE_WIDTH)) 
                );
            }
        }

        if (interval_count > 1)
        {
            // Draw segments around the circle - Intervals remaining
            for (i = 0; i < interval_count; i++)
            {
                if (i < interval)
                {
                    eve.COLOR(col_interval_active);
                }
                else
                {
                    eve.COLOR(col_interval_inactive);
                }
                arc_simple(centre_x, centre_y, 
                    r0_interval, r1_interval, 
                    furmans_top + ((i * 0x10000) / interval_count) + (0x10000 / (r1_interval * 8 / ARC_CLOCK_WIDTH)), 
                    furmans_top + (((i + 1) * 0x10000) / interval_count) - (0x10000 / (r1_interval * 8 / ARC_CLOCK_WIDTH)) 
                );
            }
        }

        if (rest == 0)
        {
            eve.COLOR(col_timer_active);
            arc_simple(centre_x, centre_y, 
                r0_timer, r1_timer,
                furmans_top, furmans_top + (0x10000 * timer) / timer_count
            );
        }
        else
        {
            eve.COLOR(col_rest_active);
            if (rest < rest_max)
            {
                arc_simple(centre_x, centre_y, 
                    r0_timer, r1_timer,
                    furmans_top + (0x10000 * rest) / rest_max, furmans_top
                );
            }
        }
        
        if (msg_text)
        {
            uint32_t msg_zoom = (0x10000 * eve.DISP_WIDTH()) / (stringwidth(&clockfont, 0x10000, msg_text)) / 2;

            if (msg_alpha == 0)
            {
                msg_alpha = 255;
                msg_timer = 5;
            }

            eve.COLOR(col_clock);
            eve.COLOR_A(msg_alpha);

            setupzoom(&clockfont, msg_zoom);
            stringdraw(&clockfont, msg_zoom, 
                centre_x - (stringwidth(&clockfont, msg_zoom, msg_text) / 2), 
                (eve.DISP_HEIGHT() / 32), msg_text);

            if (msg_timer == 0)
            {
                if (msg_alpha > 16)
                {
                    msg_alpha -= 16;
                }
                else
                {
                    msg_text = NULL;
                    msg_alpha = 0;
                }
            }
        }

        eve.DISPLAY();
        eve.CMD_SWAP();
        eve.LIB_EndCoProList();
        eve.LIB_AwaitCoProEmpty();

        uint8_t key = 0;
        if (eve_read_tag(&key))
        {
            key_debounce++;
            if ((key_debounce > key_debounce_filter) && (key != key_prev))
            {
                key_prev = key;
                if (key == 1)
                {
                    if (cycle > 0)
                    {
                        // Finish
                        cycle = 0;

                        rest = quit_rest_count;
                        rest_max = quit_rest_count;
                        
                        msg_text = "Quit";
                    }
                    else
                    {
                        // Immediate finish
                        rest = 0;
                        rest_max = 0;
                    }
                }
                else if (key == 2)
                {
                    // Skip
                    skip = 1;
                    pause = 0;

                    msg_text = "Skipped";
                }
                else if (key == 3)
                {
                    // Pause
                    if (pause == 0)
                    {                
                        pause = 1;

                        msg_text = "Pause";
                    }
                    else
                    {
                        pause = 0;

                        msg_text = "Start";
                    }
                }
            }
        }
        else
        {
            key_debounce = 0;
            key_prev = 0;
        }
    }
}

void setup_page(int *cycle_count, int *cycle_rest_count, int *interval_count, int *interval_rest_count, int *timer_count)
{
    int i;
    uint32_t text_zoom;
    uint32_t num_zoom;
    uint32_t time_zoom;

    // Make the setup screen fit on the screen. 
    // There need to be 14 rows of text or 30 characters width (whichever is smaller).
    text_zoom = MIN((0x10000 * (eve.DISP_WIDTH()) / (font_getwidth(&clockfont) * 30)),
        (0x10000 * (eve.DISP_HEIGHT()) / (font_getheight(&clockfont) * 14)));
    num_zoom = text_zoom * 3;
    time_zoom = text_zoom * 2;

    const char *msg_text;
    char num_text[16];
    uint8_t selected = 1;

    uint32_t col_text = 0xffffff;
    uint32_t col_selected = 0x00cc00;
    uint32_t col_idle = 0x000000;

    uint32_t dial_prev = 0;
    uint16_t dial_track = 0;
    const int dial_number_units = 5;
    const int dial_time_units = 60;
    const int dial_time_step = 5;

    // Touchscreen debounce
    int key_debounce = 0;
    uint8_t key_prev =0;

    uint32_t total_time = 0;

    while (1)
    {
        total_time = *cycle_count * (*cycle_rest_count + (*interval_count * (*interval_rest_count + *timer_count)));

        eve.LIB_BeginCoProList();
        eve.CMD_DLSTART();
        eve.CLEAR_COLOR_RGB(0, 0, 0);
        eve.CLEAR(1,1,1);

        eve.TAG_MASK(1);
        eve.BEGIN(eve.BEGIN_RECTS);
        // Top row active rectangles
        for (i = 0; i < 3; i++)
        {
            eve.TAG(i + 1);
            if (selected == i + 1)
            {
                eve.COLOR(col_selected);
            }
            else
            {
                eve.COLOR(col_idle);
            }
            /* ### BEGIN API > 1 ### */
            eve.VERTEX_TRANSLATE_X((eve.DISP_WIDTH() * (1 + (2 * i)) / 8) * 16);
            eve.VERTEX_TRANSLATE_Y((eve.DISP_HEIGHT() / 32) + (((font_getheight(&clockfont) * text_zoom) / 0x10000) * 0) * 16);
            eve.VERTEX2F(0, 0);
            eve.VERTEX_TRANSLATE_X((eve.DISP_WIDTH() * (3 + (2 * i)) / 8) * 16);
            eve.VERTEX_TRANSLATE_Y((eve.DISP_HEIGHT() / 32) + (((font_getheight(&clockfont) * text_zoom) / 0x10000) * 6) * 16);
            eve.VERTEX2F(0, 0);
            /* ### END API ### */
            /* ### BEGIN API == 1 ### */
            eve.VERTEX2F((eve.DISP_WIDTH() * (1 + (2 * i)) / 8) * 16,
                ((eve.DISP_HEIGHT() / 32) + (((font_getheight(&clockfont) * text_zoom) / 0x10000) * 0)) * 16);
            eve.VERTEX2F((eve.DISP_WIDTH() * (3 + (2 * i)) / 8) * 16,
                ((eve.DISP_HEIGHT() / 32) + (((font_getheight(&clockfont) * text_zoom) / 0x10000) * 6)) * 16);
            /* ### END API ### */
        }        
        // Middle row active rectangles
        for (i = 0; i < 2; i++)
        {
            eve.TAG(i + 4);
            if (selected == i + 4)
            {
                eve.COLOR(col_selected);
            }
            else
            {
                eve.COLOR(col_idle);
            }
            /* ### BEGIN API > 1 ### */
            eve.VERTEX_TRANSLATE_X((eve.DISP_WIDTH() * (1 + (2 * i)) / 8) * 16);
            eve.VERTEX_TRANSLATE_Y((eve.DISP_HEIGHT() / 32) + (((font_getheight(&clockfont) * text_zoom) / 0x10000) * 7) * 16);
            eve.VERTEX2F(0, 0);
            EVE_VERTEX_TRANSLATE_X((eve.DISP_WIDTH() * (3 + (2 * i)) / 8) * 16);
            EVE_VERTEX_TRANSLATE_Y((eve.DISP_HEIGHT() / 32) + (((font_getheight(&clockfont) * text_zoom) / 0x10000) * 13) * 16);
            eve.VERTEX2F(0, 0);
            /* ### END API ### */
            /* ### BEGIN API == 1 ### */
            eve.VERTEX2F((eve.DISP_WIDTH() * (1 + (2 * i)) / 8) * 16,
                ((eve.DISP_HEIGHT() / 32) + (((font_getheight(&clockfont) * text_zoom) / 0x10000) * 7)) * 16);
            eve.VERTEX2F((eve.DISP_WIDTH() * (3 + (2 * i)) / 8) * 16,
                ((eve.DISP_HEIGHT() / 32) + (((font_getheight(&clockfont) * text_zoom) / 0x10000) * 13)) * 16);
            /* ### END API ### */
        }

        /* ### BEGIN API > 1 ### */
        eve.VERTEX_TRANSLATE_X(0);
        eve.VERTEX_TRANSLATE_Y(0);
        /* ### END API ### */
        eve.TAG_MASK(0);

        eve.COLOR(col_text);
        setupzoom(&clockfont, text_zoom);

        // Top row titles
        msg_text = "CYCLES";
        stringdraw(&clockfont, text_zoom, 
            (eve.DISP_WIDTH() * 1 / 4) - (stringwidth(&clockfont, text_zoom, msg_text) / 2), 
            (eve.DISP_HEIGHT() / 32) + (((font_getheight(&clockfont) * text_zoom) / 0x10000) * 0), msg_text);

        msg_text = "SETS";
        stringdraw(&clockfont, text_zoom, 
            (eve.DISP_WIDTH() * 2 / 4) - (stringwidth(&clockfont, text_zoom, msg_text) / 2), 
            (eve.DISP_HEIGHT() / 32) + (((font_getheight(&clockfont) * text_zoom) / 0x10000) * 0), msg_text);
        msg_text = "PER CYCLE";
        stringdraw(&clockfont, text_zoom, 
            (eve.DISP_WIDTH() * 2 / 4) - (stringwidth(&clockfont, text_zoom, msg_text) / 2), 
            (eve.DISP_HEIGHT() / 32) + (((font_getheight(&clockfont) * text_zoom) / 0x10000) * 1), msg_text);

        msg_text = "SET";
        stringdraw(&clockfont, text_zoom, 
            (eve.DISP_WIDTH() * 3 / 4) - (stringwidth(&clockfont, text_zoom, msg_text) / 2), 
            (eve.DISP_HEIGHT() / 32) + (((font_getheight(&clockfont) * text_zoom) / 0x10000) * 0), msg_text);
        msg_text = "TIMER";
        stringdraw(&clockfont, text_zoom, 
            (eve.DISP_WIDTH() * 3 / 4) - (stringwidth(&clockfont, text_zoom, msg_text) / 2), 
            (eve.DISP_HEIGHT() / 32) + (((font_getheight(&clockfont) * text_zoom) / 0x10000) * 1), msg_text);

        // Second row titles
        msg_text = "RECOVERY";
        stringdraw(&clockfont, text_zoom, 
            (eve.DISP_WIDTH() * 1 / 4) - (stringwidth(&clockfont, text_zoom, msg_text) / 2), 
            (eve.DISP_HEIGHT() / 32) + (((font_getheight(&clockfont) * text_zoom) / 0x10000) * 7), msg_text);
        stringdraw(&clockfont, text_zoom, 
            (eve.DISP_WIDTH() * 2 / 4) - (stringwidth(&clockfont, text_zoom, msg_text) / 2), 
            (eve.DISP_HEIGHT() / 32) + (((font_getheight(&clockfont) * text_zoom) / 0x10000) * 7), msg_text);

        msg_text = "TIME";
        stringdraw(&clockfont, text_zoom, 
            (eve.DISP_WIDTH() * 1 / 4) - (stringwidth(&clockfont, text_zoom, msg_text) / 2), 
            (eve.DISP_HEIGHT() / 32) + (((font_getheight(&clockfont) * text_zoom) / 0x10000) * 8), msg_text);

        stringdraw(&clockfont, text_zoom, 
            (eve.DISP_WIDTH() * 2 / 4) - (stringwidth(&clockfont, text_zoom, msg_text) / 2), 
            (eve.DISP_HEIGHT() / 32) + (((font_getheight(&clockfont) * text_zoom) / 0x10000) * 8), msg_text);

        // Top row numbers
        setupzoom(&clockfont, num_zoom);

        uint2str(*cycle_count, num_text);
        stringdraw(&clockfont, num_zoom, 
            (eve.DISP_WIDTH() * 1 / 4) - (stringwidth(&clockfont, num_zoom, num_text) / 2), 
            (eve.DISP_HEIGHT() / 32) + (((font_getheight(&clockfont) * text_zoom) / 0x10000) * 2), num_text);

        uint2str(*interval_count, num_text);
        stringdraw(&clockfont, num_zoom, 
            (eve.DISP_WIDTH() * 2 / 4) - (stringwidth(&clockfont, num_zoom, num_text) / 2), 
            (eve.DISP_HEIGHT() / 32) + (((font_getheight(&clockfont) * text_zoom) / 0x10000) * 2), num_text);

        // Second row times
        setupzoom(&clockfont, time_zoom);

        time2str(*cycle_rest_count, num_text);
        stringdraw(&clockfont, time_zoom, 
            (eve.DISP_WIDTH() * 1 / 4) - (stringwidth(&clockfont, time_zoom, num_text) / 2), 
            (eve.DISP_HEIGHT() / 32) + (((font_getheight(&clockfont) * text_zoom) / 0x10000) * 10), num_text);

        time2str(*interval_rest_count, num_text);
        stringdraw(&clockfont, time_zoom, 
            (eve.DISP_WIDTH() * 2 / 4) - (stringwidth(&clockfont, time_zoom, num_text) / 2), 
            (eve.DISP_HEIGHT() / 32) + (((font_getheight(&clockfont) * text_zoom) / 0x10000) * 10), num_text);

        // Top row time
        time2str(*timer_count, num_text);
        stringdraw(&clockfont, time_zoom, 
            (eve.DISP_WIDTH() * 3 / 4) - (stringwidth(&clockfont, time_zoom, num_text) / 2), 
            (eve.DISP_HEIGHT() / 32) + (((font_getheight(&clockfont) * text_zoom) / 0x10000) * 3), num_text);

        // Active areas to adjust values
        // tag and add a tracker to the arc gauge point
        eve.TAG_MASK(1); // enable tagging
        eve.TAG(100);
        arc_simple_gauge(
            (eve.DISP_WIDTH() * 3 / 4), 
            (eve.DISP_HEIGHT() / 32) + (((font_getheight(&clockfont) * text_zoom) / 0x10000) * 10), 
            ((((font_getheight(&clockfont) * text_zoom) / 0x10000) * 3) * 8) / 10,
            (((font_getheight(&clockfont) * text_zoom) / 0x10000) * 3),
            0x8000, 0x8000,
            dial_track);
        // add tracker in the centre of the point
        eve.CMD_TRACK((eve.DISP_WIDTH() * 3 / 4), 
            (eve.DISP_HEIGHT() / 32) + (((font_getheight(&clockfont) * text_zoom) / 0x10000) * 10),
            1, 1, 100); 

        setupzoom(&clockfont, text_zoom);
        eve.TAG_MASK(0); // disable tagging
        msg_text = "WORKOUT LENGTH:  ";
        stringdraw(&clockfont, text_zoom, 
            (eve.DISP_WIDTH() * 1 / 4), 
            (eve.DISP_HEIGHT() / 32) + (((font_getheight(&clockfont) * text_zoom) / 0x10000) * 14), msg_text);
        time2str(total_time, num_text);
        stringdraw(&clockfont, text_zoom, 
            (eve.DISP_WIDTH() * 1 / 4) + (stringwidth(&clockfont, text_zoom, msg_text)), 
            (eve.DISP_HEIGHT() / 32) + (((font_getheight(&clockfont) * text_zoom) / 0x10000) * 14), num_text);


        eve.TAG_MASK(1); // enable tagging
        msg_text = "GO";
        eve.COLOR(col_selected);
        eve.TAG(101);
        // Start active area
        eve.BEGIN(eve.BEGIN_POINTS);
        eve.POINT_SIZE((stringwidth(&clockfont, time_zoom, msg_text) * 3 / 5) * 16);
        /* ### BEGIN API > 1 ### */
        eve.VERTEX_TRANSLATE_X((eve.DISP_WIDTH() * 3 / 4) * 16);
        eve.VERTEX_TRANSLATE_Y(((eve.DISP_HEIGHT() / 32) + (((font_getheight(&clockfont) * text_zoom) / 0x10000) * 10)) * 16);
        eve.VERTEX2F(0, 0);
        /* ### END API ### */
        /* ### BEGIN API == 1 ### */
        eve.VERTEX2F((eve.DISP_WIDTH() * 3 / 4) * 16,
            (((eve.DISP_HEIGHT() / 32) + (((font_getheight(&clockfont) * text_zoom) / 0x10000) * 10)) * 16));
        /* ### END API ### */

        eve.COLOR(col_text);
        setupzoom(&clockfont, time_zoom);
        stringdraw(&clockfont, time_zoom, 
            (eve.DISP_WIDTH() * 3 / 4) - (stringwidth(&clockfont, time_zoom, msg_text) / 2), 
            (eve.DISP_HEIGHT() / 32) + (((font_getheight(&clockfont) * text_zoom) / 0x10000) * 9), msg_text);
        eve.TAG_MASK(0); 

        eve.DISPLAY();
        eve.CMD_SWAP();
        eve.LIB_EndCoProList();
        eve.LIB_AwaitCoProEmpty();

        uint8_t key = 0;
        if (eve_read_tag(&key))
        {
            key_debounce++;
            if ((key_debounce > key_debounce_filter) && (key != key_prev))
            {
                key_prev = key;
                if ((key > 0) && (key < 100))
                {
                    selected = key;
                }
                else if (key == 101)
                {
                    // Start!
                    playBell(NOTE_C3);
                    break;
                }
            }
            
            if (key == 100)
            {
                uint32_t trackVal;
                // register has a differnt name for the FT80x series
                /* ### BEGIN API > 1 ### */
                trackVal = eve.LIB_MemRead32(eve.REG_TRACKER);
                /* ### END API ### */
                /* ### BEGIN API == 1 ### */
                trackVal = eve.LIB_MemRead32(eve.REG_TRACK);
                /* ### END API ### */
                trackVal >>= 16;

                int isTime = 0;
                int units;
                int step;
                int adjust = 0;
                uint32_t dial_pos;

                if (selected >= 3)
                {
                    isTime = 1;
                    units = dial_time_units / dial_time_step;
                    step = dial_time_step;
                }
                else
                {
                    isTime = 0;
                    units = dial_number_units;
                    step = 1;
                }

                dial_pos = (int)((trackVal * units) / 0x10000);
                
                // Find if the current rotation point is near the fully rotated position
                if (((dial_pos == 0) && (dial_prev == units - 1)) ||
                    (dial_pos == dial_prev + 1))
                {
                    adjust = +step;
                    playClick();
                }
                else if (((dial_pos == units - 1) && (dial_prev == 0)) ||
                    (dial_pos == dial_prev - 1))
                {
                    adjust = -step;
                    playClick();
                }
                dial_prev = dial_pos;

                // Update the cycles count
                if (selected == 1)
                {
                    *cycle_count += adjust;
                    if (*cycle_count < 1) *cycle_count = 1;
                    if (*cycle_count > cycle_count_max) *cycle_count = cycle_count_max;
                }
                else if (selected == 2)
                {
                    *interval_count += adjust;
                    if (*interval_count < 1) *interval_count = 1;
                    if (*interval_count > interval_count_max) *interval_count = interval_count_max;
                }
                else if (selected == 3) 
                {
                    *timer_count += adjust;
                    if (*timer_count < dial_time_step) *timer_count = dial_time_step;
                    if (*timer_count > timer_max) *timer_count = timer_max;
                }
                else if (selected == 4) 
                {
                    *cycle_rest_count += adjust;
                    if (*cycle_rest_count < dial_time_step) *cycle_rest_count = dial_time_step;
                    if (*cycle_rest_count > timer_max) *cycle_rest_count = timer_max;
                }
                else if (selected == 5) 
                {
                    *interval_rest_count += adjust;
                    if (*interval_rest_count < dial_time_step) *interval_rest_count = dial_time_step;
                    if (*interval_rest_count > timer_max) *interval_rest_count = timer_max;
                }

                dial_track = (trackVal + furmans_top) & 0xffff;
            }
        }
    }
}

void eve_display(void)
{
    // Standard setup values
    int cycle_count = 3;
    int cycle_rest_count = 60;
    int interval_count = 4;
    int interval_rest_count = 15;
    int timer_count = 90;

    while (1)
    {
        setup_page(&cycle_count, &cycle_rest_count, &interval_count, &interval_rest_count, &timer_count);
        timer_page(cycle_count, cycle_rest_count, interval_count, interval_rest_count, timer_count);
    }
}

void eve_example(void)
{
    // Setup the EVE library (### EVE RES ###)
    eve.setup(### EVE RES ###);
    // Initialise the display
    eve.Init();

    // Get a cache of the ROM font we want to use.
    font_getfontinforom(&clockfont, eve.ROMFONT_MAX);

    // Map the ROM font we are using onto a normal bitmap.
    eve.LIB_BeginCoProList();
    eve.CMD_DLSTART();
    /* ### BEGIN API == 1 ### */
    EVE_CMD_ROMFONT(SCALED_FONT, eve.ROMFONT_MAX);
    /* ### END API ### */
    /* ### BEGIN API > 1 ### */
    eve.CMD_ROMFONT(SCALED_FONT, eve.ROMFONT_MAX);
    /* ### END API ### */
    eve.DISPLAY();
    eve.CMD_SWAP();
    eve.LIB_EndCoProList();
    eve.LIB_AwaitCoProEmpty();

    // Tell the program to use the mapped handle instead of the ROM font handle.
    clockfont.handle = SCALED_FONT;

    // Calibrate the display
    Serial.print("Calibrating display...\n");
    if (eve_calibrate() != 0)
    {
        Serial.print("Exception...\n");
        while(1);
    }

    // Start example code
    Serial.print("Starting demo...\n");
    eve_display();
}
