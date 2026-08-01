/**
@file EVE_emulator.c
* /
/*
 * ============================================================================
 * (C) Copyright Bridgetek Pte Ltd
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
#ifndef _WIN32
#include <sys/types.h>
#include <sys/stat.h>
#endif

/* EVE MCU HEADER */
#ifdef _WIN32
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

// This platform specific section contains the functions which create the EVE Emulator instance

static BT8XXEMU_Emulator* Emulator = NULL;
static BT8XXEMU_Flash* EmulatorFlash = NULL;
static BT8XXEMU_EmulatorParameters* EmulatorParameters = NULL;
static BT8XXEMU_FlashParameters* EmulatorFlashParameters = NULL;
static eve_tchar_t* SDCardFolder = NULL;

// helper fucntion for setting path to flash binary images
static int MCU_SetFlashEmulatorDataFilePath(BT8XXEMU_FlashParameters* parameters, const eve_tchar_t* filePath)
{
#ifdef _WIN32
    eve_tchar_t resolvedPath[260];
    eve_tchar_t* lastSeparator;
    DWORD pathLength;
    DWORD attributes;


    if (!filePath || !filePath[0])
        return -1;

    // Preserve absolute path supplied by the application.
    if (filePath[1] == L':') {
        // print absolute path
        wprintf(L"Flash binary path: %ls\n", filePath);
        // return the result of a copy of the absolute path to the emulator parameters
        return wcscpy_s(parameters->DataFilePath, _countof(parameters->DataFilePath), filePath) == 0 ? 0 : -1;
    }

    // print path relative to exe
    wprintf(L"Relative flash binary path from executable: %ls\n", filePath);

    // Resolve relative paths beside the running executable.
    pathLength = GetModuleFileNameW(NULL, resolvedPath, _countof(resolvedPath));
    if (!pathLength || pathLength >= _countof(resolvedPath))
        return -1;

    lastSeparator = wcsrchr(resolvedPath, L'\\');
    if (!lastSeparator)
        return -1;

    lastSeparator[1] = L'\0';

    if (wcscat_s(resolvedPath, _countof(resolvedPath), filePath) != 0)
        return -1;

    // Verify that the resolved file exists.
    attributes = GetFileAttributesW(resolvedPath);
    if (attributes == INVALID_FILE_ATTRIBUTES ||
        (attributes & FILE_ATTRIBUTE_DIRECTORY)) {
        DEBUG_ERROR("ERROR: Flash file does not exist.\n");
        return -1;
    }

    // return the result of a copy of the resolved path to the emulator parameters
    return wcscpy_s(parameters->DataFilePath,
        _countof(parameters->DataFilePath),
        resolvedPath) == 0 ? 0 : -1;
#else
    struct stat st;

    if (!filePath || !filePath[0])
        return -1;

    // Verify that the file exists and is not a directory
    if (stat(filePath, &st) != 0 || !S_ISREG(st.st_mode)) {
        DEBUG_ERROR("ERROR: Flash file does not exist.\n");
        return -1;
    }

    strncpy(parameters->DataFilePath,
        filePath,
        sizeof(parameters->DataFilePath) - 1);
    parameters->DataFilePath[sizeof(parameters->DataFilePath) - 1] = '\0';

    return 0;
#endif // _WIN32
}

// helper function to resolve SD card folder path 
static const eve_tchar_t* MCU_ResolveSDFolderPath(const eve_tchar_t* folderPath)
{
#ifdef _WIN32
    static eve_tchar_t resolvedPath[260];
    eve_tchar_t* lastSeparator;
    DWORD pathLength;
    DWORD attributes;

    if (!folderPath || !folderPath[0])
        return NULL;

    // Preserve absolute path supplied by the application.
    if (folderPath[1] == L':') {
        // print absolute path
        wprintf(L"SD card folder path: %ls\n", folderPath);
        // return the absolute path
        return folderPath;
    }

    // print path relative to exe
    wprintf(L"Relative SD card folder path from executable: %ls\n", folderPath);

    // Resolve relative paths beside the running executable.
    pathLength = GetModuleFileNameW(NULL, resolvedPath, _countof(resolvedPath));
    if (!pathLength || pathLength >= _countof(resolvedPath))
        return NULL;

    lastSeparator = wcsrchr(resolvedPath, L'\\');
    if (!lastSeparator)
        return NULL;

    lastSeparator[1] = L'\0';

    if (wcscat_s(resolvedPath, _countof(resolvedPath), folderPath) != 0)
        return NULL;

    // Verify the folder exists.
    attributes = GetFileAttributesW(resolvedPath);
    if (attributes == INVALID_FILE_ATTRIBUTES || !(attributes & FILE_ATTRIBUTE_DIRECTORY)) {
        DEBUG_ERROR("ERROR: SD card folder does not exist.\n");
        return NULL;
    }

    return resolvedPath;

#else
    struct stat st;

    if (!folderPath || !folderPath[0])
        return NULL;

    if (stat(folderPath, &st) != 0 || !S_ISDIR(st.st_mode))
    {
        DEBUG_ERROR("ERROR: SD card folder does not exist.\n");
        return NULL;
    }

    return folderPath;
#endif // _WIN32
}

