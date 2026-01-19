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
 * http://brtchip.com/BRTSourceCodeLicenseAgreement/ ("the Licence Terms").
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
#include <MCU.h> // For DEBUG_PRINTF

#include "eve_example.h"

const uint8_t widths[] = EVE_ROMFONT_WIDTHS;
const uint8_t heights[] = EVE_ROMFONT_HEIGHTS;
// Choose a suitable font for the display
#if EVE_DISP_WIDTH <= 480
const uint8_t font = 26;
#elif EVE_DISP_WIDTH <= 800
const uint8_t font = 28;
#else
const uint8_t font = 30;
#endif
const uint8_t button_restore = 101;
const uint8_t button_recalibrate = 100;

struct touchscreen_calibration calib;

int eve_recalibrate(void)
{
    uint8_t dummy;
    
    // Wait for end of touch.
    while (eve_read_tag(&dummy));

    EVE_LIB_BeginCoProList();
    EVE_CMD_DLSTART();
    EVE_CLEAR_COLOR_RGB(0, 0, 0);
    EVE_CLEAR(1,1,1);
    EVE_COLOR_RGB(255, 255, 255);

    // Draw the Bridgetek logo at the top of the screen
    EVE_BEGIN(EVE_BEGIN_BITMAPS);
#if IS_EVE_API(2, 3, 4, 5)
    // Set origin on canvas using EVE_VERTEX_TRANSLATE.
    EVE_VERTEX_TRANSLATE_X(((EVE_DISP_WIDTH/2)-(eve_img_bridgetek_logo_width/2)) * 16);
    EVE_VERTEX2II(0, 0, BITMAP_BRIDGETEK_LOGO, 0);
    EVE_VERTEX_TRANSLATE_X(0);
#else
    // Place directly on canvas EVE_VERTEX_TRANSLATE not available.
    EVE_VERTEX2II((EVE_DISP_WIDTH/2)-(eve_img_bridgetek_logo_width/2), 10, BITMAP_BRIDGETEK_LOGO, 0);
#endif

    EVE_CMD_TEXT(EVE_DISP_WIDTH/2, EVE_DISP_HEIGHT/2,
            font, EVE_OPT_CENTERX | EVE_OPT_CENTERY,"Please tap on the dots");

    EVE_CMD_CALIBRATE(0);
    EVE_LIB_EndCoProList();
    if (EVE_LIB_AwaitCoProEmpty() != 0)
    {
        return -1;
    }
    return 0;
}

void eve_readcalib(void)
{
    calib.transform[0] = EVE_LIB_MemRead32(EVE_REG_TOUCH_TRANSFORM_A);
    calib.transform[1] = EVE_LIB_MemRead32(EVE_REG_TOUCH_TRANSFORM_B);
    calib.transform[2] = EVE_LIB_MemRead32(EVE_REG_TOUCH_TRANSFORM_C);
    calib.transform[3] = EVE_LIB_MemRead32(EVE_REG_TOUCH_TRANSFORM_D);
    calib.transform[4] = EVE_LIB_MemRead32(EVE_REG_TOUCH_TRANSFORM_E);
    calib.transform[5] = EVE_LIB_MemRead32(EVE_REG_TOUCH_TRANSFORM_F);
}

void eve_tohex8(uint32_t val, char *str)
{
    int i;
    for (i = 0; i < 8; i++)
    {
        str[i] = (val >> (7 - i) * 4) & 0xf;
        if (str[i] > 9) str[i] += 'A' - 10;
        else str[i] += '0';
    }
    str[8] = '\0';
}

