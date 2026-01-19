/**
 @file trig_furman.c
 */
/*
 * ============================================================================
 * (C) Copyright,  Bridgetek Pte. Ltd.
 * ============================================================================
 *
 * This source code ("the Software") is provided by Bridgetek Pte Ltd
 * ("Bridgetek") subject to the licence terms set out
 * http://www.ftdichip.com/FTSourceCodeLicenceTerms.htm ("the Licence Terms").
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

#include "trig_furman.h"

#ifndef USE_TRIG_TABLE

// Use compiler maths functions
#include <math.h>
// M_PI is not guaranteed to be defined in math.h
#ifndef M_PI
#define M_PI       3.14159265358979323846   // pi
#endif

int16_t sin_furman(uint16_t furman16)
{
    double angle = furman16 * M_PI / 0x8000;
    int16_t ret = (int16_t)(sin(angle) * 0x7fff);
    return ret;
}

int16_t cos_furman(uint16_t furman16)
{
    double angle = furman16 * M_PI / 0x8000;
    int16_t ret = (int16_t)(cos(angle) * 0x7fff);
    return ret;
}

#else

/* Use precomputed sine table for calculation of trigonometrics
 * This uses a 64 furman resolution using 257 position lookup table for 
 * the first quadrant of the circle. Results for the other 3 quadrants
 * are mirror or negaives of these values.
 */
uint16_t sinint[257] = { 
    0, 402, 804, 1206, 1608, 2010, 2412, 2813, 3215, 3617, 4018, 4419, 4821, 
    5221, 5622, 6023, 6423, 6823, 7223, 7622, 8022, 8421, 8819, 9218, 9615, 
    10013, 10410, 10807, 11203, 11599, 11995, 12390, 12785, 13179, 13573, 
    13966, 14358, 14750, 15142, 15533, 15923, 16313, 16702, 17091, 17479, 
    17866, 18252, 18638, 19023, 19408, 19791, 20174, 20557, 20938, 21319, 
    21699, 22078, 22456, 22833, 23210, 23585, 23960, 24334, 24707, 25079, 
    25450, 25820, 26189, 26557, 26924, 27290, 27655, 28019, 28382, 28744, 
    29105, 29465, 29823, 30181, 30537, 30892, 31247, 31599, 31951, 32302, 
    32651, 32999, 33346, 33691, 34035, 34378, 34720, 35061, 35400, 35737, 
    36074, 36409, 36742, 37075, 37406, 37735, 38063, 38390, 38715, 39039, 
    39361, 39682, 40001, 40319, 40635, 40950, 41263, 41574, 41885, 42193, 
    42500, 42805, 43109, 43411, 43711, 44010, 44307, 44603, 44896, 45189, 
    45479, 45768, 46055, 46340, 46623, 46905, 47185, 47463, 47739, 48014, 
    48287, 48558, 48827, 49094, 49360, 49623, 49885, 50145, 50403, 50659, 
    50913, 51165, 51415, 51664, 51910, 52155, 52397, 52638, 52876, 53113, 
    53347, 53580, 53810, 54039, 54265, 54490, 54712, 54933, 55151, 55367, 
    55581, 55793, 56003, 56211, 56416, 56620, 56821, 57021, 57218, 57413, 
    57606, 57796, 57985, 58171, 58355, 58537, 58717, 58894, 59069, 59242, 
    59413, 59582, 59748, 59912, 60074, 60234, 60391, 60546, 60699, 60849, 
    60997, 61143, 61287, 61428, 61567, 61704, 61838, 61970, 62100, 62227, 
    62352, 62474, 62595, 62713, 62828, 62941, 63052, 63161, 63267, 63370, 
    63472, 63570, 63667, 63761, 63853, 63942, 64029, 64114, 64196, 64275, 
    64353, 64427, 64500, 64570, 64637, 64702, 64765, 64825, 64883, 64938, 
    64991, 65042, 65090, 65135, 65178, 65219, 65257, 65293, 65326, 65357, 
    65385, 65411, 65435, 65456, 65474, 65490, 65504, 65515, 65523, 65530, 
    65533, 65535 }; 

/* 
 * Integer sine function 
 * Takes an input of 16-bit furman value. 
 * Returns the sine (scaled from -0x7fff to + 0x7fff as int16_t).
 */
static int16_t sin_furman_lookup(uint16_t furman16)
{
    int16_t ret = 0;

    if (furman16 <= 0x4000)
    {
        ret = sinint[(furman16) * 256 / 0x4000] / 2;
    }
    else if (furman16 < 0x8000)
    {
        ret = sinint[((0x8000 - furman16) * 256) / 0x4000] / 2;
    }
    else if (furman16 < 0xc000)
    {
        ret = - (sinint[((furman16 - 0x8000) * 256) / 0x4000] / 2);
    }
    else
    {
        ret = - (sinint[((0x10000 - (uint32_t)furman16) * 256) / 0x4000] / 2);
    }

    return ret;
}

int16_t sin_furman(uint16_t furman16)
{
    int16_t ret;
    // There are 4 * 256 discrete intervals in the sin_furman lookup
    // Interpolate between the previous and next point
    uint16_t furman0 = furman16 & (~0x3f);
    uint16_t furman1 = furman0 + 0x40;
    int16_t r0 = sin_furman_lookup(furman0);
    int16_t r1 = sin_furman_lookup(furman1);
    ret = r0 + ((r1 -r0) * (furman16 & 0x3f) / 0x40);
    return ret;
}

int16_t cos_furman(uint16_t furman16)
{
    return sin_furman(furman16 + 0x4000);
}

#endif
