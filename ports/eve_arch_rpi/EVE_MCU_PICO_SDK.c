/**
 * @file EVE_MCU_PICO_SDK.c
 * @details MCU interface for Raspberry Pi pico SDK devices.
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

// Guard against being used for incorrect platform or architecture.
// Note: PLATFORM_RP2040 will be deprecated.
#if defined(PLATFORM_RP2040) || defined(PLATFORM_PICO_SDK)

#pragma message "Compiling " __FILE__ " for Raspberry Pi pico SDK"

/* EVE MCU END */

#include <string.h>
#include <stdint.h> // for Uint8/16/32 and Int8/16/32 data types
#include <stdio.h>
#include <machine/endian.h>

#include <hardware/gpio.h>
#include <hardware/spi.h>
#include <pico/time.h>

/* Include functions for EVE-MCU-Dev library MCU layer */
#include <MCU.h>

#if defined(EVE_QSPI_ENABLE)
#error "EVE_QSPI_ENABLE is not supported by the Raspberry Pi pico SDK ports"
#endif

/* Raspberry Pi Pico pins. CS# remains under software GPIO control. */
/* Pins to match Bridgetek IDM2040-7A/MM2040EV boards. */
static const uint eve_pd_pin = 7u;
static const uint eve_cs_pin = 5u;
static const uint eve_sck_pin = 2u;
static const uint eve_mosi_pin = 3u;
static const uint eve_miso_pin = 4u;
static const uint eve_int_pin = 6u;
// Port to match Bridgetek IDM2040-7A/MM2040EV boards.
static spi_inst_t *eve_spi_port = spi0;

/* EVE MCU HEADER END */

/* EVE MCU */

// This is the pico SDK Platform specific section and contains the functions which
// enable the GPIO and SPI interfaces.

// ------------------- MCU specific initialisation  ----------------------------
int MCU_Init(void)
{

	// Initialise CS (SPI Chip Select) pin high
    gpio_init(eve_cs_pin);
    gpio_set_dir(eve_cs_pin, GPIO_OUT);
    gpio_put(eve_cs_pin, 1u);

    // Initialise PD (Power Down) pin high
    gpio_init(eve_pd_pin);
    gpio_set_dir(eve_pd_pin, GPIO_OUT);
    gpio_put(eve_pd_pin, 1u);

    // Initialise INT# (Interrupt) pin input
    gpio_init(eve_int_pin);
    gpio_set_dir(eve_int_pin, GPIO_IN);

    // Set SPI clock speed to 1 MHz
    // 1 MHz allows all EVE devices to initialise correctly
    // After initialisation the SPI speed can be increased in the MCU_Setup()
    spi_init(eve_spi_port, 1 * 1000u * 1000u);

    // Set SPI format
    spi_set_format( eve_spi_port,   // SPI instance
                    8u,             // Number of bits per transfer
                    SPI_CPOL_0,     // Polarity (CPOL)
                    SPI_CPHA_0,     // Phase (CPHA)
                    SPI_MSB_FIRST);

    // Initialize SPI pin functions
    gpio_set_function(eve_sck_pin, GPIO_FUNC_SPI);
    gpio_set_function(eve_mosi_pin, GPIO_FUNC_SPI);
    gpio_set_function(eve_miso_pin, GPIO_FUNC_SPI);

    return 0;
}

int MCU_Deinit(void)
{
    /* Leave EVE in a safe state. */
    gpio_put(eve_cs_pin, 1u);
    gpio_put(eve_pd_pin, 0u);

    /* Disable SPI. */
    spi_deinit(eve_spi_port);

    return 0;
}

int MCU_Setup(void)
{
    /* Additional SPI Configuration */
    // Increase SPI speed to 25 MHz after initialisation is complete
    // See the notes for EVE_SPI_TIMEOUT in the MCU.h file.
    spi_init(eve_spi_port, 25u * 1000u * 1000u);

    return 0;
}

#if defined(EVE_QSPI_ENABLE)
int MCU_SetSPIMode(uint8_t mode)
{
    /* QSPI Configuration */
    #error EVE_QSPI_ENABLE (QSPI interfaces to EVE) is not supported pico SDK
    return -1;
}
#endif // defined(EVE_QSPI_ENABLE)

// ########################### GPIO CONTROL ####################################

