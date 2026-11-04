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
#if CALIBRATION_STORAGE == CALIBRATION_STORAGE_PICO

#include <string.h>

/* Flash read/write capability for storing Non-volatile data */
#include <pico/flash.h>
#include <hardware/flash.h>
#include <hardware/sync.h>

/* CONSTANTS ***********************************************************************/

/**
 * @brief Location in flash to store touchscreen configuration.
 */
#define FLASH_OFFSET_CONFIG (PICO_FLASH_SIZE_BYTES - FLASH_SECTOR_SIZE)

/* LOCAL VARIABLES *****************************************************************/

static uint8_t config[FLASH_PAGE_SIZE] __aligned(FLASH_PAGE_SIZE) = {0};
const uint8_t *flash_config = (const uint8_t *) (XIP_BASE + FLASH_OFFSET_CONFIG);

/* LOCAL FUNCTIONS / INLINES *******************************************************/

// This function will be called when it's safe to call flash_range_erase
static void call_flash_range_erase(void *param) {
    uint32_t offset = (uint32_t)param;
    flash_range_erase(offset, FLASH_SECTOR_SIZE);
}

// This function will be called when it's safe to call flash_range_program
static void call_flash_range_program(void *param) {
    uint32_t offset = ((uintptr_t*)param)[0];
    const uint8_t *data = (const uint8_t *)((uintptr_t*)param)[1];
    flash_range_program(offset, data, FLASH_PAGE_SIZE);
}

/* FUNCTIONS ***********************************************************************/

/**
 * @brief Functions used to store calibration data in flash.
 * @details Note that this stored the calibration data at the very top sector of flash.
 * This method is not recommended usage as a program may utilise this flash area.
 * The implementation here is simply to show how the calibration information is 
 * obtained and stored. Other methods should be explored for a completed application.
 * A proper implementation of this method could modify the linker script to
 * reserve this area. 
 */
//@{
int8_t platform_calib_init(void)
{
    return 0;
}

int8_t platform_calib_write(void *calib, uint32_t len)
{
    // Data to write to flash must be aligned to a flash page

    // Flash is "execute in place" and so will be in use when any code that is stored in flash runs, e.g. an interrupt handler
    // or code running on a different core.
    // Calling flash_range_erase or flash_range_program at the same time as flash is running code would cause a crash.
    // flash_safe_execute disables interrupts and tries to cooperate with the other core to ensure flash is not in use
    // See the documentation for flash_safe_execute and its assumptions and limitations
    int rc = flash_safe_execute(call_flash_range_erase, (void*)FLASH_OFFSET_CONFIG, UINT32_MAX);
    hard_assert(rc == PICO_OK);

    memset(config, 0xff, FLASH_PAGE_SIZE);
    memcpy(config, calib, len);

    uintptr_t params[] = {FLASH_OFFSET_CONFIG, (uintptr_t)config};
    rc = flash_safe_execute(call_flash_range_program, params, UINT32_MAX);
    hard_assert(rc == PICO_OK);

    return 0;
}

int8_t platform_calib_read(void *calib, uint32_t len)
{
    memcpy(calib, flash_config, len);
    return 0;
}
//@}

#endif // CALIBRATION_STORAGE == CALIBRATION_STORAGE_PICO
