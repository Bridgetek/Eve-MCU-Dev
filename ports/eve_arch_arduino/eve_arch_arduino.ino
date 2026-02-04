/**
 @file eve_arch_arduino.ino
 */
/*
 * ============================================================================
 * (C) Copyright Bridgetek Pte Ltd
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

// Guard against being used for incorrect CPU type.
#if defined(ARDUINO)

#include <Arduino.h>
#include <SPI.h>

/** @brief Library Includes
 * NOTE That all the file used in the example sketch must be in the same directory
 * as the sketch. Copy this file, the include files from the include directory,
 * the source files from the source directory of the library to the sketch 
 * location.
 * It will be possible to make a library with these files.
 * Include these files as "C" files.
 */
//@{
extern "C" {
#include <EVE.h>
#include <MCU.h>
}
//@}

/** @brief Pin definitions
 */
//@{
/// Standard SPI pinouts 10(CS), 11(COPI), 12(CIPO), 13(SCK)
#define PIN_SPICLOCK 13    // SCK
#define PIN_DATAOUT 11     // MOSI (COPI)
#define PIN_DATAIN 12      // MISO (CIPO)
#define PIN_CHIPSELECT 10  // CS#
/// Additional pin for power down on EVE
#define PIN_POWERDOWN 9    // PD#
//@}

void MCU_Init(void) {
  uint8_t clr;

  SPI.begin();

  pinMode(PIN_CHIPSELECT, OUTPUT);
  pinMode(PIN_POWERDOWN, OUTPUT);

  digitalWrite(PIN_CHIPSELECT, HIGH);  //disable CS#
  digitalWrite(PIN_POWERDOWN, HIGH);   //disable HD#

  // Set SPI speed to 1 MHz 
  // 1 MHz allows all EVE devices to initialise correctly
  // After initialisation the SPI speed can be increased in the MCU_Setup()
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
}

void MCU_Deinit(void) {
  SPI.endTransaction();
  SPI.end();
}

void MCU_Setup(void) {
  SPI.endTransaction();
  
  // Increase SPI speed to 8 MHz after initialisation is complete
  // See the notes for MCU_SPI_TIMEOUT in the MCU.h file.
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
}

// Simple endian alignment for tested Arduino devices
#define bswap16(x) __builtin_bswap16(x)
#define bswap32(x) __builtin_bswap32(x)

// ########################### GPIO CONTROL ####################################

// --------------------- Chip Select line low ----------------------------------
void MCU_CSlow(void) {
  digitalWrite(PIN_CHIPSELECT, LOW);  // disable CS#
  delayMicroseconds(10);
}

// --------------------- Chip Select line high ---------------------------------
void MCU_CShigh(void) {
  digitalWrite(PIN_CHIPSELECT, HIGH);  // disable CS#
  delayMicroseconds(10);
}

// -------------------------- PD line low --------------------------------------
void MCU_PDlow(void) {
  digitalWrite(PIN_POWERDOWN, LOW);  // enable HD#
}

// ------------------------- PD line high --------------------------------------
void MCU_PDhigh(void) {
  digitalWrite(PIN_POWERDOWN, HIGH);  // disable HD#
}

// Exchange a single byte on the SPI bus
char MCU_SPIReadWrite8(uint8_t val) {
  uint8_t v = SPI.transfer(val);
  return v;
}

uint16_t MCU_SPIReadWrite16(uint16_t DataToWrite) {
  uint16_t DataRead = 0;
  DataRead = MCU_SPIReadWrite8(DataToWrite & 0xff);
  DataRead |= MCU_SPIReadWrite8((DataToWrite >> 8) & 0xff) << 8;

  return DataRead;
}

