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
#ifdef _WIN32
	errno_t err;

    if ((err = fopen_s(&h1, config_file, "w")) == 0)
#else // _WIN32
    if ((h1 = fopen(config_file, "w")) != 0)
#endif // _WIN32
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
#ifdef _WIN32
	errno_t err;

    if ((err = fopen_s(&h1, config_file, "r")) == 0)
#else // _WIN32
    if ((h1 = fopen(config_file, "r")) != 0)
#endif // _WIN32
    {
        fread(calib, sizeof(struct touchscreen_calibration), 1, h1);
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
}

void setup(void)
{
    /* Print out a welcome message... */
    printf ("(C) Copyright, Bridgetek Pte. Ltd. \r\n \r\n");
    printf ("---------------------------------------------------------------- \r\n");
    printf ("Welcome to BRT_AN_025 Example for Windows MPSSE Library\r\n");
    printf ("\n");
	printf ("Cable colours are as used for FTDI MPSSE cables. MPSSE Bus \n");
    printf ("numbers refer to the MPSSE interface signals.\n");
    printf ("Pin configuration for example:\n");
	printf ("Use MPSSE SPI connections as per Application Note AN_188:\n\n");
	printf ("Bus     Cable           Name\n");
	printf ("---     -----           ----\n");
	printf ("MPSSE0  SK (Orange)     SCLK\n");
	printf ("MPSSE1  DO (Yellow)     MOSI - Master Out Slave In\n");
	printf ("MPSSE2  DI (Green)      MISO - Master In Slave Out\n");
	printf ("MPSSE3  CS (Brown)      CS - Serial Chip Select pin\n");
	printf ("MPSSE7  GPIO L3 (Blue)  PD# - Powerdown pin\n");
	printf ("N/A     VBUS (Red)      5v supply for FT8xx\n");
	printf ("N/A     GND (Black)     Signal GND for SPI\n");
}
