/**
 * @file argb1555_multi.c
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

#if (EXAMPLE_COLOURPICKER == EXAMPLE_COLOURPICKER_MULTI)

/* CONSTANTS ***********************************************************************/

/* Configuration */

// Address to put image in RAM_G
#define IMAGE_ARGB_RAMG_ADDR   0

/* Handles and Tags */
#define IMAGE_ARGB_HANDLE    0
#define IMAGE_ARGB_TAG       10
#define TAG_BACKGROUND      255

/* Wheel bitmap Size */
#define PICKER_WIDTH     460
#define PICKER_HEIGHT    252

/* Storage for the wheel format chosen */
// EVE_FORMAT_ARGB1555 is 16-bits
#define PICKER_STORAGE uint16_t

#define PICKER_COLOUR(A) EVE_COLOR_RGB(A,A,A)

/* EXTERN VARIABLES ****************************************************************/

/* LOCAL VARIABLES *****************************************************************/

static PICKER_STORAGE pickerline[PICKER_WIDTH];

/* LOCAL FUNCTIONS / INLINES *******************************************************/

// Picker Generation.
static void generate_picker(void)
{
    int x,y;

    for (y = 0; y < PICKER_HEIGHT; y++)
    {
        for (x = 0; x < PICKER_WIDTH; x++)
        {
            uint8_t r = 0, g = 0, b = 0;
            float s = 1.0f;
            float v = 1.0f;
            float h = (1.0f * (float)x) / PICKER_WIDTH;

            if (y <= (PICKER_HEIGHT / 2))
            {
                v = (float)y / (float)(PICKER_HEIGHT / 2);
            }
            else 
            {
                s = (float)(PICKER_HEIGHT - y) / (float)(PICKER_HEIGHT / 2);
            }
 
            hsv_to_rgb(h, s, v, &r, &g, &b);
 
            pickerline[x] = rgb888_to_argb1555(r, g, b);
        }
    
        /* Upload one line of the bitmap to RAM_G */
        EVE_LIB_WriteDataToRAMG(
            (uint8_t*)pickerline,
            sizeof(pickerline),
            IMAGE_ARGB_RAMG_ADDR + (y * sizeof(pickerline))
        );
    }
}

/* FUNCTIONS ***********************************************************************/

