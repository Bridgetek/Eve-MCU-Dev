/**
 * @file EVE_MCU_RP2040.c
 * @details MCU-specific code for controlling EVE on RP2040 devices.
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
#if defined(PLATFORM_RP2040)

#pragma message "Compiling " __FILE__ " for Raspberry Pi pico RP2040"

/* EVE MCU END */

#include <string.h>
#include <stdint.h> // for Uint8/16/32 and Int8/16/32 data types
#include <stdio.h>
#include <machine/endian.h>

/* Include functions for EVE-MCU-Dev library API layer */
#include <EVE.h> 
/* Include functions for EVE-MCU-Dev library MCU layer */
#include <MCU.h>

#include <hardware/gpio.h>
#include <hardware/spi.h>
#include <pico/time.h>

// Pins to match Bridgetek IDM2040-7A board.
const uint pd_pin = 7;
const uint cs_pin = 5;
const uint sck_pin = 2;
const uint mosi_pin = 3;
const uint miso_pin = 4;
const uint int_pin = 6;
// Port to match Bridgetek IDM2040-7A board.
spi_inst_t *spi_port = spi0;

// IDM2040-21R specific functions for ST7701S configuration 
#if defined(EVE_IDM204021R)
static void lcd_send_data(uint8_t data, bool cmd);
static void lcd_spi_write(spi_inst_t *spi, uint8_t *data, uint32_t length);
static void ST7701S_init(void);
#endif

/* EVE MCU HEADER END */

/* EVE MCU */

// This is the RP240 Platform specific section and contains the functions which
// enable the GPIO and SPI interfaces.

// ------------------- MCU specific initialisation  ----------------------------
int MCU_Init(void)
{

	// The IDM2040-21R uses a ST7701S LCD Driver which must be configured before initialising EVE
#if defined(EVE_IDM204021R)
    ST7701S_init();
#endif /* defined(EVE_IDM204021R) */

    // Initialise CS (SPI Chip Select) pin high
    gpio_init(cs_pin);
    gpio_set_dir(cs_pin, GPIO_OUT);
    gpio_put(cs_pin, 1);

    // Initialise PD (Power Down) pin high
    gpio_init(pd_pin);
    gpio_set_dir(pd_pin, GPIO_OUT);
    gpio_put(pd_pin, 1);

    // Initialise INT# (Interrupt) pin input
    gpio_init(int_pin);
    gpio_set_dir(int_pin, GPIO_IN);

    // Set SPI clock speed to 1 MHz
    // 1 MHz allows all EVE devices to initialise correctly
    // After initialisation the SPI speed can be increased in the MCU_Setup()
    spi_init(spi_port, 1 * 1000 * 1000);

    // Set SPI format
    spi_set_format( spi_port, 		// SPI instance
                    8,      		// Number of bits per transfer
                    SPI_CPOL_0,		// Polarity (CPOL)
                    SPI_CPHA_0,		// Phase (CPHA)
                    SPI_MSB_FIRST);

    // Initialize SPI pin functions
    gpio_set_function(sck_pin, GPIO_FUNC_SPI);
    gpio_set_function(mosi_pin, GPIO_FUNC_SPI);
    gpio_set_function(miso_pin, GPIO_FUNC_SPI);

    return 0;
}

int MCU_Deinit(void)
{
    /* Leave EVE in a safe state. */
    gpio_put(cs_pin, 1);
    gpio_put(pd_pin, 0);

    /* Disable SPI. */
    spi_deinit(spi_port);

    return 0;
}

int MCU_Setup(void)
{
    /* QSPI Configuration */
#if defined EVE_QSPI_ENABLE
#error EVE_QSPI_ENABLE (QSPI interfaces to EVE) is currently not supported RP2040
#if IS_EVE_API(2,3,4,5)
    /* Initialize IO2 and IO3 pad/pin for quad settings */
#endif
#endif // EVE_QSPI_ENABLE

    /* Additional SPI Configuration */
    // Increase SPI speed to 25 MHz after initialisation is complete
    // See the notes for MCU_SPI_TIMEOUT in the MCU.h file.
    spi_init(spi_port, 25 * 1000 * 1000);

    return 0;
}

// ########################### GPIO CONTROL ####################################

// --------------------- Chip Select line low ----------------------------------
inline void MCU_CSlow(void)
{
    gpio_put(cs_pin, 0);
}  