// ------------------ Platform specific initialisation  ------------------------
int MCU_Init(void)
{
#ifdef _WIN32
    // print Emulator Version
    DEBUG_PRINTF("\n");
    DEBUG_PRINTF(BT8XXEMU_version());
    DEBUG_PRINTF("\n\n");

    // Set emulator type from EVE_conig.h EVE type setting
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
#define EVE_SUPPORT_CHIPID BT8XXEMU_EmulatorBT820
#endif

    // create emulator defaults
    EmulatorParameters = (BT8XXEMU_EmulatorParameters*)malloc(sizeof(BT8XXEMU_EmulatorParameters));
    if (!EmulatorParameters)
    {
        DEBUG_ERROR("ERROR: Unable to allocate emulator parameters.\n");
        MCU_Deinit();
        return -1;
    }

    // initialize default emulator parameters
    BT8XXEMU_defaults(BT8XXEMU_VERSION_API, EmulatorParameters, EVE_SUPPORT_CHIPID & 0xffff);
    // turn off EmulatorEnableDynamicDegrade and EmulatorEnableRegPwmDutyEmulation 
    EmulatorParameters->Flags &= (~BT8XXEMU_EmulatorEnableDynamicDegrade & ~BT8XXEMU_EmulatorEnableRegPwmDutyEmulation);

    // check if the flash file definition is set
#ifdef EVE_EMULATOR_FLASH_FILE
    // flash is only supoprted in EVE API level = 3,4,5
#if IS_EVE_API(3,4,5)

    DEBUG_PRINTF("EVE_EMULATOR_FLASH_FILE is defined, attempting to attach Flash to emulator.\n");

    // create flash emulator defualts
    EmulatorFlashParameters = (BT8XXEMU_FlashParameters*)malloc(sizeof(BT8XXEMU_FlashParameters));
    if (!EmulatorFlashParameters)
    {
        DEBUG_ERROR("ERROR: Unable to allocate flash emulator parameters.\n");
        MCU_Deinit();
        return -1;
    }

    // initialize default flash emulator parameters
    BT8XXEMU_Flash_defaults(BT8XXEMU_VERSION_API, EmulatorFlashParameters);

    // set the name and path of the data file we want to load onto the flash
    if (MCU_SetFlashEmulatorDataFilePath(EmulatorFlashParameters, EVE_EMULATOR_FLASH_FILE) != 0)
    {
        DEBUG_ERROR("ERROR: Unable to resolve emulator Flash file path.\n\n");
        DEBUG_ERROR("Check that the .bin file exists on the path defined by:\n");
        DEBUG_ERROR("EVE_EMULATOR_FLASH_FILE = TEXT(\"..\\\\path\\\\to\\\\bin\\\\from\\\\exe\\\\flash-name.bin\").\n");
        MCU_Deinit();
        return -1;
    }

    // check if EVE_EMULATOR_FLASH_FILE_SIZE as been defined if not use (8 MiB)
#ifndef EVE_EMULATOR_FLASH_FILE_SIZE
#define EVE_EMULATOR_FLASH_FILE_SIZE (8 * 1024 * 1024)    // 8 MiB default
#endif

    // set flash size to EVE_EMULATOR_FLASH_FILE_SIZ
    EmulatorFlashParameters->SizeBytes = EVE_EMULATOR_FLASH_FILE_SIZE;

    // Create flash emulator instance
    EmulatorFlash = BT8XXEMU_Flash_create(BT8XXEMU_VERSION_API, EmulatorFlashParameters);
    if (!EmulatorFlash)
    {
        DEBUG_ERROR("ERROR: Unable to create Emulator Flash.\n");
        MCU_Deinit();
        return -1;
    }

    // connect emulator to the flash device we have created
    EmulatorParameters->Flash = EmulatorFlash;

    DEBUG_PRINTF("SUCESS: Flash image attached to emulator.\n\n");

#else
    DEBUG_PRINTF("EVE_EMULATOR_FLASH_FILE is defined but flash is not available on EVE API = 1,2.\n");
#endif  // IS_EVE_API(3,4,5)
#endif  // EVE_EMULATOR_FLASH_FILE

    // Run the emulator on the current thread
    BT8XXEMU_run(BT8XXEMU_VERSION_API, &Emulator, EmulatorParameters);
    if (!Emulator)
    {
        DEBUG_ERROR("ERROR: Unable to create Emulator.\n");
        MCU_Deinit();
        return -1;
    }

    // check if the SD folder definition is set
#ifdef EVE_EMULATOR_SD_FOLDER
    // SD cards are only supoprted in EVE API level = 5
#if IS_EVE_API(5)

    DEBUG_PRINTF("EVE_EMULATOR_SD_FOLDER is defined, attempting to attach SD card folder to emulator.\n");

    // set SDCardFolder to supplied path in definition 
    SDCardFolder = MCU_ResolveSDFolderPath(EVE_EMULATOR_SD_FOLDER);

    // check the SDCardFolder path is valid
    if (SDCardFolder == NULL) {
        DEBUG_ERROR("ERROR: Unable to resolve emulator SD card folder file path.\n\n");
        DEBUG_ERROR("Check that the folder exists on the path defined by:\n");
        DEBUG_ERROR("EVE_EMULATOR_SD_FOLDER = TEXT(\"..\\\\path\\\\to\\\\sd\\\\folder\\\\from\\\\exe\").\n");
        MCU_Deinit();
        return -1;
    }
    else {

    // check if EVE_EMULATOR_SD_FOLDER_SIZE as been defined if not use (64 MiB)
#ifndef EVE_EMULATOR_SD_FOLDER_SIZE
#define EVE_EMULATOR_SD_FOLDER_SIZE (64 * 1024 * 1024)
#endif
        // insert the SD card folder to the eumlator (fuction returns size of inserted SD folder)
        if (BT8XXEMU_insertSDCardFolder(Emulator, SDCardFolder, EVE_EMULATOR_SD_FOLDER_SIZE, false) <= 0)
        {
            DEBUG_ERROR("ERROR: Unable to attach SD card to emulator.\n\n");
            MCU_Deinit();
            return -1;
        }

        DEBUG_PRINTF("SUCCESS: SD card folder successfully attached to emulator.\n\n");
    }
#else
    DEBUG_PRINTF("EVE_EMULATOR_SD_FOLDER is defined but SD cards are only available on EVE API = 5.\n");
#endif  // IS_EVE_API(,5)
#endif  // EVE_EMULATOR_SD_FOLDER

    return 0;
#else
    return -1;
#endif // _WIN32
}

