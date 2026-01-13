
/**
 @file eve_example.c
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
#include <stdio.h>
#include <stdlib.h>

#include <EVE.h>
#include <MCU.h> // For DEBUG_PRINTF only

#include "eve_example.h"

#include "sevenseg.h"

// ######################################################################################################################################################################################################
// #######################################################            Set properties of all of the assets to be loaded                ###################################################################
// ######################################################################################################################################################################################################

#if IS_EVE_API(1,2)
#error This project needs EVE API 3, 4 or 5 (BT81x, BT82x).
#endif

#if EVE_DISP_WIDTH < 1920 || EVE_DISP_HEIGHT < 1024
#error This project requires a screen of at least 1920x1024 pixels.
#endif

// Handle for icon cell images
const uint32_t handle_icons_large = 0;
const uint32_t handle_icons_small = 1;
const uint32_t icons_large = 200;
const uint32_t icons_small = 100;

// Number of pixels between traces
const uint32_t trace_gap = 290;
// Width of the trace line
const uint32_t trace_width = 50;
// Top left of trace area
const uint32_t trace_x = 100;
const uint32_t trace_y = 20;
// Grid line width
const uint32_t grid_width = 16;
// Sweep step - data points to scroll each time (must be < TRACE_STEP)
uint32_t sweep_step = 8;
// Number of points to leave as a gap when updating
const uint32_t sweep_gap = 50;

// Scale X axis of traces by 1.75 0x1c000 (800 points go to 1400 pixels)
// Scale X axis of traces by 1.25 0x14000 (800 points go to 1000 pixels)
const uint32_t trace_x_scale = 0x14000;
// Scale Y axis of traces by 0.75 0xc000 (256 points go to 192 pixels)
const uint32_t trace_y_scale = 0x10000;

// Top left of info area
const uint32_t info_x = 1200;
const uint32_t info_y = 20 + ((280 / 2)) - 80;//(((80 * 28) / 10)) / 2);
// Number of pixels between info blocks
const uint32_t info_gap = 290;
// Number of pixels between 7 segment digits
const uint32_t info_pitch = 110;
const uint32_t info_size = 160;
// Size of 7 segment digits
// Size of segment on seven seg is info_size / 2.0
const uint32_t info_seg_size = (160 / 2);

// Number of pixels for extra area
const uint32_t extra_max = 250;
// Centre of extra area - align with centre of HRM seven segs
const uint32_t extra_x = EVE_DISP_WIDTH - (450 / 2);
const uint32_t extra_y = 20 + ((80 * 20) / 10);
const uint32_t extra_col = 50;
// Scale to use to "pulse" heart icon
const uint32_t pulse_scale = 100;

const uint8_t pulse_threshold = 192;
const uint8_t pulse_hysteresis = 16;
const uint8_t resp_threshold = 192;
const uint8_t resp_hysteresis = 16;

// Array to receive leading edge trace data
// It is ONE item larger to hold extra status info
uint8_t sweep_edge[trace_total][TRACE_STEP];

// Updated touchscreen and status info data loaded into sweep_edge
EVE_TRACE_PROPS infostatus;

// Array to receive touchscreen and status info
uint32_t receivedata[3];

EVE_ASSET_PROPS patch_asset;
EVE_ASSET_PROPS icon_assets_small[icons_total_small];
EVE_ASSET_PROPS icon_assets_large[icons_total_large];

// Built-in fonts
const uint8_t widths[] = EVE_ROMFONT_WIDTHS;
const uint8_t heights[] = EVE_ROMFONT_HEIGHTS;
const uint8_t font_label = 30;
const uint8_t font_dialog = 32;
const uint8_t font_readout = 25;

// Predefined colours used
const uint32_t white = 0xffffff;
const uint32_t grey = 0x808080;
const uint32_t black = 0x000000;
const uint32_t red = 0xff0000;
const uint32_t green = 0x00ff00;
const uint32_t blue = 0x0000ff;
const uint32_t red_dark = 0x220000;
const uint32_t green_dark = 0x002200;
const uint32_t blue_dark = 0x000033;
const uint32_t orange = 0xffaa00;
const uint32_t purple = 0xaaaaff;

// Colours of the traces
uint32_t trace_colour[trace_total];

// Key tags
const uint8_t key_action = 100;
const uint8_t key_sentiment = 101;
const uint8_t key_lock = 102;
const uint8_t key_volume = 103;
const uint8_t key_brightness = 105;
const uint8_t key_off_on = 106;
const uint8_t key_silence = 107;
const uint8_t key_dialog = 199;

// Maximum frame rate 
// 33 ms is 30 fps
// 100 ms is 10 fps
const uint32_t frame_ms = 33;

// Periodic update
const uint32_t update_period_ms = 1000;

// Brightness range - Note PWM max is 0x80.
const uint16_t brightness_range = 96;
const uint16_t brightness_min = 32;

void eve_display_load_assets(void)
{
    uint32_t last = 0;
    int i;

#if ASSETS == USE_FLASH
    eve_flash_full_speed();
#endif

    // Images
    // Notional width and height of icons
    for (i = 0; i < icons_total_large; i++)
    {
        icon_assets_large[i].Width = icons_large;
        icon_assets_large[i].Height = icons_large;
        icon_assets_large[i].CellHeight = icons_large;
        icon_assets_large[i].Handle = handle_icons_large;
        icon_assets_large[i].Cell = i;
    }

    for (i = 0; i < icons_total_small; i++)
    {
        icon_assets_small[i].Width = icons_small;
        icon_assets_small[i].Height = icons_small;
        icon_assets_small[i].CellHeight = icons_small;
        icon_assets_small[i].Handle = handle_icons_small;
        icon_assets_small[i].Cell = i;
    }

    for (i = 0; i < icons_total_large; i++)
    {
        icon_assets_large[i].RAM_G_Start = last;
        eve_asset_load(&icon_assets_large[i], EVE_FORMAT_ARGB4); 
        last = icon_assets_large[i].RAM_G_EndAddr;
    }

    for (i = 0; i < icons_total_small; i++)
    {
        icon_assets_small[i].RAM_G_Start = last;
        eve_asset_load(&icon_assets_small[i], EVE_FORMAT_ARGB4); 
        last = icon_assets_small[i].RAM_G_EndAddr;
    }

    infostatus.RAM_G_Start = last;
    infostatus.RAM_G_EndAddr = infostatus.RAM_G_Start + (sizeof(uint32_t) * 3);
    last = infostatus.RAM_G_EndAddr;

    graph_trace_init(last);
}

void configure_bitmaps(void){

    EVE_LIB_BeginCoProList();
    EVE_CMD_DLSTART();

    // Configure images
    //----------------------------------------------------------------
    EVE_BEGIN(EVE_BEGIN_BITMAPS);

    // Configure images and fonts for use in the screen
    // Do not use a screen background on BT81x when the images are decoded from PNG files
    EVE_BITMAP_HANDLE(icon_assets_large[0].Handle);
    EVE_CMD_SETBITMAP(icon_assets_large[0].RAM_G_Start, icon_assets_large[0].Format, icons_large, icons_large);

    EVE_BITMAP_HANDLE(icon_assets_small[0].Handle);
    EVE_CMD_SETBITMAP(icon_assets_small[0].RAM_G_Start, icon_assets_small[0].Format, icons_small, icons_small);

    // Send display to EVE
    EVE_DISPLAY();
    EVE_CMD_SWAP();
    EVE_LIB_EndCoProList();
    EVE_LIB_AwaitCoProEmpty();
}

void eve_write_pwm(uint32_t val)
{
    EVE_LIB_MemWrite32(EVE_REG_PWM_DUTY, val);
}

void eve_draw_number(int32_t x, int32_t y, uint16_t size, uint16_t digit, uint32_t fgcolour, uint32_t bgcolour)
{
#if DIGITAL_READOUT_METHOD == USE_SEVENSEG_EXTENSIONS
    // Draw a seven segment number using the extension
    EVE_CMD_FGCOLOR(fgcolour);
    EVE_CMD_BGCOLOR(bgcolour);
    EVE_CMD_SEVENSEG(x, y, size, digit);
#elif  DIGITAL_READOUT_METHOD == USE_SEVENSEG_SNIPPETS
    // Draw a seven segment number using the snippet code
    sevensegment(x, y, size, digit, fgcolour, bgcolour);
#else // DIGITAL_READOUT_METHOD == USE_SCALED_FONT
    // Draw a seven segment number using a scaled font
    // Note this must be a legacy font for the VERTEX2II method to work
    uint16_t width = widths[font_readout];
    EVE_COLOR(fgcolour);
    EVE_SAVE_CONTEXT();
    EVE_BEGIN(EVE_BEGIN_BITMAPS);
    EVE_CMD_LOADIDENTITY();
    EVE_BITMAP_HANDLE(font_readout);
    // Enlargen the bitmap canvas
    EVE_BITMAP_SIZE(EVE_FILTER_NEAREST, EVE_WRAP_BORDER, EVE_WRAP_BORDER, 
        size, size * 2);
    EVE_BITMAP_SIZE_H(size >> 9, (size * 2) >> 9);
    EVE_CMD_SCALE(0x10000 + (size * 0x10000 * 3) / (width * 2), 0x10000 + (size * 0x10000 * 3) / (width * 2));
    EVE_CMD_SETMATRIX();
#if IS_EVE_API(2, 3, 4, 5)
    EVE_VERTEX_TRANSLATE_X(x * 16);
    EVE_VERTEX_TRANSLATE_Y(y * 16);
    EVE_VERTEX2II(0, 0, font_readout, ((digit & EVE_OPT_NUMBER) & EVE_OPT_NUMBER) + '0');
#else
    EVE_VERTEX2II(x, y, font_readout, ((digit & EVE_OPT_NUMBER) & EVE_OPT_NUMBER) + '0');
#endif
    // Draw decimal point is asked
    if (digit & EVE_OPT_DECIMAL)
    {
        EVE_BEGIN(EVE_BEGIN_POINTS);
        EVE_POINT_SIZE(size * 16 / 10);
        EVE_VERTEX2F(size + (size / 16), (size * 2) );
    }
    EVE_RESTORE_CONTEXT();
#endif
}

// Patches only apply to BT82x
#if IS_EVE_API(5)

int eve_loadpatch(void)
{
    eve_loadpatch_impl();
    char actual[128];

    EVE_LIB_GetCoProException(actual);
    DEBUG_PRINTF("%s\n", actual);

    return 0;
}

#endif

void eve_display(void)
{
    // Counter for moving sweep position
    uint32_t sweep_counter = 0;
    
    // Width of the gap
    uint32_t update_width = 50;

    // Pulse magnifier for animation
    uint8_t pulse = 0;
    uint8_t pulse_last = 1;
    uint32_t pulse_ms = 0;
    uint32_t pulse_freq = 0;
    // Respiration rate for animation
    uint8_t resp = 0;
    uint8_t resp_last = 1;
    uint32_t resp_ms = 0;
    uint32_t resp_freq = 0;
    // Oxygen sat
    uint16_t oxygen_sat = 0;
    // Sentiment icon
    uint32_t icon_sentiment = icon_good;
    // Action button
    uint32_t icon_updatestate = icon_pause;
    // Lock screen button
    uint32_t icon_touchstate = icon_lock_open;
    // Volume button
    uint32_t icon_volumestate = icon_volume_up;
    // Power button
    int8_t powerstate = 1;

    uint8_t key;
    uint8_t key_last = 0;

    uint32_t i;
    uint32_t k;

    // For timing screen updates
    // The platform ms clock at the conclusion of current frame
    uint32_t current_ms;
    // The platform ms clock at the conclusion of previous frame
    uint32_t previous_ms = 0;
    // Update signal for animations
    uint32_t update_ms = 0;
    // Time taken to send frame data to the coprocessor
    uint32_t render_ms = 0;
    // Update timer for dialogs
    uint32_t dialog_ms = 0;
    // Update timer for tooltips
    uint32_t tooltip_ms = 0;

    // Performance flags
    // Flag for frame rate overrun
    int8_t frame_rate_flag = 0;
    // Instrumentation for time to render frame
    uint32_t frame_render_time = 0;
    
    // Pop-up dialog text
    char *dialog = NULL;
    // Tooltips text
    char *tooltip = NULL;
    uint8_t tooltip_key = 0;
    uint16_t tooltip_x;
    uint16_t tooltip_y;

    // Brightness display position
    uint32_t brightness_val = brightness_range;
    // Tracker for tooltip on slider
    uint16_t brightness_tracker = 0;

    configure_bitmaps();
    eve_write_pwm(brightness_val + brightness_min);

    current_ms = platform_get_time();

    trace_colour[trace_hrm] =  red;
    trace_colour[trace_resp] =  green;
    trace_colour[trace_sat] =  blue;

    while(1)
    {
        // Calculate updated data "received" to display on the traces
        if (icon_updatestate == icon_pause)
        {
            uint32_t trace_offset, source_offset;
            uint32_t mask;

            // Make the trace data appear faster depending on the condition of the patient
            if (icon_sentiment == icon_sick)
            {
                mask = 1;
            }
            else if (icon_sentiment == icon_bad)
            {
                mask = 2;
            }
            else
            {
                // All data
                mask = TRACE_STEP;
            }

            for (i = 0; i < trace_total; i++)
            {
                source_offset = trace[i].SourceCounter;
                trace_offset = sweep_counter;

                uint32_t skip = 0;
                for (k = 0; k < sweep_step; k++)
                {
                    // Update the trace data
                    if (skip >= mask)
                    {
                        // Next data point
                        source_offset++;
                        if (source_offset >= trace[i].SourceSize) source_offset = 0;
                        skip = 0;
                    }
                    else
                    {
                        skip++;
                    }
                    sweep_edge[i][k] = trace[i].Source[source_offset];
                    trace_offset++;
                    if (trace_offset >= TRACE_POINTS) trace_offset = 0;
                    source_offset++;
                    if (source_offset >= trace[i].SourceSize) source_offset = 0;
                }
                trace[i].SourceCounter = source_offset;

                // Write the updated data to the trace in RAM_G
                graph_update(i, sweep_edge[i], sweep_step, sweep_counter);
            }
        }

        EVE_LIB_BeginCoProList();
        EVE_CMD_DLSTART();
        EVE_CLEAR_COLOR_RGB(0, 0, 0);
        EVE_CLEAR(1, 1, 1);
        EVE_VERTEX_FORMAT(0);

        if (powerstate)
        {
            // Draw the grid
            EVE_COLOR(grey);
            EVE_LINE_WIDTH(grid_width);
            EVE_BEGIN(EVE_BEGIN_LINES);
            for (i = 0; i < trace_total; i++)
            {
                for (k = 0; k <= 256; k += 256 / 4)
                {
                    // horizontal
                    EVE_VERTEX2F(trace_x, trace_y + (i * trace_gap) + SCALE(k, trace_y_scale));
                    EVE_VERTEX2F(trace_x + SCALE(TRACE_POINTS, trace_x_scale), trace_y + (i * trace_gap) + SCALE(k, trace_y_scale));
                }

                for (k = 0; k <= (int)TRACE_POINTS; k += (int)TRACE_POINTS / 16)
                {
                    // vertical
                    EVE_VERTEX2F(trace_x + SCALE(k, trace_x_scale), trace_y + (i * trace_gap));
                    EVE_VERTEX2F(trace_x + SCALE(k, trace_x_scale), trace_y + (i * trace_gap) + SCALE(255, trace_y_scale));
                }
            }
            
            // Boundary of FullHD screen drawn on WUXGA screen
            if (EVE_DISP_HEIGHT > 1080)
            {
                EVE_VERTEX2F(0, 1080);
                EVE_VERTEX2F(EVE_DISP_WIDTH, 1080);
            }

            // Draw the three graphs
            EVE_COLOR(white);
            EVE_LINE_WIDTH(trace_width);
            for (i = 0; i < trace_total; i++)
            {
                uint32_t sweep_rpos = sweep_counter + sweep_gap;
                if (sweep_rpos > TRACE_POINTS)
                {
                    sweep_rpos = TRACE_POINTS;
                }
                
                EVE_COLOR(trace_colour[i]);
                graph_draw(i, sweep_counter, sweep_rpos, trace_x, trace_y + (i * trace_gap), trace_x_scale, trace_y_scale);
            }
            EVE_COLOR(green);
            EVE_CMD_FGCOLOR(green);
            EVE_CMD_BGCOLOR(green_dark);
            EVE_CMD_TEXT(info_x, info_y - heights[font_label] - 10, font_label, 0, "HEART RATE - BPM");
            eve_draw_number(info_x, info_y, info_seg_size, (pulse_freq / 100) % 10, green, green_dark);
            eve_draw_number(info_x + (1 * info_pitch), info_y, info_seg_size, (pulse_freq / 10) % 10, green, green_dark);
            eve_draw_number(info_x + (2 * info_pitch), info_y, info_seg_size, (pulse_freq) % 10, green, green_dark);

            EVE_CMD_TEXT(info_x, info_y + (1 * info_gap) - heights[font_label] - 10, font_label, 0, "RESPIRATION");
            eve_draw_number(info_x, info_y + (1 * info_gap), info_seg_size, (resp_freq / 100) % 10, green, green_dark);
            eve_draw_number(info_x + (1 * info_pitch), info_y + (1 * info_gap), info_seg_size, (resp_freq / 10) % 10, green, green_dark);
            eve_draw_number(info_x + (2 * info_pitch), info_y + (1 * info_gap), info_seg_size, (resp_freq ) % 10, green, green_dark);

            EVE_CMD_TEXT(info_x, info_y + (2 * info_gap) - heights[font_label] - 10, font_label, 0, "OXYGEN SAT %");
            eve_draw_number(info_x, info_y + (2 * info_gap), info_seg_size, (oxygen_sat / 100) % 10, green, green_dark);
            eve_draw_number(info_x + (1 * info_pitch), info_y + (2 * info_gap), info_seg_size, (oxygen_sat / 10) % 10 | EVE_OPT_DECIMAL | EVE_OPT_FILL, green, green_dark);
            eve_draw_number(info_x + (2 * info_pitch), info_y + (2 * info_gap), info_seg_size, (oxygen_sat) % 10, green, green_dark);

            // 3 big buttons
            EVE_TAG_MASK(1);
            EVE_BEGIN(EVE_BEGIN_POINTS);
            // button for pulsing heart (not active)
            EVE_COLOR(white);
            EVE_POINT_SIZE(extra_max * 8);
            EVE_VERTEX2F(extra_x, extra_y);
            // button for freeze/unfreeze
            EVE_COLOR(purple);
            EVE_TAG(key_action);
            EVE_POINT_SIZE(extra_max * 8);
            EVE_VERTEX2F(extra_x, extra_y + info_gap);
            // button for cancel alarms
            EVE_COLOR(orange);
            EVE_TAG(key_silence);
            EVE_POINT_SIZE(extra_max * 8);
            EVE_VERTEX2F(extra_x, extra_y + (info_gap * 2));
            EVE_TAG_MASK(0);

            // big button icons
            EVE_BEGIN(EVE_BEGIN_BITMAPS);
            // Pulsing heart
            EVE_COLOR(red);
            EVE_SAVE_CONTEXT();
            EVE_CMD_LOADIDENTITY();
            EVE_BITMAP_HANDLE(icon_assets_large[icon_pulse].Handle);
            EVE_CELL(icon_assets_large[icon_pulse].Cell);
            // Enlargen the bitmap canvas
            EVE_BITMAP_SIZE(EVE_FILTER_NEAREST, EVE_WRAP_BORDER, EVE_WRAP_BORDER, 
                (icons_large * (0x100 + pulse_scale)) / 0x100, 
                (icons_large * (0x100 + pulse_scale)) / 0x100);
            EVE_CMD_SCALE(0x10000 + pulse * pulse_scale, 0x10000 + pulse * pulse_scale);
            EVE_CMD_SETMATRIX();
            EVE_VERTEX2F(extra_x - (((icons_large * (0x10000 + pulse * pulse_scale)) / 0x10000) / 2), 
                        extra_y - (((icons_large * (0x10000 + pulse * pulse_scale)) / 0x10000) / 2));
            EVE_RESTORE_CONTEXT();
            // Freeze/unfreeze
            EVE_TAG_MASK(1);
            EVE_BEGIN(EVE_BEGIN_BITMAPS);
            if ((icon_updatestate == icon_play) && ((uint32_t)(current_ms - update_ms) > update_period_ms / 2))
            {
                EVE_COLOR(purple);
            }
            else
            {
                EVE_COLOR(white);
            }
            EVE_TAG(key_action);
            EVE_BITMAP_HANDLE(icon_assets_large[icon_updatestate].Handle);
            EVE_CELL(icon_assets_large[icon_updatestate].Cell);
            EVE_VERTEX2F(extra_x - (icons_large / 2), 
                        extra_y + info_gap - (icons_large / 2));
            // Cancel alarms
            EVE_COLOR(white);
            EVE_TAG(key_silence);
            EVE_BITMAP_HANDLE(icon_assets_large[icon_silence].Handle);
            EVE_CELL(icon_assets_large[icon_silence].Cell);
            EVE_VERTEX2F(extra_x - (icons_large / 2), 
                        extra_y + (info_gap * 2) - (icons_large / 2));

            // Smaller buttons along bottom
            // red/green button for power
            EVE_COLOR(red);
            EVE_BEGIN(EVE_BEGIN_LINES);
            EVE_LINE_WIDTH(extra_max * 4);
            EVE_TAG(key_lock);
            EVE_VERTEX2F(trace_x - (icons_small / 2) + (icons_small * 12), 
                        trace_y + (trace_gap * 3) + (icons_small / 2));
            EVE_VERTEX2F(trace_x - (icons_small / 2) + (icons_small * 14), 
                        trace_y + (trace_gap * 3) + (icons_small / 2));
            EVE_BEGIN(EVE_BEGIN_POINTS);
            EVE_POINT_SIZE(extra_max * 4);
            // button for emotions
            if (icon_sentiment == icon_sick)
            {
                EVE_COLOR(red);
            }
            else if (icon_sentiment == icon_bad)
            {
                EVE_COLOR(orange);
            }
            else
            {
                EVE_COLOR(green);
            }
            EVE_TAG(key_sentiment);
            EVE_VERTEX2F(trace_x + (icons_small / 2) + (icons_small * 2), 
                        trace_y + (trace_gap * 3) + (icons_small / 2));
            // button for volume
            EVE_COLOR(purple);
            EVE_TAG(key_volume);
            EVE_VERTEX2F(trace_x + (icons_small / 2) + (icons_small * 4), 
                        trace_y + (trace_gap * 3) + (icons_small / 2));

            EVE_BEGIN(EVE_BEGIN_BITMAPS);
            // small off/on icon
            EVE_COLOR(white);
            // small lock/unlock icon
            EVE_TAG(key_lock);
            EVE_BITMAP_HANDLE(icon_assets_small[icon_touchstate].Handle);
            EVE_CELL(icon_assets_small[icon_touchstate].Cell);
            EVE_VERTEX2F(trace_x + (icons_small * 12), 
                        trace_y + (trace_gap * 3));
            // small emotion icon
            EVE_TAG(key_sentiment);
            if (icon_sentiment == icon_sick)
            {
                EVE_COLOR(white);
            }
            else
            {
                EVE_COLOR(black);
            }
            EVE_BITMAP_HANDLE(icon_assets_small[icon_sentiment].Handle);
            EVE_CELL(icon_assets_small[icon_sentiment].Cell);
            EVE_VERTEX2F(trace_x + (icons_small * 2), 
                        trace_y + (trace_gap * 3) );
            EVE_TAG(key_volume);
            EVE_COLOR(white);
            EVE_BITMAP_HANDLE(icon_assets_small[icon_volumestate].Handle);
            EVE_CELL(icon_assets_small[icon_volumestate].Cell);
            EVE_VERTEX2F(trace_x + (icons_small * 4), 
                        trace_y + (trace_gap * 3) );

            EVE_TAG(key_brightness);
            EVE_COLOR(white);
            EVE_CMD_FGCOLOR(green);
            EVE_CMD_BGCOLOR(green_dark);
            EVE_CMD_SLIDER(trace_x + ((icons_small * 6)), 
                        trace_y + (trace_gap * 3) + ((icons_small * 2) / 8),
                        icons_small * 4,
                        ((icons_small * 2) / 4),
                    EVE_OPT_FLAT, brightness_val, brightness_range);
            EVE_CMD_TRACK(trace_x + ((icons_small * 6)), 
                        trace_y + (trace_gap * 3) + ((icons_small * 2) / 8),
                        icons_small * 4,
                        ((icons_small * 2) / 4),
                        key_brightness);
            EVE_TAG_MASK(0);
        }

        EVE_TAG_MASK(1);
        // Smaller buttons along bottom
        EVE_BEGIN(EVE_BEGIN_POINTS);
        EVE_POINT_SIZE(extra_max * 4);
        // red button for power
        EVE_COLOR(red);
        EVE_TAG(key_off_on);
        EVE_VERTEX2F(trace_x + (icons_small / 2) + (icons_small * 0), 
                    trace_y + (trace_gap * 3) + (icons_small / 2));
        EVE_BEGIN(EVE_BEGIN_BITMAPS);
        // small off/on icon
        EVE_COLOR(white);
        EVE_TAG(key_off_on);
        EVE_BITMAP_HANDLE(icon_assets_small[icon_off_on].Handle);
        EVE_CELL(icon_assets_small[icon_off_on].Cell);
        EVE_VERTEX2F(trace_x + (icons_small * 0), 
                    trace_y + (trace_gap * 3));

        if (dialog)
        {
            EVE_COLOR(white);
            EVE_CMD_FGCOLOR(blue);
            EVE_CMD_BGCOLOR(blue_dark);
            EVE_TAG(key_dialog);
            EVE_CMD_MESSAGEBOX(font_dialog, EVE_OPT_FLAT | EVE_OPT_MSGBGALPHA, dialog);
        }
        else if (tooltip)
        {
            EVE_COLOR(white);
            EVE_CMD_FGCOLOR(blue);
            EVE_CMD_BGCOLOR(blue_dark);
            EVE_CMD_TOOLTIP(tooltip_x, tooltip_y, font_dialog, EVE_OPT_MSGTOP | EVE_OPT_FLAT | EVE_OPT_MSGBGALPHA, tooltip);
        }
            
        EVE_TAG_MASK(0);

        // Get touch TAG value
        EVE_CMD_MEMCPY(infostatus.RAM_G_Start, EVE_REG_TOUCH_TAG, sizeof(uint32_t));
        // Get screen touch X,Y coordinates
        EVE_CMD_MEMCPY(infostatus.RAM_G_Start + sizeof(uint32_t), EVE_REG_TOUCH_SCREEN_XY, sizeof(uint32_t));
        // Get display list fullness
        EVE_CMD_MEMCPY(infostatus.RAM_G_Start + (sizeof(uint32_t) * 2), EVE_REG_CMD_DL, sizeof(uint32_t));

        // Send display to EVE
        EVE_DISPLAY();
        EVE_CMD_SWAP();
        EVE_LIB_EndCoProList();
        // Await is implied in the EVE_LIB_BeginCoProList() call on the next frame
        //EVE_LIB_AwaitCoProEmpty();

        // Read in the touchscreen coordinates and DL size
        EVE_LIB_ReadDataFromRAMG((uint8_t *)&receivedata, sizeof(receivedata), infostatus.RAM_G_Start);
        // Touchscreen actions (first word of infostatus trace)
        key = receivedata[0];
        if (key != 0)
        {
            // Move the slider when awaiting tooltip
            if (key == key_brightness)
            {
                uint32_t tracker;
                tracker = EVE_LIB_MemRead32(EVE_REG_TRACKER);
                if ((tracker & 0xff) == key_brightness)
                {
                    uint16_t new_tracker = tracker >> 16;
                    if (key_last == 0)
                    {
                        brightness_tracker = new_tracker;
                        tooltip_ms = current_ms;
                    }

                    if (brightness_tracker == new_tracker)
                    {
                        tooltip_key = 0;
                    }
                    else
                    {
                        brightness_val = ((new_tracker * brightness_range) / 65536);
                        eve_write_pwm(brightness_val + brightness_min);
                        tooltip_ms = current_ms;
                    }
                    brightness_tracker = new_tracker;
                }
            }

            if (key_last == 0)
            {
                // Start a timer for the tooltip detection
                tooltip_ms = current_ms;
                key_last = key;
            }
            else if (tooltip_key == 0)
            {
                if ((uint32_t)(current_ms - tooltip_ms) > update_period_ms)
                {
                    // Tooltip to display
                    tooltip_key = key_last;
                    if (tooltip_key == key_action)
                    {
                        tooltip = "Freeze Screen";
                    }
                    if (tooltip_key == key_volume)
                    {
                        tooltip = "Change Volume";
                    }
                    if (tooltip_key == key_lock)
                    {
                        tooltip = "Lock Touchscreen";
                    }
                    if (tooltip_key == key_sentiment)
                    {
                        tooltip = "Adjust Patient Health";
                    }
                    if (tooltip_key == key_brightness)
                    {
                        tooltip = "Adjust Screen Brightness";
                    }
                    if (tooltip_key == key_off_on)
                    {
                        tooltip = "Power Off the Device";
                    }
                    if (tooltip_key == key_silence)
                    {
                        tooltip = "Cancel Alarms";
                    }
                    // Get the X,Y of the touch from the infostatus array (little endian)
                    tooltip_x = receivedata[1] >> 16;
                    tooltip_y = receivedata[1] & 0xffff;
                    
                    // Keep the tooltip active after release
                    dialog_ms = current_ms;
                }
            }
        }
        else if ((key_last) && (tooltip_key == 0))
        {
            // Action on end of touch (no tooltip)
            key = key_last;
            if (icon_touchstate == icon_lock_open)
            {
                if (key == key_action)
                {
                    if (icon_updatestate == icon_pause)
                    {
                        icon_updatestate = icon_play;
                        dialog = "Screen Frozen";
                    }
                    else
                    {
                        icon_updatestate = icon_pause;
                        dialog = "Screen Restarted";
                    }
                }
                if (key == key_sentiment)
                {
                    icon_sentiment++;
                    if (icon_sentiment > icon_sick)
                    {
                        icon_sentiment = icon_good;
                    }
                }
                if (key == key_volume)
                {
                    icon_volumestate++;
                    if (icon_volumestate > icon_volume_up)
                    {
                        icon_volumestate = icon_volume_off;
                    }
                }
                if (key == key_dialog)
                {
                    // Clear the dialog
                    dialog = NULL;
                }
                if (key == key_off_on)
                {
                    if (powerstate == 1)
                    {
                        powerstate = 0;
                        dialog = "Powering Off";
                    }
                    else
                    {
                        powerstate = 1;
                        dialog = "Powering On";
                    }
                }
            }
            if (key == key_silence)
            {
                dialog = "Alarms Cancelled";
            }
            if (key == key_lock)
            {
                if (icon_touchstate == icon_lock)
                {
                    icon_touchstate = icon_lock_open;
                    dialog = "Screen Unlocked";
                }
                else
                {
                    icon_touchstate = icon_lock;
                    dialog = "Screen Locked";
                }
            }
            if (dialog)
            {
                dialog_ms = current_ms;
            }
            key_last = 0;
        }
        else
        {
            key_last = 0;
            if (tooltip_key)
            {
                dialog_ms = current_ms;
            }
            tooltip_key = 0;
        }

        // Make the heart sign "pulse" using sweep edge data
        pulse = pulse / 2; // decrease size of pulse slower for visual effect
        for (k = 0; k < (int)sweep_step; k++)
        {
            uint8_t test = sweep_edge[trace_hrm][k];
            if (test > pulse)
            {
                pulse = test;
            }
        }

        // Work out heart rate BPM
        if ((pulse > pulse_threshold) && (pulse_last))
        {
            uint32_t pulse_period;
            pulse_last = 0;
            pulse_period = current_ms - pulse_ms;
            pulse_ms = current_ms;
            if (pulse_period > 0)
            {
                pulse_freq = 60000 / pulse_period;
            }
        }
        else if ((pulse < (pulse_threshold - pulse_hysteresis)) && (!pulse_last))
        {
            pulse_last = 1;
        }

        // Calculate the respiration rate using sweep edge data
        // threshold point is at resp_threshold
        resp = sweep_edge[trace_resp][0];
        if ((resp > resp_threshold) && (resp_last))
        {
            uint32_t resp_period;
            resp_last = 0;
            resp_period = current_ms - resp_ms;
            resp_ms = current_ms;
            if (resp_period > 0)
            {
                resp_freq = 60000 / resp_period;
            }
        }
        else if ((resp < (resp_threshold - resp_hysteresis)) && (!resp_last))
        {
            resp_last = 1;
        }

        // Update info every second
        if ((uint32_t)(current_ms - update_ms) > update_period_ms)
        {
            // Calculate the oxygen saturation level using sweep edge data
            // 255 = 100%, 155 = 90%
            oxygen_sat = 1000 + (sweep_edge[trace_sat][0] / 10) - 26;
            
            update_ms = current_ms;
        }

        // Scroll unless we are paused
        if (icon_updatestate == icon_pause)
        {
            sweep_counter += sweep_step;
            if (sweep_counter >= TRACE_POINTS)
            {
                sweep_counter = 0;
            }
        }
        
        // Limit frame rate
        // Flag if we are not acheiving that rate
        frame_rate_flag = 1;
        render_ms = platform_get_time();
        while (1)
        {
            current_ms = platform_get_time();
            if ((uint32_t)(current_ms - previous_ms) >= frame_ms) break;
            frame_rate_flag = 0;
        }
        frame_render_time = (uint32_t)(render_ms - previous_ms);
        previous_ms = current_ms;
        if (frame_rate_flag)
        {
            DEBUG_PRINTF("overrun: took %d ms\n", frame_render_time);
        }

        if ((dialog) || (tooltip))
        {
            if ((uint32_t)(current_ms - dialog_ms) > update_period_ms)
            {
                dialog = NULL;
                tooltip = NULL;
            }
        }

        uint32_t dlsize =  receivedata[2];
        DEBUG_PRINTF("DL: %08x frame %d ms\n", dlsize, frame_render_time);
    }
}

// Application Code begins here

void eve_example(const char *assets)
{
    eve_asset_properties(assets); // Configure asset properties for custom assets used in application

    EVE_Init();                 // Initialise the display

    eve_calibrate();          // Calibrate the display

    eve_display_load_assets();   // Load assets into RAM_G

    eve_display();              // Run Application
}
