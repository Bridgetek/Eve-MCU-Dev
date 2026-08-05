/**
 * @file EVE_debug.h
 * @details file provides platform specific macro definitions for debug messaging.
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

#ifndef EVE_DEBUG_H_
#define EVE_DEBUG_H_

/*
 * Enable informational debug output by default on host platforms that
 * normally provide an operating-system console. An explicitly defined
 * DEBUG_LEVEL, including DEBUG_LEVEL=0, is preserved.
 */

#ifndef DEBUG_LEVEL
#if defined(USE_MPSSE) || defined(USE_FT4222) || defined(PLATFORM_EMULATOR) 
#define DEBUG_LEVEL 1
#endif
#endif

/**
 * @brief Configure debug output.
 * @details
 * Debug output is enabled by defining DEBUG_LEVEL:
 *
 * - DEBUG_LEVEL=0 enables error messages only.
 * - DEBUG_LEVEL>0 enables error and informational messages.
 *
 * Host platforms normally write errors to stderr and informational output
 * to stdout. ESP32 uses the ESP-IDF logging API. RP2040 writes both error
 * and informational output to stdout because Pico stdio does not normally
 * separate stderr from stdout over USB or UART. On unsupported platforms,
 * the debug macros expand to no-op expressions.
 */

/* DEBUG_LEVEL=0 */
#if defined(DEBUG_LEVEL)
/* select from supported platforms */
#if defined(PLATFORM_RASPBERRYPI) || \
    defined(USE_LINUX_SPI_DEV) || \
    defined(USE_MPSSE) || \
    defined(USE_FT4222) || \
    defined(PLATFORM_EMULATOR)

#include <stdio.h>
#define EVE_DEBUG_ERROR(...) fprintf(stderr, __VA_ARGS__)

#elif defined(PLATFORM_ESP32)
#include "esp_log.h"
#define EVE_DEBUG_ERROR(...) ESP_LOGE(__FUNCTION__, __VA_ARGS__)

#elif defined(PLATFORM_RP2040)
/* Pico stdio does not normally separate stderr from stdout over USB or UART. */
#include <stdio.h>
#define EVE_DEBUG_ERROR(...) printf("[ERROR] " __VA_ARGS__)

#else
/* else map to no op if no platfrom support */
#define EVE_DEBUG_ERROR(...) ((void)0)

#endif

#else
/* else map to no op if DEBUG_LEVEL is not defined */
#define EVE_DEBUG_ERROR(...) ((void)0)

#endif /* defined(DEBUG_LEVEL) */

/* DEBUG_LEVEL>0 */
#if defined(DEBUG_LEVEL) && (DEBUG_LEVEL > 0)
/* select from supported platforms */
#if defined(PLATFORM_RASPBERRYPI) || \
    defined(USE_LINUX_SPI_DEV) || \
    defined(USE_MPSSE) || \
    defined(USE_FT4222) || \
    defined(PLATFORM_EMULATOR) || \
    defined(PLATFORM_RP2040)

#include <stdio.h>
#define EVE_DEBUG_PRINTF(...) printf(__VA_ARGS__)

#elif defined(PLATFORM_ESP32)
#include "esp_log.h"
#define EVE_DEBUG_PRINTF(...) ESP_LOGI(__FUNCTION__, __VA_ARGS__)

#else
/* else map to no op if no platfrom support */
#define EVE_DEBUG_PRINTF(...) ((void)0)

#endif

#else
/* else map to no op if DEBUG_LEVEL is not defined */
#define EVE_DEBUG_PRINTF(...) ((void)0)

#endif /* defined(DEBUG_LEVEL) && (DEBUG_LEVEL > 0) */

/* EVE DEBUG END */

#endif	/* EVE_DEBUG_H_ */
