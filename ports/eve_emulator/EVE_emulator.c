/**
 @file EVE_emulator.c
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

// Guard against being used for incorrect platform or architecture.
// USE_EMULATOR macro enables this file to open the emulator library.
// In gcc compilers this is in the Makefile. -DUSE_EMULATOR=0
// In VisualStudio this is in Project Properties -> Configuration Properties -> 
//     C/C++ -> Preprocessor -> Preprocessor Definitions.
#if defined(USE_EMULATOR)

#pragma message ("Compiling " __FILE__ " for the emulator")

#include <string.h>
#include <stdio.h>
#include <stdint.h> // for Uint8/16/32 and Int8/16/32 data types
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

/* EVE MCU HEADER */

#if defined(__linux__) || defined(__CYGWIN__)
// Linux endianness (not BSD variants)
#include <endian.h>
#include <unistd.h>
#elif defined(_WIN32)
// Windows endianness is little endian
#include <windows.h>
#else
// Other endianness (check naming conventions)
#include <sys/endian.h>
#endif // _WIN32

// From issue #25
#if defined(BYTE_ORDER) && BYTE_ORDER == ORDER_LITTLE_ENDIAN
#define HOST_IS_LITTLE_ENDIAN 1
#elif defined(BYTE_ORDER) && BYTE_ORDER == ORDER_BIG_ENDIAN
#define HOST_IS_LITTLE_ENDIAN 0
#endif

#include "bt8xxemu_inttypes.h"
#include "bt8xxemu.h"

/* EVE MCU HEADER END */

#include <EVE.h>
#include <HAL.h>
#include <MCU.h>

/* EVE MCU */

// This platform specific section contains the functions which
// enable the GPIO and SPI interfaces.

void *Emulator;
void *EmulatorFlash;
BT8XXEMU_EmulatorParameters *EmulatorParameters;
BT8XXEMU_FlashParameters *EmulatorFlashParameters; 

// ------------------ Platform specific initialisation  ------------------------

int MCU_Init(void)
{
    printf(BT8XXEMU_version());

#if (FT8XX_TYPE == FT800)
#define EVE_SUPPORT_CHIPID BT8XXEMU_EmulatorFT800
#elif (FT8XX_TYPE == FT801)
#define EVE_SUPPORT_CHIPID BT8XXEMU_EmulatorFT801
#elif (FT8XX_TYPE == FT810)
#define EVE_SUPPORT_CHIPID BT8XXEMU_EmulatorFT810
#elif (FT8XX_TYPE == FT811)
#define EVE_SUPPORT_CHIPID BT8XXEMU_EmulatorFT811
#elif (FT8XX_TYPE == FT812)
#define EVE_SUPPORT_CHIPID BT8XXEMU_EmulatorFT812
#elif (FT8XX_TYPE == FT813)
#define EVE_SUPPORT_CHIPID BT8XXEMU_EmulatorFT813
#elif (FT8XX_TYPE == BT880)
#define EVE_SUPPORT_CHIPID BT8XXEMU_EmulatorBT880
#elif (FT8XX_TYPE == BT881)
#define EVE_SUPPORT_CHIPID BT8XXEMU_EmulatorBT881
#elif (FT8XX_TYPE == BT882)
#define EVE_SUPPORT_CHIPID BT8XXEMU_EmulatorBT882
#elif (FT8XX_TYPE == BT883)
#define EVE_SUPPORT_CHIPID BT8XXEMU_EmulatorBT883
#elif (FT8XX_TYPE == BT815)
#define EVE_SUPPORT_CHIPID BT8XXEMU_EmulatorBT815
#elif (FT8XX_TYPE == BT816)
#define EVE_SUPPORT_CHIPID BT8XXEMU_EmulatorBT816
#elif (FT8XX_TYPE == BT817)
#define EVE_SUPPORT_CHIPID BT8XXEMU_EmulatorBT817
#elif (FT8XX_TYPE == BT818)
#define EVE_SUPPORT_CHIPID BT8XXEMU_EmulatorBT818
#elif (FT8XX_TYPE == BT820)
#define EVE_SUPPORT_CHIPID 0x820 // BT8XXEMU_EmulatorBT820
#endif

    // Make defaults
    EmulatorParameters = (BT8XXEMU_EmulatorParameters *)malloc(sizeof(BT8XXEMU_EmulatorParameters));
    if (!EmulatorParameters)
        return -1;
    EmulatorFlashParameters = (BT8XXEMU_FlashParameters *)malloc(sizeof(BT8XXEMU_FlashParameters));
    if (!EmulatorFlashParameters)
        return -1;

    BT8XXEMU_defaults(BT8XXEMU_VERSION_API, EmulatorParameters, EVE_SUPPORT_CHIPID & 0xffff);
    EmulatorParameters->Flags &= (~BT8XXEMU_EmulatorEnableDynamicDegrade & ~BT8XXEMU_EmulatorEnableRegPwmDutyEmulation);
    
    BT8XXEMU_Flash_defaults(BT8XXEMU_VERSION_API, EmulatorFlashParameters);
#if defined(EVE_EMULATOR_FLASH_FILE)
    strcpy(EmulatorFlashParameters->DataFilePath, EVE_EMULATOR_FLASH_FILE);
#endif
    EmulatorFlash = BT8XXEMU_Flash_create(BT8XXEMU_VERSION_API, EmulatorFlashParameters);

    BT8XXEMU_run(BT8XXEMU_VERSION_API, &Emulator, EmulatorParameters);

    return 0;
}

