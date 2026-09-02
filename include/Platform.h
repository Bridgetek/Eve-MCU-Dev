/**
 * @file platform.h
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

#ifndef _EVE_PLATFORM_HEADER_H
#define _EVE_PLATFORM_HEADER_H

#ifndef __linux__
#error This file is for Linux-based systems. It is not intended for use on Microcontrollers.
#endif

#include <stdint.h> // for Uint8/16/32 and Int8/16/32 data types

/**
 *  @brief Platform specific debugging output
 *  @details Set debugging preferences for Linux HALs which use this header files.
 *       This is different from MCU devices in that use the MCU.h header file.
 */
//@{
#undef EVE_HAL_DEBUG
#define EVE_HAL_INFO
#define EVE_HAL_ERROR
//@}

/**
 * @brief Platform allows large SPI transfers.
 * @details Set to maximum size of SPI transfers allowed, e.g. on
 *      microcontrollers. This has been added to support enhanced SPI
 *      access on BT82x.
 *      Do not make this larger than required as on BT82x (EVE API 5) 
 *      there will be a stack buffer allocated in HAL_Read of this 
 *      size plus a small number of bytes for protocol.
 *      A larger buffer is only needed if fast reads of large blocks 
 *      of data are required from the device. Normal operation can be
 *      acheived with 32-bit reads with good performance.
 */
#if IS_EVE_API(5)
#if defined(PLATFORM_RASPBERRYPI) 
#define MCU_SPI_TRANSFER sizeof(uint32_t)
#elif defined(PLATFORM_BEAGLEBONE) 
#define MCU_SPI_TRANSFER sizeof(uint32_t)
#endif
#endif

/**
 * @brief Platform SPI bus speed.
 * @details In general, a port is responsible for ensuring timeout accuracy on the SPI bus.
 *      Timeout for a read is a maximum of 7uS for BT82x.
 *      The timeout value here must be adjusted for the host system SPI clock speed.
 *      The SPI clock speed is set in the Platform_Init(void) function for a port.
 *      Values here match the default values set in the ports.
 *      At 1 MHz SPI bus the timeout is approximately 8 clock cycles (1 byte).
 *      At 20 MHz SPI bus the timeout is approximately 140 clock cycles (17.5 bytes).
 *      At 25 MHz SPI bus the timeout is approximately 175 clock cycles (24 bytes).
 *      At 60 MHz SPI bus the timeout is approximately 420 clock cycles (52 bytes).
 *      The minimum timeout allowed is 8 bytes.
 */
#if IS_EVE_API(5)
#if defined(PLATFORM_RASPBERRYPI) 
/* Raspberry Pi SPI bus is set to 1 MHz by default */
#define MCU_SPI_TIMEOUT 8
#elif defined(PLATFORM_BEAGLEBONE) 
/* The default SPI on Beaglebone to 1 MHz */
#define MCU_SPI_TIMEOUT 8
#else
/* Linux systems SPI busses are set to 1 MHz by default */
#define MCU_SPI_TIMEOUT 8
#endif
#endif

/**
 * @brief Platform specific initialisation
 * @details Must contain any platform-specific initialisation. This will typically be
 *    setting up the SPI bus, GPIOs and operating environment requirements.
 * @returns 0 if successful, -1 if failed.
 */
int Platform_Init(void);

/**
 *  @brief Platform specific de-initialisation
 *  @details Must contain any platform-specific de-initialisation. This will typically be
 *       closing the SPI bus, GPIOs and operating environment requirements.
 *  @returns 0 if successful, -1 if failed.
 */
int Platform_Deinit(void);

/**
 @brief Platform specific setup
 @details Called after the EVE has been power cycled and started. Contains
    any platform-specific configuration options for the EVE.
 @returns 0 if successful, -1 if failed.
 */
int Platform_Setup(void);

/**
 @brief Platform specific SPI transfer
 @details Called to perform a transmit/receive operation on the platform's
    SPI bus. This can be for both transmission or receiving or a combination
    of both. Multiple transfers can be grouped with the spi_ioc_transfer
    structure.
 */
int Platform_SPI_transfer(struct spi_ioc_transfer *xfer, int count);

/**
 @brief Platform specific chip select enable
 @details This function will pull the chip select line to the EVE low to
    allow data transmission on the SPI bus.
    The EVE requires chip select to toggle frequently.
 */
void Platform_CSlow(void);

/**
 @brief Platform specific chip select deassert
 @details This function will pull the chip select line to the EVE high to
    prevent data transmission on the SPI bus.
    The EVE requires chip select to toggle frequently.
 */
