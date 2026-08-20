/**
 * @file eve_example.c
 */
/*
 * ============================================================================
 * (C) Copyright,  Bridgetek Pte. Ltd.
 * ============================================================================
 *
 * This source code ("the Software") is provided by Bridgetek Pte Ltd
 * ("Bridgetek") subject to the licence terms set out
 * http://brtchip.com/BRTSourceCodeLicenseAgreement/ ("the Licence Terms").
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

/* INCLUDES ************************************************************************/

#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include <stdbool.h>

/* Include functions for EVE-MCU-Dev library API layer */
#include <EVE.h> 

#include "eve_example.h"

/* CONSTANTS ***********************************************************************/

// Configuration

#define WHEEL_FORMAT    EVE_FORMAT_RGB565

#if WHEEL_FORMAT == EVE_FORMAT_RGB565
#define WHEEL_STORAGE uint16_t
#elif WHEEL_FORMAT == EVE_FORMAT_ARGB1555
#define WHEEL_STORAGE uint16_t
#elif WHEEL_FORMAT == EVE_FORMAT_RGB332
#define WHEEL_STORAGE uint8_t
#else
#error Unsupported format for colour picker wheel bitmap
#endif

// Bitmap Size and Storage Location

#define WHEEL_SIZE      250
#define WHEEL_RAMG_ADDR 0

// Screen Layout

#define WHEEL_HANDLE    0
#define WHEEL_TAG       10
#define TAG_BACKGROUND  255

/* LOCAL VARIABLES *****************************************************************/

static WHEEL_STORAGE wheelline[WHEEL_SIZE];

/* LOCAL FUNCTIONS / INLINES *******************************************************/

// Convert RGB8 to RGB565
static uint16_t rgb888_to_rgb565(uint8_t r, uint8_t g, uint8_t b)
{
    return ((r & 0xF8) << 8) |
        ((g & 0xFC) << 3) |
        (b >> 3);
}

// Convert RGB8 to ARG1555
static uint16_t rgb888_to_argb1555(uint8_t r, uint8_t g, uint8_t b)
{
    uint16_t a = 0;
    if (r + g + b > 0) a = 1 << 15;

    return a | ((r & 0xF8) << 7) |
        ((g & 0xF8) << 2) |
        (b >> 3);
}

// Convert RGB8 to RGB332
static uint8_t rgb888_to_rgb332(uint8_t r, uint8_t g, uint8_t b)
{
    return ((r & 0xe0)) |
        ((g & 0xe0) >> 3) |
        ((b & 0xc0) >> 6);
}

// Convert RGB565 to RGB8
static void rgb565_to_rgb(uint16_t rgb565, uint8_t *r, uint8_t *g, uint8_t *b)
{
    *r = (rgb565 >> 8) & 0xf8;
    *g = (rgb565 >> 3) & 0xfc;
    *b = (rgb565 << 3) & 0xf8;
}

// Convert ARGB1555 to RGB8
static void argb1555_to_rgb(uint16_t argb1555, uint8_t *r, uint8_t *g, uint8_t *b)
{
    *r = (argb1555 >> 7) & 0xf8;
    *g = (argb1555 >> 2) & 0xf8;
    *b = (argb1555 << 3) & 0xf8;
}

// Convert RGB332 to RGB8
static void rgb332_to_rgb(uint8_t rgb565, uint8_t *r, uint8_t *g, uint8_t *b)
{
    *r = (rgb565) & 0xe0;
    *g = (rgb565 << 3) & 0xe0;
    *b = (rgb565 << 6) & 0xc0;
}

// Hue/Saturation/Brightness (HSV colour space) to RGB8 conversion
static void hsv_to_rgb(float h, float s, float v, uint8_t* r, uint8_t* g, uint8_t* b)
{
    float c = v * s;
    float x = c * (1 - fabsf(fmodf(h * 6.0f, 2.0f) - 1));
    float m = v - c;
 
    float rp = 0, gp = 0, bp = 0;
 
    if (h < 1.0f / 6.0f) { rp = c; gp = x; }
    else if (h < 2.0f / 6.0f) { rp = x; gp = c; }
    else if (h < 3.0f / 6.0f) { gp = c; bp = x; }
    else if (h < 4.0f / 6.0f) { gp = x; bp = c; }
    else if (h < 5.0f / 6.0f) { rp = x; bp = c; }
    else { rp = c; bp = x; }
 
    *r = (uint8_t)((rp + m) * 255);
    *g = (uint8_t)((gp + m) * 255);
    *b = (uint8_t)((bp + m) * 255);
}