// --------------------- Chip Select line low ----------------------------------
inline void MCU_CSlow(void)
{
    gpio_put(eve_cs_pin, 0u);
}  

// --------------------- Chip Select line high ---------------------------------
inline void MCU_CShigh(void)
{
    gpio_put(eve_cs_pin, 1u);
}

// -------------------------- PD line low --------------------------------------
inline void MCU_PDlow(void)
{
    gpio_put(eve_pd_pin, 0u);
}

// ------------------------- PD line high --------------------------------------
inline void MCU_PDhigh(void)
{
    gpio_put(eve_pd_pin, 1u);
}

// ------------------------ interrupt input ------------------------------------
inline int MCU_Int(void) 
{
    bool val = gpio_get(eve_int_pin);
    return (int)val;
}

// --------------------- SPI Send and Receive ----------------------------------

uint8_t MCU_SPIRead8(void)
{
    uint8_t DataRead = 0u;

    // Note: This platform is LITTLE_ENDIAN. 
    // Buffer receives of integers will be little endian.
    spi_read_blocking(eve_spi_port, 0u, &DataRead, 1u);

    return DataRead;
}

void MCU_SPIWrite8(uint8_t DataToWrite)
{
    // Note: This platform is LITTLE_ENDIAN. 
    // Buffer transmits of integers will be little endian.
    spi_write_blocking(eve_spi_port, &DataToWrite, 1u);
}

uint16_t MCU_SPIRead16(void)
{
    uint16_t DataRead = 0;

    // Note: This platform is LITTLE_ENDIAN. 
    // Buffer receives of integers will be little endian.
    spi_read_blocking(eve_spi_port, 0u, (uint8_t *)&DataRead, 2u);

    return DataRead;
}

void MCU_SPIWrite16(uint16_t DataToWrite)
{
    // Note: This platform is LITTLE_ENDIAN. 
    // Buffer transmits of integers will be little endian.
    spi_write_blocking(eve_spi_port, (uint8_t *)&DataToWrite, 2u);
}

void MCU_SPIWrite24(uint32_t DataToWrite)
{
    // Note: This platform is LITTLE_ENDIAN. 
    // Buffer transmits of integers will be little endian.
    spi_write_blocking(eve_spi_port, (uint8_t *)&DataToWrite, 3u);
}

uint32_t MCU_SPIRead32(void)
{
    uint32_t DataRead = 0u;

    // Note: This platform is LITTLE_ENDIAN. 
    // Buffer receives of integers will be little endian.
    spi_read_blocking(eve_spi_port, 0u, (uint8_t *)&DataRead, 4u);

    return DataRead;
}

void MCU_SPIWrite32(uint32_t DataToWrite)
{
    // Note: This platform is LITTLE_ENDIAN. 
    // Buffer transmits of integers will be little endian.
    spi_write_blocking(eve_spi_port, (uint8_t *)&DataToWrite, 4u);
}

void MCU_SPIRead(uint8_t *DataToRead, uint32_t length)
{
    spi_read_blocking(eve_spi_port, 0, DataToRead, length);
}

void MCU_SPIWrite(const uint8_t *DataToWrite, uint32_t length)
{
    spi_write_blocking(eve_spi_port, DataToWrite, length);
}

void MCU_Delay_20ms(void)
{
    sleep_ms(20u);
}

void MCU_Delay_500ms(void)
{
    sleep_ms(500u);
}

uint32_t MCU_Time_ms(void)
{
    return to_ms_since_boot(get_absolute_time());
}

// RP2040 is Little Endian. There is no sys/endian.h.
// Use toolchain defined functions.

uint16_t MCU_htobe16(uint16_t h)
{
    return __bswap16(h);
}

uint32_t MCU_htobe32(uint32_t h)
{
    return __bswap32(h);
}

uint16_t MCU_htole16(uint16_t h)
{
    return h;
}

uint32_t MCU_htole32(uint32_t h)
{
    return h;
}

uint16_t MCU_be16toh(uint16_t h)
{
    return __bswap16(h);
}

uint32_t MCU_be32toh(uint32_t h)
{
    return __bswap32(h);
}

uint16_t MCU_le16toh(uint16_t h)
{
    return h;
}

uint32_t MCU_le32toh(uint32_t h)
{
    return h;
}

/* EVE MCU END */

#endif /* defined(PLATFORM_RP2040) || defined(PLATFORM_PICO_SDK) */
