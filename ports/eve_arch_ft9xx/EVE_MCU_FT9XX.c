/**
 * @file EVE_MCU_FT9XX.c
 * @details MCU-specific code for controlling EVE on FT9xx devices.
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
#if defined(PLATFORM_FT9XX)

#pragma message "Compiling " __FILE__ " for BridgeTek FT9XX"

/* EVE MCU HEADER */

#include <string.h>
#include <stdint.h> // for Uint8/16/32 and Int8/16/32 data types
#include <machine/endian.h>

#include <ft900.h>
#include <ft900_spi.h>
#include <ft900_gpio.h>

/* Include functions for EVE-MCU-Dev library API layer */
#include <EVE.h> 
/* Include functions for EVE-MCU-Dev library MCU layer */
#include <MCU.h>

/* The Timer Prescaler will divide the 100MHz Master clock down to 2kHz */
#define TIMER_PRESCALER (50000)
#define TIMER_ONE_MILLISECOND (100000/TIMER_PRESCALER)
#define TIMER_ONE_SECOND (1000*TIMER_ONE_MILLISECOND)

// SPI Master pins
#if defined(__FT900__)

#define PIN_NUM_MISO 30
#define PIN_NUM_MOSI 29
#define PIN_NUM_IO2  31
#define PIN_NUM_IO3  32
#define PIN_NUM_CLK  27
#define PIN_NUM_CS   28
// EVE Powerdown# pin
#define PIN_NUM_PD   43
// EVE INT# pin
#define PIN_NUM_INT  42

#elif defined(__FT930__)

#define PIN_NUM_MISO 35
#define PIN_NUM_MOSI 36
#define PIN_NUM_IO2  37
#define PIN_NUM_IO3  38
#define PIN_NUM_CLK  34
#define PIN_NUM_CS   30
// EVE Powerdown# pin
#define PIN_NUM_PD   15
// EVE INT# pin
#define PIN_NUM_INT  14

#endif //

/* EVE MCU HEADER END */

// FT9xx tick timer
static void timerISR(void);
static volatile uint32_t ticks = 0;

/* EVE MCU */

// This is the FT9xx platform specific section and contains the functions which
// enable the Tiemr, GPIO and SPI interfaces.

// ------------------- MCU specific initialisation  ----------------------------
int MCU_Init(void)
{
    /* Initialize SPIM HW */
    sys_enable(sys_device_spi_master);

    /* Set CLK pin */
    gpio_function(PIN_NUM_CLK, pad_spim_sck); /* GPIO27 to SPIM_CLK */

    /* Set CS#, PD#, INT# pins */
#if defined(__FT900__)
    gpio_function(PIN_NUM_CS, pad_spim_ss0); /* GPIO28 as CS */
    gpio_function(PIN_NUM_PD, pad_gpio43); /* GPIO43 as PD */
    gpio_function(PIN_NUM_INT, pad_gpio42); /* GPIO42 as INT */
#elif defined(__FT930__)
    gpio_function(PIN_NUM_CS, pad_spim_ss0); /* GPIO30 as CS */
    gpio_function(PIN_NUM_PD, pad_gpio15); /* GPIO15 as PD */
    gpio_function(PIN_NUM_INT, pad_gpio14); /* GPIO14 as INT */
#endif

    /* Set MISO/MOSI pins */
    gpio_function(PIN_NUM_MOSI, pad_spim_mosi); /* GPIO29 to SPIM_MOSI */
    gpio_function(PIN_NUM_MISO, pad_spim_miso); /* GPIO30 to SPIM_MISO */

    /* Set pin directions  */
    gpio_dir(PIN_NUM_CLK, pad_dir_output);
    gpio_dir(PIN_NUM_CS, pad_dir_output);
    gpio_dir(PIN_NUM_MOSI, pad_dir_output);
    gpio_dir(PIN_NUM_MISO, pad_dir_input);
    gpio_dir(PIN_NUM_PD, pad_dir_output);
    gpio_dir(PIN_NUM_INT, pad_dir_input);

#if defined EVE_QSPI_ENABLE
    /* Initialize IO2 and IO3 pad/pin for quad settings */
    gpio_function(PIN_NUM_IO2, pad_spim_io2); /* GPIO31 to IO2 */
    gpio_function(PIN_NUM_IO3, pad_spim_io3); /* GPIO32 to IO3 */
    gpio_dir(PIN_NUM_IO2, pad_dir_output);
    gpio_dir(PIN_NUM_IO3, pad_dir_output);
#endif // EVE_QSPI_ENABLE

    /* CS# & PD# pins write to high */
    gpio_write(PIN_NUM_CS, 1);
    gpio_write(PIN_NUM_PD, 1);

    /* Start SPIM interface */
    // Set SPI clock speed to 12.5 MHz - See the notes for MCU_SPI_TIMEOUT in the MCU.h file.
    // Divide by 8 is 12.5 MHz
    spi_init(SPIM, spi_dir_master, spi_mode_0, 8);
    spi_option(SPIM,spi_option_fifo_size,64);
	spi_option(SPIM,spi_option_fifo,1);
	spi_option(SPIM,spi_option_fifo_receive_trigger,1);

    /* Enable Timers... */
    sys_enable(sys_device_timer_wdt);
    /* Set up the Timer tick to be 1 ms... */
    /* FT900 Rev A and B have timers that share the prescaler */
    /* FT900 Rev C and FT93x timers have dedicated prescalers */
#if defined(__FT900__)
    if (sys_check_ft900_revB()) //90x series rev B
    {
        timer_prescaler(TIMER_PRESCALER);
    }
    else
#endif
    {
        timer_prescaler(timer_select_a, TIMER_PRESCALER);
    }
    /* Set up Timer A to be triggered after 5 seconds... */
    timer_init(timer_select_a,              /* Device */
               TIMER_ONE_MILLISECOND,       /* Initial Value */
               timer_direction_down,        /* Count Direction */
               timer_prescaler_select_on,   /* Prescaler Select */
               timer_mode_continuous);      /* Timer Mode */

    /* Register the interrupt... */
    interrupt_attach(interrupt_timers, 17, timerISR);

    /* Enable all the timers... */
    timer_enable_interrupt(timer_select_a);

    /* Enable interrupts to fire... */
    interrupt_enable_globally();

    /* Start all the timers at the same time... */
    timer_start(timer_select_a);

    return 0;
}

