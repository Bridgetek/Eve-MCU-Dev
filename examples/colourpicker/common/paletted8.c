/**
 * @file image_paletted8.c
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

/* Include EVE-MCU-Dev library API layer and other required headers. */
#include <eve_example.h>

#if (EXAMPLE_COLOURPICKER == EXAMPLE_COLOURPICKER_PALETTED_RECT) || \
    (EXAMPLE_COLOURPICKER == EXAMPLE_COLOURPICKER_PALETTED_IMAGE)

/* CONSTANTS ***********************************************************************/

/* Configuration */

/* Palleted bitmap storage Location */
// Address to put LUT in RAM_G
#if IS_EVE_API(1)
#define IMAGE_PAL_LUT_RAMG_ADDR     EVE_RAM_PAL
#else // !IS_EVE_API(1)
#define IMAGE_PAL_LUT_RAMG_ADDR     0
#endif // IS_EVE_API(1)
// Address to put Index in RAM_G
#define IMAGE_PAL_INDEX_RAMG_ADDR   1024

/* Handles and Tags */
#define IMAGE_PAL_HANDLE    0
#define IMAGE_PAL_TAG       10
#define TAG_BACKGROUND      255

/* EXTERN VARIABLES ****************************************************************/

/* Paletted bitmap Size */
extern const uint32_t img_pal_width;
extern const uint32_t img_pal_height;
/* Paletted bitmap LUT */
extern const uint8_t pal_lut[];
extern const uint32_t pal_lut_size;
/* Paletted bitmap index data */
extern const uint8_t pal_index[];
extern const uint32_t pal_index_size;

/* LOCAL VARIABLES *****************************************************************/

/* LOCAL FUNCTIONS / INLINES *******************************************************/

/* FUNCTIONS ***********************************************************************/

