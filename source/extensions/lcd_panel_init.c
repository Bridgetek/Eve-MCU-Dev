/**
 * @file lcd_panel_init.c
 * @brief Optional LCD panel driver initialisation support.
 *
 * @details  Provides the lcd_driver_init() function, which is called 
 *      from EVE_Hal.c when EVE_LCD_INIT is defined before EVE initialisation.
 *      MCU-specific functionality is required to implement the interface
 *      between the host MCU and the LCD panel driver.
 *
 *      The LCD driver may share the SPI interface used by EVE, provided
 *      that a separate chip-select (CS#) signal is used. Alternatively, a 
 *      separate SPI interface may be used where implemented by the target hardware.
 *
 *      LCD driver commands may also be sent by bit-banging the required
 *      GPIO signals instead of using a hardware SPI peripheral.
 *
 * @note This file includes an example implementation for the ST7701S LCD 
 *      driver used on the Bridgetek IDM2040-21R module which utilses a 
 *      RP2040 MCU and bit banging.
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

/* Include the EVE macros and settings derived from the EVE configuration */
#include "EVE_settings.h"

#if defined(EVE_LCD_INIT)

/* LCD PANEL INIT */

/* INCLUDES ************************************************************************/

#include <extensions/lcd_panel_init.h>

#include <stdint.h>

/* FUNCTIONS ***********************************************************************/

#if defined(EVE_MODULE) && (EVE_MODULE == EVE_IDM204021R) /* IDM2040-21R LCD panel initialisation */

#include "pico/stdlib.h"
#include <stdbool.h>

// Pins to match Bridgetek IDM2040-21R module.
static const uint lcd_cs_pin = 17;
static const uint lcd_sck_pin = 10;
static const uint lcd_mosi_pin = 11;
static const uint lcd_reset_pin = 22;

/**
 * @brief Send a single command or data byte to the ST7701S.
 *
 * The ST7701S uses a 9-bit serial interface. The first bit selects
 * whether the following 8-bit value is a command or data:
 *
 *     0 = command
 *     1 = data
 *
 * The transfer is generated directly using RP2040 GPIO pins and the
 * payload is transmitted MSB first.
 */
static inline void lcd_send_byte(uint8_t data, bool command)
{
    /* Assert chip select and start with the clock low. */
    gpio_put(lcd_cs_pin, 0);
    gpio_put(lcd_sck_pin, 0);

    /* Send the command/data selector bit. */
    gpio_put(lcd_mosi_pin, command ? 0 : 1);

    sleep_ms(1);
    gpio_put(lcd_sck_pin, 1);
    sleep_ms(1);

    /* Send the eight-bit command or data value, MSB first. */
    for (uint8_t i = 0; i < 8; i++)
    {
        gpio_put(lcd_sck_pin, 0);

        gpio_put(lcd_mosi_pin, (data & 0x80U) ? 1 : 0);

        sleep_ms(1);
        gpio_put(lcd_sck_pin, 1);
        sleep_ms(1);

        data <<= 1;
    }

    /* Complete the transfer and release chip select. */
    gpio_put(lcd_sck_pin, 0);
    gpio_put(lcd_cs_pin, 1);
}

/**
 * @brief Send an ST7701S command byte.
 *
 * @param command Command byte to transmit.
 */
static inline void lcd_write_command(uint8_t command)
{
    lcd_send_byte(command, true);
}

/**
 * @brief Send an ST7701S data byte.
 *
 * @param data Data byte to transmit.
 */
static inline void lcd_write_data(uint8_t data)
{
    lcd_send_byte(data, false);
}

/**
 * @brief Configure the ST7701S LCD driver on the IDM2040-21R module.
 *
 * The configuration table stores each ST7701S command as:
 *
 *     length, command, data...
 *
 * The length includes the command byte itself. A zero-length entry marks
 * the end of the table.
 */