int MCU_Deinit(void)
{
    /* Leave EVE in a safe state. */
    MCU_CShigh();
    MCU_PDlow();

    /* Shut down the SPI master. */
    spi_uninit(SPIM);
    sys_disable(sys_device_spi_master);
    
    return 0;
}

int MCU_Setup(void)
{
    /* QSPI configuration */
#if defined EVE_QSPI_ENABLE

#if IS_EVE_API(2,3,4)
    // Turn on EVE quad-SPI for FT81x/BT81x devices.
    MCU_CSlow();
    MCU_SPIWrite24(MCU_htobe32((EVE_REG_SPI_WIDTH << 8) | (1 << 31)));
    MCU_SPIWrite8(2);
    MCU_CShigh();

    // Turn on FT9xx quad-SPI.
    spi_option(SPIM, spi_option_bus_width, 4);
#elif IS_EVE_API(5)
    // Turn on EVE quad-SPI for BT82x devices.
    MCU_CSlow();
    MCU_SPIWrite32(MCU_htobe32((EVE_REG_SPI_WIDTH << 8) | (1 << 31)));
    MCU_SPIWrite32(2);
    MCU_CShigh();

    // Turn on FT9xx quad-SPI.
    spi_option(SPIM, spi_option_bus_width, 4);
#endif // IS_EVE_API(2,3,4,5)
#endif // EVE_QSPI_ENABLE

    /* Additional SPI Configuration */
    // Turn off SPI buffering. Timing of chip select is critical.
    spi_option(SPIM, spi_option_fifo, 0);

    return 0;
}

/**
 * @brief The interrupt handler for the timers.
 *
 * This will keep a count of how many times each interrupt has fired in a global
 * variable.
*/
static void timerISR(void)
{
    if (timer_is_interrupted(timer_select_a) == 1)
    {
        ticks++;
    }
}

// ########################### GPIO CONTROL ####################################

// --------------------- Chip Select line low ----------------------------------
inline void MCU_CSlow(void)
{
    spi_open(SPIM, 0);
    // Tsac is 10 ns (EVE1), 3 ns (EVE2, EVE3, EVE4, EVE5)
    delayus(1);
}  

// --------------------- Chip Select line high ---------------------------------
inline void MCU_CShigh(void)
{
  // Tcsnh is 10 ns (EVE1), 0 ns (EVE2, EVE3, EVE4, EVE5)
  // This 1 us delay can be removed in most cases
#if IS_EVE_API(1)
    delayus(1);
#endif
    spi_close(SPIM, 0);
}

// -------------------------- PD line low --------------------------------------
inline void MCU_PDlow(void)
{
    gpio_write(PIN_NUM_PD, 0);
}

// ------------------------- PD line high --------------------------------------
inline void MCU_PDhigh(void)
{
    gpio_write(PIN_NUM_PD, 1);
}

// ------------------------ interrupt input ------------------------------------
int MCU_Int(void)
{
    return gpio_read(PIN_NUM_INT);  // EVE INT# line
}

// --------------------- SPI Send and Receive ----------------------------------

uint8_t MCU_SPIRead8(void)
{
    uint8_t DataRead = 0;

    spi_readn(SPIM, &DataRead, 1);

    return DataRead;
}

void MCU_SPIWrite8(uint8_t DataToWrite)
{
    spi_writen(SPIM, &DataToWrite, 1);
}

uint16_t MCU_SPIRead16(void)
{
    uint16_t DataRead = 0;

    spi_readn(SPIM, (uint8_t *)&DataRead, 2);

    return DataRead;
}

void MCU_SPIWrite16(uint16_t DataToWrite)
{
    spi_writen(SPIM, (uint8_t *)&DataToWrite, 2);
}

void MCU_SPIWrite24(uint32_t DataToWrite)
{
    spi_writen(SPIM, (uint8_t *)&DataToWrite, 3);
}

uint32_t MCU_SPIRead32(void)
{
    uint32_t DataRead = 0;

    spi_readn(SPIM, (uint8_t *)&DataRead, 4);

    return DataRead;
}

void MCU_SPIWrite32(uint32_t DataToWrite)
{
    spi_writen(SPIM, (uint8_t *)&DataToWrite, 4);
}

void MCU_SPIWrite(const uint8_t *DataToWrite, uint32_t length)
{
    spi_writen(SPIM, DataToWrite, length);
}

void MCU_SPIRead(uint8_t *DataToRead, uint32_t length)
{
    spi_readn(SPIM, DataToRead, length);
}

void MCU_Delay_20ms(void)
{
    delayms(20);
}

void MCU_Delay_500ms(void)
{
    delayms(500);
}

uint32_t MCU_Time_ms(void)
{
    return ticks;
}

// FT9XX is Little Endian.
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

#endif /* defined(PLATFORM_FT9XX) */
