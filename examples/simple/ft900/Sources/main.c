/**
  @file main.c
  @brief
  Empty source file for a new FT90x project.

 */
/*
 * ============================================================================
 * (C) Copyright Bridgetek Pte Ltd
 * ============================================================================
 *
 * This source code ("the Software") is provided by Bridgetek Pte Ltd
 *  ("Bridgetek ") subject to the licence terms set out
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

#include <ft900.h>
#include <ft900_dlog.h>
#include <ft900_memctl.h>

#include <EVE.h>

#include "eve_example.h"

#if defined(__FT900__)
#define GPIO_UART0_TX 48
#define GPIO_UART0_RX 49
#elif defined(__FT930__)
#define GPIO_UART0_TX 23
#define GPIO_UART0_RX 22
#endif

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

/* CONSTANTS ***********************************************************************/

/* LOCAL FUNCTIONS / INLINES *******************************************************/

void setup(void);
void debug_uart_init(void);

/* FUNCTIONS ***********************************************************************/

/**
 * @brief Functions used to store calibration data in flash.
 */
//@{
int8_t platform_calib_init(void)
{
	const __flash__ struct touchscreen_calibration *pcalibpm = (const __flash__ struct touchscreen_calibration *)dlog_pm;

	/* Check that there is an area set aside in the data section for calibration data. */
	if ((pcalibpm->key != VALID_KEY_TOUCHSCREEN) && (pcalibpm->key != 0xFFFFFFFF))
	{
		if (((uint32_t)pcalibpm & 255) != 0)
		{
			/* An aligned 256 byte Flash sector not has been correctly setup for calibration data. */
			return -1;
		}
	}

	return 0;
}

int8_t platform_calib_write(struct touchscreen_calibration *calib)
{
	uint8_t	dlog_flash[260] __attribute__((aligned(4)));
	struct touchscreen_calibration *pcalibflash = (struct touchscreen_calibration *)dlog_flash;

	/* Read calibration data from Flash to a properly aligned array. */
	CRITICAL_SECTION_BEGIN
	memcpy_flash2dat((void *)dlog_flash, (uint32_t)dlog_pm, 256);
	CRITICAL_SECTION_END

	/* Check that the Flash blank, so it is possible to write to this sector in Flash. */
	if (pcalibflash->key == 0xFFFFFFFF)
	{
		/* Copy calibration data into a properly aligned array. */
		calib->key = VALID_KEY_TOUCHSCREEN;
		memset(dlog_flash, 0xff, sizeof(dlog_flash));
		memcpy(dlog_flash, calib, sizeof(struct touchscreen_calibration));

		CRITICAL_SECTION_BEGIN
		/* This are must be set to 0xff for Flash programming to work. */
		memcpy_dat2flash ((uint32_t)dlog_pm, dlog_flash, 256);
		CRITICAL_SECTION_END;

		return 0;
	}

	return -1;
}

int8_t platform_calib_read(struct touchscreen_calibration *calib)
{
	uint8_t dlog_flash[256] __attribute__((aligned(4)));
	struct touchscreen_calibration *pcalibflash = (struct touchscreen_calibration *)dlog_flash;

	/* Read calibration data from Flash to a properly aligned array. */
	CRITICAL_SECTION_BEGIN
	memcpy_flash2dat((void *)dlog_flash, (uint32_t)dlog_pm, 256);
	CRITICAL_SECTION_END

	/* Flash blank, program memory blank: flash calibration data blank. */
	if (pcalibflash->key != VALID_KEY_TOUCHSCREEN)
	{
		return -2;
	}

	/* Calibration data is valid. */
	memcpy(calib, dlog_flash, sizeof(struct touchscreen_calibration));

	return 0;
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
	// UART initialisation
	debug_uart_init();

#if DEBUG_LEVEL > 0
	/* Print out a welcome message... */
	printf ("(C) Copyright, Bridgetek Pte. Ltd. \r\n \r\n");
	printf ("---------------------------------------------------------------- \r\n");
	printf ("Welcome to EVE-MCU-Dev Simple Example for FT9xx\r\n");
#endif
}

/* Initializes the UART for the testing */
void debug_uart_init(void)
{
	/* Enable the UART Device... */
	sys_enable(sys_device_uart0);
#if defined(__FT930__)
	/* Make GPIO23 function as UART0_TXD and GPIO22 function as UART0_RXD... */
	gpio_function(23, pad_uart0_txd); /* UART0 TXD */
	gpio_function(22, pad_uart0_rxd); /* UART0 RXD */
#else
	/* Make GPIO48 function as UART0_TXD and GPIO49 function as UART0_RXD... */
	gpio_function(48, pad_uart0_txd); /* UART0 TXD MM900EVxA CN3 pin 4 */
	gpio_function(49, pad_uart0_rxd); /* UART0 RXD MM900EVxA CN3 pin 6 */
	gpio_function(50, pad_uart0_rts); /* UART0 RTS MM900EVxA CN3 pin 8 */
	gpio_function(51, pad_uart0_cts); /* UART0 CTS MM900EVxA CN3 pin 10 */
#endif

	// Open the UART using the coding required.
	uart_open(UART0,                    /* Device */
			1,                        /* Prescaler = 1 */
			UART_DIVIDER_115200_BAUD,  /* Divider = 1302 */
			uart_data_bits_8,         /* No. buffer Bits */
			uart_parity_none,         /* Parity */
			uart_stop_bits_1);        /* No. Stop Bits */

#if DEBUG_LEVEL > 0
	uart_puts(UART0,
			"\x1B[2J" /* ANSI/VT100 - Clear the Screen */
			"\x1B[H\r\n"  /* ANSI/VT100 - Move Cursor to Home */
	);
#endif
}