// Colour Wheel Generation
static void generate_colour_wheel(void)
{
    int cx = (WHEEL_SIZE / 2);
    int cy = (WHEEL_SIZE / 2);
 
    for (int y = 0; y < WHEEL_SIZE; y++)
    {
        for (int x = 0; x < WHEEL_SIZE; x++)
        {
            float dx = (float)(x - cx);
            float dy = (float)(y - cy);
            float dist = (sqrtf(dx * dx + dy * dy) * 2) / WHEEL_SIZE;
 
            uint8_t r = 0, g = 0, b = 0;
 
            if (dist <= 1.0f)
            {
                float angle = atan2f(dy, dx);
                if (angle < 0) angle += 2.0f * 3.1415926f;
 
                float h = angle / (2.0f * 3.1415926f);
                float s = dist;
                float v = 1.0f;
 
                hsv_to_rgb(h, s, v, &r, &g, &b);
            }
 
#if WHEEL_FORMAT == EVE_FORMAT_RGB565
            wheelline[x] = rgb888_to_rgb565(r, g, b);
#elif WHEEL_FORMAT == EVE_FORMAT_ARGB1555
            wheelline[x] = rgb888_to_argb1555(r, g, b);
#elif WHEEL_FORMAT == EVE_FORMAT_RGB332
            wheelline[x] = rgb888_to_rgb332(r, g, b);
#endif
        }
    
        /* Upload one line of the bitmap to RAM_G */
        EVE_LIB_WriteDataToRAMG(
            (uint8_t*)wheelline,
            sizeof(wheelline),
            WHEEL_RAMG_ADDR + (y * sizeof(wheelline))
        );
    }
}

/* FUNCTIONS ***********************************************************************/