void Platform_CShigh(void);

/**
 @brief Platform specific power down enable
 @details This function will pull the power down line to the EVE low to
    force the device into power down mode.
    This will be done during EVE initialisation and can be done to allow
    deep power saving.
 */
void Platform_PDlow(void);

/**
 @brief Platform specific power down disable
 @details This function will pull the power down line to the EVE high to
    enable normal operation of the EVE.
    This will be done during EVE initialisation and can be done to allow
    recovery from deep power saving.
 */
void Platform_PDhigh(void);

/**
 * @brief Platform specific interrupt input
 * @details This function will check the interrupt input GPIO for an
 *      assertion of the interrupt line from the EVE device.
 *      The mechanism for detecting the input signal is platform
 *      specific.
 * @returns zero if there is no interrupt, non-zero if the EVE device is
 *      asserting an interrupt.
 */
int Platform_Int(void);

/**
 @brief Platform specific SPI write
 @details Performs an SPI write of the data block and discards the data
    received in response.
 @param DataToWrite - pointer to buffer to write.
 @param length - number of bytes to write.
 */
void Platform_SPIWrite(const uint8_t *DataToWrite, uint32_t length);

/**
 @brief Platform specific SPI 8 bit read
 @details Performs an SPI dummy write and returns the data received in
    response.
 @returns Data received from EVE.
 */
uint8_t Platform_SPIRead8(void);

/**
 @brief Platform specific SPI 8 bit write
 @details Performs an SPI write and discards the data received in
    response.
 @param Data to write to EVE.
 */
void Platform_SPIWrite8(uint8_t DataToWrite);

/**
 @brief Platform specific SPI 16 bit read
 @details Performs an SPI dummy write and returns the data received in
    response.
 @returns Data received from EVE.
 */
uint16_t Platform_SPIRead16(void);

/**
 @brief Platform specific SPI 16 bit write
 @details Performs an SPI write and discards the data received in
    response.
 @param Data to write to EVE.
 */
void Platform_SPIWrite16(uint16_t DataToWrite);

/**
 @brief Platform specific SPI 24 bit read
 @details Performs an SPI dummy write and returns the data received in
    response.
 @returns Data received from EVE.
 */
uint32_t Platform_SPIRead24(void);

/**
 @brief Platform specific SPI 24 bit write
 @details Performs an SPI write and discards the data received in
    response.
 @param Data to write to EVE.
 */
void Platform_SPIWrite24(uint32_t DataToWrite);

/**
 @brief Platform specific SPI 32 bit read
 @details Performs an SPI dummy write and returns the data received in
    response.
 @returns Data received from EVE.
 */
uint32_t Platform_SPIRead32(void);

/**
 @brief Platform specific SPI 32 bit write
 @details Performs an SPI write and discards the data received in
    response.
 @param Data to write to EVE.
 */
void Platform_SPIWrite32(uint32_t DataToWrite);

/**
 @brief Platform specific 20 ms delay
 @details Cause the platform to idle or otherwise delay for a minimum of
    20 milliseconds. This is used during initialisation to perform a
    power down of the EVE for a controlled minimum period of time.
 */
void Platform_Delay_20ms(void);

/**
 @brief Platform specific 500 ms delay
 @details Cause the platform to idle or otherwise delay for a minimum of
    500 milliseconds. This is used during initialisation to perform a
    power down of the EVE for a controlled minimum period of time.
 */
void Platform_Delay_500ms(void);

/**
 * @brief Platform specific ms clock counter
 * @details Get the current monotonic clock counter from the platform for the
 *      purpose of making a useful timeout.
 */
uint32_t Platform_Time_ms(void);

/**
 @brief Platform specific byte swapping routines
 @details EVE addresses from the HAL_SetReadAddress and HAL_SetWriteAddress
    are sent in big-endian format. However, data for registers or memory
    mapped areas are in little-endian format.
 */
//@{
uint16_t Platform_htobe16(uint16_t h);
uint32_t Platform_htobe32(uint32_t h);
uint16_t Platform_htole16(uint16_t h);
uint32_t Platform_htole32(uint32_t h);
uint16_t Platform_be16toh(uint16_t h);
uint32_t Platform_be32toh(uint32_t h);
uint16_t Platform_le16toh(uint16_t h);
uint32_t Platform_le32toh(uint32_t h);
//@}

#endif /* _EVE_PLATFORM_HEADER_H */
