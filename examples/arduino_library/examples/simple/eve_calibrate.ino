/**
 @file eve_calibrate.ino
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

#include <stdint.h>
#include "eve_example.h"

/* CONSTANTS ***********************************************************************/

/* GLOBAL VARIABLES ****************************************************************/

/* LOCAL VARIABLES *****************************************************************/

/* MACROS **************************************************************************/

/* LOCAL FUNCTIONS / INLINES *******************************************************/


/* FUNCTIONS ***********************************************************************/

int eve_calibrate() {
	struct touchscreen_calibration calib;
	uint8_t dummy;

	platform_calib_init();

	// If no store of calibration or current screen touch.
	if ((platform_calib_read(&calib) != 0) || (eve_read_tag(&dummy))) {
		// Wait for end of touch.
		while (eve_read_tag(&dummy))
			;

		eve.LIB_BeginCoProList();
		eve.CMD_DLSTART();
		eve.CLEAR_COLOR_RGB(0, 0, 0);
		eve.CLEAR(1, 1, 1);
		eve.COLOR_RGB(255, 255, 255);
		eve.CMD_TEXT(EVE_DISP_WIDTH / 2, EVE_DISP_HEIGHT / 2,
		             28, eve.OPT_CENTERX | eve.OPT_CENTERY, "Please tap on the dots");
		eve.CMD_CALIBRATE(0);
		eve.LIB_EndCoProList();
		if (eve.LIB_AwaitCoProEmpty() != 0) {
			return -1;
		}

		calib.transform[0] = eve.LIB_MemRead32(eve.REG_TOUCH_TRANSFORM_A);
		calib.transform[1] = eve.LIB_MemRead32(eve.REG_TOUCH_TRANSFORM_B);
		calib.transform[2] = eve.LIB_MemRead32(eve.REG_TOUCH_TRANSFORM_C);
		calib.transform[3] = eve.LIB_MemRead32(eve.REG_TOUCH_TRANSFORM_D);
		calib.transform[4] = eve.LIB_MemRead32(eve.REG_TOUCH_TRANSFORM_E);
		calib.transform[5] = eve.LIB_MemRead32(eve.REG_TOUCH_TRANSFORM_F);
		platform_calib_write(&calib);
	} else {
		eve.LIB_MemWrite32(eve.REG_TOUCH_TRANSFORM_A, calib.transform[0]);
		eve.LIB_MemWrite32(eve.REG_TOUCH_TRANSFORM_B, calib.transform[1]);
		eve.LIB_MemWrite32(eve.REG_TOUCH_TRANSFORM_C, calib.transform[2]);
		eve.LIB_MemWrite32(eve.REG_TOUCH_TRANSFORM_D, calib.transform[3]);
		eve.LIB_MemWrite32(eve.REG_TOUCH_TRANSFORM_E, calib.transform[4]);
		eve.LIB_MemWrite32(eve.REG_TOUCH_TRANSFORM_F, calib.transform[5]);
	}
	return 0;
}