uint32_t MCU_SPIReadWrite24(uint32_t DataToWrite) {
  uint32_t DataRead = 0;
  uint32_t temp;

  temp = MCU_SPIReadWrite8((DataToWrite >> 8) & 0xff);
  DataRead |= (temp << 8);
  temp = MCU_SPIReadWrite8((DataToWrite >> 16) & 0xff);
  DataRead |= (temp << 16);
  temp = MCU_SPIReadWrite8((DataToWrite >> 24) & 0xff);
  DataRead |= (temp << 24);

  return DataRead;
}

uint32_t MCU_SPIReadWrite32(uint32_t DataToWrite) {
  uint32_t DataRead = 0;
  uint32_t temp;

  temp = MCU_SPIReadWrite8((DataToWrite >> 0) & 0xff);
  DataRead |= (temp << 0);
  temp = MCU_SPIReadWrite8((DataToWrite >> 8) & 0xff);
  DataRead |= (temp << 8);
  temp = MCU_SPIReadWrite8((DataToWrite >> 16) & 0xff);
  DataRead |= (temp << 16);
  temp |= MCU_SPIReadWrite8((DataToWrite >> 24) & 0xff);
  DataRead |= (temp << 24);

  return DataRead;
}

void MCU_Delay_20ms(void) {
  delay(20);
}

void MCU_Delay_500ms(void) {
  delay(500);
}

// --------------------- SPI Send and Receive ----------------------------------

uint8_t MCU_SPIRead8(void) {
  uint8_t DataRead = 0;

  DataRead = MCU_SPIReadWrite8(0);

  return DataRead;
}

void MCU_SPIWrite8(uint8_t DataToWrite) {
  MCU_SPIReadWrite8(DataToWrite);
}

uint16_t MCU_SPIRead16(void) {
  uint16_t DataRead = 0;

  DataRead = MCU_SPIReadWrite16(0);

  return DataRead;
}

void MCU_SPIWrite16(uint16_t DataToWrite) {
  MCU_SPIReadWrite16(DataToWrite);
}

uint32_t MCU_SPIRead24(void) {
  uint32_t DataRead = 0;

  DataRead = MCU_SPIReadWrite24(0);

  return DataRead;
}

void MCU_SPIWrite24(uint32_t DataToWrite) {
  MCU_SPIReadWrite24(DataToWrite);
}

uint32_t MCU_SPIRead32(void) {
  uint32_t DataRead = 0;

  DataRead = MCU_SPIReadWrite32(0);

  return DataRead;
}

void MCU_SPIWrite32(uint32_t DataToWrite) {
  MCU_SPIReadWrite32(DataToWrite);
}

void MCU_SPIWrite(const uint8_t *DataToWrite, uint32_t length) {
  //TODO: replace with SPI.transfer(DataToWrite, length);
  // Note that DataToWrite is overwritten.
  uint16_t DataPointer = 0;

  while (DataPointer < length) {
    MCU_SPIWrite8(DataToWrite[DataPointer]);  // Send data byte-by-byte from array
    DataPointer++;
  }
}

void MCU_SPIRead(uint8_t *DataToRead, uint32_t length) {
  //TODO: replace with SPI.transfer(DataToRead, length);
  uint16_t DataPointer = 0;

  while (DataPointer < length) {
    DataToRead[DataPointer] = MCU_SPIRead8();  // Receive data byte-by-byte to array
    DataPointer++;
  }
}

// Arduino is Little Endian. 
// Use toolchain defined functions.
uint16_t MCU_htobe16(uint16_t h) {
  return bswap16(h);
}

uint32_t MCU_htobe32(uint32_t h) {
  return bswap32(h);
}

uint16_t MCU_htole16(uint16_t h) {
  return h;
}

uint32_t MCU_htole32(uint32_t h) {
  return h;
}

uint16_t MCU_be16toh(uint16_t h) {
  return bswap16(h);
}

uint32_t MCU_be32toh(uint32_t h) {
  return bswap32(h);
}

uint16_t MCU_le16toh(uint16_t h) {
  return h;
}

uint32_t MCU_le32toh(uint32_t h) {
  return h;
}

#endif /* defined(ARDUINO) */
