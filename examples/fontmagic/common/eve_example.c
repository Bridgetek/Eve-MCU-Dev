/**
 @file eve_example.c
 */
/*
 * ============================================================================
 * History
 * =======
 * Nov 2019		Initial beta for FT81x and FT80x
 * Mar 2020		Updated beta - added BT815/6 commands
 * Mar 2021		Beta with BT817/8 support added
 *
 *
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
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include "EVE.h"
#include "../include/HAL.h"
#include "MCU.h"

#include "eve_example.h"

struct eve_font_cache romfontcache;
struct eve_font_cache customfontcache;

uint16_t getheight(struct eve_font_cache *cache)
{
    return cache->height;
}

uint16_t getwidth(struct eve_font_cache *cache)
{
    return cache->width;
}

// Get the maximum number of fonts supported by a platform.
uint8_t getfontmax(void)
{
    uint8_t fontcount;

#if IS_EVE_API(5)
    // BT82x
    fontcount = 64;
#else
    // FT8xx, FT81x, BT81x
    fontcount = 34;
#endif

    return fontcount;
}

// Check a ROM font is supported by a platform.
int isromfont(uint8_t fontnumber)
{
#if IS_EVE_API(1)
    // FT8xx
    if ((fontnumber <= 31) && (fontnumber >= 16))
        return 1;
#else
    // FT81x, BT81x, BT82x
    if ((fontnumber <= 34) && (fontnumber >= 16))
        return 1;
#endif
    return 0;
}

// Obtain the address of a ROM font. Platform specific.
// If the  ROM font will return a pointer of zero.
uint32_t getromfontptr(uint8_t fontnumber)
{
    uint32_t fontroot, fontptr;

#if IS_EVE_API(5)
    // BT82x
    fontroot = 0x08000000 - 0x100;
#else
    // FT8xx, FT81x, BT81x
    fontroot = HAL_MemRead32(EVE_ROMFONT_TABLEADDRESS);
#endif

    if (fontnumber < getfontmax())
    {
        fontptr = HAL_MemRead32(fontroot + (fontnumber * 4));
    }
    else
    {
        fontptr = 0;
    }

    return fontptr;
}
    
void getfontinfocache(struct eve_font_cache *cache, uint8_t fontnumber, uint32_t fontptr, uint8_t first_character)
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
    
    memset(&cache->widths[0], 0, sizeof(cache->widths));
    memset(&cache->glyphs[0], 0, sizeof(cache->glyphs));

    // Read the first word of the font metric block.
    // This determines the format of the font and how it is handled.
    uint32_t format = HAL_MemRead32(fontptr);
    if (format == 0x0100AAFF)
    {
        // Extended format 1 font cache.
        // Get the font bitmap sizes.
        cache->width = HAL_MemRead32(fontptr + (uint32_t)offsetof(EVE_GPU_EXT_FONT_HEADER, FontWidthInPixels));
        cache->height = HAL_MemRead32(fontptr + (uint32_t)offsetof(EVE_GPU_EXT_FONT_HEADER, FontHeightInPixels));
        uint32_t stride = HAL_MemRead32(fontptr + (uint32_t)offsetof(EVE_GPU_EXT_FONT_HEADER, FontLayoutWidth));
        // Get the total number (and fixed maximum) of characters in the font.
        N = HAL_MemRead32(fontptr + (uint32_t)offsetof(EVE_GPU_EXT_FONT_HEADER, FontNumberCharacters));
        if (N > MAX_CHARACTERS)
        {
            N = MAX_CHARACTERS;
        }
        // Load character widths and glyph pointers.
        start_of_graphics = HAL_MemRead32(fontptr + (uint32_t)offsetof(EVE_GPU_EXT_FONT_HEADER, PointerToFontGraphicsData));
        for (page = 0; page < N / 128; page++)
        {
            gptr = HAL_MemRead32(fontptr + sizeof(EVE_GPU_EXT_FONT_HEADER) + (page * 4));
            wptr = HAL_MemRead32(fontptr + sizeof(EVE_GPU_EXT_FONT_HEADER) + (4 * ((N / 128))) + (page * 4));
            for (ch = 0; ch < 128; ch += 4)
            {
                // Read character width as a 32 bit word.
                uint32_t width4 = HAL_MemRead32(wptr + (ch & 127));
                for (w = 0; w < 4; w++)
                {
                    cache->widths[ch + w] = (width4 >> (w * 8)) & 0xff;
                    // Construct glyph pointer.
                    cache->glyphs[ch + w] = start_of_graphics + gptr + ((ch + w) * cache->height * stride);
                }
            }
        }
    }
    else if (format == 0x0200AAFF)
    {
        // Extended format 2 font cache.
        cache->legacy = 0;
        // Get the font pixel sizes.
        cache->height = HAL_MemRead32(fontptr + (uint32_t)offsetof(EVE_GPU_EXT2_FONT_HEADER, FontHeightInPixels));
        cache->width= HAL_MemRead32(fontptr + (uint32_t)offsetof(EVE_GPU_EXT2_FONT_HEADER, FontWidthInPixels));
        // Get the total number (and fixed maximum) of characters in the font.
        N = HAL_MemRead32((uint32_t)offsetof(EVE_GPU_EXT2_FONT_HEADER, FontNumberCharacters));
        if (N > MAX_CHARACTERS)
        {
            N = MAX_CHARACTERS;
        }
        // Load character widths and glyph pointers.
        // This only takes the unkerned character width.
        for (page = 0; page < N / 128; page++)
        {
            gptr = HAL_MemRead32(fontptr + sizeof(EVE_GPU_EXT2_FONT_HEADER) + (page * 4));
            for (ch = 0; ch < 128; ch++)
            {
                cdptr = HAL_MemRead32(gptr + ((ch & 127) * 4));
                cache->glyphs[ch] = HAL_MemRead32(cdptr);
                cache->widths[ch] = HAL_MemRead32(cdptr + 4) & 0xff;
            }
        }
    }
    else
    {
        // Legacy font.
        cache->legacy = 1;
        // Get the font pixel sizes.
        uint16_t stride = HAL_MemRead32(fontptr + (uint32_t)offsetof(EVE_GPU_FONT_HEADER, FontLineStride));
        cache->width = HAL_MemRead32(fontptr + (uint32_t)offsetof(EVE_GPU_FONT_HEADER, FontWidthInPixels));
        cache->height = HAL_MemRead32(fontptr + (uint32_t)offsetof(EVE_GPU_FONT_HEADER, FontHeightInPixels));
        // Get offset to glyphs.
        start_of_graphics = HAL_MemRead32(fontptr + (uint32_t)offsetof(EVE_GPU_FONT_HEADER, PointerToFontGraphicsData));
        // Load character widths and glyph pointers.
        int ch, w;
        for (ch = 0; ch < 128; ch += 4)
        {
            // Read character width as a 32 bit word.
            uint32_t width4 = HAL_MemRead32(fontptr + (uint32_t)offsetof(EVE_GPU_FONT_HEADER, FontWidth[ch]));
            for (w = 0; w < 4; w++)
            {
                cache->widths[ch + w] = (width4 >> (w * 8)) & 0xff;
                // Construct glyph pointer.
                cache->glyphs[ch + w] = start_of_graphics + ((ch + w - first_character) * cache->height * stride);
            }
        }
    }
}

void getcustomfontinfo(struct eve_font_cache *cache, uint8_t fontnumber, uint32_t fontptr, uint8_t first_character)
{
    getfontinfocache(cache, fontnumber, fontptr, first_character);
}

void getromfontinfo(struct eve_font_cache *cache, uint8_t fontnumber)
{
    uint32_t fontptr = getromfontptr(fontnumber);

    if (fontptr)
    {
        getfontinfocache(cache, fontnumber, fontptr, 32);
    }
}

void cmd_textdraw(int x, int y, struct eve_font_cache *cache, char ch)
{
    if (cache->legacy)
    {
        // This is a legacy font.
        if ((y > 511) || (x > 511))
        {
#if IS_EVE_API(2, 3, 4, 5)
            EVE_VERTEX_TRANSLATE_X(x * 16);
            EVE_VERTEX_TRANSLATE_Y(y * 16);
#endif
            EVE_VERTEX2II(0, 0, cache->handle, ch);
        }
        else
        {
            EVE_VERTEX2II(x, y, cache->handle, ch);
        }
    }
    else
    {
        // This is a RAM font.
#if IS_EVE_API(5)
        EVE_BITMAP_SOURCE_H(cache->glyphs[(int)ch] >> 24);
#endif
        EVE_BITMAP_SOURCE(cache->glyphs[(int)ch]);
        if ((y > 511) || (x > 511))
        {
#if IS_EVE_API(2, 3, 4, 5)
            EVE_VERTEX_TRANSLATE_X(x * 16);
            EVE_VERTEX_TRANSLATE_Y(y * 16);
#endif
            EVE_VERTEX2F(0, 0);
        }
        else
        {
            EVE_VERTEX2F(x * 16, y * 16);
        }
    }
}

// Rotate a text string by 90 degrees clockwise.
void cmd_textrotate(int x, int y, struct eve_font_cache *cache, char *text)
{
    // CMD_ROTATEAROUND is only available on BT82x, BT81x.
#if IS_EVE_API(3, 4, 5)
    EVE_SAVE_CONTEXT();

    // Setup the font.
    EVE_CELL(0);
    // Select the handle for the font.
    EVE_BEGIN(EVE_BEGIN_BITMAPS);
    // Manipulate the font display.
    EVE_CMD_LOADIDENTITY();
    EVE_BITMAP_HANDLE(cache->handle);
    EVE_CMD_ROTATEAROUND(
        (uint32_t)cache->width / 2, 
        (uint32_t)cache->height / 2,
        16384, 65536);
    EVE_CMD_SETMATRIX();

    int i;
    size_t len = strlen(text);
    char *ch = text;

    for (i = 0; i < len; i++)
    {
        cmd_textdraw(x, y, cache, *ch);
    
        // Move the cursor in the y-axis.
        y = y + ((uint32_t)cache->widths[(int)*ch]);

        ch++;
    }

    // Undo transformation above.
    if (isromfont(cache->handle))
    {
        EVE_CMD_RESETFONTS();
    }
    else
    {
        EVE_CMD_LOADIDENTITY();
        EVE_CMD_SETMATRIX();
    }

    EVE_RESTORE_CONTEXT();
#endif
}

// Zoom a text string by a factor.
void cmd_textzoom(int x, int y, struct eve_font_cache *cache, uint32_t zoom, char *text)
{
    EVE_SAVE_CONTEXT();

    // Setup the font.
    EVE_CELL(0);
    // Select the handle for the font.
    EVE_BEGIN(EVE_BEGIN_BITMAPS);
    // Manipulate the font display.
    EVE_CMD_LOADIDENTITY();
    EVE_BITMAP_HANDLE(cache->handle);
    EVE_BITMAP_SIZE(EVE_FILTER_NEAREST, EVE_WRAP_BORDER, EVE_WRAP_BORDER, 
        ((uint32_t)cache->width * zoom) / ZOOM_1, 
        ((uint32_t)cache->height * zoom) / ZOOM_1);
    // Apply scale to text.
    EVE_CMD_SCALE(zoom, zoom);
    EVE_CMD_SETMATRIX();

    int i;
    size_t len = strlen(text);
    char *ch = text;

    for (i = 0; i < len; i++)
    {
        cmd_textdraw(x, y, cache, *ch);
    
        // Move the cursor in the x-axis accounting for zoom factor.
        x = x + (((uint32_t)cache->widths[(int)*ch] * zoom) / ZOOM_1);

        ch++;
    }

    // Undo transformation above
    if (isromfont(cache->handle))
    {
        EVE_CMD_RESETFONTS();
    }
    else
    {
        EVE_CMD_LOADIDENTITY();
        EVE_CMD_SETMATRIX();
    }

    EVE_RESTORE_CONTEXT();
}

uint32_t loadlegacyfont(const uint8_t *fontptr, uint32_t fontsize, uint32_t fontoffset)
{
	EVE_LIB_WriteDataToRAMG(fontptr, fontsize, fontoffset);
	EVE_LIB_AwaitCoProEmpty();
	return ((fontsize + fontoffset) + 16) & (~15);
}

void eve_example(void)
{
    int action = 0;
    int y;
    int x;
    uint8_t key;
    
    // Initialise the display.
    EVE_Init();

    // Calibrate the display.
    printf("Calibrating display...\n");
    eve_calibrate();

    // Load fonts and images.
    printf("Loading font...\n");
    loadlegacyfont(font0, font0_size, font0_offset);

    // Start example code.
    printf("Starting demo...\n");
    getromfontinfo(&romfontcache, FONT_ROM);
    getcustomfontinfo(&customfontcache, FONT_CUSTOM, font0_offset, font0_first);

    do
    {
        // Start drawing test screen.
        EVE_LIB_BeginCoProList();
        EVE_CMD_DLSTART();
        EVE_CLEAR_COLOR_RGB(64,72,64);
        EVE_CLEAR(1,1,1);
        EVE_COLOR_RGB(255, 255, 255);

#if IS_EVE_API(5)
        // BT82x.
        EVE_CMD_SETFONT(FONT_CUSTOM, font0_offset, font0_first);
#elif IS_EVE_API(2,3,4)
        // BT81x.
        EVE_CMD_SETFONT2(FONT_CUSTOM, font0_offset, font0_first);
#else
        // FT80x and FT81x.
        EVE_CMD_SETFONT(FONT_CUSTOM, font0_offset);
#endif

        if (action == 0)
        {
            y = 100;
            x = 600;
            // Draw test text using a ROM font.
            EVE_CMD_TEXT(x, y, FONT_ROM, 0, "ROM font text");
            y += ((getheight(&romfontcache) * 3)/2);
            cmd_textzoom(x, y, &romfontcache, ZOOM_OUT_3_4, "ROM zoomed out text!");
            y = 100;
            x = x - getheight(&romfontcache);
            cmd_textrotate(x, y, &romfontcache, "ROM rotated text!");
            x = x - getheight(&romfontcache);
            cmd_textrotate(x, y, &romfontcache, "More ROM rotated text!");

            // Draw test text using the custom font.
            y = 100;
            x = 100;
            EVE_CMD_TEXT(x, y, FONT_CUSTOM, 0, "Custom font text");
            y += ((getheight(&customfontcache) * 3)/2);
            cmd_textzoom(x, y, &customfontcache, ZOOM_IN_2, "Custom zoomed in text!");
            y += (2 * getheight(&customfontcache));
            cmd_textzoom(x, y, &customfontcache, ZOOM_IN_4, "More Custom zoomed in text!");
            y = 100;
            x = x - getheight(&customfontcache);
            cmd_textrotate(x, y, &customfontcache, "Custom rotated text!");
            x = x - getheight(&customfontcache);
            cmd_textrotate(x, y, &customfontcache, "More Custom rotated text!");

            action++;
        }
        else if (action == 1)
        {
            y = 100;
            // Draw test text of all ASCII characters.
            EVE_CMD_TEXT(100, y, FONT_CUSTOM, 0, "!\"#$%&'()*+,-./0123456789:;<=>?");
            y += getheight(&customfontcache);
            EVE_CMD_TEXT(100, y, FONT_CUSTOM, 0, "@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_");
            y += getheight(&customfontcache);
            EVE_CMD_TEXT(100, y, FONT_CUSTOM, 0, "`abcdefghijklmnopqrstuvwxyz{|}");
            y +=getheight(&customfontcache);
            EVE_CMD_TEXT(100, y, FONT_ROM, 0, "!\"#$%&'()*+,-./0123456789:;<=>?");
            y += getheight(&romfontcache);
            EVE_CMD_TEXT(100, y, FONT_ROM, 0, "@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_");
            y += getheight(&romfontcache);
            EVE_CMD_TEXT(100, y, FONT_ROM, 0, "`abcdefghijklmnopqrstuvwxyz{|}");
            y += getheight(&romfontcache);

            action++;
        }
        else if (action == 2)
        {
            y = 100;
            // Draw test text of ASCII characters.
            EVE_CMD_TEXT(100, y, FONT_CUSTOM, 0, "\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2a\x2b\x2c\x2d\x2e\x2f" \
                        "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3a\x3b\x3c\x3d\x3e\x3f");
            y += getheight(&customfontcache);
            EVE_CMD_TEXT(100, y, FONT_CUSTOM, 0, "\x40\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f" \
                        "\x50\x51\x52\x53\x54\x55\x56\x57\x58\x59\x5a\x5b\x5c\x5d\x5e\x5f");
            y += getheight(&customfontcache);
            EVE_CMD_TEXT(100, y, FONT_CUSTOM, 0, "\x60\x61\x62\x63\x64\x65\x66\x67\x68\x69\x6a\x6b\x6c\x6d\x6e\x6f" \
                        "\x70\x71\x72\x73\x74\x75\x76\x77\x78\x79\x7a\x7b\x7c\x7d\x7e");
            y += getheight(&customfontcache);
            cmd_textzoom(100, y, &customfontcache, ZOOM_IN_2, "\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2a\x2b\x2c\x2d\x2e\x2f" \
                        "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3a\x3b\x3c\x3d\x3e\x3f");
            y += (2 * getheight(&customfontcache));
            cmd_textzoom(100, y, &customfontcache, ZOOM_IN_2, "\x40\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f" \
                        "\x50\x51\x52\x53\x54\x55\x56\x57\x58\x59\x5a\x5b\x5c\x5d\x5e\x5f");
            y += (2 * getheight(&customfontcache));
            cmd_textzoom(100, y, &customfontcache, ZOOM_IN_4, "\x60\x61\x62\x63\x64\x65\x66\x67\x68\x69\x6a\x6b\x6c\x6d\x6e\x6f" \
                        "\x70\x71\x72\x73\x74\x75\x76\x77\x78\x79\x7a\x7b\x7c\x7d\x7e");

            action = 0;
        }

        EVE_DISPLAY();
        EVE_CMD_SWAP();
        EVE_LIB_EndCoProList();
        EVE_LIB_AwaitCoProEmpty();
        
        // Wait for touchscreen before moving to the next action.
        while (eve_read_tag(&key) == 0);
        // Debounce touchscreen.
        while (eve_read_tag(&key) != 0);
        
    } while (1);
}
