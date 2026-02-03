/**
 @file eve_assetload_flash.c
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
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include <EVE.h>
#include <MCU.h> // For DEBUG_PRINTF only

#include "eve_example.h"

#if (ASSETS == USE_FLASH) || (ASSETS == USE_FLASHIMAGE)

#if ASSETS == USE_FLASHIMAGE

const char *flashimage;
const char *assetdir;

/** @brief Portable version of fopen/fopen_s
 *  @details This is provided in the main.c file if the platform is able to use
 *           a file system for accessing assets.
 */
extern FILE *port_fopen(char const * _FileName, char const * _Mode);

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
#endif

// Code to set flash to full speed mode
void eve_flash_full_speed(void)
{
    uint8_t Flash_Status = 0;

#if IS_EVE_API(3,4)
    // Detach Flash
    EVE_LIB_BeginCoProList();
    EVE_CMD_FLASHDETATCH();
    EVE_LIB_EndCoProList();
    EVE_LIB_AwaitCoProEmpty();

    Flash_Status = EVE_LIB_MemRead8(EVE_REG_FLASH_STATUS);

    if (EVE_FLASH_STATUS_DETACHED != Flash_Status)
    {
        EVE_LIB_BeginCoProList();
        EVE_CMD_DLSTART();
        EVE_CLEAR_COLOR_RGB(255, 0, 0);
        EVE_CLEAR(1,1,1);
        EVE_COLOR_RGB(255,255,255);
        EVE_CMD_TEXT(100, 50, 28, 0, "Error detaching flash");
        EVE_CMD_TEXT(100,100, 28, EVE_OPT_FORMAT, "Mode is %d ", Flash_Status);
        EVE_DISPLAY();
        EVE_CMD_SWAP();
        EVE_LIB_EndCoProList();
        EVE_LIB_AwaitCoProEmpty();
        while(1)
        {
        }
    }

    // Attach
    EVE_LIB_BeginCoProList();
    EVE_CMD_FLASHATTACH();
    EVE_LIB_EndCoProList();
    EVE_LIB_AwaitCoProEmpty();

    Flash_Status = EVE_LIB_MemRead8(EVE_REG_FLASH_STATUS);

    if (EVE_FLASH_STATUS_BASIC != Flash_Status)
    {
        EVE_LIB_BeginCoProList();
        EVE_CMD_DLSTART();
        EVE_CLEAR_COLOR_RGB(0, 255, 0);
        EVE_CLEAR(1,1,1);
        EVE_COLOR_RGB(255,255,255);
        EVE_CMD_TEXT(100, 50, 28, 0, "Error attaching flash");
        EVE_CMD_TEXT(100,100, 28, EVE_OPT_FORMAT, "Mode is %d ", Flash_Status);
        EVE_DISPLAY();
        EVE_CMD_SWAP();
        EVE_LIB_EndCoProList();
        EVE_LIB_AwaitCoProEmpty();
        while(1)
        {
        }
    }

    // Fast mode
    EVE_LIB_BeginCoProList();
    EVE_CMD_FLASHFAST(0);
    EVE_LIB_EndCoProList();
    EVE_LIB_AwaitCoProEmpty();

    Flash_Status = EVE_LIB_MemRead8(EVE_REG_FLASH_STATUS);

    if (EVE_FLASH_STATUS_FULL != Flash_Status)
    {

        EVE_LIB_BeginCoProList();
        EVE_CMD_DLSTART();
        EVE_CLEAR(1,1,1);
        EVE_CLEAR_COLOR_RGB(0, 0, 255);
        EVE_COLOR_RGB(255,255,255);
        EVE_CMD_TEXT(100, 50, 28, 0, "Error going to full mode");
        EVE_CMD_TEXT(100,100, 28, EVE_OPT_FORMAT, "Mode is %d ", Flash_Status);
        EVE_DISPLAY();
        EVE_CMD_SWAP();
        EVE_LIB_EndCoProList();
        EVE_LIB_AwaitCoProEmpty();
        while(1)
        {
        }
    }
#endif
}

// Patches only apply to BT82x
#if IS_EVE_API(5)

