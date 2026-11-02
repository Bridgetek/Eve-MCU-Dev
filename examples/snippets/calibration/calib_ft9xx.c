/**
 * @file calib_pico.c
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

/* INCLUDES ************************************************************************/

#include <stdint.h>

#include "calib.h"

/* Only compile this code if calibration storage is needed on pico SDK. */
#if CALIBRATION_STORAGE == CALIBRATION_STORAGE_FT9XX

#include <string.h>
#include <ft900.h>

/**
 @brief Defined area in flash to store EVE calibration data.
 @details This 256 byte array is initialised to all 1s. It will be used to store the
      touch screen calibration data. There is a key placed at the start of the array
      when the calibration is programmed, this indicates that the data is valid.
      Only if this array is in place will calibration data be programmed.
      The size is 256 bytes and it is aligned on a 256 byte boundary as the minimum
      programmable Flash sector is 256 bytes.
 */
__flash__ uint8_t __attribute__((aligned(256))) dlog_pm[256] = {[0 ... 255] = 0xff };

/* FUNCTIONS ***********************************************************************/

/**
 * @brief Functions used to store calibration data in flash.
 */
//@{
int8_t platform_calib_init(void)
{
    /* The Flash destination must be aligned to a 256-byte page. */
    if (((uint32_t)dlog_pm & 0xFFU) != 0)
    {
        return -1;
    }

    /* Calibration data must fit within the Flash page. */
    if (sizeof(struct touchscreen_calibration) > 256)
    {
        return -1;
    }

    return 0;
}

int8_t platform_calib_write(struct touchscreen_calibration *calib)
{
    uint8_t	dlog_flash[256] __attribute__((aligned(4)));

    /* Read the existing Flash page into a properly aligned array. */
    CRITICAL_SECTION_BEGIN
    memcpy_flash2dat((void *)dlog_flash, (uint32_t)dlog_pm, sizeof(dlog_flash));
    CRITICAL_SECTION_END

    /* The Flash page must be blank before it can be programmed. */
    for (uint32_t i = 0; i < sizeof(dlog_flash); i++)
    {
        if (dlog_flash[i] != 0xFF)
        {
            return -1;
        }
    }

    /* Copy calibration data into a properly aligned array. */
    memset(dlog_flash, 0xff, sizeof(dlog_flash));
    memcpy(dlog_flash, calib, sizeof(struct touchscreen_calibration));

    CRITICAL_SECTION_BEGIN
    /* This are must be set to 0xff for Flash programming to work. */
    memcpy_dat2flash((uint32_t)dlog_pm, dlog_flash, sizeof(dlog_flash));
    CRITICAL_SECTION_END

    return 0;
}

int8_t platform_calib_read(struct touchscreen_calibration *calib)
{
    uint8_t dlog_flash[256] __attribute__((aligned(4)));

    /* Read calibration data from Flash to a properly aligned array. */
    CRITICAL_SECTION_BEGIN
    memcpy_flash2dat((void *)dlog_flash, (uint32_t)dlog_pm, sizeof(dlog_flash));
    CRITICAL_SECTION_END

    /* Return the stored data */
    memcpy(calib, dlog_flash, sizeof(struct touchscreen_calibration));

    return 0;
}
//@}

#endif // CALIBRATION_STORAGE == CALIBRATION_STORAGE_FT9XX
