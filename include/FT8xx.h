/**
 @file FT8xx.h
 */
/*
 * ============================================================================
 * History
 * =======
 * Nov 2019        Initial beta for FT81x and FT80x
 * Mar 2020        Updated beta - added BT815/6 commands
 * Mar 2021        Beta with BT817/8 support added
 * Nov 2023        Added BT88x definitions, added CMD_FLASHPROGRAM
 * Dec 2024        Updates for BT82x and split into devices
 *
 *
 *
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

#ifndef _FT8XX_H_
#define    _FT8XX_H_

/* Definitions used for target device selection */
#define FT800 800
#define FT801 801
#define FT810 810
#define FT811 811
#define FT812 812
#define FT813 813
#define BT880 880
#define BT881 881
#define BT882 882
#define BT883 883
#define BT815 815
#define BT816 816
#define BT817 817
#define BT818 818
#define BT820 820

/* For capacitive versions */
#define CTOUCH_MODE_COMPATIBILITY 1
#define CTOUCH_MODE_EXTENDED 0

#undef EVE_API
#undef EVE_SUB_API

#if (FT8XX_TYPE == FT800)
#define EVE_API 1

#elif (FT8XX_TYPE == FT801)
#define EVE_API 1

#elif (FT8XX_TYPE == FT810)
#define EVE_API 2
#define EVE_SUB_API 1

#elif (FT8XX_TYPE == FT811)
#define EVE_API 2
#define EVE_SUB_API 1

#elif (FT8XX_TYPE == FT812)
#define EVE_API 2
#define EVE_SUB_API 1

#elif (FT8XX_TYPE == FT813)
#define EVE_API 2
#define EVE_SUB_API 1

#elif (FT8XX_TYPE == BT880)
#define EVE_API 2
#define EVE_SUB_API 2

#elif (FT8XX_TYPE == BT881)
#define EVE_API 2
#define EVE_SUB_API 2

#elif (FT8XX_TYPE == BT882)
#define EVE_API 2
#define EVE_SUB_API 2

#elif (FT8XX_TYPE == BT883)
#define EVE_API 2
#define EVE_SUB_API 2

#elif (FT8XX_TYPE == BT815)
#define EVE_API 3

#elif (FT8XX_TYPE == BT816)
#define EVE_API 3

#elif (FT8XX_TYPE == BT817)
#define EVE_API 4

#elif (FT8XX_TYPE == BT818)
#define EVE_API 4

#elif (FT8XX_TYPE == BT820)
#define EVE_API 5

#else
#warning FT8XX_TYPE definition not recognised.
#endif

#undef EVE1_ENABLE // deprecated
#undef EVE2_ENABLE // deprecated
#undef EVE3_ENABLE // deprecated
#undef EVE4_ENABLE // deprecated
#undef EVE5_ENABLE // deprecated
#if EVE_API == 1
#define EVE1_ENABLE // deprecated
#elif EVE_API == 2
#define EVE2_ENABLE // deprecated
#elif EVE_API == 3
#define EVE3_ENABLE // deprecated
#elif EVE_API == 4
#define EVE4_ENABLE // deprecated
#elif EVE_API == 5
#define EVE5_ENABLE // deprecated
#endif

/** Macros to allow us to select which API a command applies to.
 * For APIs supported use the following:
 *  
 * #if IS_EVE_API(api1, api2, ...)
 * 
 * Where the APIs supported are in the macro parenthesis. The EVE 
 * API is set in the EVE_API macro.
 * So, to support BT815, BT817, BT820 this will need EVE APIs 3, 4, and 5.
 *
 * #if IS_EVE_API(3,4,5)
 * 
 * On Visual Studio it is necessary to enable the "/Zc:preprocessor" option
 * to enable preprocessor conformance mode.
 * This works by counting the number of parameters then calling a chain of
 * macros to make a chain of conditions for each parameters in the macros
 * IS_EVE_API_1/2/3/4/5. (This can be expanded to more than 5 in future)
 * (x == n)||(y == n)||...
 * The parameters are counted by pasting the passed parameters into the 
 * parameters passed from NUM_ARGS to _NUM_ARGS and selectng the Nth one,
 * which then becomes one of the numbers after __VA_ARGS__ in NUM_ARGS.
 * The first arguments in _NUM_ARGS are dummies.
 */
#define _NUM_ARGS(X,X5,X4,X3,X2,X1,N,...) N
#define NUM_ARGS(...) _NUM_ARGS(0, ## __VA_ARGS__ ,5,4,3,2,1,0)

#define IS_EVE_API_1(a)     (a == EVE_API)
#define IS_EVE_API_2(a,b)   (IS_EVE_API_1(a) || IS_EVE_API_1(b) )
#define IS_EVE_API_3(a,...) (IS_EVE_API_1(a) || IS_EVE_API_2( __VA_ARGS__) )
#define IS_EVE_API_4(a,...) (IS_EVE_API_1(a) || IS_EVE_API_3( __VA_ARGS__) )
#define IS_EVE_API_5(a,...) (IS_EVE_API_1(a) || IS_EVE_API_4( __VA_ARGS__) )

