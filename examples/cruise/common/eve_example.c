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

#include <stdint.h>
#include <EVE.h>

#include "eve_helper.h"
#include "sevenseg.h"

#if IS_EVE_API(1)
#error This example requires EVE API 2 or above.
#endif

#if EVE_DISP_WIDTH < 480
#error This example needs a screen width of 480 pixels or above.
#endif

// Draw a stencil to show a circular display
#define TARGET_CIRCULAR
#define TARGET_SCREEN_RADIUS 240
// Font for km/h or other units
#define UNITS_FONT 25
// Font for action areas
#define ACTION_FONT 31
// Font for extenal controls
#define EXT_FONT 29
// Location for display zones
#define TARGET_AREA_TOP ((TARGET_SCREEN_RADIUS * 3) / 10)
#define TARGET_AREA_BOTTOM ((TARGET_SCREEN_RADIUS * 2) - TARGET_AREA_TOP)
#define TARGET_SCREEN_MIRROR(x) ((TARGET_SCREEN_RADIUS * 2) - x)
// Seven segment size and gap between segments
#define SEGMENT_SIZE 100
#define SEGMENT_GAP (SEGMENT_SIZE + ((SEGMENT_SIZE * 4) / 10))
// Remain area on right for buttons and controls
#if EVE_DISP_WIDTH > 480
// Only draw this if the 480x480 circular display is NOT used
#define BUTTON_GAP 50
#define BUTTON_WIDTH (EVE_DISP_WIDTH - (TARGET_SCREEN_RADIUS * 2) - (BUTTON_GAP * 2))
#if BUTTON_WIDTH > 400
#undef BUTTON_WIDTH
#define BUTTON_WIDTH 400
#endif
#endif
// Maximum speed
#define MAX_SPEED 160

const uint32_t scrbg = 0x000000;
const uint32_t dullfg = 0x800000;
const uint32_t dullbg = 0x200000;
const uint32_t redfg = 0xff0000;
const uint32_t redbg = 0x300000;
const uint32_t grnfg = 0x00ff00;
const uint32_t grnbg = 0x003000;
const uint32_t blufg = 0x0000ff;
const uint32_t blubg = 0x000030;