static inline void ST7701S_init(void)
{
    /* ST7701S configuration for the IDM2040-21R */
    // clang-format off
    static const uint8_t st7701s_commands[] = {
      /* Length includes command byte */
      /* Select command page 0. */
      6,  0xFF, 0x77, 0x01, 0x00, 0x00, 0x10,
      /* Display timing and frame configuration. */
      3,  0xC0, 0x3B, 0x00,						// Display line setting
      3,  0xC1, 0x0B, 0x02,						// Porch control
      3,  0xC2, 0x07, 0x02,						// Inversion / frame rate control
      /* Positive voltage gamma control. */
      17, 0xB0, 0x00, 0x11, 0x16, 0x0E, 0x11, 0x06, 0x05, 0x09,
          0x08, 0x21, 0x06, 0x13, 0x10, 0x29, 0x31, 0x18,
      /* Negative voltage gamma control. */
      17, 0xB1, 0x00, 0x11, 0x16, 0x0E, 0x11, 0x07, 0x05, 0x09,
          0x09, 0x21, 0x05, 0x13, 0x11, 0x2A, 0x31, 0x18,
      /* Select command page 1. */
      6,  0xFF, 0x77, 0x01, 0x00, 0x00, 0x11,
      /* Panel voltage and power configuration. */
      2,  0xB0, 0x6D,			// Vop amplitude
      2,  0xB1, 0x37,			// VCOM amplitude
      2,  0xB2, 0x81,			// VGH voltage
      2,  0xB3, 0x80,			// Test command setting
      2,  0xB5, 0x43,			// VGL voltage
      2,  0xB7, 0x85,			// Power control 1
      2,  0xB8, 0x20,			// Power control 2
      /* Source driver timing configuration. */
      2,  0xC1, 0x78,			// Source pre-drive timing
      2,  0xC2, 0x78,			// Source EQ2 setting
      /* RGB and colour configuration. */
      2,  0xC3, 0x8C,			// RGB control
      2,  0xCD, 0x08,			// Colour control
      /* Interface configuration. */
      2,  0xD0, 0x88,			// MIPI setting 1
      /* Image enhancement and noise reduction. */
      4,  0xE0, 0x00, 0x00, 0x02,
      6,  0xE1, 0x03, 0xA0, 0x00, 0x00, 0x04,
      /* Sharpness control. */
      14, 0xE2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      /* Colour calibration control. */
      5,  0xE3, 0x00, 0x00, 0x11, 0x00,
      /* Skin tone control. */
      3,  0xE4, 0x22, 0x00,
      /* Gate-in-panel timing configuration. */
      17, 0xE5, 0x05, 0xEC, 0xA0, 0xA0, 0x07, 0xEE, 0xA0, 0xA0,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      5,  0xE6, 0x00, 0x00, 0x11, 0x00,
      3,  0xE7, 0x22, 0x00,
      17, 0xE8, 0x06, 0xED, 0xA0, 0xA0, 0x08, 0xEF, 0xA0, 0xA0,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      8,  0xEB, 0x00, 0x00, 0x40, 0x40, 0x00, 0x00, 0x00,
      17, 0xED, 0xFF, 0xFF, 0xFF, 0xBA, 0x0A, 0xBF, 0x45, 0xFF,
          0xFF, 0x54, 0xFB, 0xA0, 0xAB, 0xFF, 0xFF, 0xFF,
      7,  0xEF, 0x10, 0x0D, 0x04, 0x08, 0x3F, 0x1F,
      /* Configure VAP and VAN settings. */
      6,  0xFF, 0x77, 0x01, 0x00, 0x00, 0x13,
      2,  0xEF, 0x08,
      /* Return to command page 0. */
      6,  0xFF, 0x77, 0x01, 0x00, 0x00, 0x00,
      /* Display orientation. */
      2,  0x36, 0x00,
      /* RGB pixel format. */
      2,  0x3A, 0x66,
      /* End of configuration table. */
      0
    };
    // clang-format on

    /* Process each table entry. The first byte contains the total entry
     * length, followed by one command byte and zero or more associated data bytes. */
    for (const uint8_t *p = st7701s_commands; *p != 0;)
    {
      uint8_t len = *p++;

      /* First byte of each entry is always the command. */
      lcd_write_command(*p++);

      /* Send any remaining bytes as command parameters. */
      for (uint8_t i = 1; i < len; i++)
          lcd_write_data(*p++);
    }

    /* Exit sleep mode. The panel requires a delay after this
     * command before it can safely be enabled. */
    lcd_write_command(0x11);
    sleep_ms(100);

    /* Enable display output. */
    lcd_write_command(0x29);
}

/**
 * @brief Initialise the ST7701S LCD interface on the IDM2040-21R module.
 *
 * Configures the GPIO used by the ST7701S serial interface, resets the
 * display driver, and loads the panel configuration.
 */
static inline void IDM204021R_LCD_Init(void)
{
    /* Configure the ST7701S serial interface pins as GPIO outputs.
     *
     * These pins are driven directly to generate the 9-bit command/data
     * protocol required by the LCD driver. */
    gpio_init(lcd_cs_pin);
    gpio_init(lcd_sck_pin);
    gpio_init(lcd_mosi_pin);

    gpio_set_dir(lcd_cs_pin, GPIO_OUT);
    gpio_set_dir(lcd_sck_pin, GPIO_OUT);
    gpio_set_dir(lcd_mosi_pin, GPIO_OUT);

    /* Set the serial interface to its idle state.
     * Chip select is inactive high and the serial clock idles low. */
    gpio_put(lcd_cs_pin, 1);
    gpio_put(lcd_sck_pin, 0);
    gpio_put(lcd_mosi_pin, 1);

    /* Reset the ST7701S before loading the panel configuration.
     * This ensures the LCD driver starts from a known state. */
    gpio_init(lcd_reset_pin);
    gpio_set_dir(lcd_reset_pin, GPIO_OUT);

    gpio_put(lcd_reset_pin, 1);
    sleep_ms(20);

    gpio_put(lcd_reset_pin, 0);
    sleep_ms(20);

    gpio_put(lcd_reset_pin, 1);
    sleep_ms(20);

    /* Program the ST7701S with the IDM2040-21R panel configuration. */
    ST7701S_init();
}

#endif /* defined(EVE_MODULE) && (EVE_MODULE == EVE_IDM204021R) */

/**
 * @brief Initialise the LCD panel driver, where required.
 */
int lcd_driver_init(void)
{

#if defined(EVE_MODULE) && (EVE_MODULE == EVE_IDM204021R)
    // run the IDM2040-21R LCD init if the module is defined.
    IDM204021R_LCD_Init();
#endif /* defined(EVE_MODULE) && (EVE_MODULE == EVE_IDM204021R) */

    return 0;
}

/* LCD PANEL INIT */

#endif /* defined(EVE_LCD_INIT) */
