/**
 * @file main.c
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

#include "pico/stdlib.h"
#include "pico/time.h"

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

/* LOCAL FUNCTIONS / INLINES *******************************************************/

void setup(void);

/* FUNCTIONS ***********************************************************************/

/** @brief Portable version of fopen/fopen_s
 */
//@{
FILE *port_fopen(char const * _FileName, char const * _Mode)
{
#if defined(_MSC_VER)
    FILE *h1;
    errno_t err;
    err = fopen_s(&h1, _FileName, _Mode);
    if (err)
    {
        return NULL;
    }
    return h1;
#else
    return fopen(_FileName, _Mode);
#endif
}
//@}

/** @brief Functions used to get platform time
 */
//@{
uint32_t platform_get_time(void)
{
    uint32_t time_ms;
    time_ms = to_ms_since_boot(get_absolute_time());
    return time_ms;
}
//@}

int main(void)
{
    /* Setup UART */
    setup();

    /* Start example code */
    eve_example(NULL);

    // function never returns
    for (;;) ;
}

void setup(void)
{
    int ch = 0;
    // Initialise stdio ports as configured in CMakeLists.txt
    stdio_init_all();

    // Turn on the pico LED to show activity
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 1);
    
#if DEBUG_LEVEL > 0
    // Wait for stdio (UART/USB) connection if enabled
    // NOTE: this will prevent the example from running until the COM port presented to the host machine is opened
    while (!stdio_usb_connected()) { sleep_ms(100);  }
#endif

    /* Print out a welcome message... */
    printf ("(C) Copyright, Bridgetek Pte. Ltd. \r\n \r\n");
    printf ("---------------------------------------------------------------- \r\n");
    printf ("Welcome to the EVE-MCU-Dev Racecar Example for Raspberry Pi Pico RP2040\r\n");
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

}