void eve_display_paletted8(void)
{
    // Colour picker position on screen
    uint16_t const image_x = 10;
    uint16_t const image_y = 10;

    uint16_t info_x = image_x + img_pal_width + 10;
    if (info_x + 100 > EVE_DISP_WIDTH)
    {
        info_x = EVE_DISP_WIDTH - 100;
    }
    uint16_t info_y = 130;

    uint32_t img_touch_x = 0;
    uint32_t img_touch_y = 0;

    uint32_t touch_pointer_x = 0;
    uint32_t touch_pointer_y = 0;

    uint32_t touch_xy = 0;
    uint32_t touch_x = 0;
    uint32_t touch_y = 0;
    uint8_t tag_val = 0;

    uint32_t colorbytesloc = 0;
    uint32_t colorbytes = 0;

    uint32_t cr = 0;
    uint32_t cg = 0;
    uint32_t cb = 0;

    uint32_t colorbyteindex = 0;

    // Copy Look Up Table (LUT) to RAM_G
    // Copy to address IMAGE_PAL_LUT_RAMG_ADDR
    EVE_LIB_WriteDataToRAMG(pal_lut, pal_lut_size, IMAGE_PAL_LUT_RAMG_ADDR);

    // Copy Index date to RAM_G
    // Inflate to address IMAGE_PAL_INDEX_RAMG_ADDR
    EVE_LIB_BeginCoProList();
#if !IS_EVE_API(5)
    EVE_CMD_INFLATE(IMAGE_PAL_INDEX_RAMG_ADDR);
#else // IS_EVE_API(5)
    EVE_CMD_INFLATE(IMAGE_PAL_INDEX_RAMG_ADDR, 0);
#endif // !IS_EVE_API(5)
    EVE_LIB_WriteDataToCMD(pal_index, pal_index_size);
    EVE_LIB_EndCoProList();
    EVE_LIB_AwaitCoProEmpty();

    // Instantiate the paletted bitmap
    EVE_LIB_BeginCoProList();
    EVE_CMD_DLSTART();
    EVE_BITMAP_HANDLE(IMAGE_PAL_HANDLE);
#if !IS_EVE_API(1)
    EVE_CMD_SETBITMAP(IMAGE_PAL_INDEX_RAMG_ADDR, EVE_FORMAT_PALETTED8, img_pal_width, img_pal_height);
#else // IS_EVE_API(1)
    EVE_BITMAP_SOURCE(IMAGE_PAL_INDEX_RAMG_ADDR);
    EVE_BITMAP_LAYOUT(EVE_FORMAT_PALETTED, img_pal_width, img_pal_height);
    EVE_BITMAP_SIZE(EVE_FILTER_BILINEAR, EVE_WRAP_BORDER, EVE_WRAP_BORDER, img_pal_width, img_pal_height);
#endif // !IS_EVE_API(1)
    EVE_DISPLAY();
    EVE_CMD_SWAP();
    EVE_LIB_EndCoProList();
    EVE_LIB_AwaitCoProEmpty();

    int i = 10;
    do
    {
        EVE_LIB_BeginCoProList();
        EVE_CMD_DLSTART();
        EVE_CLEAR_TAG(TAG_BACKGROUND);
        EVE_CLEAR_COLOR_RGB(0, 0, 0);
        EVE_CLEAR(1,1,1);

        // Set color to white for following objects.
        EVE_COLOR_RGB(255,255,255);

        // Disable Tagging.
        EVE_TAG_MASK(0);

        // Load the Paletted 8 image.
        // See the BT81x Programmers Guide for details of how to load a Paletted 8 image.
        // Blending is used to build the image up using the different color components.

        EVE_SCISSOR_XY(image_x, image_y);
        EVE_SCISSOR_SIZE(EVE_DISP_WIDTH - (image_x * 2), EVE_DISP_HEIGHT - (image_y * 2));

        EVE_BEGIN(EVE_BEGIN_BITMAPS);
        EVE_BITMAP_HANDLE(IMAGE_PAL_HANDLE);

#if !IS_EVE_API(1)
        // On EVE2 onwards draw paletted images using a blend function
        EVE_BLEND_FUNC(EVE_BLEND_ONE, EVE_BLEND_ZERO);
        //Draw Alpha channel
        EVE_COLOR_MASK(0,0,0,1);
        EVE_PALETTE_SOURCE(IMAGE_PAL_LUT_RAMG_ADDR + 3);
        EVE_VERTEX2II(image_x, image_y, IMAGE_PAL_HANDLE, 0);
        //Draw Red channel
        EVE_BLEND_FUNC(EVE_BLEND_DST_ALPHA, EVE_BLEND_ONE_MINUS_DST_ALPHA);
        EVE_COLOR_MASK(1,0,0,0);
        EVE_PALETTE_SOURCE(IMAGE_PAL_LUT_RAMG_ADDR + 2);
        EVE_VERTEX2II (image_x, image_y, IMAGE_PAL_HANDLE, 0);
        //Draw Green channel
        EVE_COLOR_MASK(0,1,0,0);
        EVE_PALETTE_SOURCE(IMAGE_PAL_LUT_RAMG_ADDR + 1);
        EVE_VERTEX2II(image_x, image_y, IMAGE_PAL_HANDLE, 0);
        //Draw Blue channel
        EVE_COLOR_MASK(0,0,1,0);
        EVE_PALETTE_SOURCE(IMAGE_PAL_LUT_RAMG_ADDR);
        EVE_VERTEX2II(image_x, image_y, IMAGE_PAL_HANDLE, 0);
#else // IS_EVE_API(1)
        EVE_COLOR_MASK(1,1,1,0);
        EVE_VERTEX2II(image_x, image_y, IMAGE_PAL_HANDLE, 0);
#endif // !IS_EVE_API(1)

        // Reset scissor
        EVE_SCISSOR_SIZE(2047,2047);
        EVE_SCISSOR_XY(0,0);

        // Return the color mask to all-enabled and the blend function
        // and color setting.
        // This ensures the actions used above to load the Paletted 8
        // image do not affect subsequent items on the screen.
        EVE_COLOR_MASK(1,1,1,1);
        EVE_BLEND_FUNC(EVE_BLEND_SRC_ALPHA, EVE_BLEND_ONE_MINUS_SRC_ALPHA);
        EVE_COLOR_RGB(255,255,255);

        // Print the details.
        // Touch X coordinate
        EVE_CMD_TEXT(info_x, info_y + 0, 26, 0, "X:");
        EVE_CMD_NUMBER(info_x + 30, info_y + 0, 26, 0, touch_x);
        // Touch Y coordinate
        EVE_CMD_TEXT(info_x, info_y + 20, 26, 0, "Y:");
        EVE_CMD_NUMBER(info_x + 30, info_y + 20, 26, 0, touch_y);
        // Red color value
        EVE_CMD_TEXT(info_x, info_y + 50, 26, 0, "R:");
        EVE_CMD_NUMBER(info_x + 30, info_y + 40, 26, 0, cr);
        // Green color value
        EVE_CMD_TEXT(info_x, info_y + 70, 26, 0, "G:");
        EVE_CMD_NUMBER(info_x + 30, info_y + 60, 26, 0, cg);
        // Blue color value
        EVE_CMD_TEXT(info_x, info_y + 90, 26, 0, "B:");
        EVE_CMD_NUMBER(info_x + 30, info_y + 80, 26, 0, cb);
        // Tag value
        EVE_CMD_TEXT(info_x, info_y + 110, 26, 0, "tag:");
        EVE_CMD_NUMBER(info_x + 30, info_y + 110, 26, 0, tag_val);

        // Draw a rectangle which will display the color chosen on the
        // color picker.
        // This rectangle and the above debug output are positioned to
        // the right hand side of the color picker itself.
        // The line width allows it to have rounded corners.
        EVE_BEGIN(EVE_BEGIN_RECTS);
        EVE_LINE_WIDTH(10 * 16);
        EVE_COLOR_RGB((cr), (cg), (cb));
        EVE_VERTEX2F((info_x + 10) * 16, (image_y + 20) * 16);
        EVE_VERTEX2F((info_x + 70) * 16, (image_y + 80) * 16);
        EVE_END();

        // Make a set of cursor lines to mark the current touch point.
        // Set the color to black and the line width to 1 pixel.
        // Note that this could be extended to display a small bitmap
        // arrow or target shape or to adjust the cursor color to make
        // it more clear against the color picker background.
        EVE_BEGIN(EVE_BEGIN_LINES);
        EVE_COLOR_RGB(0, 0, 0);
        EVE_LINE_WIDTH(1 * 16);
        // Vertical line
        // 10 pixels below the center of the cursor point
        EVE_VERTEX2F((touch_pointer_x) * 16,(touch_pointer_y - 10) * 16);
        // to 10 pixels above the center of the cursor point
        EVE_VERTEX2F((touch_pointer_x) * 16,(touch_pointer_y + 10) * 16);
        // Horizontal line
        // 10 pixels to the left of the center of the cursor point
        EVE_VERTEX2F((touch_pointer_x - 10) * 16,(touch_pointer_y) * 16);
        // 10 pixels to the right of the center of the cursor point
        EVE_VERTEX2F((touch_pointer_x + 10) * 16,(touch_pointer_y) * 16);
        EVE_END();


        // Draw an invisible rectangle tagged with tag IMAGE_PAL_TAG
        // You could tag the image instead when placing it on the screen but
        // this allows greater control over the area which you want to have
        // active.
        // In this case, the tagged area is 2 pixels smaller than the image
        // but this can be adjusted as desired.
        // Enable tagging
        EVE_TAG_MASK(1);
        // Set tag value
        EVE_TAG(IMAGE_PAL_TAG);
        // Make a rectangle
        EVE_BEGIN(EVE_BEGIN_RECTS);
        // Disable writes to Red, Green and Blue components
        EVE_COLOR_MASK(0,0,0,255);
        // Top left corner is 2 pixels inside the image
        EVE_VERTEX2F((image_x + 2) * 16,(image_y + 2) * 16);
        // Bottom Right corner is 2 pixels inside the image
        EVE_VERTEX2F((image_x + img_pal_width - 2) * 16,(image_y + img_pal_height - 2) * 16);
        // Disable tagging
        EVE_TAG_MASK(0);

        // Restore writes to all color channels so that if any items are added afterwards they are drawn correctly
        EVE_COLOR_MASK(255,255,255,255);
        // Finish the display list
        EVE_DISPLAY();
        // Swap onto the screen to make it visible
        EVE_CMD_SWAP();
        // Finish the list of commands
        EVE_LIB_EndCoProList();
        // Await for the commands to be executed
        EVE_LIB_AwaitCoProEmpty();

        // Read the touch data

        // Read the current Tag value and the coordinates used to find it
#if IS_EVE_API(1,2,3,4)
        // 8 bit read on EVE API 1-4
        tag_val = EVE_LIB_MemRead8(EVE_REG_TOUCH_TAG);
#else // !IS_EVE_API(1,2,3,4)
        // 32 bit read on EVE API 5
        tag_val = EVE_LIB_MemRead32(EVE_REG_TOUCH_TAG);
#endif // IS_EVE_API(1,2,3,4)
        // Get coordinates
        touch_xy = EVE_LIB_MemRead32(EVE_REG_TOUCH_TAG_XY);
        // Get the X and Y values of the coordinate
        touch_y = (touch_xy & 0x0000FFFF);
        touch_x = ((touch_xy & 0xFFFF0000) >> 16);

        // Process the touch data

        if (tag_val == 10)
        {
            // Remove any offset from the top-left position of image
            img_touch_x = touch_x - image_x;
            img_touch_y = touch_y - image_y;

            // Make sure that any coordinates obtained by the code above are restricted to within the color picker area
            // If any coordinate is outside, then force it to the respective boundary so the cursor cannot be dragged outside the color picker area
            if (img_touch_x < image_x)
                img_touch_x = image_x;
            else if (img_touch_x > (image_x + img_pal_width))
                img_touch_x = image_x + img_pal_width;

            if (img_touch_y < image_y)
                img_touch_y = image_y;
            else if (img_touch_y > (image_y + img_pal_height))
                img_touch_y = image_y + img_pal_height;

            // ################# Use the touch data to find the color from the image data #####################

            // Look through the image data to find the pixel being touched
            // Reading this will return the 8-bit index value which we can look up
            // Start address in RAM_G + X + (Y * length of line)
            colorbytesloc = IMAGE_PAL_INDEX_RAMG_ADDR + (img_touch_x + (img_touch_y * img_pal_width));

            // Can add additional error checking here.
            //if (colorbytesloc > (img_pal_width * img_pal_height))
            //	colorbytesloc = (img_pal_width * img_pal_height);
#if IS_EVE_API(1,2,3,4)
            // Read an 8 bit value from RAM_G
            colorbyteindex = EVE_LIB_MemRead8(colorbytesloc);
#else // !IS_EVE_API(1,2,3,4)
            // Read 32 bit value and shift result into the low 8 bits
            colorbyteindex = EVE_LIB_MemRead32(colorbytesloc & (~3));
            colorbyteindex = colorbyteindex >> ((colorbytesloc & 3) * 8);
            colorbyteindex &= 0xff;
#endif // IS_EVE_API(1,2,3,4)
            // Now read the actual color value from the LUT.
            // From the start of the LUT data, add the index value * 4 (as the index is for 32-bit values)
            colorbytes = EVE_LIB_MemRead32(IMAGE_PAL_LUT_RAMG_ADDR + (colorbyteindex * 4));

            // ARRR RRGG GGGB BBBB
            // find the R, G and B components which are 8-bit in this case as it is Paletted 8
            cb = (uint8_t)(colorbytes);
            cg = (uint8_t)(colorbytes >> 8);
            cr = (uint8_t)(colorbytes >>16);

            // Also set the value to be used for the cursor
            touch_pointer_x = touch_x;
            touch_pointer_y = touch_y;

        }
    } while (LOOP);

}

#endif // (EXAMPLE_COLOURPICKER == EXAMPLE_COLOURPICKER_PALETTED_RECT) || \
    // (EXAMPLE_COLOURPICKER == EXAMPLE_COLOURPICKER_PALETTED_IMAGE)
