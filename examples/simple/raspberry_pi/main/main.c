/**
  @file main.c
 */
/*
 * ============================================================================
 * History
 * =======
 * Nov 2019		Initial version
 *
 *
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

#include "EVE.h"

#include "eve_example.h"

/* CONSTANTS ***********************************************************************/

/**
 * @brief Filename to store touchscreen configuration.
 */
const char *config_file = "config.bin";

/* LOCAL FUNCTIONS / INLINES *******************************************************/

void setup(void);

/* FUNCTIONS ***********************************************************************/

/**
 * @brief Functions used to store calibration data in file.
 */
//@{
int8_t platform_calib_init(void)
{
    return 1;
}

int8_t platform_calib_write(struct touchscreen_calibration *calib)
{
    FILE *h1;

    if ((h1 = fopen(config_file, "w")) != NULL)
    {
        fwrite(calib, sizeof(struct touchscreen_calibration), 1, h1);
        fclose(h1); 
		return 0;
    }
    return -1;
}

int8_t platform_calib_read(struct touchscreen_calibration *calib)
{
    FILE *h1;
    size_t res;

    if ((h1 = fopen(config_file, "r")) != NULL)
    {
        res = fread(calib, sizeof(struct touchscreen_calibration), 1, h1);
        (void)res;
        fclose(h1); 
		return 0;
    }
    return -1;
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
#if DEBUG_LEVEL > 0
    /* Print out a welcome message... */
    printf ("(C) Copyright, Bridgetek Pte. Ltd. \r\n \r\n");
    printf ("---------------------------------------------------------------- \r\n");
    printf ("Welcome to EVE-MCU-Dev Simple Example for Raspberry Pi\r\n");
    printf ("\n");
	printf ("Pin configuration for example:\n");
	printf ("Use SPI hardware bus which is /dev/spidev0.0 in Linux.\n");
	printf ("J8 Pin 19 - MOSI (SPI0_D1)\n");
	printf ("J8 Pin 21 - MISO (SPI0_D0)\n");
	printf ("J8 Pin 23 - SCLK (SPI0_SCLK)\n");
	printf ("J8 Pin 22 - CS (GPIO25) - Note this is not the SPI0_CS0 pin\n");
	printf ("J8 Pin 18 - PD# (GPIO24) - Powerdown pin\n");
	printf ("J8 Pin 2 - 5v supply for FT8xx\n");
	printf ("J8 Pin 25 - signal GND for SPI\n");
#endif
}