int MCU_Deinit(void)
{
    // Release emulator
    if (Emulator)
    {
        BT8XXEMU_stop(Emulator);
        BT8XXEMU_destroy(Emulator);
        Emulator = NULL;
    }

    // Release flash
    if (EmulatorFlash)
    {
        BT8XXEMU_Flash_destroy(EmulatorFlash);
        EmulatorFlash = NULL;
    }

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

    // Clean up SD card folder path
    if (SDCardFolder)
    {
        SDCardFolder = NULL;
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
    // check here if the emulator window is still running 
    if (!BT8XXEMU_isRunning(Emulator)) {
        // if the window is no longer running, then call MCU_Deinit() to destory the emluator
        MCU_Deinit();
        // exit application with code 0
        exit(0);
    }
 
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
    //NOTE: called in EVE_API.c after MCU_Init(), which reults in the emulator instance being immediately killed.

    /*
    if (!Emulator)
    {
        BT8XXEMU_run(BT8XXEMU_VERSION_API, &Emulator, EmulatorParameters);
    }

    BT8XXEMU_stop(Emulator);
    BT8XXEMU_destroy(Emulator);
    Emulator = NULL;
    */
}

// ------------------------- PD line high --------------------------------------
void MCU_PDhigh(void)
{
    //NOTE: called in EVE_API.c after MCU_PDlow(), which reults in the emulator instance being restarted.

    /*
    if (Emulator)
    {
        BT8XXEMU_stop(Emulator);
        BT8XXEMU_destroy(Emulator);
        Emulator = NULL;
    }

    BT8XXEMU_run(BT8XXEMU_VERSION_API, &Emulator, EmulatorParameters);
    */
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

void MCU_SPIWrite(const uint8_t* DataToWrite, uint32_t length) {
    //TODO: replace with SPI.transfer(DataToWrite, length);
    // Note that DataToWrite is overwritten.
    uint16_t DataPointer = 0;

    while (DataPointer < length) {
        MCU_SPIWrite8(DataToWrite[DataPointer]);  // Send data byte-by-byte from array
        DataPointer++;
    }
}

void MCU_SPIRead(uint8_t* DataToRead, uint32_t length) {
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
