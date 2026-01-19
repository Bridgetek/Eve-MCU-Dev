/**
    @file eve_example.h
 **/
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

#ifndef _EVE_EXAMPLE_H
#define _EVE_EXAMPLE_H

#include <stdint.h>



#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "patch_textscale.h"

/* Functions called from eve_example code to platform specific code */
uint32_t platform_get_time(void);

/* Entry point to the example code */
void eve_example(const char *assets);

// ---- Method for storing assets ----

// Use Flash storage to keep preconverted assets
#define USE_FLASH 1
// Use local file system (on host device) to keep preconverted assets
#define USE_FLASHIMAGE 2
// Use C arrays to store PNG files and raw assets
#define USE_C_ARRAYS 3
// Use local file system to store PNG files and raw assets
// (Convert to EVE assets as required)
#define USE_FILES 4

// Choose the methods for storing assets
#ifndef ASSETS
#define ASSETS USE_FLASHIMAGE
#endif

// Validate asset storage method is appropriate
// Asset storage options which require a filesystem use the stdio library
#if (ASSETS == USE_FLASHIMAGE) || (ASSETS == USE_FILES)
// If the target platform does not support stdio 
// then this line will result in a compilation error
#include <stdio.h>
#endif

/* Array containing the bitmap sizes of ROM fonts. */
uint8_t eve_romfont_width(uint8_t font);
uint8_t eve_romfont_height(uint8_t font);
void eve_set_romfont_width(uint8_t font, uint8_t w);
void eve_set_romfont_height(uint8_t font, uint8_t h);
// Asset storage information
typedef struct EVE_ASSET_PROPS_s
{
    uint8_t Handle;
    uint8_t Cell;
#if ((ASSETS == USE_FLASH) || (ASSETS == USE_FLASHIMAGE))
    uint32_t Flash_Start;
    uint32_t Flash_Size;
#elif (ASSETS == USE_C_ARRAYS) 
    const uint8_t * Array_Ptr;
    uint32_t Array_Size;
#elif (ASSETS == USE_FILES)
    const char *filename;
#endif
    uint32_t RAM_G_Start;
    uint32_t RAM_G_EndAddr;
    uint32_t Format;
    uint16_t Width;
    uint16_t Height;
    uint16_t CellHeight;
} EVE_ASSET_PROPS;

extern EVE_ASSET_PROPS patch_asset; 
extern EVE_ASSET_PROPS Carbon_Fiber_800x480_asset; 
extern EVE_ASSET_PROPS LED_32x32_asset;  
extern EVE_ASSET_PROPS Trackmap_96x96_asset;
extern EVE_ASSET_PROPS Arrows_96x192_asset;
extern EVE_ASSET_PROPS Widget_Gear_152x152_asset;
extern EVE_ASSET_PROPS Widget_RPM_152x56_asset;
extern EVE_ASSET_PROPS Widget_Speed_104x56_asset;
extern EVE_ASSET_PROPS Bottom_Bar_800x8_asset;
extern EVE_ASSET_PROPS Car_Overhead_44x80_asset;
extern EVE_ASSET_PROPS Battery_Cells_40x1440_asset;
extern EVE_ASSET_PROPS eurostile_150_L8;

/* Asset loading function depends on the storage method in ASSETS macro */
void eve_asset_properties(const char *assets);
void eve_asset_load(EVE_ASSET_PROPS *asset, uint32_t loadimage);
int eve_loadpatch_impl(void);

#if ASSETS == USE_FLASH
void eve_flash_full_speed(void);
#endif

#include "touch.h"
#include "maths/trig_furman.h"

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* _EVE_EXAMPLE_H */
