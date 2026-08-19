/**
 * @file EVE_emulator.c
 * @details Platform-specific code for controlling running a EVE Emulator instance.
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
  // PLATFORM_EMULATOR macro enables this file to open the emulator library.
  // In gcc compilers this is in the Makefile. -DPLATFORM_EMULATOR
  // In Visual Studio this is in Project Properties -> Configuration Properties -> 
  //     C/C++ -> Preprocessor -> Preprocessor Definitions.
#if defined(PLATFORM_EMULATOR)

#pragma message ("Compiling " __FILE__ " for the emulator")

#ifdef _WIN64

/* EVE MCU HEADER */

#include <string.h>
#include <stdio.h>
#include <stdint.h> // for Uint8/16/32 and Int8/16/32 data types
#include <stdlib.h>

// Windows endianness is little endian
#include <windows.h>

// From issue #25
#if defined(BYTE_ORDER) && BYTE_ORDER == ORDER_LITTLE_ENDIAN
#define HOST_IS_LITTLE_ENDIAN 1
#elif defined(BYTE_ORDER) && BYTE_ORDER == ORDER_BIG_ENDIAN
#define HOST_IS_LITTLE_ENDIAN 0
#endif

#include "bt8xxemu_inttypes.h"
#include "bt8xxemu.h"

/* Include functions for EVE-MCU-Dev library API layer */
#include <EVE.h> // for IS_EVE_API() macro
/* Include functions for EVE-MCU-Dev library Hardware Abstraction layer */
#include <HAL.h> // for HAL_MemWrite8()
/* Include functions for EVE-MCU-Dev library MCU layer */
#include <MCU.h>
/* Include the EVE debug-output macro definitions */
#include <EVE_debug.h>

/* EVE MCU HEADER END */

/* EVE MCU */

// This platform specific section contains the functions which create the EVE Emulator instance

static BT8XXEMU_Emulator* Emulator = NULL;
static BT8XXEMU_EmulatorParameters* EmulatorParameters = NULL;

static BT8XXEMU_Flash* EmulatorFlash = NULL;
static BT8XXEMU_FlashParameters* EmulatorFlashParameters = NULL;

typedef enum
{
    MCU_PATH_FILE,
    MCU_PATH_FOLDER
} MCU_PathType;

/**
 * @brief Determines whether a Windows path is absolute.
 *
 * Absolute paths:
 *   C:\folder\file.bin
 *   C:/folder/file.bin
 *   \\server\share\file.bin
 *   //server/share/file.bin
 */
static bool MCU_IsAbsoluteWindowsPath(const eve_tchar_t* path)
{
    if (!path || !path[0])
        return false;

    /* UNC path */
    if ((path[0] == L'\\' || path[0] == L'/') &&
        (path[1] == L'\\' || path[1] == L'/'))
    {
        return true;
    }

    /* Drive-rooted path */
    if (path[1] == L':' &&
        (path[2] == L'\\' || path[2] == L'/'))
    {
        return true;
    }

    return false;
}

/**
 * @brief Resolves and validates an emulator asset path.
 *
 * @details Absolute paths are used as supplied. Relative paths are resolved against
 *      the directory containing the executable. The resolved path is also checked
 *      to ensure that it exists and matches the requested file or folder type.
 *
 * @param pathType Expected path type: file or folder.
 * @param path     Path supplied by the emulator configuration.
 *
 * @return The validated path, or NULL if the path cannot be resolved or is invalid.
 */
