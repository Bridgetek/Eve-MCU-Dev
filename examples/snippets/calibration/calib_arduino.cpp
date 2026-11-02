/**
 * @file calib_arduino.cpp
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

extern "C" {
#include "calib.h"
}

/* Only compile this code if calibration storage is needed on Arduino. */

/**
 * @brief Functions used to store calibration data in EEPROM.
 * @details Boards with an EEPROM library
 */
//@{

#if CALIBRATION_STORAGE == CALIBRATION_STORAGE_ARDUINO_EEPROM

/* INCLUDES ************************************************************************/

#include <EEPROM.h>

/* FUNCTIONS ***********************************************************************/

int8_t platform_calib_init(void)
{
    return 0;
}

int8_t platform_calib_write(struct touchscreen_calibration *calib)
{
    EEPROM.begin();
    EEPROM.put(0, *calib);
    EEPROM.end();
    return 0;
}

int8_t platform_calib_read(struct touchscreen_calibration *calib)
{
    EEPROM.begin();
    EEPROM.get(0, *calib);
    EEPROM.end();
    return 0;
}

#elif CALIBRATION_STORAGE == CALIBRATION_STORAGE_ARDUINO_EEPROM_ESP32

/* INCLUDES ************************************************************************/

#include <EEPROM.h>

/* FUNCTIONS ***********************************************************************/

int8_t platform_calib_init(void)
{
    return 0;
}

int8_t platform_calib_write(struct touchscreen_calibration *calib)
{
    EEPROM.begin(sizeof(struct touchscreen_calibration));
    EEPROM.put(0, *calib);
    EEPROM.commit();
    EEPROM.end();
    return 0;
}

int8_t platform_calib_read(struct touchscreen_calibration *calib)
{
    EEPROM.begin(sizeof(struct touchscreen_calibration));
    EEPROM.get(0, *calib);
    EEPROM.end();
    return 0;
}

#elif CALIBRATION_STORAGE == CALIBRATION_STORAGE_ARDUINO_FLASH_PICO

/* CONSTANTS ***********************************************************************/

/**
 * @brief Location in flash to store touchscreen configuration.
 */
#define FLASH_OFFSET_CONFIG (PICO_FLASH_SIZE_BYTES - FLASH_SECTOR_SIZE)

/* LOCAL VARIABLES *****************************************************************/

static uint8_t config[FLASH_PAGE_SIZE] __aligned(FLASH_PAGE_SIZE) = {0};

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

int8_t platform_calib_write(struct touchscreen_calibration *calib)
{
    // Data to write to flash must be aligned to a flash page
    uint32_t ints = save_and_disable_interrupts();

    memset(config, 0xff, FLASH_PAGE_SIZE);
    memcpy(config, calib, sizeof(struct touchscreen_calibration));

    flash_range_erase(FLASH_OFFSET_CONFIG, FLASH_SECTOR_SIZE);
    flash_range_program(FLASH_OFFSET_CONFIG, (const uint8_t *)config, FLASH_PAGE_SIZE);
    restore_interrupts (ints);

    return 0;
}

int8_t platform_calib_read(struct touchscreen_calibration *calib)
{
    struct touchscreen_calibration *p = (struct touchscreen_calibration *)(XIP_BASE + FLASH_OFFSET_CONFIG);
    memcpy(calib, p, sizeof(struct touchscreen_calibration));
    return 0;
}

#else

int8_t platform_calib_init(void)
{
    return -1;
}

int8_t platform_calib_write(struct touchscreen_calibration *calib)
{
    return 0;
}

int8_t platform_calib_read(struct touchscreen_calibration *calib)
{
    return -1;
}

#endif // CALIBRATION_STORAGE == CALIBRATION_STORAGE_ARDUINO_EEPROM_*
//@}
