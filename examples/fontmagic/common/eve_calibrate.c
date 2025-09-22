/**
 @file eve_calibrate.c
 */
/*
 * ============================================================================
 * History
 * =======
 * Nov 2019		Initial beta for FT81x and FT80x
 * Mar 2020		Updated beta - added BT815/6 commands
 * Mar 2021     Beta with BT817/8 support added
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

#include <stdint.h>

#include "EVE.h"
#include "HAL.h"

#include "eve_example.h"

/* CONSTANTS ***********************************************************************/

/* GLOBAL VARIABLES ****************************************************************/

/* LOCAL VARIABLES *****************************************************************/

/* MACROS **************************************************************************/

/* LOCAL FUNCTIONS / INLINES *******************************************************/


/* FUNCTIONS ***********************************************************************/

void eve_calibrate(void)
{
	struct touchscreen_calibration calib;
	uint8_t dummy;

	platform_calib_init();

	// If no store of calibration or current screen touch.
	if ((platform_calib_read(&calib) != 0) || (eve_read_tag(&dummy)))
	{
		// Wait for end of touch.
		while (eve_read_tag(&dummy));

		EVE_LIB_BeginCoProList();
		EVE_CMD_DLSTART();
		EVE_CLEAR_COLOR_RGB(0, 0, 0);
		EVE_CLEAR(1,1,1);
		EVE_COLOR_RGB(255, 255, 255);
		EVE_CMD_TEXT(EVE_DISP_WIDTH/2, EVE_DISP_HEIGHT/2,
				28, EVE_OPT_CENTERX | EVE_OPT_CENTERY,"Please tap on the dots");
		EVE_CMD_CALIBRATE(0);
		EVE_LIB_EndCoProList();
		EVE_LIB_AwaitCoProEmpty();

		calib.transform[0] = HAL_MemRead32(EVE_REG_TOUCH_TRANSFORM_A);
		calib.transform[1] = HAL_MemRead32(EVE_REG_TOUCH_TRANSFORM_B);
		calib.transform[2] = HAL_MemRead32(EVE_REG_TOUCH_TRANSFORM_C);
		calib.transform[3] = HAL_MemRead32(EVE_REG_TOUCH_TRANSFORM_D);
		calib.transform[4] = HAL_MemRead32(EVE_REG_TOUCH_TRANSFORM_E);
		calib.transform[5] = HAL_MemRead32(EVE_REG_TOUCH_TRANSFORM_F);
		platform_calib_write(&calib);
	}
	else
	{
		HAL_MemWrite32(EVE_REG_TOUCH_TRANSFORM_A, calib.transform[0]);
		HAL_MemWrite32(EVE_REG_TOUCH_TRANSFORM_B, calib.transform[1]);
		HAL_MemWrite32(EVE_REG_TOUCH_TRANSFORM_C, calib.transform[2]);
		HAL_MemWrite32(EVE_REG_TOUCH_TRANSFORM_D, calib.transform[3]);
		HAL_MemWrite32(EVE_REG_TOUCH_TRANSFORM_E, calib.transform[4]);
		HAL_MemWrite32(EVE_REG_TOUCH_TRANSFORM_F, calib.transform[5]);
	}
}
