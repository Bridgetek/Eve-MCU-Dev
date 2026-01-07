/**
  @file main.c
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
 * appliance. There are exclusions of FTDI liability for certain types of loss
 * such as: special loss or damage; incidental loss or damage; indirect or
 * consequential loss or damage; loss of income; loss of business; loss of
 * profits; loss of revenue; loss of contracts; business interruption; loss of
 * the use of money or anticipated savings; loss of information; loss of
 * opportunity; loss of goodwill or reputation; and/or loss of, damage to or
 * corruption of data.
 * There is a monetary cap on FTDI's liability.
 * The Software may have subsequently been amended by another user and then
 * distributed by that other user ("Adapted Software").  If so that user may
 * have additional licence terms that apply to those amendments. However, FTDI
 * has no liability in relation to those amendments.
 * ============================================================================
 */

/* INCLUDES ************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#ifndef _WIN32
#include <sys/time.h>
#endif
#if defined(PLATFORM_RP2040)
#include <pico/time.h>
#endif

#include <EVE.h>
typedef
#if defined(PLATFORM_RP2040)
    absolute_time_t
#else
    struct timespec
#endif
    platform_time_t;

#include "eve_example.h"

/* CONSTANTS ***********************************************************************/

/**
 * @brief Filename to store touchscreen configuration.
 */
const char *config_file = "config.bin";

/* LOCAL FUNCTIONS / INLINES *******************************************************/

void setup(void);

/* FUNCTIONS ***********************************************************************/

/** @brief Portable version of fopen/fopen_s
 */
FILE *port_fopen(char const * _FileName, char const * _Mode)
{
#if defined(_MSC_VER)
    FILE *h1;
    errno_t err;
    err = fopen_s(&h1, _FileName, _Mode);
    if (err)
    {
        return NULL;
    }
    return h1;
#else
    return fopen(_FileName, _Mode);
#endif
}

/**
 * @brief Functions used to store calibration data in file.
 */
//@{
int8_t platform_calib_init(void)
{
    return 1;
}

int8_t platform_calib_write(struct touchscreen_calibration *calib)
{
    FILE *h1 = port_fopen( config_file, "w");
    if( !h1 )
    {
        return -1;
    }

    fwrite(calib, sizeof(struct touchscreen_calibration), 1, h1);
    fclose(h1);
    return 0;
}

int8_t platform_calib_read(struct touchscreen_calibration *calib)
{
    FILE *h1 = port_fopen( config_file, "r");
    if( !h1 )
    {
        return -1;
    }
    fread(calib, sizeof(struct touchscreen_calibration), 1, h1);
    fclose(h1);
    return 0;
}
//@}

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
#if defined(PLATFORM_RP2040)
    *tm = get_absolute_time();
#else
    clock_gettime(CLOCK_MONOTONIC, tm);
#endif
}

static uint32_t diff_platform_time(platform_time_t *start, platform_time_t *end)
{
    uint32_t diff;
#if defined(PLATFORM_RP2040)
    diff = (absolute_time_diff_us(*start, *end) / 1000);
#else
    diff = (uint32_t)((end->tv_sec - start->tv_sec) * 1000 + (end->tv_nsec - start->tv_nsec) / 1000000);
#endif
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
//@}

int main(int argc, char **argv)
{
    char *assets = NULL;

    /* Setup UART */
    setup();

    if (argc > 1)
    {
        assets = argv[1];
    }

    /* Start example code */
    eve_example(assets);
}

void setup(void)
{
    /* Print out a welcome message... */
    printf ("(C) Copyright, Bridgetek Pte. Ltd. \r\n \r\n");
    printf ("---------------------------------------------------------------- \r\n");
    printf ("Welcome to BRT_AN_025 Example for Windows FT4222 Library\r\n");
    printf ("\n");

}