void eve_display_argb1555(void)
{
    // Colour picker position on screen.
    uint16_t const image_x = 10;
    uint16_t const image_y = 10;

    uint16_t info_x = image_x + PICKER_WIDTH + 10;
    if (info_x + 130 > EVE_DISP_WIDTH)
    {
        info_x = EVE_DISP_WIDTH - 130;
    }
    uint16_t info_y = 130;

    uint32_t img_touch_x = 0;
    uint32_t img_touch_y = 0;

    uint32_t touch0_pointer_x = 0;
    uint32_t touch0_pointer_y = 0;
    uint32_t touch1_pointer_x = 60;
    uint32_t touch1_pointer_y = 50;

    uint32_t touch0_xy = 0;
    uint32_t touch0_x = 0;
    uint32_t touch0_y = 0;
    uint8_t tag0_val = 0;
    uint32_t touch1_xy = 0;
    uint32_t touch1_x = 0;
    uint32_t touch1_y = 0;
    uint8_t tag1_val = 0;

    uint32_t colorbytesloc = 0;
    uint32_t colorbytes = 0;

    uint8_t c0r = 0;
    uint8_t c0g = 0;
    uint8_t c0b = 0;
    uint8_t c1r = 0;
    uint8_t c1g = 0;
    uint8_t c1b = 0;

    uint32_t colorbyteindex = 0;

    // Generate a colour wheel in RAM_G.
    EVE_DEBUG_PRINTF("Generating Picker...\n");
    generate_picker();

    // Load the colour wheel as a bitmap into a handle.
    EVE_DEBUG_PRINTF("Configuring Picker...\n");
    // Instantiate the ARGB1555 bitmap.
    EVE_LIB_BeginCoProList();
    EVE_CMD_DLSTART();
    EVE_BITMAP_HANDLE(IMAGE_ARGB_HANDLE);
#if !IS_EVE_API(1)
    EVE_CMD_SETBITMAP(IMAGE_ARGB_RAMG_ADDR, EVE_FORMAT_ARGB1555, PICKER_WIDTH, PICKER_HEIGHT);
#else // IS_EVE_API(1)
    EVE_BITMAP_SOURCE(IMAGE_ARGB_RAMG_ADDR);
    EVE_BITMAP_LAYOUT(EVE_FORMAT_ARGB1555, PICKER_WIDTH * sizeof(PICKER_STORAGE), PICKER_HEIGHT);
    EVE_BITMAP_SIZE(EVE_FILTER_BILINEAR, EVE_WRAP_BORDER, EVE_WRAP_BORDER, PICKER_WIDTH, PICKER_HEIGHT);
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

        EVE_SCISSOR_XY(image_x, image_y);
        EVE_SCISSOR_SIZE(EVE_DISP_WIDTH - (image_x * 2), EVE_DISP_HEIGHT - (image_y * 2));

        // Draw the colour picker rectangle.
        EVE_BEGIN(EVE_BEGIN_BITMAPS);
        EVE_BITMAP_HANDLE(IMAGE_ARGB_HANDLE);
        EVE_COLOR_MASK(1,1,1,0);
        EVE_VERTEX2II(image_x, image_y, IMAGE_ARGB_HANDLE, 0);

        // Reset scissor.
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
        // Touch X coordinate.
        EVE_CMD_TEXT(info_x, info_y + 0, 26, 0, "X:");
        EVE_CMD_NUMBER(info_x + 30, info_y + 0, 26, 0, touch0_x);
        EVE_CMD_NUMBER(info_x + 80, info_y + 0, 26, 0, touch1_x);
        // Touch Y coordinate.
        EVE_CMD_TEXT(info_x, info_y + 20, 26, 0, "Y:");
        EVE_CMD_NUMBER(info_x + 30, info_y + 20, 26, 0, touch0_y);
        EVE_CMD_NUMBER(info_x + 80, info_y + 20, 26, 0, touch1_y);
        // Red color value.
        EVE_CMD_TEXT(info_x, info_y + 50, 26, 0, "R:");
        EVE_CMD_NUMBER(info_x + 30, info_y + 50, 26, 0, c0r);
        EVE_CMD_NUMBER(info_x + 80, info_y + 50, 26, 0, c1r);
        // Green color value.
        EVE_CMD_TEXT(info_x, info_y + 70, 26, 0, "G:");
        EVE_CMD_NUMBER(info_x + 30, info_y + 70, 26, 0, c0g);
        EVE_CMD_NUMBER(info_x + 80, info_y + 70, 26, 0, c1g);
        // Blue color value.
        EVE_CMD_TEXT(info_x, info_y + 90, 26, 0, "B:");
        EVE_CMD_NUMBER(info_x + 30, info_y + 90, 26, 0, c0b);
        EVE_CMD_NUMBER(info_x + 80, info_y + 90, 26, 0, c1b);
        // Tag value.
        EVE_CMD_TEXT(info_x, info_y + 110, 26, 0, "tag:");
        EVE_CMD_NUMBER(info_x + 30, info_y + 110, 26, 0, tag0_val);
        EVE_CMD_NUMBER(info_x + 80, info_y + 110, 26, 0, tag1_val);

        // Draw a rectangle which will display the color chosen on the
        // color picker.
        // This rectangle and the above debug output are positioned to
        // the right hand side of the color picker itself.
        // The line width allows it to have rounded corners.
        EVE_BEGIN(EVE_BEGIN_RECTS);
        EVE_LINE_WIDTH(10 * 16);
        EVE_COLOR_RGB((c0r), (c0g), (c0b));
        EVE_VERTEX2F((info_x + 30) * 16, (image_y + 20) * 16);
        EVE_VERTEX2F((info_x + 50) * 16, (image_y + 80) * 16);
        EVE_COLOR_RGB((c1r), (c1g), (c1b));
        EVE_VERTEX2F((info_x + 80) * 16, (image_y + 20) * 16);
        EVE_VERTEX2F((info_x + 100) * 16, (image_y + 80) * 16);
        EVE_END();

        // Make a set of cursor lines to mark the current touch point.
        // Note that this could be extended to display a small bitmap
        // arrow or target shape or to adjust the cursor color to make
        // it more clear against the color picker background.
        EVE_BEGIN(EVE_BEGIN_LINES);
        EVE_LINE_WIDTH(1 * 16);
        // Cross Hair point 0.
        // Vertical line, contrasting colour.
        PICKER_COLOUR(255 - ((touch0_pointer_y - image_y) * PICKER_HEIGHT) / PICKER_HEIGHT);
        // 10 pixels below the center of the cursor point.
        EVE_VERTEX2F((touch0_pointer_x) * 16,(touch0_pointer_y - 10) * 16);
        // to 10 pixels above the center of the cursor point
        EVE_VERTEX2F((touch0_pointer_x) * 16,(touch0_pointer_y + 10) * 16);
        // Horizontal line.
        // 10 pixels to the left of the center of the cursor point.
        EVE_VERTEX2F((touch0_pointer_x - 10) * 16,(touch0_pointer_y) * 16);
        // 10 pixels to the right of the center of the cursor point.
        EVE_VERTEX2F((touch0_pointer_x + 10) * 16,(touch0_pointer_y) * 16);

        // Cross Hair point 1.
        // Vertical line, contrasting colour.
        PICKER_COLOUR(255 - ((touch1_pointer_y - image_y) * PICKER_HEIGHT) / PICKER_HEIGHT);
        // 10 pixels below the center of the cursor point.
        EVE_VERTEX2F((touch1_pointer_x) * 16,(touch1_pointer_y - 10) * 16);
        // to 10 pixels above the center of the cursor point.
        EVE_VERTEX2F((touch1_pointer_x) * 16,(touch1_pointer_y + 10) * 16);
        // Horizontal line.
        // 10 pixels to the left of the center of the cursor point.
        EVE_VERTEX2F((touch1_pointer_x - 10) * 16,(touch1_pointer_y) * 16);
        // 10 pixels to the right of the center of the cursor point.
        EVE_VERTEX2F((touch1_pointer_x + 10) * 16,(touch1_pointer_y) * 16);
        EVE_END();

        // As there are two different touch points / cursors, 
        // we add a letter A and B to label them.
        // Note that we use an offset of +3 in the X direction and -20 in the 
        // Y direction compared to the center of the cursor.
        // The font coordinate is for the Top-Left of the letter A or B and so 
        // this offset positions it in the top-left quadrant of the cursor
        // without any overlap with the cursor lines.
        // Use a contrasting colour.
        EVE_BEGIN(EVE_BEGIN_BITMAPS);
        EVE_BITMAP_HANDLE(22);
        // Using font 22, label point 0 as "A" which is cell 0x41 of the font (ANSI code 0x41 = A)
        EVE_CELL(0x41);
        PICKER_COLOUR(255 - ((touch0_pointer_y - image_y) * PICKER_HEIGHT) / PICKER_HEIGHT);
        EVE_VERTEX2F((touch0_pointer_x + 3) * 16, (touch0_pointer_y - 20) * 16);
        // Using font 22, label point 1 as "B" which is cell 0x42 of the font (ANSI code 0x42 = B)
        EVE_CELL(0x42);
        PICKER_COLOUR(255 - ((touch0_pointer_y - image_y) * PICKER_HEIGHT) / PICKER_HEIGHT);
        EVE_VERTEX2F((touch1_pointer_x + 3) * 16, (touch1_pointer_y - 20) * 16);
        EVE_END();

        // Draw an invisible rectangle tagged with tag IMAGE_ARGB_TAG
        // You could tag the image instead when placing it on the screen but
        // this allows greater control over the area which you want to have
        // active.
        // In this case, the tagged area is 2 pixels smaller than the image
        // but this can be adjusted as desired.
        // Enable tagging.
        EVE_TAG_MASK(1);
        // Set tag value.
        EVE_TAG(IMAGE_ARGB_TAG);
        // Make a rectangle.
        EVE_BEGIN(EVE_BEGIN_RECTS);
        // Disable writes to Red, Green and Blue components.
        EVE_COLOR_MASK(0,0,0,255);
        // Top left corner is 2 pixels inside the image.
        EVE_VERTEX2F((image_x + 2) * 16,(image_y + 2) * 16);
        // Bottom Right corner is 2 pixels inside the image.
        EVE_VERTEX2F((image_x + PICKER_WIDTH - 2) * 16,(image_y + PICKER_HEIGHT - 2) * 16);
        // Disable tagging
        EVE_TAG_MASK(0);

        // Restore writes to all color channels so that if any items are 
        // added afterwards they are drawn correctly.
        EVE_COLOR_MASK(255,255,255,255);
        // Finish the display list.
        EVE_DISPLAY();
        // Swap onto the screen to make it visible.
        EVE_CMD_SWAP();
        // Finish the list of commands.
        EVE_LIB_EndCoProList();
        // Await for the commands to be executed.
        EVE_LIB_AwaitCoProEmpty();

        // Read the touch data

        // Read the current Tag value and the coordinates used to find it.
#if IS_EVE_API(1,2,3,4)
        // 8 bit read on EVE API 1-4
        tag0_val = EVE_LIB_MemRead8(EVE_REG_TOUCH_TAG);
        tag1_val = EVE_LIB_MemRead8(EVE_REG_TOUCH_TAG1);
#else // !IS_EVE_API(1,2,3,4)
        // 32 bit read on EVE API 5
        tag0_val = EVE_LIB_MemRead32(EVE_REG_TOUCH_TAG);
        tag1_val = EVE_LIB_MemRead32(EVE_REG_TOUCH_TAG1);
#endif // IS_EVE_API(1,2,3,4)
        // Get coordinates
        touch0_xy = EVE_LIB_MemRead32(EVE_REG_CTOUCH_TOUCH0_XY);
#if IS_EVE_API(1,2,3,4)
        // Use compatibility mode
        touch1_xy = EVE_LIB_MemRead32(EVE_REG_CTOUCH_TOUCH1_XY);
#else // IS_EVE_API(5)
        // Extended mode set by default on EVE5 onwards
        touch1_xy = EVE_LIB_MemRead32(EVE_REG_CTOUCH_TOUCHA_XY);
#endif
        // Get the X and Y values of the coordinate
        touch0_y = (touch0_xy & 0x0000FFFF);
        touch0_x = ((touch0_xy & 0xFFFF0000) >> 16);
        touch1_y = (touch1_xy & 0x0000FFFF);
        touch1_x = ((touch1_xy & 0xFFFF0000) >> 16);

        // Process the touch data

        // If there is a touch on Tag 0
        if (touch0_xy != 0x80008000)
        {
            // If the touch is on Tag 10 (Cursor A) then take the coordinates 
            // into the point 0 (Cursor A) variables.
            if (tag0_val == IMAGE_ARGB_TAG)
            {
                touch0_pointer_x = touch0_x;
                touch0_pointer_y = touch0_y;
            }
            // Or if the touch is on Tag 11 (Cursor B) then take the coordinates 
            // into the point 1 (Cursor B) variables.
            else if (tag0_val == IMAGE_ARGB_TAG + 1)
            {
                touch1_pointer_x = touch0_x;
                touch1_pointer_y = touch0_y;
            }
        }

        // If there is a touch on Tag 1.
        if (touch1_xy != 0x80008000)
        {
            // If the touch is on Tag 10 (Cursor A) then take the coordinates 
            // into the point 0 (Cursor A) variables.
            if (tag1_val == IMAGE_ARGB_TAG)
            {
                touch0_pointer_x = touch1_x;
                touch0_pointer_y = touch1_y;
            }
            // Or if the touch is on Tag 11 (Cursor B) then take the coordinates 
            // into the point 1 (Cursor B) variables.
            else if (tag1_val == IMAGE_ARGB_TAG + 1)
            {
                touch1_pointer_x = touch1_x;
                touch1_pointer_y = touch1_y;
            }
        }

        // Make sure that any coordinates obtained by the code above 
        // are restricted to within the color picker area.
        // If any coordinate is outside, then force it to the respective 
        // boundary so the cursor cannot be dragged outside the color picker area.
        if (touch0_pointer_x < image_x)
            touch0_pointer_x = image_x;
        else if (touch0_pointer_x > (uint32_t)(image_x + PICKER_WIDTH))
            touch0_pointer_x = image_x + PICKER_WIDTH;

        if (touch0_pointer_y < image_y)
            touch0_pointer_y = image_y;
        else if (touch0_pointer_y > (uint32_t)(image_y + PICKER_HEIGHT))
            touch0_pointer_y = image_y + PICKER_HEIGHT;

        if (touch1_pointer_x < image_x)
            touch1_pointer_x = image_x;
        else if (touch1_pointer_x > (uint32_t)(image_x + PICKER_WIDTH))
            touch1_pointer_x = image_x + PICKER_WIDTH;

        if (touch1_pointer_y < image_y)
            touch1_pointer_y = image_y;
        else if (touch1_pointer_y > (uint32_t)(image_y + PICKER_HEIGHT))
            touch1_pointer_y = image_y + PICKER_HEIGHT;

        // ------------ For touch point 0 (tag0 == 10) -----------------------

        // Remove any offset from the top-left position of image.
        img_touch_x = touch0_pointer_x - image_x;
        img_touch_y = touch0_pointer_y - image_y;

        // read through the image data to find byte corresponding to the coordinate.
        // start address in RAM_G + (Y * length of line) + X.
        colorbytesloc = IMAGE_ARGB_RAMG_ADDR + (img_touch_x * 2 + (img_touch_y * PICKER_WIDTH * 2));

        // Read the 2 bytes from this address (2 bytes because it is ARGB1555).
        // These are encoded as ARRR RRGG GGGB BBBB.
#if IS_EVE_API(1,2,3,4)
        // Read a 16 bit value from RAM_G
        colorbytes = EVE_LIB_MemRead16(colorbytesloc);
#else // !IS_EVE_API(1,2,3,4)
        // Read 32 bit value and shift result into the low 8 bits.
        colorbytes = EVE_LIB_MemRead32(colorbytesloc & (~3));
        colorbytes = colorbytes >> ((colorbytesloc & 3) * 8);
        colorbytes &= 0xffff;
#endif // IS_EVE_API(1,2,3,4)

        // Find the R, G and B components within the bits of these 2 bytes.
        // Shift so that the 5 bits are in the upper bits of the byte.
        argb1555_to_rgb(colorbytes, &c0r, &c0g, &c0b);

        // ------------ For touch point 1 (tag0 == 11) -----------------------

        // Remove any offset from the top-left position of image
        img_touch_x = touch1_pointer_x - image_x;
        img_touch_y = touch1_pointer_y - image_y;

        // read through the image data to find byte corresponding to the coordinate
        // start address in RAM_G + (Y * length of line) + X.
        colorbytesloc = IMAGE_ARGB_RAMG_ADDR + (img_touch_x * 2 + (img_touch_y * PICKER_WIDTH * 2));

        // Read the 2 bytes from this address (2 bytes because it is ARGB1555).
        // These are encoded as ARRR RRGG GGGB BBBB.
#if IS_EVE_API(1,2,3,4)
        // Read a 16 bit value from RAM_G.
        colorbytes = EVE_LIB_MemRead16(colorbytesloc);
#else // !IS_EVE_API(1,2,3,4)
        // Read 32 bit value and shift result into the low 8 bits.
        colorbytes = EVE_LIB_MemRead32(colorbytesloc & (~3));
        colorbytes = colorbytes >> ((colorbytesloc & 3) * 8);
        colorbytes &= 0xffff;
#endif // IS_EVE_API(1,2,3,4)

        // Find the R, G and B components within the bits of these 2 bytes.
        // Shift so that the 5 bits are in the upper bits of the byte.
        argb1555_to_rgb(colorbytes, &c1r, &c1g, &c1b);

    } while (LOOP);

}

#endif // (EXAMPLE_COLOURPICKER == EXAMPLE_COLOURPICKER_MULTI)