// --------------------- Chip Select line high ---------------------------------
inline void MCU_CShigh(void)
{
    gpio_put(cs_pin, 1);
}

// -------------------------- PD line low --------------------------------------
inline void MCU_PDlow(void)
{
    gpio_put(pd_pin, 0);
}

// ------------------------- PD line high --------------------------------------
inline void MCU_PDhigh(void)
{
    gpio_put(pd_pin, 1);
}

// ------------------------ interrupt input ------------------------------------
inline int MCU_Int(void) 
{
    bool val = gpio_get(int_pin);
    return (int)val;
}

// --------------------- SPI Send and Receive ----------------------------------

uint8_t MCU_SPIRead8(void)
{
    uint8_t DataRead = 0;

    // Note: This platform is LITTLE_ENDIAN. 
    // Buffer receives of integers will be little endian.
    spi_read_blocking(spi_port, 0, &DataRead, 1);

    return DataRead;
}

void MCU_SPIWrite8(uint8_t DataToWrite)
{
    // Note: This platform is LITTLE_ENDIAN. 
    // Buffer transmits of integers will be little endian.
    spi_write_blocking(spi_port, &DataToWrite, 1);
}

uint16_t MCU_SPIRead16(void)
{
    uint16_t DataRead = 0;

    // Note: This platform is LITTLE_ENDIAN. 
    // Buffer receives of integers will be little endian.
    spi_read_blocking(spi_port, 0, (uint8_t *)&DataRead, 2);

    return DataRead;
}

void MCU_SPIWrite16(uint16_t DataToWrite)
{
    // Note: This platform is LITTLE_ENDIAN. 
    // Buffer transmits of integers will be little endian.
    spi_write_blocking(spi_port, (uint8_t *)&DataToWrite, 2);
}

void MCU_SPIWrite24(uint32_t DataToWrite)
{
    // Note: This platform is LITTLE_ENDIAN. 
    // Buffer transmits of integers will be little endian.
    spi_write_blocking(spi_port, (uint8_t *)&DataToWrite, 3);
}

uint32_t MCU_SPIRead32(void)
{
    uint32_t DataRead = 0;

    // Note: This platform is LITTLE_ENDIAN. 
    // Buffer receives of integers will be little endian.
    spi_read_blocking(spi_port, 0, (uint8_t *)&DataRead, 4);

    return DataRead;
}

void MCU_SPIWrite32(uint32_t DataToWrite)
{
    // Note: This platform is LITTLE_ENDIAN. 
    // Buffer transmits of integers will be little endian.
    spi_write_blocking(spi_port, (uint8_t *)&DataToWrite, 4);
}

void MCU_SPIRead(uint8_t *DataToRead, uint32_t length)
{
    spi_read_blocking(spi_port, 0, DataToRead, length);
}

void MCU_SPIWrite(const uint8_t *DataToWrite, uint32_t length)
{
    spi_write_blocking(spi_port, DataToWrite, length);
}

void MCU_Delay_20ms(void)
{
    sleep_ms(20);
}

