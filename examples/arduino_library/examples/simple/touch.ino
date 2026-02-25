/**
 @file touch.ino
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

#include <### EVE CLASS ###.h>

#include "touch.h"

/* CONSTANTS ***********************************************************************/

/* GLOBAL VARIABLES ****************************************************************/

/* LOCAL VARIABLES *****************************************************************/

/* MACROS **************************************************************************/

/* LOCAL FUNCTIONS / INLINES *******************************************************/

static int calib_key_pressed(void)
{
    // Detect screen is being touched at startup
    // NOTE: Not available on BT82x (EVE API 5)
/* ### BEGIN API < 5 ### */
    return eve_key_detect();
/* ### END API ### */
/* ### BEGIN API >= 5 ### */
    return 0;
/* ### END API ### */
}

/* FUNCTIONS ***********************************************************************/

int eve_key_detect(void)
{
    int key_detect = 0;

/* ### BEGIN API < 5 ### */
    if (!(EVE_LIB_MemRead16(EVE_REG_TOUCH_SCREEN_XY) & 0x8000)) {
        key_detect = 1;
    }
/* ### END API ### */
/* ### BEGIN API >= 5 ### */
    if (!(EVE_LIB_MemRead32(EVE_REG_TOUCH_SCREEN_XY) & 0x8000)) {
        key_detect = 1;
    }
/* ### END API ### */

    return key_detect;
}

/* Read a (single) touch tag from the touch controller. 
 * Return non-zero if there is a valid touch tag. Zero if no touch. 
 */
int eve_read_tag(uint8_t *key)
{
    uint8_t Read_tag;
    int key_detect = 0;

/* ### BEGIN API < 5 ### */
    Read_tag = EVE_LIB_MemRead8(EVE_REG_TOUCH_TAG);
    if (!(EVE_LIB_MemRead16(EVE_REG_TOUCH_SCREEN_XY) & 0x8000)) {
        key_detect = 1;
        *key = Read_tag;
    }
/* ### END API ### */
/* ### BEGIN API >= 5 ### */
    Read_tag = EVE_LIB_MemRead32(EVE_REG_TOUCH_TAG);
    if (!(EVE_LIB_MemRead32(EVE_REG_TOUCH_SCREEN_XY) & 0x8000)) {
        key_detect = 1;
        *key = Read_tag;
    }
/* ### END API ### */

    return key_detect;
}

int eve_calibrate(void)
{
    struct touchscreen_calibration calib;

    // Transform matrix definition
    // x' = x * A + y * B + C
    // y' = x * D + y * E + F

    // Uncharacterised panels
    int valid = 0;
    // Try to read a touch configuration if the platform support it
    // NOTE: platform_calib_init to return zero if supported non-zero if now
    if (platform_calib_init() == 0)
    {
        // Do not read calibration information if screen is being touched at start
        // NOTE: Not available on BT82x (EVE API 5)
        if (!calib_key_pressed())
        {
            // Read calibration information from platform
            if (platform_calib_read(&calib) == 0)
            {
                // Verify the information is valid
                if (calib.key == VALID_KEY_TOUCHSCREEN)
                {
                    valid = 1;
                }
            }
        }
    }

    // If no store of calibration or current screen touch.
    if (!valid)
    {
        // Wait for end of touch.
        // NOTE: Not available on BT82x (EVE API 5)
        while (calib_key_pressed()) {};

        EVE_LIB_BeginCoProList();
        EVE_CMD_DLSTART();
        EVE_CLEAR_COLOR_RGB(0, 0, 0);
        EVE_CLEAR(1,1,1);
        EVE_COLOR_RGB(255, 255, 255);
        EVE_CMD_TEXT(eve.DISP_WIDTH()/2, eve.DISP_HEIGHT()/2,
                28, EVE_OPT_CENTERX | EVE_OPT_CENTERY,"Please tap on the dots");
        EVE_CMD_CALIBRATE(0);
        EVE_LIB_EndCoProList();
        if (EVE_LIB_AwaitCoProEmpty() != 0)
        {
            return -1;
        }

        calib.key = VALID_KEY_TOUCHSCREEN;
        calib.transform[0] = EVE_LIB_MemRead32(EVE_REG_TOUCH_TRANSFORM_A);
        calib.transform[1] = EVE_LIB_MemRead32(EVE_REG_TOUCH_TRANSFORM_B);
        calib.transform[2] = EVE_LIB_MemRead32(EVE_REG_TOUCH_TRANSFORM_C);
        calib.transform[3] = EVE_LIB_MemRead32(EVE_REG_TOUCH_TRANSFORM_D);
        calib.transform[4] = EVE_LIB_MemRead32(EVE_REG_TOUCH_TRANSFORM_E);
        calib.transform[5] = EVE_LIB_MemRead32(EVE_REG_TOUCH_TRANSFORM_F);
        platform_calib_write(&calib);
    }

    EVE_LIB_MemWrite32(EVE_REG_TOUCH_TRANSFORM_A, calib.transform[0]);
    EVE_LIB_MemWrite32(EVE_REG_TOUCH_TRANSFORM_B, calib.transform[1]);
    EVE_LIB_MemWrite32(EVE_REG_TOUCH_TRANSFORM_C, calib.transform[2]);
    EVE_LIB_MemWrite32(EVE_REG_TOUCH_TRANSFORM_D, calib.transform[3]);
    EVE_LIB_MemWrite32(EVE_REG_TOUCH_TRANSFORM_E, calib.transform[4]);
    EVE_LIB_MemWrite32(EVE_REG_TOUCH_TRANSFORM_F, calib.transform[5]);
    
    // Reset the touch controller with the new transform (not strictly needed)
/* ### BEGIN API < 5 ### */
    EVE_LIB_MemWrite8(EVE_REG_CPURESET, 2);
    EVE_LIB_MemWrite8(EVE_REG_CPURESET, 0);
/* ### END API ### */
/* ### BEGIN API >= 5 ### */
    EVE_LIB_MemWrite32(EVE_REG_CPURESET, 2);
    EVE_LIB_MemWrite32(EVE_REG_CPURESET, 0);
/* ### END API ### */

    return 0;
}