static const eve_tchar_t* MCU_ResolvePath(MCU_PathType pathType, const eve_tchar_t* path)
{
    static eve_tchar_t resolvedPath[MAX_PATH];
    eve_tchar_t* lastSeparator;
    DWORD pathLength;
    DWORD attributes;

    if (!path || !path[0])
        return NULL;

    // Absolute paths are used as supplied; only validate the target type.
    if (MCU_IsAbsoluteWindowsPath(path))
    {
        // Print the absolute path.
        // EVE_Debug.h does not provide a wprintf macro.
#if (defined(DEBUG_LEVEL) && (DEBUG_LEVEL > 0)) 
        wprintf(L"Path: %ls\n", path);
#endif

        // Verify that the path exists and has the expected type.
        attributes = GetFileAttributesW(path);
        if (pathType == MCU_PATH_FOLDER)
        {
            if (attributes == INVALID_FILE_ATTRIBUTES || !(attributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                EVE_DEBUG_ERROR("ERROR: SD card folder does not exist.\n");
                return NULL;
            }
        }
        else if (pathType == MCU_PATH_FILE)
        {
            if (attributes == INVALID_FILE_ATTRIBUTES || (attributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                EVE_DEBUG_ERROR("ERROR: Flash (.bin) file does not exist.\n");
                return NULL;
            }
        }
        else 
        {
            EVE_DEBUG_ERROR("ERROR: Invalid path type.\n");
            return NULL;
        }

        // return the absolute path.
        return path;
    }

    // Print the path relative to the executable.
    // EVE_Debug.h does not provide a wprintf macro..
#if (defined(DEBUG_LEVEL) && (DEBUG_LEVEL > 0)) 
    wprintf(L"Path from executable: %ls\n", path);
#endif

    // Resolve relative paths against the directory containing the executable
    pathLength = GetModuleFileNameW(NULL, resolvedPath, _countof(resolvedPath));

    if (!pathLength)
    {
        EVE_DEBUG_ERROR("ERROR: Unable to obtain executable path.\n");
        return NULL;
    }

    if (pathLength >= _countof(resolvedPath))
    {
        EVE_DEBUG_ERROR("ERROR: Executable path exceeds MAX_PATH.\n");
        return NULL;
    }

    lastSeparator = wcsrchr(resolvedPath, L'\\');

    if (!lastSeparator)
        return NULL;

    lastSeparator[1] = L'\0';

    // Treat a single leading slash or backslash as relative to the executable directory
    const eve_tchar_t* relativePath = path;

    if (relativePath[0] == L'/' || relativePath[0] == L'\\')
        ++relativePath;

    if (!relativePath[0])
        return NULL;

    if (wcscat_s(resolvedPath, _countof(resolvedPath), relativePath) != 0)
    {
        EVE_DEBUG_ERROR("ERROR: Resolved asset path exceeds MAX_PATH.\n");
        return NULL;
    }

    // Verify that the resolved path exists and has the expected type.
    attributes = GetFileAttributesW(resolvedPath);
    if (pathType == MCU_PATH_FOLDER) {
        if (attributes == INVALID_FILE_ATTRIBUTES || !(attributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            EVE_DEBUG_ERROR("ERROR: SD card folder does not exist.\n");
            return NULL;
        }
    }
    else if (pathType == MCU_PATH_FILE) {
        if (attributes == INVALID_FILE_ATTRIBUTES || (attributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            EVE_DEBUG_ERROR("ERROR: Flash (.bin) file does not exist.\n");
            return NULL;
        }
    }
    else
    {
        EVE_DEBUG_ERROR("ERROR: Invalid path type.\n");
        return NULL;
    }

    // Return the resolved path.
    return resolvedPath;
}

// External flash emulation is supported by EVE API levels 3, 4, and 5.
#if IS_EVE_API(3,4,5)

/**
 * @brief Creates and attaches an emulated flash device.
 *
 * @details Resolves the configured flash file, initializes the flash emulator
 *      parameters, creates the flash device, and attaches it to the emulator
 *      configuration.
 *
 * @param path Path to the flash binary file.
 *
 * @return 0 on success, or -1 if the flash device cannot be configured or created.
 */
static int MCU_AttachFlash(const eve_tchar_t* path) {

    EVE_DEBUG_PRINTF("EVE_EMULATOR_FLASH_FILE is defined, attempting to attach flash (.bin) file to emulator...\n");

    // Allocate and initialize the flash emulator configuration.
    EmulatorFlashParameters = (BT8XXEMU_FlashParameters*)malloc(sizeof(BT8XXEMU_FlashParameters));
    if (!EmulatorFlashParameters)
    {
        EVE_DEBUG_ERROR("ERROR: Unable to allocate flash emulator parameters.\n");
        return -1;
    }

    // Populate the configuration with emulator defaults.
    BT8XXEMU_Flash_defaults(BT8XXEMU_VERSION_API, EmulatorFlashParameters);

    // Resolve the flash file path supplied by the definition.
    const eve_tchar_t* flashPath = MCU_ResolvePath(MCU_PATH_FILE, path);

    if (!flashPath)
    {
        EVE_DEBUG_ERROR("ERROR: Unable to resolve emulator flash (.bin) file path.\n\n");
        EVE_DEBUG_ERROR("Check that the flash (.bin) file exists on the path defined by:\n");
        EVE_DEBUG_ERROR("EVE_EMULATOR_FLASH_FILE = TEXT(\"flash-name.bin\").\n");
        EVE_DEBUG_ERROR("OR\n");
        EVE_DEBUG_ERROR("EVE_EMULATOR_FLASH_FILE = TEXT(\"..\\\\path\\\\to\\\\bin\\\\from\\\\exe\\\\flash-name.bin\").\n\n");
        return -1;
    }

    // Copy the resolved flash path into the emulator configuration.
    errno_t result = wcscpy_s(EmulatorFlashParameters->DataFilePath, _countof(EmulatorFlashParameters->DataFilePath), flashPath);

    if (result != 0)
    {
        EVE_DEBUG_ERROR("ERROR: Unable to copy emulator flash path.\n");
        return -1;
    }

    // Default the emulated flash size to 8 MiB when no size is configured.
#ifndef EVE_EMULATOR_FLASH_FILE_SIZE
#define EVE_EMULATOR_FLASH_FILE_SIZE (8 * 1024 * 1024)  /* 8 MiB default */
#endif /* EVE_EMULATOR_FLASH_FILE_SIZE */

    // Set flash size to EVE_EMULATOR_FLASH_FILE_SIZE
    EmulatorFlashParameters->SizeBytes = EVE_EMULATOR_FLASH_FILE_SIZE;

    // Create the emulated flash device.
    EmulatorFlash = BT8XXEMU_Flash_create(BT8XXEMU_VERSION_API, EmulatorFlashParameters);
    if (!EmulatorFlash)
    {
        EVE_DEBUG_ERROR("ERROR: Unable to create Emulator Flash.\n");
        return -1;
    }

    // Attach the flash device to the emulator configuration.
    EmulatorParameters->Flash = EmulatorFlash;

    return 0;
}

#endif /* IS_EVE_API(3,4,5) */

// SD card folder emulation is supported by EVE API level 5 only.
#if IS_EVE_API(5)

/**
 * @brief Attaches a folder as an emulated SD card.
 *
 * @details Resolves and validates the configured folder, then inserts it into the
 *       running emulator as an SD card image.
 *
 * @param path Path to the folder containing the SD card contents.
 *
 * @return 0 on success, or -1 if the folder cannot be resolved or attached.
 */
static int MCU_AttachSDFolder(const eve_tchar_t* path) {

    EVE_DEBUG_PRINTF("EVE_EMULATOR_SD_FOLDER is defined, attempting to attach SD card folder to emulator...\n");

    // Resolve the SD card folder path supplied by the definition.
    const eve_tchar_t* sdPath = MCU_ResolvePath(MCU_PATH_FOLDER, path);

    // Verify that the SD card folder path is valid.
    if (sdPath == NULL)
    {
        EVE_DEBUG_ERROR("ERROR: Unable to resolve emulator SD card folder file path.\n\n");
        EVE_DEBUG_ERROR("Check that the folder exists on the path defined by:\n");
        EVE_DEBUG_ERROR("EVE_EMULATOR_SD_FOLDER = TEXT(\"sdFolderName\").\n");
        EVE_DEBUG_ERROR("OR\n");
        EVE_DEBUG_ERROR("EVE_EMULATOR_SD_FOLDER = TEXT(\"..\\\\path\\\\to\\\\sd\\\\folder\\\\from\\\\exe\\\\sdFolderName\").\n\n");
        return -1;
    }

    // Check whether EVE_EMULATOR_SD_FOLDER_SIZE has been defined, if not set (0).
    // Passing zero for minimumSize lets the emulator calculate the required FAT32 image size from the folder contents.
#ifndef EVE_EMULATOR_SD_FOLDER_SIZE
#define EVE_EMULATOR_SD_FOLDER_SIZE 0
#endif  /* EVE_EMULATOR_SD_FOLDER_SIZE */

    // Insert the SD card folder into the emulator instance.
    // The function returns the size of the inserted SD card folder.
    if (BT8XXEMU_insertSDCardFolder(Emulator, sdPath, EVE_EMULATOR_SD_FOLDER_SIZE, false) <= 0)
    {
        EVE_DEBUG_ERROR("ERROR: Unable to attach SD card to emulator.\n\n");
        return -1;
    }

    return 0; 
}

#endif /* IS_EVE_API(5) */

// ------------------ Platform specific initialisation  ------------------------
int MCU_Init(void)
{
    // Print Emulator Version
    EVE_DEBUG_PRINTF("\n");
    EVE_DEBUG_PRINTF("%s", BT8XXEMU_version());
    EVE_DEBUG_PRINTF("\n\n");

    // Map the configured EVE device type to the corresponding emulator device.
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
#else
#error Unsupported FT8XX_TYPE for EVE Emulator
#endif

    // Allocate and initialize the emulator configuration.
    EmulatorParameters = (BT8XXEMU_EmulatorParameters*)malloc(sizeof(BT8XXEMU_EmulatorParameters));
    if (!EmulatorParameters)
    {
        EVE_DEBUG_ERROR("ERROR: Unable to allocate emulator parameters.\n");
        MCU_Deinit();
        return -1;
    }

    // Populate the configuration with emulator defaults.
    BT8XXEMU_defaults(BT8XXEMU_VERSION_API, EmulatorParameters, EVE_SUPPORT_CHIPID & 0xffff);
    // Disable EmulatorEnableDynamicDegrade and EmulatorEnableRegPwmDutyEmulation
    EmulatorParameters->Flags &= ~(BT8XXEMU_EmulatorEnableDynamicDegrade | BT8XXEMU_EmulatorEnableRegPwmDutyEmulation);

#ifdef EVE_EMULATOR_FLASH_FILE
#if IS_EVE_API(3,4,5)
    // Attach flash to the emulator configuration.
    if (MCU_AttachFlash(EVE_EMULATOR_FLASH_FILE) == 0)
    {
        EVE_DEBUG_PRINTF("SUCCESS: Flash (.bin) file attached to emulator.\n\n");
    }
    else
    {
        MCU_Deinit();
        return -1;
    }
#else
#error EVE_EMULATOR_FLASH_FILE requires EVE API 3, 4, or 5
#endif /* IS_EVE_API(3,4,5) */
#endif /* EVE_EMULATOR_FLASH_FILE */

    // Create and start the emulator instance on the current thread.
    BT8XXEMU_run(BT8XXEMU_VERSION_API, &Emulator, EmulatorParameters);
    if (!Emulator)
    {
        EVE_DEBUG_ERROR("ERROR: Unable to create Emulator.\n");
        MCU_Deinit();
        return -1;
    }

    // Attach the configured SD card folder after the emulator has started.
#ifdef EVE_EMULATOR_SD_FOLDER
#if IS_EVE_API(5)
    // Attach the SD card folder to the running emulator.
    if (MCU_AttachSDFolder(EVE_EMULATOR_SD_FOLDER) == 0)
    {
        EVE_DEBUG_PRINTF(
            "SUCCESS: SD card folder successfully attached to emulator.\n\n");
    }
    else
    {
        MCU_Deinit();
        return -1;
    }
#else
#error EVE_EMULATOR_SD_FOLDER requires EVE API 5
#endif /* IS_EVE_API(5) */
#endif /* EVE_EMULATOR_SD_FOLDER */

    // Release the emulator CPU from reset on EVE API levels 1 through 4.
#if IS_EVE_API(1,2,3,4)
    HAL_MemWrite8(EVE_REG_CPURESET, 0);
#endif /* IS_EVE_API(1,2,3,4) */

    EVE_DEBUG_PRINTF("SUCCESS: Emulator instance running.\n\n");

    return 0;
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

    // Release flash emulator
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

    // Release flash emulator parameters
    if (EmulatorFlashParameters)
    {
        free(EmulatorFlashParameters);
        EmulatorFlashParameters = NULL;
    }

    return 0;
}

int MCU_Setup(void)
{
    /* No Additional "SPI" Configuration */
    return 0;
}

// --------------------- Chip Select line low ----------------------------------
void MCU_CSlow(void)
{
    // Check whether the emulator window is still running 
    if (!Emulator || !BT8XXEMU_isRunning(Emulator)) {
        // If the window is no longer running, call MCU_Deinit() to destroy the emulator
        MCU_Deinit();
        // Exit application with exit code 0
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
    /*
     * PD emulation is intentionally disabled. EVE_API.c calls MCU_PDlow()
     * immediately after MCU_Init(), which would destroy the newly created
     * emulator instance.
     */
}

// ------------------------- PD line high --------------------------------------
void MCU_PDhigh(void)
{
    /*
     * PD emulation is intentionally disabled. EVE_API.c calls MCU_PDhigh()
     * immediately after MCU_PDlow(), which would result in the emulator
     * instance being restarted.
     */
}

// ------------------------- Delay functions -----------------------------------

void MCU_Delay_20ms(void)
{
    Sleep(20);
}

void MCU_Delay_500ms(void)
{
    Sleep(500);
}

// --------------------- SPI Send and Receive ----------------------------------

// Exchange a single byte on the SPI bus
uint8_t MCU_SPIReadWrite8(uint8_t val)
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

void MCU_SPIWrite16(uint16_t DataToWrite)
{
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

    uint32_t DataPointer = 0;

    while (DataPointer < length) {
        MCU_SPIWrite8(DataToWrite[DataPointer]);  // Send data byte-by-byte from array
        DataPointer++;
    }
}

void MCU_SPIRead(uint8_t* DataToRead, uint32_t length) {

    uint32_t DataPointer = 0;

    while (DataPointer < length) {
        DataToRead[DataPointer] = MCU_SPIRead8();  // Receive data byte-by-byte to array
        DataPointer++;
    }
}

uint16_t MCU_htobe16(uint16_t h)
{
    return _byteswap_ushort(h);
}

uint32_t MCU_htobe32(uint32_t h)
{
    return _byteswap_ulong(h);
}

uint16_t MCU_htole16(uint16_t h)
{
    return (h);
}

uint32_t MCU_htole32(uint32_t h)
{
    return (h);
}

uint16_t MCU_be16toh(uint16_t h)
{
    return _byteswap_ushort(h);
}

uint32_t MCU_be32toh(uint32_t h)
{
    return _byteswap_ulong(h);
}

uint16_t MCU_le16toh(uint16_t h)
{
    return (h);
}

uint32_t MCU_le32toh(uint32_t h)
{
    return (h);
}

/* EVE MCU END */

#else

#error EVE Emulator is only supported on x64 Windows

#endif // _WIN64

#endif /* defined(PLATFORM_EMULATOR) */