void eve_display(void)
{
    // Colour wheel position on screen
    const uint16_t image_x = 50;
    const uint16_t image_y = 50;

    // Gap between colour wheel and info
    const uint16_t gap_x = 50;
    const uint16_t gap_y = 50;

    uint32_t touch_xy = 0;
    uint32_t touch_x = 0;
    uint32_t touch_y = 0;

    uint8_t tag_val = 0;

    uint32_t touch_cursor_x = 0;
    uint32_t touch_cursor_y = 0;

    uint8_t cr = 0;
    uint8_t cg = 0;
    uint8_t cb = 0;

    bool tag_outofrange = false;

    while(1)
    {
        // Begin a new co-processor list
        EVE_LIB_BeginCoProList();
        // Begin a display list
        EVE_CMD_DLSTART();
        // Tag will read as TAG_BACKGROUND (255) for any untagged areas of the screen when touched
        EVE_CLEAR_TAG(TAG_BACKGROUND);
        // Clear the screen with black
        EVE_CLEAR_COLOR_RGB(0, 0, 0);
        // clear colour, tag, stencil
        EVE_CLEAR(1, 1, 1);
        // Ensure following items are not tagged
        EVE_TAG_MASK(0);

        // The generated colour wheel bitmap has a hard, non-anti-aliased round edge.
        // To smooth the edge, we draw an invisible circle and use blending so that the bitmap is
        // visible only within the circle. This gives the colour wheel a cleaner, anti-aliased outline.
        // The same technique can also be applied to other shapes, such as lines and rectangles.

        // Alpha mask on only
        EVE_COLOR_MASK(0, 0, 0, 1);
        // Draw points
        EVE_BEGIN(EVE_BEGIN_POINTS);
        // Draw circle diameter of colour picker wheel
        EVE_POINT_SIZE((WHEEL_SIZE / 2) * 16);
        // Position at the center of where the colour picker will be (colour picker is 250x250)
        EVE_VERTEX2F((image_x + (WHEEL_SIZE / 2)) * 16, (image_y + (WHEEL_SIZE / 2)) * 16);
        // End points
        EVE_END();

        // Enable colours, alpha disabled
        EVE_COLOR_MASK(1, 1, 1, 0);
        // Blend the bitmap colours into the cicle present in the alpha bufffer
        EVE_BLEND_FUNC(EVE_BLEND_DST_ALPHA, EVE_BLEND_ONE_MINUS_DST_ALPHA);

        // Place the colour picker circle at the coordinates defined previously
        EVE_BEGIN(EVE_BEGIN_BITMAPS);
        EVE_VERTEX2II(image_x, image_y, WHEEL_HANDLE, 0);
        EVE_END();

        // Set the blending back to default so that any later items will not be affected
        EVE_BLEND_FUNC(EVE_BLEND_SRC_ALPHA, EVE_BLEND_ONE_MINUS_SRC_ALPHA);		
        // Re-enabble colours and alpha
        EVE_COLOR_MASK(1, 1, 1, 1);

        // If screen has been touched outwith the colour picker, draw a red circle to alert user that they have touched invalid area
        // This will be a red ring with inner diameter 138 and outer diameter of 142
        // This is updated after the display list and so uses the value here on the next time the screen is rendered
        if (tag_outofrange)
        {
            //save graphics context
            EVE_SAVE_CONTEXT();

            // Alpha mask on only
            EVE_COLOR_MASK(0, 0, 0, 1);
            EVE_COLOR_A(255);
            // Clear the colour buffer
            EVE_CLEAR(1, 0, 0);

            // Draw points
            EVE_BEGIN(EVE_BEGIN_POINTS);

            // Add shape into alpha buffer
            EVE_BLEND_FUNC(EVE_BLEND_ONE, EVE_BLEND_ONE_MINUS_SRC_ALPHA);
            // Draw circle diameter slightly larger than colour picker circle
            EVE_POINT_SIZE(((WHEEL_SIZE / 2) + 10) * 16);
            // Position at the center of where the colour picker will be (colour picker is 250x250)
            EVE_VERTEX2F((image_x + (WHEEL_SIZE / 2)) * 16, (image_y + (WHEEL_SIZE / 2)) * 16);

            // Remove shape from alpha buffer
            EVE_BLEND_FUNC(EVE_BLEND_ZERO, EVE_BLEND_ONE_MINUS_SRC_ALPHA);
            // Draw a circle diameter slightly smaller than the outer circle
            EVE_POINT_SIZE(((WHEEL_SIZE / 2) + 1) * 16);
            // Draw the circle at the center of where the colour picker will be
            EVE_VERTEX2F((image_x + (WHEEL_SIZE / 2)) * 16, (image_y + (WHEEL_SIZE / 2)) * 16);

            // Turn the colour back on
            EVE_COLOR_MASK(1, 1, 1, 1);
            // Blend into the shape we created in the alpha buffer
            EVE_BLEND_FUNC(EVE_BLEND_DST_ALPHA, EVE_BLEND_ONE_MINUS_DST_ALPHA);
            // Draw in red
            EVE_COLOR_RGB(255, 0, 0);
            // Set the operation to draw 
            EVE_POINT_SIZE(((WHEEL_SIZE / 2) + 10) * 16);
            // Draw the circle at the center of where the colour picker will be
            EVE_VERTEX2F((image_x + (WHEEL_SIZE / 2)) * 16, (image_y + (WHEEL_SIZE / 2)) * 16);

            // Finish drawing points
            EVE_END();

            // Restore graphics context
            EVE_RESTORE_CONTEXT();
        }

        // Print some details for debugging purposes
        // Touch X,Y coordinates
        EVE_CMD_TEXT((image_x + WHEEL_SIZE + gap_x), (image_x + gap_y + 20), 26, EVE_OPT_FORMAT , "X : %d ", touch_x);
        EVE_CMD_TEXT((image_x + WHEEL_SIZE + gap_x), (image_x + gap_y + 40), 26, EVE_OPT_FORMAT , "Y : %d ", touch_y);
        // Red, green and blue values selected on the colour picker
        EVE_CMD_TEXT((image_x + WHEEL_SIZE + gap_x), (image_x + gap_y + 70), 26, EVE_OPT_FORMAT  , "R : %d ", cr);
        EVE_CMD_TEXT((image_x + WHEEL_SIZE + gap_x), (image_x + gap_y + 90), 26, EVE_OPT_FORMAT , "G : %d ", cg);
        EVE_CMD_TEXT((image_x + WHEEL_SIZE + gap_x), (image_x + gap_y + 110), 26, EVE_OPT_FORMAT , "B : %d ", cb);
        // Tag value of selection
        EVE_CMD_TEXT((image_x + WHEEL_SIZE + gap_x), (image_x + gap_y + 140), 26, EVE_OPT_FORMAT , "Tag : %d ", tag_val);

        // Draw a rectangle which will display the colour chosen on the colour picker
        // This rectangle and the above debug output are positioned to the right hand side of the colour picker itself
        // The line width allows it to have rounded corners
        EVE_BEGIN(EVE_BEGIN_RECTS);
        EVE_LINE_WIDTH(10 * 16);
        EVE_COLOR_RGB((cr), (cg), (cb));
        EVE_VERTEX2F((image_x + WHEEL_SIZE + gap_x) * 16, image_x * 16);
        EVE_VERTEX2F((image_x + WHEEL_SIZE + gap_x + gap_x) * 16, (image_x + gap_y) * 16);
        EVE_END();

        // Make a set of cursor lines to mark the current touch point
        // Set the colour to black and the line width to 1 pixel
        // Note that this could be extended to display a small bitmap arrow or target shape
        // Or to adjust the cursor colour to make it most clear against the colour picker background.
        EVE_BEGIN(EVE_BEGIN_LINES);
        EVE_COLOR_RGB(0, 0, 0);
        EVE_LINE_WIDTH(1 * 16);
        // Vertical line
        // 10 pixels below the center of the cursor point
        EVE_VERTEX2F(touch_cursor_x * 16, (touch_cursor_y - 10) * 16);
        // to 10 pixels above the center of the cursor point
        EVE_VERTEX2F(touch_cursor_x * 16, (touch_cursor_y + 10) * 16);
        // Horizontal line
        // 10 pixels to the left of the center of the cursor point
        EVE_VERTEX2F((touch_cursor_x - 10) * 16, touch_cursor_y * 16);
        // 10 pixels to the right of the center of the cursor point
        EVE_VERTEX2F((touch_cursor_x + 10) * 16, touch_cursor_y * 16);
        EVE_END();

        // Draw an invisible circle tagged with tag 10
        // This circle is used as the colour picker image is actually a square shape and we don't want the tag to be valid on the corners
        // By using the invisible circle, only the actual colour wheel area is tagged.
        // Enable tagging
        EVE_TAG_MASK(1);
        // Set tag to 10 for any items drawn after this
        EVE_TAG(WHEEL_TAG);
        // Begin points
        EVE_BEGIN(EVE_BEGIN_POINTS);
        // Disable writes to the R, G and B and so draw to the alpha buffer only
        EVE_COLOR_MASK(0, 0, 0, 1);
        // Draw a circle slightly smaller than the radius of the colour picker
        EVE_POINT_SIZE(((WHEEL_SIZE - 2) / 2) * 16);
        // Draw the circle at the center of where the colour picker will be
        EVE_VERTEX2F((image_x + (WHEEL_SIZE / 2)) * 16, (image_y + (WHEEL_SIZE / 2)) * 16);
        // Disable tagging for any subsequent items
        EVE_TAG_MASK(0);

        // Finish the display list
        EVE_DISPLAY();
        // Swap onto the screen to make it visible
        EVE_CMD_SWAP();
        // Finish the list of commands
        EVE_LIB_EndCoProList();
        // Await for the commands to be executed
        EVE_LIB_AwaitCoProEmpty();

        // Read the current Tag value and the coordinates used to find it
        // Get Tag value
        tag_val = EVE_LIB_MemRead32(EVE_REG_TOUCH_TAG);
        // Get coordinates
        touch_xy = EVE_LIB_MemRead32(EVE_REG_TOUCH_TAG_XY);
        // Get the X and Y values of the coordinate
        touch_y = (touch_xy & 0x0000FFFF);
        touch_x = ((touch_xy & 0xFFFF0000) >> 16);

        if(tag_val == 10)
        {
            uint32_t pixel_addr = 0;
            uint32_t pixel_val = 0;

            // Remove any offset from the top-left position of image
            uint32_t img_touch_x= touch_x - image_x;
            uint32_t img_touch_y = touch_y - image_y;

            // read through the image data to find byte corresponding to the coordinate
            // start address in RAM_G + (Y * length of line) + X
            pixel_addr = WHEEL_RAMG_ADDR + ((img_touch_x * (sizeof(WHEEL_STORAGE))) + (img_touch_y * WHEEL_SIZE * (sizeof(WHEEL_STORAGE))));
            pixel_val = EVE_LIB_MemRead32(pixel_addr);

            if ((sizeof(WHEEL_STORAGE)) == 2)
            {
                // Correct uint32_t to 16 bit pixel
                pixel_val = pixel_val >> ((pixel_addr & 1) * 16);
            }
            else if ((sizeof(WHEEL_STORAGE)) == 1)
            {
                // Correct uint32_t to 8 bit pixel
                pixel_val = pixel_val >> ((pixel_addr & 3) * 8);
            }

#if WHEEL_FORMAT == EVE_FORMAT_RGB565
            rgb565_to_rgb(pixel_val, &cr, &cg, &cb);
#elif WHEEL_FORMAT == EVE_FORMAT_ARGB1555
            argb1555_to_rgb(pixel_val, &cr, &cg, &cb);
#elif WHEEL_FORMAT == EVE_FORMAT_RGB332
            rgb332_to_rgb(pixel_val, &cr, &cg, &cb);
#endif
            // Also set the coordinate for the cursor (relative to the screen and not just the image)
            touch_cursor_x = touch_x; 
            touch_cursor_y = touch_y;
        }

        // If the screen is touched but with tag TAG_BACKGROUND (255) which we set 
        // with CLEAR_TAG() at the start of the display list

        // Then the user is touching a part of the screen outwith the colour picker
        // Set a variable to cause the red ring to be shown around the colour picker
        if (tag_val == TAG_BACKGROUND)
        {
            tag_outofrange = true;
        }
        else
        {
            tag_outofrange = false;
        }
    }
}

