/**
 @file EVE_MCU_PSOC6.c
 @brief Infineon PSoC 6 ModusToolbox HAL port for Bridgetek EVE.
 */

#if defined(PLATFORM_INFINEON_PSOC6)

#include <stdint.h>
#include "cyhal.h"
#include "cybsp.h"
#include "EVE.h"
#include "MCU.h"
#include "eve_psoc6_config.h"

static cyhal_spi_t eve_spi;
static bool eve_spi_initialized;

static int eve_spi_set_frequency(uint32_t frequency)
{
    cy_rslt_t result = cyhal_spi_set_frequency(&eve_spi, frequency);
    return (result == CY_RSLT_SUCCESS) ? 0 : -1;
}

/* Millisecond counter */
volatile static uint32_t ms_ticks = 0;

/* Millisecond timer callback */
static void SysTick_Callback(void)
{
    ms_ticks++;
}

int MCU_Init(void)
{
    cy_rslt_t result;

    /* Initialise SysTick and register a 1 ms callback */
    Cy_SysTick_Init(CY_SYSTICK_CLOCK_SOURCE_CLK_CPU, SystemCoreClock / 1000);
    Cy_SysTick_SetCallback(0, SysTick_Callback);
    Cy_SysTick_Enable();

    result = cyhal_gpio_init(EVE_PSOC6_CS_PIN, CYHAL_GPIO_DIR_OUTPUT,
                            CYHAL_GPIO_DRIVE_STRONG, true);
    if (result != CY_RSLT_SUCCESS)
        return -1;

    result = cyhal_gpio_init(EVE_PSOC6_PD_PIN, CYHAL_GPIO_DIR_OUTPUT,
                            CYHAL_GPIO_DRIVE_STRONG, true);
    if (result != CY_RSLT_SUCCESS)
    {
        cyhal_gpio_free(EVE_PSOC6_CS_PIN);
        return -1;
    }

    result = cyhal_spi_init(&eve_spi,
                            EVE_PSOC6_MOSI_PIN,
                            EVE_PSOC6_MISO_PIN,
                            EVE_PSOC6_SCLK_PIN,
                            NC,
                            NULL,
                            8,
                            CYHAL_SPI_MODE_00_MSB,
                            false);
    if (result != CY_RSLT_SUCCESS)
    {
        cyhal_gpio_free(EVE_PSOC6_PD_PIN);
        cyhal_gpio_free(EVE_PSOC6_CS_PIN);
        return -1;
    }

    eve_spi_initialized = true;
    return eve_spi_set_frequency(EVE_PSOC6_SPI_INIT_HZ);
}

int MCU_Deinit(void)
{
    if (eve_spi_initialized)
    {
        cyhal_spi_free(&eve_spi);
        eve_spi_initialized = false;
    }
    cyhal_gpio_free(EVE_PSOC6_PD_PIN);
    cyhal_gpio_free(EVE_PSOC6_CS_PIN);
    return 0;
}

int MCU_Setup(void)
{
    return eve_spi_set_frequency(EVE_PSOC6_SPI_RUN_HZ);
}

void MCU_CSlow(void)
{
    cyhal_gpio_write(EVE_PSOC6_CS_PIN, false);
}

void MCU_CShigh(void)
{
    cyhal_gpio_write(EVE_PSOC6_CS_PIN, true);
}

void MCU_PDlow(void)
{
    cyhal_gpio_write(EVE_PSOC6_PD_PIN, false);
}

void MCU_PDhigh(void)
{
    cyhal_gpio_write(EVE_PSOC6_PD_PIN, true);
}

int MCU_Int(void)
{
    return cyhal_gpio_read(EVE_PSOC6_INT_PIN);
}

void MCU_Delay_20ms(void)
{
    cyhal_system_delay_ms(20U);
}

void MCU_Delay_500ms(void)
{
    cyhal_system_delay_ms(500U);
}

uint32_t MCU_Time_ms(void)
{
    return ms_ticks;
}

