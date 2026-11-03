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

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* Include EVE-MCU-Dev library API layer and other required headers. */
#include <eve_example.h>

/* CONSTANTS ***********************************************************************/


/* LOCAL FUNCTIONS / INLINES *******************************************************/

void setup(void);

/* FUNCTIONS ***********************************************************************/

int main(int argc, char **argv)
{
    char *assets = NULL;

    /* Setup */
    setup();

#if (ASSETS == USE_FLASH)
    EVE_DEBUG_PRINTF("Ensure device Flash is programmed with image in \"assets\" directory.\n");
#elif (ASSETS == USE_FLASHIMAGE) || (ASSETS == USE_FILES)
    if (argc > 1)
    {
        assets = argv[1];
    }
    else
    {
#if (ASSETS == USE_FLASHIMAGE)
        EVE_DEBUG_PRINTF("USAGE: The path to the flash images must be passed as a parameter.\n");
#else // (ASSETS == USE_FILES)
        EVE_DEBUG_PRINTF("USAGE: The path to the assets must be passed as a parameter.\n");
#endif
        exit(-1);
    }
#endif

    /* Start example code */
    eve_example(assets);
}

void setup(void)
{
    /* Print out a welcome message... */
    printf ("(C) Copyright, Bridgetek Pte. Ltd. \r\n \r\n");
    printf ("---------------------------------------------------------------- \r\n");
    printf ("Welcome to the EVE-MCU-Dev Racecar Example for Windows FT4222 Library\r\n");
    printf ("\n");

}
