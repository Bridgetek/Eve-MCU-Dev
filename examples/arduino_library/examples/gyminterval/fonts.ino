/**
 @file fonts.c
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
#include <stddef.h>

#include <### EVE CLASS ###.h>

/**
 @brief EVE library handle.
 @details This is the one instance of the EVE library. Available as a global.
 */
extern ### EVE CLASS ### eve;

#include "fonts.h"

/* ### BEGIN API == 1 ### */
// The FT800 does not have the CMD_ROMFONT feature. 
// This routine will map a bitmap handle onto a ROM font.
void EVE_CMD_ROMFONT(uint32_t font, uint32_t romfont)
{
    // Temporarily finish the coprocessor list
    eve.LIB_EndCoProList();

    uint32_t fontptr = font_getromptr(romfont);
    uint32_t fontsrc = eve.LIB_MemRead32(fontptr + (uint32_t)offsetof(### EVE CLASS ###::GPU_FONT_HEADER, PointerToFontGraphicsData));
    uint32_t width = eve.LIB_MemRead32(fontptr + (uint32_t)offsetof(### EVE CLASS ###::GPU_FONT_HEADER, FontWidthInPixels));
    uint32_t height = eve.LIB_MemRead32(fontptr + (uint32_t)offsetof(### EVE CLASS ###::GPU_FONT_HEADER, FontHeightInPixels));
    uint32_t format = eve.LIB_MemRead32(fontptr + (uint32_t)offsetof(### EVE CLASS ###::GPU_FONT_HEADER, FontBitmapFormat));
    uint32_t linestride = eve.LIB_MemRead32(fontptr + (uint32_t)offsetof(### EVE CLASS ###::GPU_FONT_HEADER, FontLineStride));

    // Resume the list
    eve.LIB_BeginCoProList();
    eve.BEGIN(eve.BEGIN_BITMAPS);
    eve.BITMAP_HANDLE(font);
    eve.BITMAP_SOURCE(fontsrc);
    eve.BITMAP_LAYOUT(format, linestride, height);
    eve.BITMAP_SIZE(eve.FILTER_NEAREST, eve.WRAP_BORDER, eve.WRAP_BORDER, width, height);
    
    // Do not finish but a CMD_SWAP must be executed to update the changes made.
}
/* ### END API ### */

// Get the maximum number of fonts supported by a platform.
uint8_t font_getmax(void)
{
    return eve.ROMFONT_MAX;
}

// Obtain the address of a ROM font. Platform specific.
// If the  ROM font will return a pointer of zero.
uint32_t font_getromptr(uint8_t fontnumber)
{
    uint32_t fontroot, fontptr;

    /* ### BEGIN API >= 5 ### */
    // BT82x
    fontroot = 0x08000000 - 0x100;
    if (fontnumber <= font_getmax())
    {
        fontptr = eve.LIB_MemRead32(fontroot + (fontnumber * 4));
    }
    else
    {
        fontptr = 0;
    }
    /* ### END API ### */

    /* ### BEGIN API < 5 ### */
    // FT8xx, FT81x, BT81x
    fontroot = eve.LIB_MemRead32(eve.ROMFONT_TABLEADDRESS);
    if (fontnumber <= font_getmax())
    {
        fontptr = fontroot + ((fontnumber - 16) * sizeof(### EVE CLASS ###::GPU_FONT_HEADER));
    }
    else
    {
        fontptr = 0;
    }
    /* ### END API ### */

    return fontptr;
}

static void getfontinfocache(struct eve_font_cache *cache, uint8_t fontnumber, uint32_t fontptr, uint8_t first_character)
{
    uint32_t page;
    uint32_t gptr;
    uint32_t wptr;
    uint32_t cdptr;
    int ch, w;
    uint32_t N;
    int32_t start_of_graphics;

    cache->legacy = 0;
    cache->handle = fontnumber;
    cache->first = first_character;
    
    // Read the first word of the font metric block.
    // This determines the format of the font and how it is handled.
    /* ### BEGIN API >= 4 ### */
    uint32_t format = eve.LIB_MemRead32(fontptr);
    if (format == 0x0100AAFF)
    {
        // Extended format 1 font cache.
        // Get the font bitmap sizes.
        cache->width = eve.LIB_MemRead32(fontptr + (uint32_t)offsetof(### EVE CLASS ###::GPU_EXT_FONT_HEADER, FontWidthInPixels));
        cache->height = eve.LIB_MemRead32(fontptr + (uint32_t)offsetof(### EVE CLASS ###::GPU_EXT_FONT_HEADER, FontHeightInPixels));
        cache->format = eve.LIB_MemRead32(fontptr + (uint32_t)offsetof(### EVE CLASS ###::GPU_EXT_FONT_HEADER, FontBitmapFormat));
        cache->linestride = eve.LIB_MemRead32(fontptr + (uint32_t)offsetof(### EVE CLASS ###::GPU_EXT_FONT_HEADER, FontLayoutWidth));
        // Get the total number (and fixed maximum) of characters in the font.
        N = eve.LIB_MemRead32(fontptr + (uint32_t)offsetof(### EVE CLASS ###::GPU_EXT_FONT_HEADER, FontNumberCharacters));
        if (N > FONT_MAX_CHARACTERS)
        {
            N = FONT_MAX_CHARACTERS;
        }
        // Load character widths and glyph pointers.
        start_of_graphics = eve.LIB_MemRead32(fontptr + (uint32_t)offsetof(### EVE CLASS ###::GPU_EXT_FONT_HEADER, PointerToFontGraphicsData));
        for (page = 0; page < N / 128; page++)
        {
            gptr = eve.LIB_MemRead32(fontptr + sizeof(### EVE CLASS ###::GPU_EXT_FONT_HEADER) + (page * 4));
            wptr = eve.LIB_MemRead32(fontptr + sizeof(### EVE CLASS ###::GPU_EXT_FONT_HEADER) + (4 * ((N / 128))) + (page * 4));
            for (ch = 0; ch < 128; ch += 4)
            {
                // Read character width as a 32 bit word.
                uint32_t width4 = eve.LIB_MemRead32(wptr + (ch & 127));
                for (w = 0; w < 4; w++)
                {
                    cache->widths[ch + w] = (width4 >> (w * 8)) & 0xff;
                    // Construct glyph pointer.
                    cache->glyphs[ch + w] = start_of_graphics + gptr + ((ch + w) * cache->height * cache->linestride);
                }
            }
        }
    }
    else 
    /* ### END API ### */
    /* ### BEGIN API >= 5 ### */
    if (format == 0x0200AAFF)
    {
        // Extended format 2 font cache.
        cache->legacy = 0;
        // Get the font pixel sizes.
        cache->height = eve.LIB_MemRead32(fontptr + (uint32_t)offsetof(### EVE CLASS ###::GPU_EXT2_FONT_HEADER, FontHeightInPixels));
        cache->width= eve.LIB_MemRead32(fontptr + (uint32_t)offsetof(### EVE CLASS ###::GPU_EXT2_FONT_HEADER, FontWidthInPixels));
        cache->format = eve.LIB_MemRead32(fontptr + (uint32_t)offsetof(### EVE CLASS ###::GPU_EXT2_FONT_HEADER, FontBitmapFormat));
        cache->linestride = eve.LIB_MemRead32(fontptr + (uint32_t)offsetof(### EVE CLASS ###::GPU_EXT2_FONT_HEADER, FontLayoutWidth));
        // Get the total number (and fixed maximum) of characters in the font.
        N = eve.LIB_MemRead32(fontptr + (uint32_t)offsetof(### EVE CLASS ###::GPU_EXT2_FONT_HEADER, FontNumberCharacters));

        if (N > FONT_MAX_CHARACTERS)
        {
            N = FONT_MAX_CHARACTERS;
        }
        // Load character widths and glyph pointers.
        // This only takes the unkerned character width.
        for (page = 0; page < N / 128; page++)
        {
            gptr = eve.LIB_MemRead32(fontptr + sizeof(### EVE CLASS ###::GPU_EXT2_FONT_HEADER) + (page * 4));
            for (ch = 0; ch < 128; ch++)
            {
                cdptr = eve.LIB_MemRead32(gptr + ((ch & 127) * 4));
                cache->glyphs[ch] = eve.LIB_MemRead32(cdptr);
                cache->widths[ch] = eve.LIB_MemRead32(cdptr + 4) & 0xff;
            }
        }
    }
    else
    /* ### END API ### */
    {
        // Legacy font.
        cache->legacy = 1;
        // Get the font pixel sizes.
        cache->width = eve.LIB_MemRead32(fontptr + (uint32_t)offsetof(### EVE CLASS ###::GPU_FONT_HEADER, FontWidthInPixels));
        cache->height = eve.LIB_MemRead32(fontptr + (uint32_t)offsetof(### EVE CLASS ###::GPU_FONT_HEADER, FontHeightInPixels));
        cache->format = eve.LIB_MemRead32(fontptr + (uint32_t)offsetof(### EVE CLASS ###::GPU_FONT_HEADER, FontBitmapFormat));
        cache->linestride = eve.LIB_MemRead32(fontptr + (uint32_t)offsetof(### EVE CLASS ###::GPU_FONT_HEADER, FontLineStride));
        // Get offset to glyphs.
        start_of_graphics = eve.LIB_MemRead32(fontptr + (uint32_t)offsetof(### EVE CLASS ###::GPU_FONT_HEADER, PointerToFontGraphicsData));
        // Load character widths and glyph pointers.
        int ch, w;
        for (ch = 0; ch < 128; ch += 4)
        {
            // Read character width as a 32 bit word.
            uint32_t width4 = eve.LIB_MemRead32(fontptr + (uint32_t)offsetof(### EVE CLASS ###::GPU_FONT_HEADER, FontWidth[0]) + ch);
            for (w = 0; w < 4; w++)
            {
                cache->widths[ch + w] = (width4 >> (w * 8)) & 0xff;
                // Construct glyph pointer.
                cache->glyphs[ch + w] = start_of_graphics + ((ch + w - first_character) * cache->height * cache->linestride);
            }
        }
    }
}

void font_getfontinfocustom(struct eve_font_cache *cache, uint8_t fontnumber, uint32_t fontptr, uint8_t first_character)
{
    getfontinfocache(cache, fontnumber, fontptr, first_character);
}

void font_getfontinforom(struct eve_font_cache *cache, uint8_t fontnumber)
{
    uint32_t fontptr = font_getromptr(fontnumber);
    if (fontptr)
    {
        getfontinfocache(cache, fontnumber, fontptr, 0);
    }
}

uint16_t font_getheight(struct eve_font_cache *cache)
{
    return cache->height;
}

uint16_t font_getwidth(struct eve_font_cache *cache)
{
    return cache->width;
}

uint16_t font_getcharwidth(struct eve_font_cache *cache, uint8_t ch)
{
    return (uint16_t)cache->widths[(int)ch];
}
