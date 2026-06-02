/**
 @file eve_example.c
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
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <EVE.h>

#include "eve_example.h"

// Structure to hold information to draw animated bubbles
struct bubbles_s
{
    int16_t x;      // bubble x location
    int16_t y;      // bubble y location (non-zero means bubble is active)
    int16_t size;   // bubble size
};
// Number of bubbles to animate
#define BUBBLE_COUNT 50

// Structure to hold information to draw animated fish
struct fishes_s
{
    int16_t x;      // fish x location
    int16_t y;      // fish y location
    int8_t dir;     // fish direction (non-zero means fish is active)
    int8_t speed;   // fish speed
    int8_t type;    // fish type
};
// Number of fish to animate
#define FISH_COUNT 50
// Maximum number of fish types
#define FISH_MAX 4
// Structure to hold information to draw animated fish
struct fish_size_s
{
    int16_t width;  // fish width
    int16_t height; // fish height
    uint32_t addr;  // fish start address
    uint8_t handle; // fish handle
} fish_info_g[FISH_MAX];

// Structure to hold information to draw animated propellor
struct prop_size_s
{
    int16_t width;  // prop width
    int16_t height; // prop height
    uint32_t addr;  // prop start address
    uint8_t handle; // prop handle
 } prop_info_g;

static inline void draw_vertex(int16_t x, int16_t y)
{
#if IS_EVE_API(1)
    EVE_VERTEX2F(x * 16, y * 16);
#else
    EVE_VERTEX2F(x, y);
#endif
}

// Animate bubbles rising past the lens
static void animate_bubbles(void)
{
    // Array to hold data for BUBBLE_COUNT animated bubbles
    static struct bubbles_s bubbles[BUBBLE_COUNT] = {0};
    int anim = 0;

    EVE_SAVE_CONTEXT();
    // Add a new bubble if there is space
    if (rand() < (RAND_MAX / 100))
    {
        for (anim = 0; anim < BUBBLE_COUNT; anim++)
        {
            // Free bubble slot
            if (bubbles[anim].y == 0)
            {
                // Bubble at a random position in the screen
                bubbles[anim].x = rand() % EVE_DISP_WIDTH;
                // Bubble starts at the bottom
                bubbles[anim].y = EVE_DISP_HEIGHT;
                // Random bubble size
                bubbles[anim].size = rand() % (EVE_DISP_WIDTH / 64) + (EVE_DISP_WIDTH / 64);
                
                // Just add one bubble
                break;
            }
        }
    }
#if !IS_EVE_API(1)
    EVE_VERTEX_FORMAT(0);
#endif
    EVE_BEGIN(EVE_BEGIN_POINTS);
    EVE_COLOR(0xd0d0FF);
    for (anim = 0; anim < BUBBLE_COUNT; anim++)
    {
        if (bubbles[anim].y > 0)
        {
            EVE_CLEAR_STENCIL(0);
            EVE_CLEAR(0,1,0);
            EVE_COLOR_MASK(0,0,0,0);
            EVE_STENCIL_OP(EVE_STENCIL_KEEP, EVE_STENCIL_INCR);
            // Draw a stancil for this bubble
            // Outer edge
            EVE_POINT_SIZE((bubbles[anim].size + 1) * 16);
            draw_vertex(bubbles[anim].x, bubbles[anim].y); // stencil 1
            // Inner edge
            EVE_POINT_SIZE((bubbles[anim].size - 1) * 16);
            draw_vertex(bubbles[anim].x, bubbles[anim].y); // stencil 2

            EVE_COLOR_MASK(1,1,1,1);
            EVE_STENCIL_OP(EVE_STENCIL_KEEP, EVE_STENCIL_KEEP);
            // Outer edge
            EVE_STENCIL_FUNC(EVE_TEST_EQUAL, 1, 255);
            EVE_POINT_SIZE((bubbles[anim].size + 2) * 16);
            EVE_COLOR_A(0x80);
            draw_vertex(bubbles[anim].x, bubbles[anim].y); // stencil 1
            // Inner of bubble
            EVE_STENCIL_FUNC(EVE_TEST_EQUAL, 2, 255);
            EVE_COLOR_A(0x40);
            draw_vertex(bubbles[anim].x - (bubbles[anim].size), bubbles[anim].y - (bubbles[anim].size)); // stencil 2
            EVE_COLOR_A(0xFF);
            EVE_STENCIL_FUNC(EVE_TEST_ALWAYS, 0, 255);
            
            // Animate upwards
            bubbles[anim].y--;
        }
    }
    EVE_RESTORE_CONTEXT();
}

// Animate propellor graphic
static void animate_prop(void)
{
    static int counter = 0;
    static uint16_t angle = 0;
    if (counter == 0)
    {
        angle += 0x1000;
        counter = 5;
    }
    counter--;

    EVE_SAVE_CONTEXT();
#if !IS_EVE_API(1)
    EVE_VERTEX_FORMAT(0);
#endif
    EVE_BEGIN(EVE_BEGIN_BITMAPS);
    EVE_BITMAP_HANDLE(prop_info_g.handle);
    EVE_COLOR(0x804000);
    uint16_t height = (EVE_DISP_HEIGHT) / 6;
    uint16_t width = (prop_info_g.width * height) / prop_info_g.height;
    uint32_t scale = (((height) * 0x10000) / prop_info_g.height);

    // Scale the bitmap width to the size we need
    EVE_CMD_LOADIDENTITY();
    // Scale the image
    EVE_CMD_SCALE(scale, scale);
    EVE_CMD_TRANSLATE((prop_info_g.width * 0x10000) / 2, (prop_info_g.height * 0x10000) / 2); 
    // Rotate the image
    EVE_CMD_ROTATE(angle);
    EVE_CMD_TRANSLATE(~(prop_info_g.width * 0x10000) / 2, ~(prop_info_g.height * 0x10000) / 2); 
    // Set new bitmap transform matrix
    EVE_CMD_SETMATRIX(); 
    // Resize the bitmap
    EVE_BITMAP_SIZE(EVE_FILTER_NEAREST, EVE_WRAP_BORDER, EVE_WRAP_BORDER, width, height);
#if IS_EVE_API (2,3,4,5)
    EVE_BITMAP_SIZE_H((width >> 9), (height >> 9));
#endif

    draw_vertex(0, 0); 
    draw_vertex(EVE_DISP_WIDTH - width, 0); 
    
    EVE_RESTORE_CONTEXT();
}

// Animate fish swimming past the lens
static void animate_fish(void)
{
    // Array to hold data for FISH_COUNT animated fish
    static struct fishes_s fishes[FISH_COUNT] = {0};
    int anim = 0;

    EVE_SAVE_CONTEXT();
    // Add a new fish if there is space
    if (rand() < (RAND_MAX / 100))
    {
        for (anim = 0; anim < FISH_COUNT; anim++)
        {
            // Free fish slot
            if (fishes[anim].dir == 0)
            {
                // Random direction of swimming
                fishes[anim].dir = (rand() - (RAND_MAX / 2));
                // Random speed of swimming
                fishes[anim].speed = (rand() & 7);
                // Random fish type
                fishes[anim].type = (rand() % FISH_MAX);
                // Fish starts at the left or right
                if (fishes[anim].dir > 0)
                {
                    fishes[anim].x = -fish_info_g[fishes[anim].type].width;
                }
                else
                {
                    fishes[anim].x = EVE_DISP_WIDTH;
                }
                // Fish at a random position in the screen
                fishes[anim].y = (rand() % EVE_DISP_HEIGHT);
                
                // Just add one fish
                break;
            }
        }
    }
#if !IS_EVE_API(1)
    EVE_VERTEX_FORMAT(0);
#endif
    EVE_BEGIN(EVE_BEGIN_POINTS);
    EVE_COLOR(0xFFFFFF);
    for (anim = 0; anim < FISH_COUNT; anim++)
    {
        if (fishes[anim].dir != 0)
        {
            int8_t handle = fish_info_g[fishes[anim].type].handle;

            EVE_BEGIN(EVE_BEGIN_BITMAPS);
            EVE_BITMAP_HANDLE(handle);
            // Animate across the screen
            // All the fish face R to L though
            if (fishes[anim].dir < 0)
            {
                // Flip horiontally to swim L to R
                EVE_CMD_LOADIDENTITY();
                // scale by a midpoint of the image
                // get the width from the image information
                EVE_CMD_TRANSLATE((fish_info_g[handle].width / 2) * 0x10000, 0);
                // scale by -1 in the x axis to mirror the image
                EVE_CMD_SCALE((-1 * 0x10000), (1 * 0x10000));
                EVE_CMD_TRANSLATE((-1 * ((fish_info_g[handle].width / 2) * 0x10000)), 0);
                // set new bitmap transform matrix
                EVE_CMD_SETMATRIX();
            }

            draw_vertex(fishes[anim].x, fishes[anim].y); 
            
            if (fishes[anim].dir < 0)
            {
                // Fish normally swims in this direction R to L
                fishes[anim].x -= fishes[anim].speed;
                if (fishes[anim].x < -fish_info_g[fishes[anim].type].width)
                {
                    fishes[anim].dir = 0;
                }
                EVE_CMD_LOADIDENTITY();
                EVE_CMD_SETMATRIX();
            }
            else
            {
                // Fish is swimmin L to R
                fishes[anim].x += fishes[anim].speed;
                if (fishes[anim].x >= EVE_DISP_WIDTH)
                {
                    fishes[anim].dir = 0;
                }
            }
        }
    }
    EVE_RESTORE_CONTEXT();
}

static uint32_t load_images(const uint8_t *z_data, const uint32_t z_size, int handle, uint8_t fmt, uint8_t stride, uint16_t img_width, uint16_t img_height, uint32_t start_addr)
{
    uint32_t sz = z_size;
    const uint8_t *zd = z_data;
    uint32_t chunk;

    EVE_LIB_BeginCoProList();
    // Load image and use the built-in pointer for the address

#if IS_EVE_API(1,2,3,4)        
    EVE_CMD_INFLATE(-1);
#else
    EVE_CMD_INFLATE(-1, 0);
#endif
    do {
        chunk = 512;
        if (sz < chunk)
        {
            chunk = sz;
        }
        if (chunk > 0)
        {
            EVE_LIB_WriteDataToCMD(zd, chunk);
        }
        zd += chunk;
        sz -= chunk;
    } while (sz);
    EVE_LIB_EndCoProList();
    EVE_LIB_AwaitCoProEmpty();

    EVE_LIB_BeginCoProList();
    EVE_CMD_DLSTART();
    EVE_BEGIN(EVE_BEGIN_BITMAPS);
    EVE_BITMAP_HANDLE(handle);
    // Optional to mask the bitmap source here with 0x3FFFFF to ensure that only the valid bits for addressing within RAM_G are set.
    // BT81x now supports additional addressing where the source is in flash (see bitmap_source in the programmers guide)
    EVE_BITMAP_SOURCE(start_addr & 0x3FFFFF);
    EVE_BITMAP_LAYOUT(fmt, img_width * stride, img_height);
#if IS_EVE_API(2, 3, 4, 5)
    EVE_BITMAP_LAYOUT_H((img_width * stride) >> 10, img_height >> 9);
#endif
    EVE_BITMAP_SIZE(EVE_FILTER_NEAREST, EVE_WRAP_BORDER, EVE_WRAP_BORDER,
            img_width, img_height);
#if IS_EVE_API(2, 3, 4, 5)
    EVE_BITMAP_SIZE_H(img_width >> 9, img_height >> 9);
#endif
    EVE_END();

    EVE_DISPLAY();
    EVE_CMD_SWAP();
    EVE_LIB_EndCoProList();
    EVE_LIB_AwaitCoProEmpty();

    // Get the end of this image to know where the next one starts
    uint32_t end_addr;
    EVE_LIB_GetPtr(&end_addr);

    return (end_addr);
}

static uint32_t load_fish_images(const uint8_t *z_data, const uint32_t z_size, int handle, uint8_t fmt, uint8_t stride, uint16_t img_width, uint16_t img_height, uint32_t start_addr)
{
    uint32_t end_addr = load_images(z_data, z_size, handle, fmt, stride, img_width, img_height, start_addr);
    static int fish_count = 0;

    fish_info_g[fish_count].width = img_width;
    fish_info_g[fish_count].height = img_height;
    fish_info_g[fish_count].addr = start_addr;
    fish_info_g[fish_count].handle = handle;
    fish_count++;

    return end_addr;
}

static uint32_t load_prop_image(const uint8_t *z_data, const uint32_t z_size, int handle, uint8_t fmt, uint8_t stride, uint16_t img_width, uint16_t img_height, uint32_t start_addr)
{
    uint32_t end_addr = load_images(z_data, z_size, handle, fmt, stride, img_width, img_height, start_addr);

    prop_info_g.width = img_width;
    prop_info_g.height = img_height;
    prop_info_g.addr = start_addr;
    prop_info_g.handle = handle;

    return end_addr;
}

// External linkage to fish images
extern const uint32_t parrotfish_116x43_ARGB1555_size; 
extern const uint32_t surgeonfish_133x56_ARGB1555_size;
extern const uint32_t clownfish_102x54_ARGB1555_size; 
extern const uint32_t yellowtang_128x81_ARGB1555_size;
extern const uint8_t parrotfish_116x43_ARGB1555[]; 
extern const uint8_t surgeonfish_133x56_ARGB1555[];
extern const uint8_t clownfish_102x54_ARGB1555[]; 
extern const uint8_t yellowtang_128x81_ARGB1555[];
extern const uint8_t clownfish_102x54_ARGB1555[]; 
extern const uint8_t yellowtang_128x81_ARGB1555[];

// External linkage to propellor images
extern const uint32_t prop1_128x128_ARGB2_size;
extern const uint8_t prop1_128x128_ARGB2[];

void eve_display(void)
{
    // Size of the indicators on the dashboard
    uint16_t compass_radius = EVE_DISP_WIDTH / 6;
    uint16_t depth_height = (EVE_DISP_HEIGHT * 2) / 3;
    uint16_t depth_width = EVE_DISP_WIDTH / 12;

    // Variables detemining how the animation of the widget appears
    int depth = 0;
    int max_depth = 9500;
    // Compass heading
    int heading = 0;

    // Variables for size and position
    // Centre the widgets
    uint16_t x_compass = (EVE_DISP_WIDTH / 2);
    uint16_t y_compass = EVE_DISP_HEIGHT - (compass_radius / 3);
    uint16_t x_depth = 0;
    uint16_t y_depth = (EVE_DISP_HEIGHT - depth_height) / 2;

    uint32_t addr = 0;
    int fish_handle = 0;
    addr = load_fish_images(clownfish_102x54_ARGB1555, clownfish_102x54_ARGB1555_size, fish_handle++, EVE_FORMAT_ARGB1555, 2, 102, 54, addr);
    addr = load_fish_images(parrotfish_116x43_ARGB1555, parrotfish_116x43_ARGB1555_size, fish_handle++, EVE_FORMAT_ARGB1555, 2, 116, 43, addr);
    addr = load_fish_images(surgeonfish_133x56_ARGB1555, surgeonfish_133x56_ARGB1555_size, fish_handle++, EVE_FORMAT_ARGB1555, 2, 133, 56, addr);
    addr = load_fish_images(yellowtang_128x81_ARGB1555, yellowtang_128x81_ARGB1555_size, fish_handle++, EVE_FORMAT_ARGB1555, 2, 128, 81, addr);
    int prop_handle = fish_handle;
    addr = load_prop_image(prop1_128x128_ARGB2, prop1_128x128_ARGB2_size, prop_handle++, EVE_FORMAT_ARGB2, 1, 128, 128, addr);

    while (1)
    {
        EVE_LIB_BeginCoProList();
        EVE_CMD_DLSTART();
        EVE_CLEAR_COLOR_RGB(0, 0, 0);
        EVE_CLEAR(1,1,1);

#if IS_EVE_API(5)
        // Central gradient background for EVE 5
        EVE_CMD_CGRADIENT(EVE_CGRADIENT_CORNER_ZERO, 0, 0, EVE_DISP_WIDTH, EVE_DISP_HEIGHT, 0x000020, 0x000080);
#else
        // Standard gradient background for EVE 1,2,3,4
        EVE_CMD_GRADIENT(0, 0, 0x000020, EVE_DISP_WIDTH, EVE_DISP_HEIGHT, 0x000080);
#endif

        animate_fish();
        animate_bubbles();

        animate_prop();

        compass_bulkhead(x_compass, y_compass, compass_radius, OPT_COMPASS_BULKHEAD_RECT | OPT_COMPASS_TRANSPARENT /*| OPT_COMPASS_BULKHEAD_USE_FONT*/, heading);
        sub_depth(x_depth, y_depth, depth_width, depth_height, OPT_COMPASS_TRANSPARENT, depth, 60 * SUB_UNITS_SCALE);

        EVE_DISPLAY();
        EVE_CMD_SWAP();
        EVE_LIB_EndCoProList();
        EVE_LIB_AwaitCoProEmpty();

        depth++;
        heading++;
    }
}

void eve_example(void)
{
    // Initialise the display
    EVE_Init();

    // Calibrate the display
    printf("Calibrating display...\n");
    eve_calibrate();

    // Start example code
    printf("Starting demo:\n");
    eve_display();
}
