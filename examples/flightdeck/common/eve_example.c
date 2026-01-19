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
#include <stdlib.h>
#include <EVE.h>

#include "eve_example.h"

#if IS_EVE_API(1, 2, 3, 4)
#error This example requires EVE API 5 or above.
#endif

#if EVE_DISP_WIDTH < 480
#error This example needs a screen width of 480 pixels or above.
#endif

void eve_display(void)
{
    // Size of the indicators on the dashboard
    uint16_t dial_radius = min(EVE_DISP_HEIGHT, EVE_DISP_WIDTH) / 3;

    // Variables detemining how the animation of the widget appears
    int anim_pitch_num = 300;
    int anim_pitch_denom = 15;
    int anim_climb_num = 300;
    int anim_climb_denom = 5;
    int anim_roll_num = 100;
    int anim_roll_denom = 1;
    int anim_alt_num = 5;
    int anim_alt_denom = 10000;
    // Limits of animation positions
    int16_t max_pitch = DEG2FURMAN(60);
    int16_t max_climb = DEG2FURMAN(20);
    int16_t max_roll = DEG2FURMAN(50);
    int max_alt = 9500;
    // Animation counter
    int anim = 0;

    // Variables to have attitude indicator readings
    int16_t pitch = 0;
    int16_t climb = 0;
    int16_t roll = 0;
    int alt = 0;
    
    // Variables for size and position
    // Centre the widgets
    uint16_t xatt = (EVE_DISP_WIDTH / 4);
    uint16_t yatt = (EVE_DISP_HEIGHT / 2);
    uint16_t xalt = (EVE_DISP_WIDTH * 3 / 4);
    uint16_t yalt = (EVE_DISP_HEIGHT / 2);

    while (1)
    {
        EVE_LIB_BeginCoProList();
        EVE_CMD_DLSTART();
        EVE_CLEAR_COLOR_RGB(0, 0, 0);
        EVE_CLEAR(1,1,1);

        EVE_CMD_CGRADIENT(EVE_CGRADIENT_CORNER_ZERO, 0, 0, EVE_DISP_WIDTH, EVE_DISP_HEIGHT, 0x000020, 0x000080);

        attwidget(xatt, yatt, dial_radius, pitch, climb, roll);
        altwidget(xalt, yalt, dial_radius, alt);

        EVE_DISPLAY();
        EVE_CMD_SWAP();
        EVE_LIB_EndCoProList();
        EVE_LIB_AwaitCoProEmpty();

        pitch = max_pitch * sin_furman(((anim * anim_pitch_num) / anim_pitch_denom) & 0xffff) / 0x8000;
        climb = max_climb * sin_furman(((anim * anim_climb_num) / anim_climb_denom) & 0xffff) / 0x8000;
        roll = max_roll * sin_furman(((anim * anim_roll_num) / anim_roll_denom) & 0xffff) / 0x8000;
        alt = (max_alt / 2) + (max_alt / 2) * sin_furman(((0x10000 * anim * anim_alt_num) / anim_alt_denom)) / 0x8000;
        anim+=1;
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