void eve_display(void)
{
    int set_speed = 65;
    int current_speed = 55;

    uint8_t key = 0;
    uint8_t keyprev = 0;
    int16_t x, y;
    enum state {
        e_disabled,
        e_enabled,
    };
    enum state cruise_arm = e_disabled;
    enum state cruise_active = e_disabled;

    do {
        EVE_LIB_BeginCoProList();
        EVE_CMD_DLSTART();
        EVE_CLEAR_COLOR_RGB(0, 0, 0);
        EVE_CLEAR(1,1,1);

        uint32_t fg_setpt = 0xffffff;
        uint32_t bg_setpt = 0x000000;

        // Colour selections
        if (cruise_arm == e_disabled)
        {
            fg_setpt = dullfg;
            bg_setpt = dullbg;
        }
        else if (cruise_arm == e_enabled)
        {
            if (cruise_active == e_disabled)
            {
                fg_setpt = redfg;
                bg_setpt = redbg;
            }
            else
            {
                fg_setpt = grnfg;
                bg_setpt = grnbg;
            }
        }

#ifdef TARGET_CIRCULAR
        // Stencil for a circular screen
        EVE_COLOR_RGB(0, 0, 0);
        EVE_STENCIL_OP(EVE_STENCIL_KEEP, EVE_STENCIL_INCR);
        EVE_BEGIN(EVE_BEGIN_POINTS);
        EVE_POINT_SIZE(TARGET_SCREEN_RADIUS * 16);
        EVE_VERTEX2F(TARGET_SCREEN_RADIUS * 16, TARGET_SCREEN_RADIUS * 16);
        EVE_STENCIL_FUNC(EVE_TEST_NOTEQUAL, 0, 255);
#endif // CIRCULAR

        // Top centre of action buttons
        x = TARGET_SCREEN_RADIUS;
        y = (TARGET_SCREEN_RADIUS * 3 / 10);

        // Set Cancel Resume Buttons
        EVE_COLOR(0xffffff);

        // Gradient at the top
        EVE_SAVE_CONTEXT();
        EVE_SCISSOR_XY(0, 0);
        EVE_SCISSOR_SIZE(TARGET_SCREEN_RADIUS * 2, (TARGET_SCREEN_RADIUS - SEGMENT_SIZE));
        if ((cruise_arm == e_enabled) && (cruise_active == e_disabled))
        {
            // Resume at the top.
            EVE_CMD_GRADIENT(TARGET_SCREEN_RADIUS, 0, grnfg, TARGET_SCREEN_RADIUS, SEGMENT_SIZE, scrbg);
        }
        else if ((cruise_arm == e_disabled) || ((cruise_arm == e_enabled) && (cruise_active == e_enabled)))
        {
            // Preset speeds when cruise disabled or active
            EVE_CMD_GRADIENT(TARGET_SCREEN_RADIUS, 0, grnfg, TARGET_SCREEN_RADIUS, SEGMENT_SIZE, scrbg);
        }
        EVE_RESTORE_CONTEXT();

        // Gradients at the bottom
        EVE_SAVE_CONTEXT();
        EVE_SCISSOR_XY(0, (TARGET_SCREEN_RADIUS + SEGMENT_SIZE));
        EVE_SCISSOR_SIZE(TARGET_SCREEN_RADIUS * 2, (TARGET_SCREEN_RADIUS - SEGMENT_SIZE) + 1);
        if (cruise_arm == e_enabled)
        {
            if (cruise_active == e_disabled)
            {
                // Set at the bottom.
                EVE_CMD_GRADIENT(TARGET_SCREEN_RADIUS, (TARGET_SCREEN_RADIUS + SEGMENT_SIZE), scrbg, TARGET_SCREEN_RADIUS, TARGET_SCREEN_RADIUS * 2, grnfg);
            }
            else
            {
                // Cancel at the bottom.
                EVE_CMD_GRADIENT(TARGET_SCREEN_RADIUS, (TARGET_SCREEN_RADIUS + SEGMENT_SIZE), scrbg, TARGET_SCREEN_RADIUS, TARGET_SCREEN_RADIUS * 2,redfg);
            }
        }
        else
        {
            // Cancel at the bottom.
        }
        EVE_RESTORE_CONTEXT();

        // Buttons at the top
        if ((cruise_arm == e_enabled) && (cruise_active == e_disabled))
        {
            // Resume at the top.
            EVE_CMD_TEXT(TARGET_SCREEN_RADIUS, (TARGET_AREA_TOP * 3) / 4, ACTION_FONT, EVE_OPT_CENTER, "RESUME");
        }
        else if ((cruise_arm == e_disabled) || ((cruise_arm == e_enabled) && (cruise_active == e_enabled)))
        {
            // Preset speeds when cruise disabled or active
            EVE_TAG(10);
            EVE_CMD_TEXT(TARGET_SCREEN_RADIUS - SEGMENT_SIZE, TARGET_AREA_TOP, ACTION_FONT, EVE_OPT_CENTER, "50");
            EVE_TAG(11);
            EVE_CMD_TEXT(TARGET_SCREEN_RADIUS, (TARGET_AREA_TOP * 3) / 4, ACTION_FONT, EVE_OPT_CENTER, "80");
            EVE_TAG(12);
            EVE_CMD_TEXT(TARGET_SCREEN_RADIUS + SEGMENT_SIZE, TARGET_AREA_TOP, ACTION_FONT, EVE_OPT_CENTER, "100");
        }

        // Buttons at the bottom
        EVE_TAG(20);
        if (cruise_arm == e_enabled)
        {
            if (cruise_active == e_disabled)
            {
                // Set at the bottom.
                EVE_CMD_TEXT(TARGET_SCREEN_RADIUS, TARGET_AREA_BOTTOM, ACTION_FONT, EVE_OPT_CENTER, "SET");
            }
            else
            {
                // Cancel at the bottom.
                EVE_CMD_TEXT(TARGET_SCREEN_RADIUS, TARGET_AREA_BOTTOM, ACTION_FONT, EVE_OPT_CENTER, "CANCEL");
            }
        }
        else
        {
            // Cancel at the bottom.
            EVE_CMD_TEXT(TARGET_SCREEN_RADIUS, TARGET_AREA_BOTTOM, ACTION_FONT, EVE_OPT_CENTER, "DISABLED");
        }

        // Top left of seven segment display
        x = TARGET_SCREEN_RADIUS - (((SEGMENT_GAP * 2) + SEGMENT_SIZE) / 2);
        y = TARGET_SCREEN_RADIUS - SEGMENT_SIZE;

        // Main Cruise set point
        EVE_COLOR(fg_setpt);
        EVE_CMD_TEXT(x + (((SEGMENT_GAP * 2) + SEGMENT_SIZE) / 2), y - eve_romfont_height(UNITS_FONT), UNITS_FONT, EVE_OPT_CENTER, "km/h");
        if (cruise_arm == e_enabled)
        {
            sevensegment(x + (SEGMENT_GAP * 0), y, SEGMENT_SIZE, ((set_speed/100)%10), fg_setpt, bg_setpt);
            sevensegment(x + (SEGMENT_GAP * 1), y, SEGMENT_SIZE, ((set_speed/10)%10), fg_setpt, bg_setpt);
            sevensegment(x + (SEGMENT_GAP * 2), y, SEGMENT_SIZE, ((set_speed/1)%10), fg_setpt, bg_setpt);
        }
        else
        {
            sevensegment(x + (SEGMENT_GAP * 0), y, SEGMENT_SIZE, 16, fg_setpt, bg_setpt);
            sevensegment(x + (SEGMENT_GAP * 1), y, SEGMENT_SIZE, 16, fg_setpt, bg_setpt);
            sevensegment(x + (SEGMENT_GAP * 2), y, SEGMENT_SIZE, 16, fg_setpt, bg_setpt);
        }

        // See through tag areas for top area
        EVE_COLOR_A(0);
        EVE_TAG(10);
        EVE_BEGIN(EVE_BEGIN_RECTS);
        EVE_LINE_WIDTH(16);
        EVE_VERTEX2F(0, 0);
        EVE_VERTEX2F((TARGET_SCREEN_RADIUS - (SEGMENT_SIZE / 2)) * 16, (TARGET_SCREEN_RADIUS - SEGMENT_SIZE) * 16);
        EVE_TAG(11);
        EVE_BEGIN(EVE_BEGIN_RECTS);
        EVE_LINE_WIDTH(16);
        EVE_VERTEX2F((TARGET_SCREEN_RADIUS - (SEGMENT_SIZE / 2)) * 16, 0);
        EVE_VERTEX2F((TARGET_SCREEN_RADIUS + (SEGMENT_SIZE / 2)) * 16, (TARGET_SCREEN_RADIUS - SEGMENT_SIZE) * 16);
        EVE_TAG(12);
        EVE_BEGIN(EVE_BEGIN_RECTS);
        EVE_LINE_WIDTH(16);
        EVE_VERTEX2F((TARGET_SCREEN_RADIUS + (SEGMENT_SIZE / 2)) * 16, 0);
        EVE_VERTEX2F((TARGET_SCREEN_RADIUS * 2) * 16, (TARGET_SCREEN_RADIUS - SEGMENT_SIZE) * 16);
        // See through tag area for bottom area
        EVE_TAG(20);
        EVE_BEGIN(EVE_BEGIN_RECTS);
        EVE_LINE_WIDTH(16);
        EVE_VERTEX2F(0, (TARGET_SCREEN_RADIUS + SEGMENT_SIZE) * 16);
        EVE_VERTEX2F((TARGET_SCREEN_RADIUS * 2) * 16, (TARGET_SCREEN_RADIUS * 2) * 16);
        // Restore non see throughness
        EVE_COLOR_A(255);

#ifdef TARGET_CIRCULAR
        EVE_STENCIL_FUNC(EVE_TEST_ALWAYS, 0, 255);
#endif // CIRCULAR

#if EVE_DISP_WIDTH > 480
        // Click wheel actions
        EVE_COLOR(0xffffff);
        EVE_CMD_FGCOLOR(blufg);
        // Depress wheel, enable/disable cruise.
        EVE_TAG(100);
        EVE_CMD_BUTTON((TARGET_SCREEN_RADIUS * 2) + BUTTON_GAP, BUTTON_GAP, BUTTON_WIDTH, (eve_romfont_height(EXT_FONT) * 2), EXT_FONT, 0, "button click");
        // Wheel actions.
        EVE_TAG(0);
        EVE_CMD_TEXT((TARGET_SCREEN_RADIUS * 2) + BUTTON_GAP + (BUTTON_WIDTH / 2), BUTTON_GAP + (eve_romfont_height(EXT_FONT) * 3), EXT_FONT, EVE_OPT_CENTERX, "turn wheel");
        // Turn clockwise, go faster.
        EVE_TAG(101);
        EVE_CMD_BUTTON((TARGET_SCREEN_RADIUS * 2) + BUTTON_GAP + (BUTTON_WIDTH / 2), BUTTON_GAP + (eve_romfont_height(EXT_FONT) * 4), BUTTON_WIDTH / 2, (eve_romfont_height(EXT_FONT) * 2), EXT_FONT, 0, "+");
        // Turn anti-clockwise, go slower.
        EVE_TAG(102);
        EVE_CMD_BUTTON((TARGET_SCREEN_RADIUS * 2) + BUTTON_GAP, BUTTON_GAP + (eve_romfont_height(EXT_FONT) * 4), BUTTON_WIDTH / 2, (eve_romfont_height(EXT_FONT) * 2), EXT_FONT, 0, "-");
        // Car inputs.
        EVE_TAG(0);
        EVE_CMD_TEXT((TARGET_SCREEN_RADIUS * 2) + BUTTON_GAP + (BUTTON_WIDTH / 2), BUTTON_GAP + (eve_romfont_height(EXT_FONT) * 7), EXT_FONT, EVE_OPT_CENTERX, "road speed");
        EVE_TAG(103);
        EVE_CMD_SLIDER((TARGET_SCREEN_RADIUS * 2) + BUTTON_GAP, BUTTON_GAP + (eve_romfont_height(EXT_FONT) * 9), BUTTON_WIDTH, (eve_romfont_height(EXT_FONT) * 1), 0, (current_speed * 400) / MAX_SPEED, 400);
        EVE_CMD_TRACK((TARGET_SCREEN_RADIUS * 2) + BUTTON_GAP, BUTTON_GAP + (eve_romfont_height(EXT_FONT) * 9), BUTTON_WIDTH, (eve_romfont_height(EXT_FONT) * 1), 103);
        // Brake!
        EVE_TAG(104);
        EVE_CMD_FGCOLOR(redfg);
        EVE_CMD_BUTTON((TARGET_SCREEN_RADIUS * 2) + BUTTON_GAP, BUTTON_GAP + (eve_romfont_height(EXT_FONT) * 12), BUTTON_WIDTH, (eve_romfont_height(EXT_FONT) * 2), 28, 0, "Brake!");
#endif

        EVE_DISPLAY();
        EVE_CMD_SWAP();
        EVE_LIB_EndCoProList();
        EVE_LIB_AwaitCoProEmpty();

        while (eve_read_tag(&key) == 0);

        // Debounce keys.
        if (key != keyprev)
        {
            keyprev = key;

            // Conditions for bottom button area
            if ((cruise_arm == e_enabled) && (cruise_active == e_disabled))
            {
                // SET button when cruise inactive
                if (key == 20)
                {
                    // Update set speed to road speed
                    set_speed = current_speed;
                    cruise_active = e_enabled;
                }
            }
            else if ((cruise_arm == e_enabled) && (cruise_active == e_enabled))
            {
                // CANCEL button when cruise active
                if (key == 20)
                {
                    cruise_active = e_disabled;
                }
            }

            // Conditions for top button area
            if ((cruise_arm == e_enabled) && (cruise_active == e_disabled))
            {
                // RESUME button when cruise inactive
                if ((key == 10) || (key == 11) || (key == 12))
                {
                    // Update road speed to match set speed when active
                    current_speed = set_speed;
                    cruise_active = e_enabled;
                }
            }
            else if ((cruise_arm == e_disabled) || ((cruise_arm == e_enabled) && (cruise_active == e_enabled)))
            {
                // Preset speeds when cruise disabled or active
                if (key == 10)
                {
                    set_speed = current_speed = 50;
                    cruise_arm = e_enabled;
                    cruise_active = e_enabled;
                }
                else if (key == 11)
                {
                    set_speed = current_speed = 80;
                    cruise_arm = e_enabled;
                    cruise_active = e_enabled;
                }
                else if (key == 12)
                {
                    set_speed = current_speed = 100;
                    cruise_arm = e_enabled;
                    cruise_active = e_enabled;
                }
            }

            // ENABLE/DISABLE cruise (ARM/DISARM)
            if (key == 100)
            {
                if (cruise_arm == e_disabled)
                {
                    cruise_arm = e_enabled;
                    cruise_active = e_disabled;
                }
                else if (cruise_arm == e_enabled)
                {
                    cruise_arm = e_disabled;
                }
            }
            // FASTER button event
            if (key == 101)
            {
                set_speed++;
                if (set_speed > 120)
                {
                    set_speed = 120;
                }
                current_speed = set_speed;
            }
            // SLOWER button event
            if (key == 102)
            {
                set_speed--;
                if (set_speed < 30)
                {
                    set_speed = 30;
                }
                current_speed = set_speed;
            }
            // Road speed slider
            if (key == 103)
            {
                uint32_t tracker;
                tracker = EVE_LIB_MemRead32(EVE_REG_TRACKER);
                if ((tracker & 0xff) == 103)
                {
                    // 65535 -> MAX_SPEED
                    // 32768 -> MAX_SPEED / 2
                    // 0 -> 0 km/h
                    current_speed = ((tracker >> 16) *  MAX_SPEED) / 65535;
                }
            }
            // BRAKE! button event
            if (key == 104)
            {
                cruise_active = e_disabled;
            }
        }

    } while (1);
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
