/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "pico/stdlib.h"

/**
  @file main.c
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
 * appliance. There are exclusions of FTDI liability for certain types of loss
 * such as: special loss or damage; incidental loss or damage; indirect or
 * consequential loss or damage; loss of income; loss of business; loss of
 * profits; loss of revenue; loss of contracts; business interruption; loss of
 * the use of money or anticipated savings; loss of information; loss of
 * opportunity; loss of goodwill or reputation; and/or loss of, damage to or
 * corruption of data.
 * There is a monetary cap on FTDI's liability.
 * The Software may have subsequently been amended by another user and then
 * distributed by that other user ("Adapted Software").  If so that user may
 * have additional licence terms that apply to those amendments. However, FTDI
 * has no liability in relation to those amendments.
 * ============================================================================
 */

/* INCLUDES ************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* Flash read/write capability for storing Non-volatile data */
#include <hardware/flash.h>
#include <hardware/sync.h>

#include <EVE.h>

#include "eve_example.h"

/* CONSTANTS ***********************************************************************/

/**
 * @brief Location in flash to store touchscreen configuration.
 */
#define FLASH_OFFSET_CONFIG (256 * 1024)

/* LOCAL FUNCTIONS / INLINES *******************************************************/

void setup(void);

/* FUNCTIONS ***********************************************************************/

/**
 * @brief Functions used to store calibration data in file.
 */
//@{
int8_t platform_calib_init(void)
{
    return 0;
}

int8_t platform_calib_write(struct touchscreen_calibration *calib)
{
#if (PICO_FLASH_SIZE_BYTES < FLASH_OFFSET_CONFIG + FLASH_SECTOR_SIZE)
#error Configuration written above top of flash
#endif
    // Data to write to flash must be aligned to a flash page
    uint8_t config[FLASH_PAGE_SIZE] __aligned(FLASH_PAGE_SIZE);
    uint32_t ints = save_and_disable_interrupts();

	calib->key = VALID_KEY_TOUCHSCREEN;
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
	if (p->key == VALID_KEY_TOUCHSCREEN)
    {
        memcpy(calib, p, sizeof(struct touchscreen_calibration));
        return 0;
    }

    return -2;
}
//@}

int main(void)
{
    /* Setup UART */
    setup();

    /* Start example code */
    eve_example();

    // function never returns
    for (;;) ;
}

void setup(void)
{
    int ch = 0;

#if DEBUG_LEVEL > 0
    // Initialise stdio ports as configured in CMakeLists.txt
    stdio_init_all();
#endif

    // Turn on the pico LED to show activity
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 1);
    
#if DEBUG_LEVEL > 0
    /* Print out a welcome message... */
    printf ("(C) Copyright, Bridgetek Pte. Ltd. \r\n \r\n");
    printf ("---------------------------------------------------------------- \r\n");
    printf ("Welcome to EVE-MCU-Dev Simple Example for Raspberry Pi Pico RP2040\r\n");
    printf ("\n");
	printf ("Pin configuration for example:\n");
	printf ("Use SPI 1 hardware bus to match IDM2040-7A from Bridgetek\n");
	printf ("Pin 15 - MOSI (GPIO11)\n");
	printf ("Pin 16 - MISO (GPIO12)\n");
	printf ("Pin 14 - SCLK (GPIO10)\n");
	printf ("Pin 17 - CS (GPIO13) - Note this is not the SPI0_CS0 pin\n");
	printf ("Pin 10 - PD# (GPIO7) - Powerdown pin\n");
	printf ("Pin 40 - 5v supply for FT8xx\n");
	printf ("Pin 23 - signal GND for SPI\n");
#endif
}
