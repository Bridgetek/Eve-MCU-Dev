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
#include <stdlib.h>

#include <EVE.h>

#if IS_EVE_API(1,2,3,4)
#error This example requires EVE API 5 or above.
#endif

#include "patch_dronefpv.h"

#include "eve_helper.h"
#include "flightcontrols.h"
#include "trig_furman.h"

#if EVE_DISP_WIDTH < 480
#error This example needs a screen width of 480 pixels or above.
#endif

// Bitmap handles
#define HND_LVDSRX 1

// Incoming signal is 1920 x 1080
#define LVDSRX_W 1920
#define LVDSRX_H 1080

#define LVDSRX_CORE_SETUP_VALUE     0x03   // 2 channels, two pixels per clock
#define LVDSRX_SETUP_VALUE          0x17   // One pixel per clock, 2 channels, VESA 24
#define LVDSRX_CTRL_VALUE           0x8c8c // Ch0 Deskew 0x8, Ch0 clock sel, Frange 0x2

void video_LVDS(void)
{
    const uint32_t lvdsrx_w = LVDSRX_W;
    const uint32_t lvdsrx_h = LVDSRX_H;
    int lvds_connected;
    uint32_t lvdsrx_data_addr_prev;
    uint32_t lvdsrx_data_addr_new;
    uint32_t lvdsrx_data_addr;
    uint32_t conn;
    int count;

    // Size of the indicators on the dashboard
    uint16_t dial_radius = 200;

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

    printf("LVDS start...\n");
    lvds_connected = 1;
    EVE_LIB_BeginCoProList();
    // LVDSRX System registers
    // This must be swapchain2
    EVE_CMD_REGWRITE(EVE_REG_LVDSRX_CORE_DEST, EVE_SWAPCHAIN_2);
    EVE_CMD_REGWRITE(EVE_REG_LVDSRX_CORE_FORMAT, EVE_FORMAT_RGB8);
    EVE_CMD_REGWRITE(EVE_REG_LVDSRX_CORE_DITHER, 0);
    EVE_CMD_REGWRITE(EVE_REG_LVDSRX_CORE_CAPTURE, 1);

    // LVDSRX_CORE_SETUP register
    EVE_CMD_REGWRITE(EVE_REG_LVDSRX_CORE_SETUP, LVDSRX_CORE_SETUP_VALUE);
    EVE_CMD_REGWRITE(EVE_REG_LVDSRX_CORE_ENABLE, 1);

    // LVDS startup
    EVE_CMD_LVDSSETUP(LVDSRX_SETUP_VALUE, LVDSRX_CTRL_VALUE);
    EVE_CMD_LVDSSTART();
    EVE_LIB_EndCoProList();
    EVE_LIB_AwaitCoProEmpty();

    printf("Waiting for LVDS connection...\n");
    do
    {
        EVE_LIB_LVDSConn(&conn);
    } while (conn == 0);
    
    EVE_LIB_BeginCoProList();
    // Get the memory address of the SWAPCHAIN_2 buffer
    EVE_CMD_WAITCOND(EVE_REG_SC2_STATUS, EVE_TEST_EQUAL, 1, 1);
    EVE_CMD_REGWRITE(EVE_REG_SC2_STATUS, 0x3);
    EVE_CMD_REGREAD(EVE_REG_SC2_ADDR, 0);
    EVE_LIB_EndCoProList();
    EVE_LIB_AwaitCoProEmpty();
    lvdsrx_data_addr_prev = 0;
    lvdsrx_data_addr = EVE_LIB_GetResult(1);

    // Variables for size and position
    // Centre the widgets
    uint16_t xatt = dial_radius;
    uint16_t yatt = LVDSRX_H - dial_radius;
    uint16_t xalt = LVDSRX_W - dial_radius;
    uint16_t yalt = LVDSRX_H - dial_radius;

    printf("Demo starting...\n");
    count = 0;

    // Main loop
    while (1)
    {
        // Test for LVDS connection
        // Get the memory address of the current SWAPCHAIN_2 buffer
        EVE_LIB_BeginCoProList();
        EVE_CMD_WAITCOND(EVE_REG_SC2_STATUS, EVE_TEST_NOTEQUAL, 0, 3);
        EVE_CMD_REGWRITE(EVE_REG_SC2_STATUS, 0x3);
        EVE_CMD_REGREAD(EVE_REG_SC2_ADDR, 0);
        EVE_CMD_LVDSCONN(0);
        EVE_LIB_EndCoProList();
        EVE_LIB_AwaitCoProEmpty();

        conn = EVE_LIB_GetResult(1);
        lvdsrx_data_addr_new = EVE_LIB_GetResult(3);

        if (conn == 0)
        {
            // Not connected or synced
            if (lvds_connected == 2) 
            {
                // Disable LVDS
                printf("LVDS sync lost stopped\n");
                EVE_LIB_BeginCoProList();
                EVE_CMD_LVDSSTOP();
                EVE_LIB_EndCoProList();
                EVE_LIB_AwaitCoProEmpty();
                lvds_connected = 0;
            }
            if (lvds_connected == 0)
            {
                // Enable LVDS
                printf("LVDS re-start\n");
                EVE_LIB_BeginCoProList();
                EVE_CMD_LVDSSTART();
                EVE_LIB_EndCoProList();
                EVE_LIB_AwaitCoProEmpty();
                lvds_connected = 1;
            }
        }
        else
        {
            if (lvds_connected == 1)
            {
                // Sync established
                printf("LVDS re-synced\n");
                // 2 is normal connected state
                lvds_connected = 2;
            }
        }

        if (lvdsrx_data_addr != lvdsrx_data_addr_new)
        {
            lvdsrx_data_addr_prev = lvdsrx_data_addr;
            lvdsrx_data_addr = lvdsrx_data_addr_new;

            EVE_LIB_BeginCoProList();
            EVE_CMD_DLSTART();
            EVE_CLEAR_COLOR_RGB(0, 80, 0);
            EVE_CLEAR(1,1,1);

            if (lvds_connected == 2)
            {
                EVE_BEGIN(EVE_BEGIN_BITMAPS);
                EVE_BITMAP_HANDLE(HND_LVDSRX);
                EVE_CMD_SETBITMAP(lvdsrx_data_addr_prev, EVE_FORMAT_RGB8, lvdsrx_w, lvdsrx_h);
                EVE_VERTEX2F(0, 0);
            }

            attwidget(xatt, yatt, dial_radius, pitch, climb, roll);
            altwidget(xalt, yalt, dial_radius, alt);

            EVE_DISPLAY();
            EVE_CMD_SWAP();
            EVE_LIB_EndCoProList();
            EVE_LIB_AwaitCoProEmpty();

            //MCU_Delay_20ms();

            pitch = max_pitch * sin_furman(((anim * anim_pitch_num) / anim_pitch_denom) & 0xffff) / 0x8000;
            climb = max_climb * sin_furman(((anim * anim_climb_num) / anim_climb_denom) & 0xffff) / 0x8000;
            roll = max_roll * sin_furman(((anim * anim_roll_num) / anim_roll_denom) & 0xffff) / 0x8000;
            alt = (max_alt / 2) + (max_alt / 2) * sin_furman(((0x10000 * anim * anim_alt_num) / anim_alt_denom)) / 0x8000;
            anim+=1;
        }
    }
}

void eve_display(void)
{
    EVE_LIB_BeginCoProList();
    EVE_CMD_REGWRITE(EVE_REG_SC2_SIZE, 2);
    // Start the swapchain 2 buffer at 0x5000000 - second buffer immediately after
    EVE_CMD_REGWRITE(EVE_REG_SC2_PTR0, (5 << 24));
    EVE_CMD_REGWRITE(EVE_REG_SC2_PTR1, (5 << 24) + (LVDSRX_W * LVDSRX_H * 3));
    EVE_LIB_EndCoProList();
    EVE_LIB_AwaitCoProEmpty();

    printf("Swapchain 2: 0x%x and 0x%x\n", EVE_LIB_MemRead32(EVE_REG_SC2_PTR0), EVE_LIB_MemRead32(EVE_REG_SC2_PTR1));

    video_LVDS();
}

void eve_example(void)
{
    // Initialise the display
    EVE_Init();

    printf("Loading patch...\n");
    if (eve_loadpatch())
    {
        printf("Failed to load patch file...\n");
        exit(-1);
    }

    // Calibrate the display
    printf("Calibrating display...\n");
    eve_calibrate();

    // Start example code
    printf("Starting demo:\n");
    eve_display();
}
