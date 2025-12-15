/**
 @file eve_assetload_flash_eve3.c
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
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include <EVE.h>
#include <MCU.h> // For DEBUG_PRINTF only

#include "eve_example.h"

#if IS_EVE_API(5)

#if (ASSETS == USE_FLASH) || (ASSETS == USE_FLASHIMAGE)

#if ASSETS == USE_FLASHIMAGE
extern const char *flashimage;
extern const char *assetdir;
extern const char *join(const char *dirname, const char *filename);
#endif

void eve_asset_properties(const char *assets)
{
#if ASSETS == USE_FLASHIMAGE
    // NOTE: Path is relative to the runtime directory of the executable
    if (assets)
    {
        assetdir = assets;
    }
    else
    {
        assetdir = "";
    }
    flashimage = join(assetdir, "flash-820-default.bin");

    (void)assets;
#endif

    // Patch
    patch_asset.Flash_Start = /* offset patch_textscale.bin */ 0   ;
    patch_asset.Flash_Size = /* size patch_textscale.bin */ 2424   ;

    // Images
    Carbon_Fiber_800x480_asset.Flash_Start = /* offset Carbon_Fiber_800x480_ASTC_8X8.raw */ 2432   ;
    Carbon_Fiber_800x480_asset.Flash_Size = /* size Carbon_Fiber_800x480_ASTC_8X8.raw */ 96000   ;
    LED_32x32_asset.Flash_Start = /* offset LED_32x32_ASTC_4X4.raw */ 107648   ;
    LED_32x32_asset.Flash_Size = /* size LED_32x32_ASTC_4X4.raw */ 1024   ;
    Trackmap_96x96_asset.Flash_Start = /* offset trackmap_96x96_ASTC_4X4.raw */ 98432   ;
    Trackmap_96x96_asset.Flash_Size = /* size trackmap_96x96_ASTC_4X4.raw */ 9216   ;
    Arrows_96x192_asset.Flash_Start = /* offset arrows_96x192_ASTC_4X4.raw */ 146112  ;
    Arrows_96x192_asset.Flash_Size = /* size arrows_96x192_ASTC_4X4.raw */ 18432  ;
    Widget_Gear_152x152_asset.Flash_Start = /* offset Widget_Gear_152x152_ASTC_4X4.raw */ 108672   ;
    Widget_Gear_152x152_asset.Flash_Size = /* size Widget_Gear_152x152_ASTC_4X4.raw */ 23104   ;
    Widget_RPM_152x56_asset.Flash_Start = /* offset Widget_RPM_152x56_ASTC_4X4.raw */ 131776   ;
    Widget_RPM_152x56_asset.Flash_Size = /* size Widget_RPM_152x56_ASTC_4X4.raw */ 8512   ;
    Widget_Speed_104x56_asset.Flash_Start = /* offset Widget_Speed_104x56_ASTC_4X4.raw */ 140288   ;
    Widget_Speed_104x56_asset.Flash_Size = /* size Widget_Speed_104x56_ASTC_4X4.raw */ 5824   ;
    Bottom_Bar_800x8_asset.Flash_Start = /* offset Bottom_Bar_800x8_ASTC_4X4.raw */ 222144   ;
    Bottom_Bar_800x8_asset.Flash_Size = /* size Bottom_Bar_800x8_ASTC_4X4.raw */ 6400   ;
    Car_Overhead_44x80_asset.Flash_Start = /* offset Car_Overhead_44x80_ASTC_4X4.raw */ 228544   ;
    Car_Overhead_44x80_asset.Flash_Size = /* size Car_Overhead_44x80_ASTC_4X4.raw */ 3520   ;
    Battery_Cells_40x1440_asset.Flash_Start = /* offset Battery_Cells_40x1440_ASTC_4X4.raw */ 164544   ;
    Battery_Cells_40x1440_asset.Flash_Size = /* size Battery_Cells_40x1440_ASTC_4X4.raw */ 57600   ;

    // Fonts
    eurostile_150_L8.Flash_Start = /* offset eurostile_150_L8.raw */ 232064   ;
    eurostile_150_L8.Flash_Size = /* size eurostile_150_L8.raw */ 93988   ;

}

#endif

#endif