void MCU_Delay_500ms(void)
{
    sleep_ms(500);
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

/* IDM2040-21R specific functionality */
#if defined(EVE_IDM204021R)

// Pins to match Bridgetek IDM2040-21R board.
const uint lcd_cs_pin = 17;
const uint lcd_sck_pin = 10;
const uint lcd_mosi_pin = 11;
const uint lcd_reset_pin = 22;
// Port to match Bridgetek IDM2040-21R board.
spi_inst_t *spi_port_lcd = spi1;

// --------------- ST7701S LCD driver specific initialisation  --------------------
static void lcd_send_data(uint8_t data, bool cmd)
{
	uint16_t trans = 0;

	gpio_put(lcd_cs_pin, 0);
	sleep_ms(1);
	if (cmd)
		trans = data << 7;
	else
		trans = 0x8000 | data << 7;
	spi_write16_blocking(spi_port_lcd, &trans, 1);// 9 data bits 
	sleep_ms(1);
	gpio_put(lcd_cs_pin, 1);
}

static void lcd_spi_write(spi_inst_t* spi, uint8_t* data, uint32_t length)
{
	//send command
	lcd_send_data(data[0], true);

	if (length > 1)
	{
		for (int i = 1; i < length; i++)
			lcd_send_data(data[i], false);
	}
}

/**
 * @brief Function to configure ST7701S LCD driver on the IDM2040-21R module.
 */
static void ST7701S_init(void)
{
	/* ST7701S configuration data */
	uint8_t data1[6] = { 0xFF, 0x77, 0x01, 0x00, 0x00, 0x10 }; // Select Page 0
	uint8_t data2[3] = { 0xC0, 0x3B, 0x00 }; // display line setting
	uint8_t data3[3] = { 0xC1, 0x0B, 0x02 }; // porch control
	uint8_t data4[3] = { 0xC2, 0x07, 0x02 }; // inversion / frame rate control
	uint8_t data5[2] = { 0xCC, 0x10 };
	// Pos Voltage Gamma Control
	uint8_t data6[17] = { 0xB0, 0x00, 0x11, 0x16, 0x0E, 0x11, 0x06, 0x05, 0x09, 0x08, 0x21, 0x06, 0x13, 0x10, 0x29, 0x31, 0x18 };
	// Neg Voltage Gamma Control
	uint8_t data7[17] = { 0xB1, 0x00, 0x11, 0x16, 0x0E, 0x11, 0x07, 0x05, 0x09, 0x09, 0x21, 0x05, 0x13, 0x11, 0x2A, 0x31, 0x18 };
	uint8_t data8[6] = { 0xFF, 0x77, 0x01, 0x00, 0x00, 0x11 };
	uint8_t data9[2] = { 0xB0, 0x6D };  // Vop Amplitude
	uint8_t data10[2] = { 0xB1, 0x37 }; // VCOM amplitude setting
	uint8_t data11[2] = { 0xB2, 0x81 }; // VGH Voltage setting 
	uint8_t data12[2] = { 0xB3, 0x80 }; // TEST Command Setting
	uint8_t data13[2] = { 0xB5, 0x43 }; // VGL Voltage setting 
	uint8_t data14[2] = { 0xB7, 0x85 }; // Power Control 1
	uint8_t data15[2] = { 0xB8, 0x20 }; // Power Control 2
	uint8_t data16[2] = { 0xC1, 0x78 }; // Source pre_drive timing set1
	uint8_t data17[2] = { 0xC2, 0x78 }; // Source EQ2 Setting
	uint8_t data39[2] = { 0xCD, 0x08 }; // Colour Control
	uint8_t data38[2] = { 0xC3, 0x8C }; // RGB control
	uint8_t data18[2] = { 0xD0, 0x88 }; // MIPI setting 1
	uint8_t data19[4] = { 0xE0, 0x00, 0x00, 0x02 }; // Sunlight Readable Enhancement
	uint8_t data20[6] = { 0xE1, 0x03, 0xA0, 0x00, 0x00, 0x04 }; // Noise Reduce control
	// Sharpness control
	uint8_t data21[14] = { 0xE2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	// Colour calibration control
	uint8_t data22[5] = { 0xE3, 0x00, 0x00, 0x11, 0x00 };
	uint8_t data23[3] = { 0xE4, 0x22, 0x00 }; // Skin tone contorl
	uint8_t data24[17] = { 0xE5, 0x05, 0xEC, 0xA0, 0xA0, 0x07, 0xEE, 0xA0, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	uint8_t data25[5] = { 0xE6, 0x00, 0x00, 0x11, 0x00 };
	uint8_t data26[3] = { 0xE7, 0x22, 0x00 };
	uint8_t data27[17] = { 0xE8, 0x06, 0xED, 0xA0, 0xA0, 0x08, 0xEF, 0xA0, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	uint8_t data28[8] = { 0xEB, 0x00, 0x00, 0x40, 0x40, 0x00, 0x00, 0x00 };
	uint8_t data29[17] = { 0xED, 0xFF, 0xFF, 0xFF, 0xBA, 0x0A, 0xBF, 0x45, 0xFF, 0xFF, 0x54, 0xFB, 0xA0, 0xAB, 0xFF, 0xFF,0xFF};
	uint8_t data30[7] = { 0xEF, 0x10, 0x0D, 0x04, 0x08, 0x3F, 0x1F };
	// VAP & VAN
	uint8_t data31[6] = { 0xFF, 0x77, 0x01, 0x00, 0x00, 0x13 };
	uint8_t data32[2] = { 0xEF, 0x08 };
	uint8_t data33[6] = { 0xFF, 0x77, 0x01, 0x00, 0x00, 0x00 };
	uint8_t data34[2] = { 0x36, 0x00 };
	uint8_t data35[2] = { 0x3A, 0x66,  };
	uint8_t data36 = 0x11; // sleep out
	uint8_t data37 = 0x29; // Display on

	/* Intialise LCD SPI port */
	spi_init(spi_port_lcd, 1000 * 1000);
	gpio_set_function(lcd_sck_pin, GPIO_FUNC_SPI);
	gpio_set_function(lcd_mosi_pin, GPIO_FUNC_SPI);
	spi_set_format(spi_port_lcd, 16, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

	/* Chip select is active-low, so we'll initialise it to a driven-high state */
	gpio_init(lcd_cs_pin);
	gpio_set_dir(lcd_cs_pin, GPIO_OUT);
	gpio_put(lcd_cs_pin, 1);

	/* Reset LCD */
	gpio_init(lcd_reset_pin);
	gpio_set_dir(lcd_reset_pin, GPIO_OUT);
	gpio_put(lcd_reset_pin, 1);
	sleep_ms(20);
	gpio_put(lcd_reset_pin, 0);
	sleep_ms(20);
	gpio_put(lcd_reset_pin, 1);
	sleep_ms(20);

	/* Write ST7701S config data to LCD */
	lcd_spi_write(spi_port_lcd, data1, sizeof(data1));
	lcd_spi_write(spi_port_lcd, data2, sizeof(data2));
	lcd_spi_write(spi_port_lcd, data3, sizeof(data3));
	lcd_spi_write(spi_port_lcd, data4, sizeof(data4));
	lcd_spi_write(spi_port_lcd, data6, sizeof(data6));
	lcd_spi_write(spi_port_lcd, data7, sizeof(data7));
	lcd_spi_write(spi_port_lcd, data8, sizeof(data8));
	lcd_spi_write(spi_port_lcd, data9, sizeof(data9));
	lcd_spi_write(spi_port_lcd, data10, sizeof(data10));
	lcd_spi_write(spi_port_lcd, data11, sizeof(data11));
	lcd_spi_write(spi_port_lcd, data12, sizeof(data12));
	lcd_spi_write(spi_port_lcd, data13, sizeof(data13));
	lcd_spi_write(spi_port_lcd, data14, sizeof(data14));
	lcd_spi_write(spi_port_lcd, data15, sizeof(data15));
	lcd_spi_write(spi_port_lcd, data16, sizeof(data16));
	lcd_spi_write(spi_port_lcd, data17, sizeof(data17));
	lcd_spi_write(spi_port_lcd, data38, sizeof(data38));
	lcd_spi_write(spi_port_lcd, data39, sizeof(data39));
	lcd_spi_write(spi_port_lcd, data18, sizeof(data18));
	lcd_spi_write(spi_port_lcd, data19, sizeof(data19));
	lcd_spi_write(spi_port_lcd, data20, sizeof(data20));
	lcd_spi_write(spi_port_lcd, data21, sizeof(data21));
	lcd_spi_write(spi_port_lcd, data22, sizeof(data22));
	lcd_spi_write(spi_port_lcd, data23, sizeof(data23));
	lcd_spi_write(spi_port_lcd, data24, sizeof(data24));
	lcd_spi_write(spi_port_lcd, data25, sizeof(data25));
	lcd_spi_write(spi_port_lcd, data26, sizeof(data26));
	lcd_spi_write(spi_port_lcd, data27, sizeof(data27));
	lcd_spi_write(spi_port_lcd, data28, sizeof(data28));
	lcd_spi_write(spi_port_lcd, data29, sizeof(data29));
	lcd_spi_write(spi_port_lcd, data30, sizeof(data30));
	lcd_spi_write(spi_port_lcd, data31, sizeof(data31));
	lcd_spi_write(spi_port_lcd, data32, sizeof(data32));
	lcd_spi_write(spi_port_lcd, data33, sizeof(data33));
	lcd_spi_write(spi_port_lcd, data34, sizeof(data34));
	lcd_spi_write(spi_port_lcd, data35, sizeof(data35));
	sleep_ms(100);
	lcd_spi_write(spi_port_lcd, &data36, 1);
	sleep_ms(100);
	lcd_spi_write(spi_port_lcd, &data37, 1);

	/* Deintialise LCD SPI port */
	spi_deinit(spi_port_lcd);

}

#endif /* defined(EVE_IDM204021R) */

/* EVE MCU END */

#endif /* defined(PLATFORM_RP2040) */