int MCU_Deinit(void)
{
    // Release emulator
    if (Emulator)
    {
        BT8XXEMU_stop(Emulator);
        BT8XXEMU_destroy(Emulator);
    }
    Emulator = NULL;

    // Release flash
    if (EmulatorFlash)
    {
        BT8XXEMU_Flash_destroy(EmulatorFlash);
    }
    EmulatorFlash = NULL;

    // Release emulator parameters
    if (EmulatorParameters)
    {
        free(EmulatorParameters);
        EmulatorParameters = NULL;
    }
    if (EmulatorFlashParameters)
    {
        free(EmulatorFlashParameters);
        EmulatorFlashParameters = NULL;
    }

    return 0;
}

int MCU_Setup(void)
{
    return 0;
}

// --------------------- Chip Select line low ----------------------------------
void MCU_CSlow(void)
{
    BT8XXEMU_chipSelect(Emulator, 1);
}

// --------------------- Chip Select line high ---------------------------------
void MCU_CShigh(void)
{
    BT8XXEMU_chipSelect(Emulator, 0);
}

// -------------------------- PD line low --------------------------------------
void MCU_PDlow(void)
{
    if (!Emulator)
    {
        BT8XXEMU_run(BT8XXEMU_VERSION_API, &Emulator, EmulatorParameters);
    }

    BT8XXEMU_stop(Emulator);
    BT8XXEMU_destroy(Emulator);
    Emulator = NULL;
}

// ------------------------- PD line high --------------------------------------
void MCU_PDhigh(void)
{
    if (Emulator)
    {
        BT8XXEMU_stop(Emulator);
        BT8XXEMU_destroy(Emulator);
        Emulator = NULL;
    }

    BT8XXEMU_run(BT8XXEMU_VERSION_API, &Emulator, EmulatorParameters);
}

// ------------------------- Delay functions -----------------------------------

void MCU_Delay_20ms(void)
{
#ifdef _WIN32
    Sleep(20);
#else
    usleep(20 * 1000);
#endif
}

void MCU_Delay_500ms(void)
{
#ifdef _WIN32
    Sleep(500);
#else
    usleep(500 * 1000);
#endif
}

// --------------------- SPI Send and Receive ----------------------------------

// Exchange a single byte on the SPI bus
char MCU_SPIReadWrite8(uint8_t val) 
{
    uint8_t valrx = BT8XXEMU_transfer(Emulator, val); 
    return valrx;
}

uint16_t MCU_SPIReadWrite16(uint16_t DataToWrite) 
{
  uint16_t DataRead = 0;
  uint16_t temp;

  temp = (MCU_SPIReadWrite8((DataToWrite >> 0) & 0xff) & 0xff);
  DataRead |= (temp << 0);
  temp = (MCU_SPIReadWrite8((DataToWrite >> 8) & 0xff) & 0xff);
  DataRead |= (temp << 8);

  return DataRead;
}

uint32_t MCU_SPIReadWrite24(uint32_t DataToWrite) 
{
  uint32_t DataRead = 0;
  uint32_t temp;

  temp = (MCU_SPIReadWrite8((DataToWrite >> 0) & 0xff) & 0xff);
  DataRead |= (temp << 8);
  temp = (MCU_SPIReadWrite8((DataToWrite >> 8) & 0xff) & 0xff);
  DataRead |= (temp << 16);
  temp = (MCU_SPIReadWrite8((DataToWrite >> 16) & 0xff) & 0xff);
  DataRead |= (temp << 24);

  return DataRead;
}

uint32_t MCU_SPIReadWrite32(uint32_t DataToWrite) 
{
  uint32_t DataRead = 0;
  uint32_t temp;

  temp = (MCU_SPIReadWrite8((DataToWrite >> 0) & 0xff) & 0xff);
  DataRead |= (temp << 0);
  temp = (MCU_SPIReadWrite8((DataToWrite >> 8) & 0xff) & 0xff);
  DataRead |= (temp << 8);
  temp = (MCU_SPIReadWrite8((DataToWrite >> 16) & 0xff) & 0xff);
  DataRead |= (temp << 16);
  temp = (MCU_SPIReadWrite8((DataToWrite >> 24) & 0xff) & 0xff);
  DataRead |= (temp << 24);

  return DataRead;
}

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

uint16_t MCU_htobe16(uint16_t h)
{
#ifdef _WIN32
    return _byteswap_ushort(h);
#else // _WIN32
    return htobe16(h);
#endif // _WIN32
    }

uint32_t MCU_htobe32(uint32_t h)
{
#ifdef _WIN32
    return _byteswap_ulong(h);
#else // _WIN32
    return htobe32(h);
#endif // _WIN32
}

uint16_t MCU_htole16(uint16_t h)
{
#ifdef _WIN32
    return (h);
#else // _WIN32
    return htole16(h);
#endif // _WIN32
}

uint32_t MCU_htole32(uint32_t h)
{
#ifdef _WIN32
    return (h);
#else // _WIN32
    return htole32(h);
#endif // _WIN32
}

uint16_t MCU_be16toh(uint16_t h)
{
#ifdef _WIN32
    return _byteswap_ushort(h);
#else // _WIN32
    return be16toh(h);
#endif // _WIN32
}

uint32_t MCU_be32toh(uint32_t h)
{
#ifdef _WIN32
    return _byteswap_ulong(h);
#else // _WIN32
    return be32toh(h);
#endif // _WIN32
}

uint16_t MCU_le16toh(uint16_t h)
{
#ifdef _WIN32
    return (h);
#else // _WIN32
    return le16toh(h);
#endif // _WIN32
}

uint32_t MCU_le32toh(uint32_t h)
{
#ifdef _WIN32
    return (h);
#else // _WIN32
    return le32toh(h);
#endif // _WIN32
}

/* EVE MCU END */

#endif /* defined(USE_EMULATOR) */