#define _IS_EVE_API_N(N, ...) IS_EVE_API_ ## N(__VA_ARGS__)
#define _IS_EVE_API(N, ...)  _IS_EVE_API_N(N, __VA_ARGS__)
#define IS_EVE_API(...)      _IS_EVE_API(NUM_ARGS(__VA_ARGS__), ## __VA_ARGS__)

/** EVE API definitions. */

#if IS_EVE_API(1)

//###########################################################################
//#####################################  EVE 1 ##############################
//###########################################################################

#include "FT80x.h"

#elif IS_EVE_API(2)

//###########################################################################
//#####################################  EVE 2 ##############################
//###########################################################################

#include "FT81x.h"

#elif IS_EVE_API(3, 4)

//###########################################################################
//###################################  EVE 3/4 ##############################
//###########################################################################

#include "BT81x.h"

#elif IS_EVE_API(5)

//###########################################################################
//#####################################  EVE 5 ##############################
//###########################################################################

#include "BT82x.h"

#else

#error No EVE support level selected. FT8XX_TYPE must be set to a supported device.

#endif

// Legacy Metric Block for Fonts

#define FT_GPU_NUMCHAR_PERFONT         (128)
#define FT_GPU_FONT_TABLE_SIZE         (148)

/* Font table structure */
/* On EVE4 onwards this is called the Legacy Font Structure. */
typedef struct
{
    /* Width of each character font from 0 to 127 */
    uint8_t    FontWidth[FT_GPU_NUMCHAR_PERFONT];
    /* Bitmap format of font - this is the same as the EVE_FORMAT_x, where x is L1, L2, L4 etc. */
    uint32_t    FontBitmapFormat;
    /* Font line stride in bytes */
    uint32_t    FontLineStride;
    /* Font width in pixels */
    uint32_t    FontWidthInPixels;
    /* Font height in pixels */
    uint32_t    FontHeightInPixels;
    /* Pointer to font graphics raw data */
    uint32_t    PointerToFontGraphicsData;
} EVE_GPU_FONT_HEADER;

#if IS_EVE_API(4, 5)

/* On EVE4 onwards this is called the Extended Font Structure. */
typedef struct
{
    /* Signature of font structure - must be 0x0100AAFF */
    uint32_t    Signature;
    /* Total size of font block in bytes */
    uint32_t    Size;
    /* Bitmap format of font - this is the same as the EVE_FORMAT_x, where x is L1, L2, L4 etc. */
    uint32_t    FontBitmapFormat;
    /* Bitmap swizzle - as used in BITMAP_SWIZZLE command */
    uint32_t    FontBitmapSwizzle;
    /* Font layout width in bytes */
    uint32_t    FontLayoutWidth;
    /* Font layout height */
    uint32_t    FontLayoutHeight;
    /* Font width in pixels */
    uint32_t    FontWidthInPixels;
    /* Font height in pixels */
    uint32_t    FontHeightInPixels;
    /* Pointer to font graphics raw data */
    uint32_t    PointerToFontGraphicsData;
    /* Number of characters */
    uint32_t    FontNumberCharacters;
} EVE_GPU_EXT_FONT_HEADER;

#endif

#if IS_EVE_API(5)

/* On EVE5 onwards this is called the Extended 2 Font Structure. */
typedef struct
{
    /* Signature of font structure - must be 0x0200AAFF */
    uint32_t    Signature;
    /* Total size of font block in bytes */
    uint32_t    Size;
    /* Bitmap format and flags of font */
    /* Format is the same as the EVE_FORMAT_x, where x is L1, L2, L4 etc. */
    /* Flags denotes binary and line break behaviour */
    uint32_t    FontBitmapFormat;
    /* Bitmap swizzle - as used in BITMAP_SWIZZLE command */
    uint32_t    FontBitmapSwizzle;
    /* Font layout width in bytes */
    uint32_t    FontLayoutWidth;
    /* Font layout height */
    uint32_t    FontLayoutHeight;
    /* Font width in pixels */
    uint32_t    FontWidthInPixels;
    /* Font height in pixels */
    uint32_t    FontHeightInPixels;
    /* Font padding and leading in pixels */
    uint32_t    FontPadLeadInPixels;
    /* Number of characters */
    uint32_t    FontNumberCharacters;
    /* Font midline and baseline in pixels */
    uint32_t    FontMidlineBaselineInPixels;
} EVE_GPU_EXT2_FONT_HEADER;

typedef struct
{
    /* Pointer to font graphics raw data */
    uint32_t    PointerToFontGraphicsData;
    /* Width of font character */
    uint32_t    Width;
} EVE_GPU_EXT2_CHAR_DESCRIPTOR;

#endif

#endif    /* _FT8XX_H_ */
