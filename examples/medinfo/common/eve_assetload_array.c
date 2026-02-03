/**
 @file eve_assetload_array.c
 */
/*
 * ============================================================================
 * (C) Copyright,  Bridgetek Pte. Ltd.
 * ============================================================================
 *
 * This source code ("the Software") is provided by Bridgetek Pte Ltd
 * ("Bridgetek") subject to the licence terms set out
 * https://brtchip.com/wp-content/uploads/2021/11/BRT_Software_License_Agreement.pdf ("the Licence Terms").
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

#include <EVE.h>
#include <MCU.h> // For DEBUG_PRINTF only

#include "eve_example.h"

#if ASSETS == USE_C_ARRAYS

// Patch
extern const uint8_t patch_medinfo2[];
extern const uint32_t patch_medinfo2_size;

// Icons (200x200)
extern const uint8_t play_circle[];
extern const uint32_t play_circle_size;
extern const uint8_t pause_circle[];
extern const uint32_t pause_circle_size;
extern const uint8_t ecg_heart[];
extern const uint32_t ecg_heart_size;
extern const uint8_t silence[];
extern const uint32_t silence_size;

// Icons (100x100)
extern const uint8_t lock[];
extern const uint32_t lock_size;
extern const uint8_t lock_open_right[];
extern const uint32_t lock_open_right_size;
extern const uint8_t volume_off[];
extern const uint32_t volume_off_size;
extern const uint8_t volume_down[];
extern const uint32_t volume_down_size;
extern const uint8_t volume_up[];
extern const uint32_t volume_up_size;
extern const uint8_t good[];
extern const uint32_t good_size;
extern const uint8_t bad[];
extern const uint32_t bad_size;
extern const uint8_t sick[];
extern const uint32_t sick_size;
extern const uint8_t off_on[];
extern const uint32_t off_on_size;

// Patches only apply to BT82x
#if IS_EVE_API(5)

int eve_loadpatch_impl(void)
{
    uint32_t total = 0;
    uint32_t chunk;
    const uint8_t *ptr = patch_asset.Array_Ptr;

    EVE_LIB_BeginCoProList();
    EVE_CMD_LOADPATCH(0);
    do {
        chunk = (size_t)(patch_asset.Array_Size - total);
        if (chunk > 256) 
        {
            chunk = 256;
        }
        EVE_LIB_WriteDataToCMD(ptr, chunk);
        total += (uint32_t)chunk;
        ptr += (uint32_t)chunk;
    } while (chunk == 256);
    EVE_CMD_SWAP();
    EVE_LIB_EndCoProList();
    EVE_LIB_AwaitCoProEmpty();

    return 0;
}

#endif

void eve_asset_load(EVE_ASSET_PROPS *asset, uint32_t loadimage)
{
    const uint8_t *ptr = asset->Array_Ptr;
    uint32_t total = 0;
    uint32_t chunk;

    // Read directy from flash
    if (loadimage)
    {
        DEBUG_PRINTF("Load image from array\n");

        EVE_LIB_BeginCoProList();
        EVE_CMD_LOADIMAGE(asset->RAM_G_Start, 0);
        do {
            chunk = (size_t)(asset->Array_Size - total);
            if (chunk > 256) 
            {
                chunk = 256;
            }
            EVE_LIB_WriteDataToCMD(ptr, chunk);
            total += (uint32_t)chunk;
            ptr += (uint32_t)chunk;
        } while (chunk == 256);
        EVE_LIB_EndCoProList();
        EVE_LIB_AwaitCoProEmpty();
        uint32_t dummy;
        uint32_t w, h;
#if IS_EVE_API(3)
        EVE_LIB_GetProps(&dummy, &w, &h);
        // EVE3 does not support CMD_GETPTR on CMD_LOADIMAGE
        asset->RAM_G_EndAddr = asset->RAM_G_Start + (w * h * 2);
        // EVE3 does not support CMD_GETIMAGE for CMD_LOADIMAGE results
        uint32_t filetype = MCU_be32toh(*(uint32_t*)(asset->Array_Ptr));
        if (filetype == 0x89504e47)
        {
            // Array holds a PNG file
            asset->Format = EVE_FORMAT_ARGB1555;
            DEBUG_PRINTF("PNG file\n");
        }
        else if (filetype == 0xffd8ff0e)
        {
            // Array holds a JPG file
            asset->Format = EVE_FORMAT_RGB565;
            DEBUG_PRINTF("JPG file\n");
        }
        else
        {
            DEBUG_PRINTF("Unknown file type\n");
        }
#elif IS_EVE_API(4,5)
        // EVE4 and EVE5 support CMD_GETPTR on CMD_LOADIMAGE
        EVE_LIB_GetPtr(&asset->RAM_G_EndAddr);
        // EVE4 and EVE5 support CMD_GETIMAGE for CMD_LOADIMAGE results
        EVE_LIB_GetImage(&dummy, &asset->Format, &w, &h, &dummy);
#endif
        DEBUG_PRINTF("Image: w %d h %d fmt %d\n", w, h, asset->Format);

        if ((asset->Width != w) || (asset->Height != h))
        {
            DEBUG_PRINTF("MISMATCH image size must match: width %d = %d; height %d = %d\n", asset->Width, w, asset->Height, h);
        }
    }
    else
    {
        DEBUG_PRINTF("Load to memory from array\n");

        EVE_LIB_BeginCoProList();
        EVE_CMD_MEMWRITE(asset->RAM_G_Start, asset->Array_Size);
        do {
            chunk = (size_t)(asset->Array_Size - total);
            if (chunk > 256) 
            {
                chunk = 256;
            }
            EVE_LIB_WriteDataToCMD(ptr, chunk);
            total += (uint32_t)chunk;
            ptr += (uint32_t)chunk;
        } while (chunk == 256);
        EVE_LIB_EndCoProList();
        EVE_LIB_AwaitCoProEmpty();
        
        asset->RAM_G_EndAddr = asset->RAM_G_Start + total;
    }

    DEBUG_PRINTF("RAM_G: %d -> %d\n", asset->RAM_G_Start, asset->RAM_G_EndAddr);
}

void eve_asset_properties(const char *assets)
{
    (void)assets;

    // Patch
    patch_asset.Array_Ptr = patch_medinfo2;
    patch_asset.Array_Size = patch_medinfo2_size;

    // Icons (200x200)
    icon_assets_large[icon_play].Array_Ptr = play_circle;
    icon_assets_large[icon_play].Array_Size = play_circle_size;
    icon_assets_large[icon_pause].Array_Ptr = pause_circle;
    icon_assets_large[icon_pause].Array_Size = pause_circle_size;
    icon_assets_large[icon_pulse].Array_Ptr = ecg_heart;
    icon_assets_large[icon_pulse].Array_Size = ecg_heart_size;
    icon_assets_large[icon_silence].Array_Ptr = silence;
    icon_assets_large[icon_silence].Array_Size = silence_size;

    // Icons (100x100)
    icon_assets_small[icon_lock].Array_Ptr = lock;
    icon_assets_small[icon_lock].Array_Size = lock_size;
    icon_assets_small[icon_lock_open].Array_Ptr = lock_open_right;
    icon_assets_small[icon_lock_open].Array_Size = lock_open_right_size;
    icon_assets_small[icon_volume_off].Array_Ptr = volume_off;
    icon_assets_small[icon_volume_off].Array_Size = volume_off_size;
    icon_assets_small[icon_volume_down].Array_Ptr = volume_down;
    icon_assets_small[icon_volume_down].Array_Size = volume_down_size;
    icon_assets_small[icon_volume_up].Array_Ptr = volume_up;
    icon_assets_small[icon_volume_up].Array_Size = volume_up_size;
    icon_assets_small[icon_good].Array_Ptr = good;
    icon_assets_small[icon_good].Array_Size = good_size;
    icon_assets_small[icon_bad].Array_Ptr = sick;
    icon_assets_small[icon_bad].Array_Size = sick_size;
    icon_assets_small[icon_sick].Array_Ptr = sick;
    icon_assets_small[icon_sick].Array_Size = sick_size;
    icon_assets_small[icon_off_on].Array_Ptr = off_on;
    icon_assets_small[icon_off_on].Array_Size = off_on_size;
}

#endif // ASSETS == USE_C_ARRAYS