int eve_loadpatch_impl(void)
{
#if ASSETS == USE_FLASH

    // Read directy from flash
    EVE_LIB_BeginCoProList();
    EVE_CMD_FLASHSOURCE(patch_asset.Flash_Start);
    EVE_CMD_LOADPATCH(EVE_OPT_FLASH);
    EVE_LIB_EndCoProList();
    EVE_LIB_AwaitCoProEmpty();
    
#elif ASSETS == USE_FLASHIMAGE

    FILE *h1;
    uint8_t buffer[256];
    uint32_t total = 0;
    uint32_t chunk;
    size_t xfer;
    
    h1 = port_fopen(flashimage, "rb");
    if (h1)
    {
        int res;

        EVE_LIB_BeginCoProList();
        EVE_CMD_LOADPATCH(0);
        res = fseek(h1, patch_asset.Flash_Start, SEEK_SET);
        if (res != 0)
        {
            DEBUG_PRINTF("seek error\n");
            return -1;
        }
        do {
            chunk = (size_t)(patch_asset.Flash_Size - total);
            if (chunk > sizeof(buffer)) 
            {
                chunk = sizeof(buffer);
            }
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
        DEBUG_PRINTF("file error\n");
        return -1;
    }

#endif
    
    return 0;
}

#endif

void eve_asset_load(EVE_ASSET_PROPS *asset, uint32_t loadimage)
{

#if ASSETS == USE_FLASH

    // Read directy from flash
    if (loadimage)
    {
        DEBUG_PRINTF("Load image from flash\n");

        EVE_LIB_BeginCoProList();
        EVE_CMD_FLASHSOURCE(asset->Flash_Start);
        EVE_CMD_LOADIMAGE(asset->RAM_G_Start, EVE_OPT_FLASH);
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

        if ((asset->Width != w) || (asset->CellHeight != h))
        {
            DEBUG_PRINTF("MISMATCH image size must match: width %d = %d; height %d = %d\n", asset->Width, w, asset->CellHeight, h);
        }
    }
    else
    {
        DEBUG_PRINTF("Load to memory from flash\n");

        EVE_LIB_BeginCoProList();
        EVE_CMD_FLASHREAD(asset->RAM_G_Start, asset->Flash_Start, asset->Flash_Size); // Destination, Source, Size
        EVE_LIB_EndCoProList();
        EVE_LIB_AwaitCoProEmpty();
    
        asset->RAM_G_EndAddr = asset->RAM_G_Start + asset->Flash_Size;
    }

    DEBUG_PRINTF("RAM_G: %d -> %d\n", asset->RAM_G_Start, asset->RAM_G_EndAddr);

#elif ASSETS == USE_FLASHIMAGE

    FILE *h1;
    uint8_t buffer[256];
    uint32_t total = 0;
    uint32_t chunk;
    size_t xfer;
    
    h1 = port_fopen(flashimage, "rb");
    if (h1)
    {
        int res;
        res = fseek(h1, asset->Flash_Start, SEEK_SET);
        if (res != 0)
        {
            DEBUG_PRINTF("seek error\n");
            return;
        }
        // Decode image
        if (loadimage)
        {
            DEBUG_PRINTF("Load image from %s\n", flashimage);

            EVE_LIB_BeginCoProList();
            EVE_CMD_LOADIMAGE(asset->RAM_G_Start, 0);

            do {
                chunk = (size_t)(asset->Flash_Size - total);
                if (chunk > sizeof(buffer)) 
                {
                    chunk = sizeof(buffer);
                }
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

            if ((asset->Width != w) || (asset->CellHeight != h))
            {
                DEBUG_PRINTF("MISMATCH image size must match: width %d = %d; height %d = %d\n", asset->Width, w, asset->CellHeight, h);
            }
        }
        else
        {
            DEBUG_PRINTF("Load to memory from %s\n", flashimage);
    
            EVE_LIB_BeginCoProList();
            EVE_CMD_MEMWRITE(asset->RAM_G_Start, asset->Flash_Size);

            do {
                chunk = (size_t)(asset->Flash_Size - total);
                if (chunk > sizeof(buffer)) 
                {
                    chunk = sizeof(buffer);
                }
                xfer = fread(buffer, 1, (size_t)chunk, h1);
                if (xfer > 0)
                {
                    EVE_LIB_WriteDataToCMD(buffer, (uint32_t)xfer);
                }
                total += (uint32_t)xfer;
            } while (xfer == sizeof(buffer));
            EVE_LIB_EndCoProList();
            EVE_LIB_AwaitCoProEmpty();
            
            asset->RAM_G_EndAddr = asset->RAM_G_Start + total;
        }

        fclose(h1); 

        DEBUG_PRINTF("RAM_G: %d -> %d\n", asset->RAM_G_Start, asset->RAM_G_EndAddr);
    }
    else
    {
        DEBUG_PRINTF("RAM_G: flash file error %s\n", flashimage);
    }

#endif

}

#endif
