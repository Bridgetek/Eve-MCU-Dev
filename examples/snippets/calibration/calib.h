/**
 * @file calib.h
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

#ifndef _EVE_CALIB_H
#define _EVE_CALIB_H

#include <stdint.h>

/* Get the touchscreen calibration storage structure. */
#include "../touch.h"

/* No calibration data is stored. 
 * Selects: "calib_none.c" 
 */
#define CALIBRATION_STORAGE_NONE 0

/* Calibration data is stored in a file system using standard file system methods. 
 * Selects: "calib_file.c" 
 */
#define CALIBRATION_STORAGE_FILE 1

/* Calibration data is stored in pico program memory flash.
 * Selects: "calib_pico.c" 
 */
#define CALIBRATION_STORAGE_PICO 2

/* Calibration data is stored in EEPROM using Arduino library.
 * Selects: "calib_arduino.cpp" 
 */
#define CALIBRATION_STORAGE_ARDUINO_EEPROM 3
#define CALIBRATION_STORAGE_ARDUINO_EEPROM_ESP32 4
#define CALIBRATION_STORAGE_ARDUINO_FLASH_PICO 5

/* Calibration data is stored in ESP32 flash.
 * Selects: "calib_esp32.c" 
 */
#define CALIBRATION_STORAGE_ESP32 6

/* Calibration data is stored in FT9xx program memory flash.
 * Selects: "calib_ft9xx.c" 
 */
#define CALIBRATION_STORAGE_FT9XX 7

// Condition 1: File system available - running on Windows or Linux
#if defined(_WIN32) || defined(linux)
#define CALIBRATION_STORAGE CALIBRATION_STORAGE_FILE
// Condition 2: Using pico SDK - standalone and within Arduino
#elif defined(PICO_SDK_VERSION_STRING)
#define CALIBRATION_STORAGE CALIBRATION_STORAGE_PICO
// Condition 3: Supported Arduino devices
#elif defined(ARDUINO)
    // Condition 3a: Built-in EEPROM library for Arduino
    // AVR, MEGAAVR, Renesas, Renesas Portenta, Renesas Uno
    #if defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_MEGAAVR) \
        || defined(ARDUINO_ARCH_RENESAS)
    #define CALIBRATION_STORAGE CALIBRATION_STORAGE_ARDUINO_EEPROM
    // Condition 3b: ESP32 architecture Arduinos
    #elif defined(ARDUINO_ARCH_ESP32)
    #define CALIBRATION_STORAGE CALIBRATION_STORAGE_ARDUINO_EEPROM_ESP32
    // Condition 3c: Pico SDK on Arduino
    #elif defined(PICO_SDK_VERSION_STRING)
    #define CALIBRATION_STORAGE CALIBRATION_STORAGE_ARDUINO_FLASH_PICO
    // Condition 3d: No support on available on Arduino
    #else
    #define CALIBRATION_STORAGE CALIBRATION_STORAGE_NONE
    #endif
// Condition 4: ESP32 devices
#elif defined(ESP32)
#define CALIBRATION_STORAGE CALIBRATION_STORAGE_ESP32
// Condition 5: FT9xx devices
#elif defined(__FT930__) || defined(__FT900__)
#define CALIBRATION_STORAGE CALIBRATION_STORAGE_FT9XX
#else
#define CALIBRATION_STORAGE CALIBRATION_STORAGE_NONE
#endif

#endif /* _EVE_CALIB_H */
