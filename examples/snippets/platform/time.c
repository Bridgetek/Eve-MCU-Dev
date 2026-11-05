/**
 * @file time.c
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

#include <stdint.h>

#if defined(_WIN32) || defined(linux)

#include <time.h>
#ifndef _MSC_VER
#include <sys/time.h>
#endif

/** @brief Functions used to get platform time
 */
//@{
typedef struct timespec platform_time_t;

#ifdef _MSC_VER
// Code only needed for Windows MSVC compilations
// This will make a glock_gettime function similar enough to POSIX.
#include <windows.h>
#include <winnt.h>

#define CLOCK_MONOTONIC 0
#define MS_PER_SEC      1000ULL     // MS = milliseconds
#define US_PER_MS       1000ULL     // US = microseconds
#define HNS_PER_US      10ULL       // HNS = hundred-nanoseconds (e.g., 1 hns = 100 ns)
#define NS_PER_US       1000ULL

#define HNS_PER_SEC     (MS_PER_SEC * US_PER_MS * HNS_PER_US)
#define NS_PER_HNS      (100ULL)    // NS = nanoseconds
#define NS_PER_SEC      (MS_PER_SEC * US_PER_MS * NS_PER_US)

static int clock_gettime(int clockname, struct timespec* tv)
{
    static LARGE_INTEGER ticksPerSec;
    LARGE_INTEGER ticks;

    (void)clockname;

    if (!ticksPerSec.QuadPart) 
    {
        QueryPerformanceFrequency(&ticksPerSec);
        if (!ticksPerSec.QuadPart) 
        {
            errno = ENOTSUP;
            return -1;
        }
    }

    QueryPerformanceCounter(&ticks);

    tv->tv_sec = (long)(ticks.QuadPart / ticksPerSec.QuadPart);
    tv->tv_nsec = (long)(((ticks.QuadPart % ticksPerSec.QuadPart) * NS_PER_SEC) / ticksPerSec.QuadPart);

    return 0;
}
#endif

static void get_platform_time(platform_time_t *tm)
{
    clock_gettime(CLOCK_MONOTONIC, tm);
}

static uint32_t diff_platform_time(platform_time_t *start, platform_time_t *end)
{
    uint32_t diff;
    diff = (uint32_t)((end->tv_sec - start->tv_sec) * 1000 + (end->tv_nsec - start->tv_nsec) / 1000000);
    return diff;
}

uint32_t platform_get_time(void)
{
    uint32_t time_ms;
    platform_time_t now;
    get_platform_time(&now);
    time_ms = (uint32_t)now.tv_sec * 1000 + (uint32_t)now.tv_nsec / 1000000;
    return time_ms;
}

#elif defined(ARDUINO)

#include <Arduino.h>

uint32_t platform_get_time(void)
{
    return millis();
}

#elif defined(PLATFORM_PICO_SDK)

#include <pico/time.h>

uint32_t platform_get_time(void)
{
    return to_ms_since_boot(get_absolute_time());
}

#elif defined(PLATFORM_ESP32)

#include "esp_timer.h"

uint32_t platform_get_time(void)
{
    return esp_timer_get_time() / 1000;
}

#else

// Unsupported time on platform

uint32_t platform_get_time(void)
{
    return 0;
}

#endif