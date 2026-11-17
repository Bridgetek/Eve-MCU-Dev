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

/* Include EVE-MCU-Dev library API layer */
#include <EVE.h>

/* EXTERN/FUNCTIONS ****************************************************************/

extern void api_test_dl(uint32_t t1, uint32_t t2);
extern void api_test_cmd(uint32_t t1, uint32_t t2);

/* CONSTANTS ***********************************************************************/

#define T0 0x00000000
#define T1 0xffffffff

#define TA 0xaaaaaaaa
#define T5 0x55555555

/* LOCAL FUNCTIONS / INLINES *******************************************************/

void setup(void);

/* FUNCTIONS ***********************************************************************/

int main(void)
{
    /* Setup UART */
    setup();

    /* Start tester code */
    /* Initialise the display */
    EVE_Init();

    /* Call API compilation test for this API. */

    /* Just display opcodes for display list commands: */
    printf("Test DL Opcodes:\n");
    api_test_dl(T0, T0);

    /* Show the masks for parameters in the display list commands: */
    printf("Test DL Masking:\n");
    api_test_dl(T1, T1);

    /* Generate a pattern in the display list command parameters. */
    printf("Test DL Pattern %x/%x:\n", TA, T5);
    api_test_dl(TA, T5);
    /* Generate the reverse pattern in the display list command parameters. */
    printf("Test DL Pattern %x/%x:\n", T5, TA);
    api_test_dl(T5, TA);
    
    /* Generate a pattern in the co-processor command parameters. */
    printf("Test CMD Pattern %x/%x:\n", TA, T5);
    api_test_cmd(TA, T5);

    /* Go no further - return success */
    return 0;
}

void setup(void)
{
    /* Print out a welcome message... */
    printf ("(C) Copyright, Bridgetek Pte. Ltd.\n\n");
    printf ("----------------------------------------------------------------\n");
    printf ("Welcome to EVE-MCU-Dev API Tester Program\n");
    printf ("\n");
}