// Application Code begins here
void eve_example(void)
{
    // Initialise the display
    EVE_DEBUG_PRINTF("Initialising display...\n");
    if (EVE_Init() != 0)
    {
        EVE_DEBUG_ERROR("ERROR: EVE_Init() failed.\n");
        while (1);
    }

    // Calibrate the display
    EVE_DEBUG_PRINTF("Calibrating display...\n");
    if (eve_calibrate() != 0)
    {
        EVE_DEBUG_ERROR("ERROR: eve_calibrate() failed.\n");
    }

    // Generate a colour wheel in RAM_G
    EVE_DEBUG_PRINTF("Generating Colour Wheel...\n");
    generate_colour_wheel();

    // Load the colour wheel as a bitmap into a handle
    EVE_DEBUG_PRINTF("configuring Colour Wheel...\n");
    EVE_LIB_BeginCoProList();
    EVE_CMD_DLSTART();
    EVE_BEGIN(EVE_BEGIN_BITMAPS);
    EVE_BITMAP_HANDLE(WHEEL_HANDLE);
    EVE_CMD_SETBITMAP(WHEEL_RAMG_ADDR, WHEEL_FORMAT, 250, 250);
    EVE_BITMAP_SIZE(EVE_FILTER_NEAREST, EVE_WRAP_BORDER, EVE_WRAP_BORDER, 250, 250);
    EVE_DISPLAY();
    EVE_CMD_SWAP();
    EVE_LIB_EndCoProList();
    EVE_LIB_AwaitCoProEmpty();

    // Start example code
    EVE_DEBUG_PRINTF("Starting demo:\n");
    eve_display();          // Run Application
}
