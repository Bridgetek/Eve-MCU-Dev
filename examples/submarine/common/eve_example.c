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
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <EVE.h>

#include "eve_example.h"

void eve_display(void)
{
    // Size of the indicators on the dashboard
    uint16_t compass_radius = EVE_DISP_WIDTH / 6;
    uint16_t depth_height = (EVE_DISP_HEIGHT * 2) / 3;
    uint16_t depth_width = EVE_DISP_WIDTH / 12;

    // Variables detemining how the animation of the widget appears
    int depth = 0;
    int max_depth = 9500;
    // Compass heading
    int heading = 0;

    // Animation counter
    int anim = 0;

    // Variables for size and position
    // Centre the widgets
    uint16_t x_compass = (EVE_DISP_WIDTH / 2);
    uint16_t y_compass = EVE_DISP_HEIGHT - (compass_radius / 3);
    uint16_t x_depth = 0;
    uint16_t y_depth = (EVE_DISP_HEIGHT - depth_height) / 2;

    while (1)
    {
        EVE_LIB_BeginCoProList();
        EVE_CMD_DLSTART();
        EVE_CLEAR_COLOR_RGB(0, 0, 0);
        EVE_CLEAR(1,1,1);

#if IS_EVE_API(5)
        // Central gradient background for EVE 5
        EVE_CMD_CGRADIENT(EVE_CGRADIENT_CORNER_ZERO, 0, 0, EVE_DISP_WIDTH, EVE_DISP_HEIGHT, 0x000020, 0x000080);
#else
        // Standard gradient background for EVE 1,2,3,4
        EVE_CMD_GRADIENT(0, 0, 0x000020, EVE_DISP_WIDTH, EVE_DISP_HEIGHT, 0x000080);
#endif

        compass_bulkhead(x_compass, y_compass, compass_radius, OPT_COMPASS_BULKHEAD_RECT | OPT_COMPASS_TRANSPARENT /*| OPT_COMPASS_BULKHEAD_USE_FONT*/, heading);
        sub_depth(x_depth, y_depth, depth_width, depth_height, OPT_COMPASS_TRANSPARENT, depth, 60 * SUB_UNITS_SCALE);

        EVE_DISPLAY();
        EVE_CMD_SWAP();
        EVE_LIB_EndCoProList();
        EVE_LIB_AwaitCoProEmpty();

        depth++;
        heading++;
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