uint8_t MCU_SPIReadWrite8(uint8_t data_to_write)
{
    uint8_t data_read = 0U;
    (void)cyhal_spi_transfer(&eve_spi, &data_to_write, 1U,
                             &data_read, 1U, 0U);
    return data_read;
}

uint16_t MCU_SPIReadWrite16(uint16_t data_to_write)
{
    uint8_t tx[2] = {
        (uint8_t)(data_to_write & 0xffU),
        (uint8_t)((data_to_write >> 8) & 0xffU)
    };
    uint8_t rx[2] = { 0U, 0U };
    (void)cyhal_spi_transfer(&eve_spi, tx, sizeof(tx), rx, sizeof(rx), 0U);
    return (uint16_t)rx[0] | ((uint16_t)rx[1] << 8);
}

uint32_t MCU_SPIReadWrite24(uint32_t data_to_write)
{
    uint8_t tx[3] = {
        (uint8_t)(data_to_write & 0xffU),
        (uint8_t)((data_to_write >> 8) & 0xffU),
        (uint8_t)((data_to_write >> 16) & 0xffU)
    };
    uint8_t rx[3] = { 0U, 0U, 0U };
    (void)cyhal_spi_transfer(&eve_spi, tx, sizeof(tx), rx, sizeof(rx), 0U);
    return (uint32_t)rx[0] | ((uint32_t)rx[1] << 8) | ((uint32_t)rx[2] << 16);
}

uint32_t MCU_SPIReadWrite32(uint32_t data_to_write)
{
    uint8_t tx[4] = {
        (uint8_t)(data_to_write & 0xffU),
        (uint8_t)((data_to_write >> 8) & 0xffU),
        (uint8_t)((data_to_write >> 16) & 0xffU),
        (uint8_t)((data_to_write >> 24) & 0xffU)
    };
    uint8_t rx[4] = { 0U, 0U, 0U, 0U };
    (void)cyhal_spi_transfer(&eve_spi, tx, sizeof(tx), rx, sizeof(rx), 0U);
    return (uint32_t)rx[0] | ((uint32_t)rx[1] << 8) |
           ((uint32_t)rx[2] << 16) | ((uint32_t)rx[3] << 24);
}

uint8_t MCU_SPIRead8(void) { return MCU_SPIReadWrite8(0U); }
void MCU_SPIWrite8(uint8_t value) { (void)MCU_SPIReadWrite8(value); }
uint16_t MCU_SPIRead16(void) { return MCU_SPIReadWrite16(0U); }
void MCU_SPIWrite16(uint16_t value) { (void)MCU_SPIReadWrite16(value); }
void MCU_SPIWrite24(uint32_t value) { (void)MCU_SPIReadWrite24(value); }
uint32_t MCU_SPIRead32(void) { return MCU_SPIReadWrite32(0U); }
void MCU_SPIWrite32(uint32_t value) { (void)MCU_SPIReadWrite32(value); }

void MCU_SPIWrite(const uint8_t *data_to_write, uint32_t length)
{
    if ((data_to_write != NULL) && (length != 0U))
        (void)cyhal_spi_transfer(&eve_spi, data_to_write, length, NULL, 0U, 0U);
}

void MCU_SPIRead(uint8_t *data_to_read, uint32_t length)
{
    if ((data_to_read != NULL) && (length != 0U))
        (void)cyhal_spi_transfer(&eve_spi, NULL, 0U, data_to_read, length, 0U);
}

uint16_t MCU_htobe16(uint16_t value) { return __builtin_bswap16(value); }
uint32_t MCU_htobe32(uint32_t value) { return __builtin_bswap32(value); }
uint16_t MCU_htole16(uint16_t value) { return value; }
uint32_t MCU_htole32(uint32_t value) { return value; }
uint16_t MCU_be16toh(uint16_t value) { return __builtin_bswap16(value); }
uint32_t MCU_be32toh(uint32_t value) { return __builtin_bswap32(value); }
uint16_t MCU_le16toh(uint16_t value) { return value; }
uint32_t MCU_le32toh(uint32_t value) { return value; }

#endif /* PLATFORM_INFINEON_PSOC6 */
