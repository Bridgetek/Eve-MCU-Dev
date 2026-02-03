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
 * https://brtchip.com/wp-content/uploads/2021/11/BRT_Software_License_Agreement.pdf ("the Licence Terms").
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
 * appliance. There are exclusions of Bridgetek liability for certain types
of loss
 * such as: special loss or damage; incidental loss or damage; indirect or
 * consequential loss or damage; loss of income; loss of business; loss of
 * profits; loss of revenue; loss of contracts; business interruption; loss of
 * the use of money or anticipated savings; loss of information; loss of
 * opportunity; loss of goodwill or reputation; and/or loss of, damage to or
 * corruption of data.
 * There is a monetary cap on Bridgetek's liability.
 * The Software may have subsequently been amended by another user and then
 * distributed by that other user ("Adapted Software").  If so that user may
 * have additional licence terms that apply to those amendments. However,
Bridgetek
 * has no liability in relation to those amendments.
 * ============================================================================
 */


#define bswap16(x) (((x) >> 8) | ((x) << 8))
#define bswap32(x) (((x) >> 24) | (((x) & 0x00FF0000) >> 8) \
                  | (((x) & 0x0000FF00) << 8) | ((x) << 24))

#include <string.h>
#include <stdint.h> // for Uint8/16/32 and Int8/16/32 data types

#include <EVE.h>

#include <eve_example.h>

//@{
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
//@}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{

    eve_example();

    /* Infinite loop */
    while (1)
    {
    };

    return 0;
}

