/**
 @file eve_assetload_file.c
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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <EVE.h>
#include <MCU.h> // For DEBUG_PRINTF only

#include "eve_example.h"

#if ASSETS == USE_FILES

/** @brief Portable version of fopen/fopen_s
 *  @details This is provided in the main.c file if the platform is able to use
 *           a file system for accessing assets.
 */
extern FILE *port_fopen(char const * _FileName, char const * _Mode);

static const char *assetdir;

const char *join(const char *dirname, const char *filename)
{
    size_t joinedlen = strlen(dirname) + strlen(filename) + 16;
    char *sep = "";
    char *joined = (char *)malloc(joinedlen);
    
    if (!joined)
    {
        DEBUG_PRINTF("Failed to malloc %zu chars for patch path \"%s\" and \"%s\"\n", joinedlen, dirname, filename);
        exit(-3);
    }
    
    if (strlen(dirname))
    {
        sep = "/";
    }
    sprintf_s(joined, joinedlen, "%s%s%s", dirname, sep, filename);

    return joined;
}

// Patches only apply to BT82x
#if IS_EVE_API(5)

int eve_loadpatch_impl(void)
{
    FILE *h1;
    uint8_t buffer[256];
    uint32_t total = 0;
    uint32_t chunk;
    size_t xfer;

    h1 = port_fopen(patch_asset.filename, "rb");
    if (h1)
    {
        EVE_LIB_BeginCoProList();
        EVE_CMD_LOADPATCH(0);
        do {
            chunk = sizeof(buffer);
            xfer = fread(buffer, 1, (size_t)chunk, h1);
            if (xfer > 0)
            {
                EVE_LIB_WriteDataToCMD(buffer, chunk);
                total += (uint32_t)xfer;
            }
        } while (xfer == sizeof(buffer));
        fclose(h1); 
        EVE_CMD_SWAP();
        EVE_LIB_EndCoProList();
        EVE_LIB_AwaitCoProEmpty();
    }
    else
    {
        DEBUG_PRINTF("Patch file error\n");
        exit(-1);
    }

    return 0;
}

#endif

void eve_asset_load(EVE_ASSET_PROPS *asset, uint32_t loadimage)
{
    FILE *h1;
    uint8_t buffer[512];
    uint32_t total = 0;
    uint32_t chunk;
    size_t xfer;
    char *ext;
    
    ext = strrchr(asset->filename, '.');
    if (ext)
    {
        if (strncmp(ext, ".png", 4) == 0)
        {
            // Default load format of PNG files
            loadimage = EVE_FORMAT_ARGB4;
        }
        if (strncmp(ext, ".jpg", 4) == 0)
        {
            // Default load format of JPG files
            loadimage = EVE_FORMAT_RGB565;
        }
    }
    
    h1 = port_fopen(asset->filename, "rb");
    if (h1)
    {
        size_t sz;
        fseek(h1, 0L, SEEK_END);
        sz = ftell(h1);
        fseek(h1, 0L, SEEK_SET);
        
        // Decode image
        if (loadimage)
        {
            DEBUG_PRINTF("Load image %s\n", asset->filename);

            EVE_LIB_BeginCoProList();
            EVE_CMD_LOADIMAGE(asset->RAM_G_Start, 0);

            do {
                chunk = sizeof(buffer);
                xfer = fread(buffer, 1, (size_t)chunk, h1);
                if (xfer > 0)
                {
                    EVE_LIB_WriteDataToCMD(buffer, (uint32_t)xfer);
                }
                total += (uint32_t)xfer;
            } while (xfer == sizeof(buffer));
            EVE_LIB_EndCoProList();
            EVE_LIB_AwaitCoProEmpty();

            uint32_t dummy;
            uint32_t w, h;
#if IS_EVE_API(3)
            EVE_LIB_GetProps(&dummy, &w, &h);
            // EVE3 does not support CMD_GETPTR on CMD_LOADIMAGE
            asset->RAM_G_EndAddr = asset->RAM_G_Start + (w * h * 2);
            // EVE3 does not support CMD_GETIMAGE for CMD_LOADIMAGE results
            asset->Format = (uint32_t)loadimage;
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
            DEBUG_PRINTF("Load to memory %s\n", asset->filename);
    
            EVE_LIB_BeginCoProList();
            EVE_CMD_MEMWRITE(asset->RAM_G_Start, (uint32_t)sz);

            do {
                chunk = sizeof(buffer);
                xfer = fread(buffer, 1, (size_t)chunk, h1);
                if (xfer > 0)
                {
                    EVE_LIB_WriteDataToCMD(buffer, (uint32_t)xfer);
                }
                total += (uint32_t)xfer;
            } while (xfer == sizeof(buffer));
            EVE_LIB_EndCoProList();
            EVE_LIB_AwaitCoProEmpty();
            
            asset->Format = EVE_FORMAT_BARGRAPH;
            asset->RAM_G_EndAddr = asset->RAM_G_Start + total;
        }

        fclose(h1); 

        DEBUG_PRINTF("RAM_G: %d -> %d\n", asset->RAM_G_Start,  asset->RAM_G_EndAddr);
    }
    else
    {
        DEBUG_PRINTF("RAM_G: file error %s\n", asset->filename);
        exit(-1);
    }
}

void eve_asset_properties(const char *assets)
{
    // NOTE: Paths are relative to the runtime directory of the executable
    if (assets)
    {
        assetdir = assets;
    }
    else
    {
        assetdir = "";
    }

    // Patch
    patch_asset.filename = join(assetdir, "patch_textscale.bin");

    // Images
    Carbon_Fiber_800x480_asset.filename = join(assetdir, "Carbon_Fiber_800x480.png");
    LED_32x32_asset.filename = join(assetdir, "LED_32x32.png");
    Trackmap_96x96_asset.filename = join(assetdir, "trackmap_96x96.png");
    Arrows_96x192_asset.filename = join(assetdir, "arrows_96x192.png");
    Widget_Gear_152x152_asset.filename = join(assetdir, "Widget_Gear_152x152.png");
    Widget_RPM_152x56_asset.filename = join(assetdir, "Widget_RPM_152x56.png");
    Widget_Speed_104x56_asset.filename = join(assetdir, "Widget_Speed_104x56.png");
    Bottom_Bar_800x8_asset.filename = join(assetdir, "Bottom_Bar_800x8.png");
    Car_Overhead_44x80_asset.filename = join(assetdir, "Car_Overhead_44x80.png");
    Battery_Cells_40x1440_asset.filename = join(assetdir, "Battery_Cells_40x1440.png");

    // Font
    eurostile_150_L8.filename = join(assetdir, "eurostile_150_L8.raw");
}

#endif