void eve_display(void)
{
    int i;

    // User interface
    uint8_t key = 0;
    uint8_t tag;
    int touch_detect = 0;
    // Drawing position of controls
    int16_t ypos;

    // Transformed x,y position
    uint32_t xy = 0;
    // Raw x,y position
    uint32_t xyr = 0;

    char hexval[16];

    eve_readcalib();
    
    do {
        EVE_LIB_BeginCoProList();
        EVE_CMD_DLSTART();
        EVE_CLEAR_COLOR_RGB(0, 0, 0);
        EVE_CLEAR(1,1,1);
        EVE_COLOR_RGB(255, 255, 255);

        // Draw the Bridgetek logo at the top of the screen
        EVE_BEGIN(EVE_BEGIN_BITMAPS);
#if IS_EVE_API(2, 3, 4, 5)
        // Set origin on canvas using EVE_VERTEX_TRANSLATE.
        EVE_VERTEX_TRANSLATE_X(((EVE_DISP_WIDTH/2)-(eve_img_bridgetek_logo_width/2)) * 16);
        EVE_VERTEX2II(0, 0, BITMAP_BRIDGETEK_LOGO, 0);
        EVE_VERTEX_TRANSLATE_X(0);
#else
        // Place directly on canvas EVE_VERTEX_TRANSLATE not available.
        EVE_VERTEX2II((EVE_DISP_WIDTH/2)-(eve_img_bridgetek_logo_width/2), 10, BITMAP_BRIDGETEK_LOGO, 0);
#endif
        ypos = 10 + (eve_img_bridgetek_logo_height * 3) / 2;

        EVE_CMD_TEXT(EVE_DISP_WIDTH/2, ypos, font, EVE_OPT_CENTERX, "Touchscreen Calibration Test");
        ypos += (heights[font] * 2);

        EVE_TAG_MASK(1);
        EVE_TAG(button_recalibrate);
        EVE_CMD_BUTTON((EVE_DISP_WIDTH / 2) - (widths[font] * 11), ypos, widths[font] * 10, heights[font] * 3, font, 0, "Recalibrate");
        EVE_TAG(button_restore);
        EVE_CMD_BUTTON((EVE_DISP_WIDTH / 2) + (widths[font] * 1), ypos, widths[font] * 10, heights[font] * 3, font, 0, "Restore");
        ypos += (heights[font] * 3);
        EVE_TAG_MASK(0);

        EVE_CMD_TEXT(10, ypos, font, 0, "X,Y:");
        EVE_CMD_TEXT(10, ypos + heights[font], font, 0, "Raw X,Y:");

        // Show raw and transformed touches when the screen is touched.
        if (touch_detect)
        {
            uint16_t x = (xy>>16);
            uint16_t y = xy & 0xffff;
            uint16_t xr = (xyr>>16);
            uint16_t yr = xyr & 0xffff;

            // Write the touch position on the screen.
            EVE_CMD_NUMBER(widths[font] * 6, ypos, font, 0, x);
            EVE_CMD_NUMBER(widths[font] * 9, ypos, font, 0, y);
            EVE_CMD_NUMBER(widths[font] * 6, ypos + heights[font], font, 0, xr);
            EVE_CMD_NUMBER(widths[font] * 9, ypos + heights[font], font, 0, yr);

            // Draw cross-hairs for the touch position.
            EVE_BEGIN(EVE_BEGIN_LINES);
            EVE_VERTEX_FORMAT(0);
            EVE_COLOR_RGB(255,255,255);
            EVE_VERTEX2F(0, y);
            EVE_VERTEX2F(EVE_DISP_WIDTH,y);
            EVE_VERTEX2F(x, 0);
            EVE_VERTEX2F(x, EVE_DISP_HEIGHT);
        }

        ypos += (heights[font] * 3);

        for (i = 0; i < 6; i++)
        {
            hexval[0] = i + 'A';
            hexval[1] = ':';
            hexval[2] = '\0';
            EVE_CMD_TEXT(10, ypos, font, 0, hexval);
            eve_tohex8(calib.transform[i], hexval);
            EVE_CMD_TEXT(widths[font] * 2 , ypos, font, 0, hexval);
            ypos += heights[font];
        }
        
        EVE_DISPLAY();
        EVE_CMD_SWAP();
        EVE_LIB_EndCoProList();
        EVE_LIB_AwaitCoProEmpty();

        touch_detect = eve_read_tag(&tag);
        if (touch_detect)
        {
            // Read the tag register on the device
            xy = EVE_LIB_MemRead32(EVE_REG_TOUCH_SCREEN_XY);
            xyr = EVE_LIB_MemRead32(EVE_REG_TOUCH_RAW_XY);
        }
        else if (key == button_recalibrate)
        {
            // Recalibrate...
            DEBUG_PRINTF("Perform recalibration.\n");
            eve_recalibrate();
            eve_readcalib();
            tag = 0;
        }
        else if (key == button_restore)
        {
            // Restore calibration...
            DEBUG_PRINTF("Restoring saved calibration.\n");
            eve_calibrate();
            eve_readcalib();
            tag = 0;
        }

        key = tag;
    } while (1);
}

void eve_example(void)
{
    // Initialise the display
    EVE_Init();

    // Load images
    DEBUG_PRINTF("Loading images...\n");
    eve_load_images(0);

    while (1)
    {
        // Calibrate the display
        DEBUG_PRINTF("Calibrating display...\n");
        if (eve_calibrate() != 0)
        {
            DEBUG_PRINTF("Exception...\n");
            while(1);
        }

        // Start example code
        DEBUG_PRINTF("Starting demo:\n");
        eve_display();
    }
}
