/**
 @file eve_example.c
 */
/*
 * ============================================================================
 * History
 * =======
 * Dec 2021		Initial release
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

// Select which example to run. Un-comment only ONE of the examples at any time
//#define ColorPicker_CircularARGB1555
#define ColorPicker_RectangularARGB1555_MultiTouch
//#define ColorPicker_RectangularPaletted8
//#define ColorPicker_ImagePaletted8

#include <stdint.h>
#include "EVE.h"
#include "../include/HAL.h"
#include "MCU.h"
#include "eve_example.h"
#include "ft900.h"



void ConnectToFlash(void)
{
		uint8_t Flash_Status = 0;

		// Detach Flash
		EVE_LIB_BeginCoProList();
		EVE_CMD_FLASHDETATCH();
		EVE_LIB_EndCoProList();
		EVE_LIB_AwaitCoProEmpty();

		Flash_Status = EVE_LIB_MemRead8(EVE_REG_FLASH_STATUS);

		// If the flash status isn't 'detached', go to an error screen
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

		// Attach Flash
		EVE_LIB_BeginCoProList();
		EVE_CMD_FLASHATTACH();
		EVE_LIB_EndCoProList();
		EVE_LIB_AwaitCoProEmpty();

		Flash_Status = EVE_LIB_MemRead8(EVE_REG_FLASH_STATUS);

		// If the flash status isn't 'attached', go to an error screen
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

		// Set Flash to Fast Mode
		EVE_LIB_BeginCoProList();
		EVE_CMD_FLASHFAST(0);
		EVE_LIB_EndCoProList();
		EVE_LIB_AwaitCoProEmpty();

		Flash_Status = EVE_LIB_MemRead8(EVE_REG_FLASH_STATUS);

		// If the flash status isn't 'fast', go to an error screen
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

}


void SetUp_PWM(void)
{
	/* Enable the PWM subsystem */
	sys_enable(sys_device_pwm);

	gpio_function(56, pad_pwm0); /* PWM0 */
	gpio_function(57, pad_pwm1); /* PWM1 */
	gpio_function(58, pad_pwm2); /* PWM2 */

	gpio_function(52, pad_pwm4); /* PWM4 */
	gpio_function(53, pad_pwm5); /* PWM5 */
	gpio_function(54, pad_pwm6); /* PWM6 */


	/* Initialise the PWM Subsystem... */
	 pwm_init(
		 1,                      /* Prescaler */
		 0xFFFF,                 /* Max count */			//was 0xffff
		 0);                     /* Shots (Infinite) */


	 /* ---------------------- Set up PWM 0 ----------------------- */
	 /* Set output levels */
	 pwm_levels(
		 0,                      /* Counter */
		 pwm_state_high,         /* Initial State */
		 pwm_restore_enable);    /* Rollover State */
	 /* Set Compare value */
	 pwm_compare(
		 0,                      /* Counter */
		 0x0);                /* Compare Value */
	 /* Make Counter 0 Toggle PWM0 */
	 pwm_add_toggle(
		 0,                      /* PWM Channel */
		 0);                     /* Counter */


	 /* ---------------------- Set up PWM 0 ----------------------- */
	 /* Set up PWM 1... */
	 /* Set output levels */
	 pwm_levels(
		 1,                      /* Counter */
		 pwm_state_high,         /* Initial State */
		 pwm_restore_enable);    /* Rollover State */
	 /* Set Compare value */
	 pwm_compare(
		 1,                      /* Counter */
		 0x0);                /* Compare Value */
	 /* Make Counter 1 Toggle PWM1 */
	 pwm_add_toggle(
		 1,                      /* PWM Channel */
		 1);                     /* Counter */

	 /* ---------------------- Set up PWM 0 ----------------------- */
	 /* Set up PWM 2... */
	 /* Set output levels */
	 pwm_levels(
		 2,                      /* Counter */
		 pwm_state_high,         /* Initial State */
		 pwm_restore_enable);    /* Rollover State */
	 /* Set Compare value */
	 pwm_compare(
		 2,                      /* Counter */
		 0x0);                /* Compare Value */
	 /* Make Counter 2 Toggle PWM2 */
	 pwm_add_toggle(
		 2,                      /* PWM Channel */
		 2);                     /* Counter */

	 /* ---------------------- Set up PWM 0 ----------------------- */
	 /* Set up PWM 4... */
	 /* Set output levels */
	 pwm_levels(
		 4,                      /* Counter */
		 pwm_state_high,         /* Initial State */
		 pwm_restore_enable);    /* Rollover State */
	 /* Set Compare value */
	 pwm_compare(
		 4,                      /* Counter */
		 0x0);                /* Compare Value */
	 /* Make Counter 4 Toggle PWM4 */
	 pwm_add_toggle(
		 4,                      /* PWM Channel */
		 4);                     /* Counter */


	 /* ---------------------- Set up PWM 0 ----------------------- */
	 /* Set up PWM 5... */
	 /* Set output levels */
	 pwm_levels(
		 5,                      /* Counter */
		 pwm_state_high,         /* Initial State */
		 pwm_restore_enable);    /* Rollover State */
	 /* Set Compare value */
	 pwm_compare(
		 5,                      /* Counter */
		 0x0);                /* Compare Value */
	 /* Make Counter 5 Toggle PWM5 */
	 pwm_add_toggle(
		 5,                      /* PWM Channel */
		 5);                     /* Counter */

	 /* ---------------------- Set up PWM 0 ----------------------- */
	 /* Set up PWM 6... */
	 /* Set output levels */
	 pwm_levels(
		 6,                      /* Counter */
		 pwm_state_high,         /* Initial State */
		 pwm_restore_enable);    /* Rollover State */
	 /* Set Compare value */
	 pwm_compare(
		 6,                      /* Counter */
		 0x0);                /* Compare Value */
	 /* Make Counter 6 Toggle PWM6 */
	 pwm_add_toggle(
		 6,                      /* PWM Channel */
		 6);                     /* Counter */




	 /* Enable PWM... */
	 pwm_enable();


}







// ===========================================================================================================================
// ===========================================================================================================================
// ===========================================================================================================================
// ====================================== CIRCULAR COLOR PICKER WITH ARGB1555 ================================================
// ===========================================================================================================================
// ===========================================================================================================================
// ===========================================================================================================================

#if defined ColorPicker_CircularARGB1555

void eve_display_ColorPicker_CircularARGB1555(void)
{
	#define Image_Data_RAM_G 			0		// Address to put Image in RAM_G
	#define Image_Width					250		// Image width
	#define Image_Height				250		// Image height

	#define	Image_X						50		// Image X position on screen
	#define	Image_Y						50		// Image Y position on screen

	uint32_t TouchXY = 0;
	uint32_t TouchX = 0;
	uint32_t TouchY = 0;

	uint8_t TagVal = 0;

	uint32_t Img_touch_X = 0;
	uint32_t Img_touch_Y = 0;

	uint32_t Touch_Pointer_X = 0;
	uint32_t Touch_Pointer_Y = 0;

	uint32_t ColorBytesLoc = 0;
	uint32_t ColorBytes = 0;

	uint32_t ColorByteR = 0;
	uint32_t ColorByteG = 0;
	uint32_t ColorByteB = 0;

	uint8_t TagOutOfRange = 0x00;


	// resolution ', 250, 'x', 250, 'format ', 'ARGB1555', 'stride ', 500, ' total size ', 125000)*/

	// ################################## Enable PWM ######################################

	// Set up the PWM channels on the FT900 for the LEDs
	SetUp_PWM();

	// ################################## Copy Images from Flash to RAM_G ######################################

	// Connect to the flash at full speed
	ConnectToFlash();

	// copy the image from flash to RAM

	// Copy to address Image_Data_RAM_G (defined as address 0 above)	(Must be 4-byte aligned)
	// Source Data at address 4288 in Flash 							(Must be 64-byte aligned)
	// Copy length of 125056 bytes										(Must be multiple of 4)
	EVE_LIB_BeginCoProList();
	EVE_CMD_FLASHREAD(Image_Data_RAM_G, 4288, 125056);
	EVE_LIB_EndCoProList();
	EVE_LIB_AwaitCoProEmpty();

	// If the image was in the MCU flash as a data array, the line below would be used instead:
	//EVE_LIB_WriteDataToRAMG(ColorPicker, sizeof(ColorPicker), Image_Data_RAM_G);

	// Ensure the PWM channels are set to 0
	pwm_compare(0, 0);		// PWM channel 0 (Red)
	pwm_compare(1, 0);		// PWM channel 1 (Green)
	pwm_compare(2, 0);		// PWM channel 2 (Blue)

	pwm_compare(4, 0);		// PWM channel 4 (Red)
	pwm_compare(5, 0);		// PWM channel 5 (Green)
	pwm_compare(6, 0);		// PWM channel 6 (Blue)

	while(1)
	{

    	// ################################## Create the display content ######################################


		EVE_LIB_BeginCoProList();														// Begin a new co-processor list
		EVE_CMD_DLSTART();																// Begin a display list
		EVE_CLEAR_TAG(255);																// Tag will read as 255 for any untagged areas of the screen when touched
		EVE_CLEAR_COLOR_RGB(0, 0, 0);
		EVE_CLEAR(1,1,1);																// Clear the screen

		EVE_CMD_SETBITMAP(Image_Data_RAM_G, EVE_FORMAT_ARGB1555, Image_Width, Image_Height);	// Set the bitmap properties of the circular color picker

		EVE_TAG_MASK(0);																// Ensure following items are not tagged

		// If screen has been touched outwith the color picker, draw a red circle to alert user that they have touched invalid area
		// This will be a red ring with inner diameter 138 and outer diameter of 142
		// This is updated after the display list and so uses the value here on the next time the screen is rendered
		// NOTE: This is before the color picker is drawn in the display list and so the color picker image will be on top of it
		// Therefore, there is no need to make the center of the red circle transparent
		if(TagOutOfRange == 0xFF)
		{
			EVE_BEGIN(EVE_BEGIN_POINTS);												// Draw points
			EVE_COLOR_RGB((0), (0), (0));												// Draw a black circle.
			EVE_POINT_SIZE(138*16);														// Set diameter 138 slightly larger than color picker circle
			EVE_VERTEX2F((Image_X+125)*16,(Image_Y +125)*16);							// Position at the center of where the color picker will be (color picker is 250x250)
			EVE_COLOR_RGB((255), (0), (0));												// Draw a red circle
			EVE_POINT_SIZE(142*16);														// Slightly larger than the black circle
			EVE_VERTEX2F((Image_X+125)*16,(Image_Y +125)*16);							// Also at the center of where the color picker will be
			EVE_END();																	// Finish drawing circles
		}

		// Return color to white for following objects.
		// Color must be white to ensure images are displayed in their original colors
		EVE_COLOR_RGB(255,255,255);

		// Place the color picker circle at the coordinates defined previously
		EVE_BEGIN(EVE_BEGIN_BITMAPS);
		EVE_VERTEX2II((Image_X),(Image_Y),0,0);
		EVE_END();

		// Print some details for debugging purposes
		EVE_CMD_TEXT(380, 130, 26, EVE_OPT_FORMAT , "X is %d ", TouchX);		// Touch X coordinate
		EVE_CMD_TEXT(380, 150, 26, EVE_OPT_FORMAT , "Y is %d ", TouchY);		// Touch Y coordinate
		EVE_CMD_TEXT(380, 170, 26, EVE_OPT_FORMAT  , "R is %d ", ColorByteR);	// Red value
		EVE_CMD_TEXT(380, 190, 26, EVE_OPT_FORMAT , "G is %d ", ColorByteG);	// Green value
		EVE_CMD_TEXT(380, 210, 26, EVE_OPT_FORMAT , "B is %d ", ColorByteB);	// Blue value
		EVE_CMD_TEXT(380, 230, 26, EVE_OPT_FORMAT , "TagVal is %d ", TagVal);	// Tag value

		// Draw a rectangle which will display the color chosen on the color picker
		// This rectangle and the above debug output are positioned to the right hand side of the color picker itself
		// The line width allows it to have rounded corners
		EVE_BEGIN(EVE_BEGIN_RECTS);
		EVE_LINE_WIDTH(10*16);
		EVE_COLOR_RGB((ColorByteR), (ColorByteG), (ColorByteB));
		EVE_VERTEX2F(380*16,50*16);
		EVE_VERTEX2F(440*16,100*16);
		EVE_END();


		// Make a set of cursor lines to mark the current touch point
		// Set the color to black and the line width to 1 pixel
		// Note that this could be extended to display a small bitmap arrow or target shape
		// Or to adjust the cursor color to make it most clear against the color picker background.
		EVE_BEGIN(EVE_BEGIN_LINES);
		EVE_COLOR_RGB(0, 0, 0);
		EVE_LINE_WIDTH(1*16);
		// Vertical line
		EVE_VERTEX2F((Touch_Pointer_X)*16,(Touch_Pointer_Y-10)*16);		// 10 pixels below the center of the cursor point
		EVE_VERTEX2F((Touch_Pointer_X)*16,(Touch_Pointer_Y+10)*16);		// to 10 pixels above the center of the cursor point
		// Horizontal line
		EVE_VERTEX2F((Touch_Pointer_X-10)*16,(Touch_Pointer_Y)*16);		// 10 pixels to the left of the center of the cursor point
		EVE_VERTEX2F((Touch_Pointer_X+10)*16,(Touch_Pointer_Y)*16);		// 10 pixels to the right of the center of the cursor point
		EVE_END();

		// Draw an invisible circle tagged with tag 10
		// This circle is used as the color picker image is actually a square shape and we don't want the tag to be valid on the corners
		// By using the invisible circle, only the actual color wheel area is tagged.
		EVE_TAG_MASK(1);												// Enable tagging
		EVE_TAG(10);													// Set tag to 10 for any items drawn after this
		EVE_BEGIN(EVE_BEGIN_POINTS);									// Begin points
		EVE_COLOR_MASK(0,0,0,255);										// Disable writes to the R, G and B and so draw to the alpha buffer only
		EVE_POINT_SIZE(121*16);											// Draw a circle slightly smaller than the radius of the color picker
		EVE_VERTEX2F((Image_X+125)*16,(Image_Y +125)*16);				// and centered on the same coordinate as the color picker
		EVE_TAG_MASK(0);												// Disable tagging for any subsequent items

		EVE_DISPLAY();													// Finish the display list
		EVE_CMD_SWAP();													// Swap onto the screen to make it visible
		EVE_LIB_EndCoProList();											// Finish the list of commands
		EVE_LIB_AwaitCoProEmpty();										// Await for the commands to be executed


    	// ################################## Read the touch data ######################################


		// Read the current Tag value and the coordinates used to find it
        TagVal = EVE_LIB_MemRead8(EVE_REG_TOUCH_TAG);                                   // Get Tag value
		TouchXY = EVE_LIB_MemRead32(EVE_REG_TOUCH_TAG_XY);								// Get coordinates
		TouchY = (TouchXY & 0x0000FFFF);											// Get the X and Y values of the coordinate
		TouchX = ((TouchXY & 0xFFFF0000) >> 16);


    	// ################################## Process the touch data ######################################
    	// ################# Use the touch data to find the color from the image data #####################



		if(TagVal ==10)
		{
			// Remove any offset from the top-left position of image
			Img_touch_X = TouchX - Image_X;
			Img_touch_Y = TouchY - Image_Y;

			// read through the image data to find byte corresponding to the coordinate
			// start address in RAM_G + (Y * length of line) + X
			ColorBytesLoc = Image_Data_RAM_G + (Img_touch_X*2 + (Img_touch_Y*Image_Width*2));
			ColorBytes = EVE_LIB_MemRead16(ColorBytesLoc);

			// ARGB1555 data has format ARRR RRGG GGGB BBBB
			// Find the R, G and B components
			// Shift them so that they end up in the most significant 5 bits of the byte.
			ColorByteR = (((ColorBytes & 0x7C00) >> 10)<<3);	// 00000000 00000000 00000000 RRRRR000
			ColorByteG = (((ColorBytes & 0x03D0) >> 5)<<3);		// 00000000 00000000 00000000 GGGGG000
			ColorByteB = (((ColorBytes & 0x001F))<< 3);			// 00000000 00000000 00000000 BBBBB000

			Touch_Pointer_X = TouchX;							// Also set the coordinate for the cursor (relative to the screen and not just the image)
			Touch_Pointer_Y = TouchY;

			// Set the PWM outputs
			// Note that the shifting etc. used above and here could be optimised into one shifting operation
			// but has been done separately here for clarity and for most flexibility as the PWM resolution may differ on the chosen MCU

			// output to PWM channels 0, 1, 2
			pwm_compare(0, ColorByteR<<8);
			pwm_compare(1, ColorByteG<<8);
			pwm_compare(2, ColorByteB<<8);

			// Also mirror the same result on PWM channels 4, 5, 6 (second LED strip)
			pwm_compare(4, ColorByteR<<8);
			pwm_compare(5, ColorByteG<<8);
			pwm_compare(6, ColorByteB<<8);
		}

		// If the screen is touched but with tag 255 which we set with CLEAR_TAG() at the start of the display list
		// Then the user is touching a part of the screen outwith the color picker
		// Set a variable to cause the red ring to be shown around the color picker
		if(TagVal ==255)
		{
			TagOutOfRange = 0xFF;
		}
		else
		{
			TagOutOfRange = 0x00;
		}

	}
}
#endif




//// ===========================================================================================================================
//// ===========================================================================================================================
//// ===========================================================================================================================
//// ====================================== SQUARE COLOR PICKER WITH MULTI TOUCH ================================================
//// ===========================================================================================================================
//// ===========================================================================================================================
//// ===========================================================================================================================

#if defined ColorPicker_RectangularARGB1555_MultiTouch

void eve_display_ColorPicker_RectangularARGB1555_MultiTouch()
{

#define Image_Data_RAM_G 		0		// Address to put Image in RAM_G
#define Image_Width				790
#define Image_Height			470

#define	Image_X					20		// Image X position on screen
#define	Image_Y					20		// Image Y position on screen

	uint32_t Img_touch_X = 0;
	uint32_t Img_touch_Y = 0;

	uint32_t ColorBytesLoc = 0;
	uint16_t ColorBytes = 0;
	uint32_t ColorBytes_CHA_R = 0;
	uint32_t ColorBytes_CHA_G = 0;
	uint32_t ColorBytes_CHA_B = 0;
	uint32_t ColorBytes_CHB_R = 0;
	uint32_t ColorBytes_CHB_G = 0;
	uint32_t ColorBytes_CHB_B = 0;

	uint32_t Point0_X = 100;
	uint32_t Point0_Y = 100;
	uint32_t Point1_X = 200;
	uint32_t Point1_Y = 200;

	uint8_t Tag0 = 0;
	uint32_t Tag0_Coord = 0;
	uint16_t Tag0_Coord_X = 0;
	uint16_t Tag0_Coord_Y = 0;
	uint8_t Tag1 = 0;
	uint32_t Tag1_Coord = 0;
	uint16_t Tag1_Coord_X = 0;
	uint16_t Tag1_Coord_Y = 0;

	// ################################## Enable Multi-Touch ######################################

	EVE_LIB_MemWrite8(EVE_REG_CTOUCH_EXTENDED,0);									// Switch to extended (multi-touch) mode

	// ################################## Enable PWM ######################################

	// Set up the PWM channels on the FT900 for the LEDs
	SetUp_PWM();

	// ################################## Copy Images from Flash to RAM_G ######################################

	// Connect to the flash at full speed
	ConnectToFlash();

	// copy the image from flash to RAM

	// Copy to address Image_Data_RAM_G (defined as address 0 above)		(Must be 4-byte aligned)
	// Source Data at address 129344 in Flash 							(Must be 64-byte aligned)
	// Copy length of 742656 bytes										(Must be multiple of 4)
	EVE_LIB_BeginCoProList();
	EVE_CMD_FLASHREAD(0, 129344  , 742656);
	EVE_LIB_EndCoProList();
	EVE_LIB_AwaitCoProEmpty();


    while(1)
    {
    	// ################################## Create the display content ######################################


		EVE_LIB_BeginCoProList();														// Begin a new co-processor list
		EVE_CMD_DLSTART();																// Begin a display list
		EVE_CLEAR_TAG(255);																// STag will read as 255 for any untagged areas of the screen when touched
		EVE_CLEAR_COLOR_RGB(0, 0, 0);
		EVE_CLEAR(1,1,1);																// Clear the screen

    	EVE_CMD_SETBITMAP(Image_Data_RAM_G, EVE_FORMAT_ARGB1555, Image_Width, Image_Height); // Set the bitmap properties of the rectangular color picker

		// Place the color picker at the coordinates defined previously
		EVE_BEGIN(EVE_BEGIN_BITMAPS);
		EVE_VERTEX2F(Image_X *16, Image_Y *16);
		EVE_END();

		// Draw invisible circles for each of the two touch points.
		// This ensures the user has a reasonable area as a handle to move the cursors with but does not obscure the image below.
		EVE_COLOR_MASK(0,0,0,255);														// Disable writes to the R, G and B components to make the circle invisible
    	EVE_BEGIN(EVE_BEGIN_POINTS);
    	EVE_POINT_SIZE(40*16);															// Set size to 40 pixels
    	EVE_TAG_MASK(1);																// Enable tagging of the following items

		EVE_TAG(10);
    	EVE_VERTEX2F(Point0_X*16, Point0_Y*16);											// Draw an invisible circle tagged with tag 10
    	EVE_TAG(11);
    	EVE_VERTEX2F(Point1_X*16, Point1_Y*16);											// Draw an invisible circle tagged with tag 11
		EVE_TAG_MASK(0);																// Prevent any further items being tagged


		// Make a set of cursor lines to mark the current touch point, for each of the two points
		// Set the color to black and the line width to 1 pixel
		// Note that this could be extended to display a small bitmap arrow or target shape
		// Or to adjust the cursor color to make it most clear against the color picker background.
		// Note that we must return the COLOR_MASK first so that we are drawing the R, G and B components again after turning them off above
		EVE_COLOR_MASK(255,255,255,255);
		EVE_BEGIN(EVE_BEGIN_LINES);
		EVE_COLOR_RGB(0, 0, 0);
		EVE_LINE_WIDTH(1*16);
		// Cross Hair point 0
		// Vertical line
		EVE_VERTEX2F((Point0_X)*16,(Point0_Y-10)*16);					// 10 pixels below the center of the cursor point
		EVE_VERTEX2F((Point0_X)*16,(Point0_Y+10)*16);					// to 10 pixels above the center of the cursor point
		// Horizontal line
		EVE_VERTEX2F((Point0_X-10)*16,(Point0_Y)*16);					// 10 pixels to the left of the center of the cursor point
		EVE_VERTEX2F((Point0_X+10)*16,(Point0_Y)*16);					// 10 pixels to the right of the center of the cursor point

		// Cross Hair point 1
		// Vertical line
		EVE_VERTEX2F((Point1_X)*16,(Point1_Y-10)*16);					// 10 pixels below the center of the cursor point
		EVE_VERTEX2F((Point1_X)*16,(Point1_Y+10)*16);					// to 10 pixels above the center of the cursor point
		// Horizontal line
		EVE_VERTEX2F((Point1_X-10)*16,(Point1_Y)*16);					// 10 pixels to the left of the center of the cursor point
		EVE_VERTEX2F((Point1_X+10)*16,(Point1_Y)*16);					// 10 pixels to the right of the center of the cursor point

		// As there are two different touch points / cursors, we add a letter A and B to label them
		// Note that we use an offset of +3 in the X direction and -20 in the Y direction compared to the center of the cursor
		// The font coordinate is for the Top-Left of the letter A or B and so this offset positions it in the top-left quadrant of the cursor
		// without any overlap with the cursor lines
		EVE_BEGIN(EVE_BEGIN_BITMAPS);
		EVE_BITMAP_HANDLE(27);
		// Using font 27, label point 0 as "A" which is cell 0x41 of the font (ANSI code 0x41 = A)
		EVE_CELL(0x41);
		EVE_VERTEX2F((Point0_X+3)*16, (Point0_Y-20)*16);
		// Using font 27, label point 1 as "B" which is cell 0x42 of the font (ANSI code 0x42 = B)
		EVE_CELL(0x42);
		EVE_VERTEX2F((Point1_X+3)*16, (Point1_Y-20)*16);

		EVE_END();

		EVE_COLOR_RGB(255,255,255);

		// Print some details for debugging purposes

		EVE_CMD_TEXT(830,130, 26, EVE_OPT_FORMAT , "Color A: %d %d %d", ColorBytes_CHA_R, ColorBytes_CHA_G, ColorBytes_CHA_B);	// Color set by color picker cursor A
		EVE_CMD_TEXT(830,150, 26, EVE_OPT_FORMAT , "Touch XY A: %d %d", Point0_X, Point0_Y);									// Coordinate of color picker cursor A

		EVE_CMD_TEXT(830,280, 26, EVE_OPT_FORMAT , "Color B: %d %d %d", ColorBytes_CHB_R, ColorBytes_CHB_G, ColorBytes_CHB_B);	// Color set by color picker cursor B
		EVE_CMD_TEXT(830,300, 26, EVE_OPT_FORMAT , "Touch XY B: %d %d", Point1_X, Point1_Y);									// Coordinate of color picker cursor B

		EVE_CMD_TEXT(830,350, 26, EVE_OPT_FORMAT , "Tag0: %d", Tag0);															// Tag 0 value
		EVE_CMD_TEXT(830,370, 26, EVE_OPT_FORMAT , "Tag0 Coord: %d %d", Tag0_Coord_X, Tag0_Coord_Y);							// Coordinates for Tag 0
		EVE_CMD_TEXT(830,390, 26, EVE_OPT_FORMAT , "Tag1: %d", Tag1);															// Tag 1 value
		EVE_CMD_TEXT(830,410, 26, EVE_OPT_FORMAT , "Tag1 Coord: %d %d", Tag1_Coord_X, Tag1_Coord_Y);							// Coordinates for Tag 1

		// Draw a rectangle for cursor A which will display the color chosen on the color picker
		// This rectangle and the above debug output are positioned to the right hand side of the color picker itself
		// The line width allows it to have rounded corners

		EVE_BEGIN(EVE_BEGIN_RECTS);
		EVE_LINE_WIDTH(20*16);

		EVE_COLOR_RGB((ColorBytes_CHA_R), (ColorBytes_CHA_G), (ColorBytes_CHA_B));
		EVE_VERTEX2F(850*16,50*16);
		EVE_VERTEX2F(950*16,100*16);

		// Draw a second rectangle to indicate the color selected by cursor B
		EVE_COLOR_RGB((ColorBytes_CHB_R), (ColorBytes_CHB_G), (ColorBytes_CHB_B));
		EVE_VERTEX2F(850*16,200*16);
		EVE_VERTEX2F(950*16,250*16);

		EVE_DISPLAY();													// Finish the display list
		EVE_CMD_SWAP();													// Swap onto the screen to make it visible
		EVE_LIB_EndCoProList();											// Finish the list of commands
		EVE_LIB_AwaitCoProEmpty();										// Await for the commands to be executed



    	// ################################## Read the touch data ######################################

		// Read the current Tag value for Tag 0 and the coordinates used to find it
		// Also read the Tag and coordinates for Tag 1

		// Touch Tag 0 is the first touch which the user puts on the screen
		// This could be on either point and so is not always related to a certain cursor

    	Tag0 = EVE_LIB_MemRead8(EVE_REG_TOUCH_TAG);
    	Tag0_Coord = EVE_LIB_MemRead32(EVE_REG_CTOUCH_TOUCH0_XY);
    	//Tag0_Coord = EVE_LIB_MemRead32(EVE_REG_TOUCH_TAG_XY);
    	Tag0_Coord_X = (Tag0_Coord >> 16);
    	Tag0_Coord_Y = (Tag0_Coord & 0x0000FFFF);

		// Touch Tag 1 is the second touch (second finger on the screen in multi-touch use) which the user puts on the screen
		// This could be on either point and so is not always related to a certain cursor

    	Tag1 = EVE_LIB_MemRead8(EVE_REG_TOUCH_TAG1);
    	Tag1_Coord = EVE_LIB_MemRead32(EVE_REG_CTOUCH_TOUCH1_XY);
    	//Tag1_Coord = EVE_LIB_MemRead32(EVE_REG_TOUCH_TAG1_XY);
    	Tag1_Coord_X = (Tag1_Coord >> 16);
    	Tag1_Coord_Y = (Tag1_Coord & 0x0000FFFF);

    	// ################################## Process the touch data ######################################

    	// If there is a touch on Tag 0
    	if(Tag0_Coord != 0x80008000)
    	{
    		// If the touch is on Tag 10 (Cursor A) then take the coordinates into the point 0 (Cursor A) variables
    		if(Tag0 == 10)
    		{
    			Point0_X = Tag0_Coord_X;
    			Point0_Y = Tag0_Coord_Y;
    		}
    		else if (Tag0 == 11) // Or if the touch is on Tag 11 (Cursor B) then take the coordinates into the point 1 (Cursor B) variables
    		{
    			Point1_X = Tag0_Coord_X;
    			Point1_Y = Tag0_Coord_Y;
    		}
    	}

    	// If there is a touch on Tag 1
    	if(Tag1_Coord != 0x80008000)
    	{
    		if(Tag1 == 10)	// If the touch is on Tag 10 (Cursor A) then take the coordinates into the point 0 (Cursor A) variables
    		{
    			Point0_X = Tag1_Coord_X;
    			Point0_Y = Tag1_Coord_Y;
    		}
    		else if (Tag1 == 11) // Or if the touch is on Tag 11 (Cursor B) then take the coordinates into the point 1 (Cursor B) variables
    		{
    			Point1_X = Tag1_Coord_X;
    			Point1_Y = Tag1_Coord_Y;
    		}

    	}


    	// Make sure that any coordinates obtained by the code above are restricted to within the color picker area
    	// If any coordinate is outside, then force it to the respective boundary so the cursor cannot be dragged outside the color picker area
    	if(Point0_X < Image_X)
    		Point0_X = Image_X;
		else if (Point0_X > (Image_X + Image_Width))
			Point0_X = Image_X + Image_Width;

		if(Point0_Y < Image_Y)
			Point0_Y = Image_Y;
		else if (Point0_Y > (Image_Y + Image_Height))
			Point0_Y = Image_Y + Image_Height;

		if(Point1_X < Image_X)
			Point1_X = Image_X;
		else if (Point1_X > (Image_X + Image_Width))
			Point1_X = Image_X + Image_Width;

		if(Point1_Y < Image_Y)
			Point1_Y = Image_Y;
		else if (Point1_Y > (Image_Y + Image_Height))
			Point1_Y = Image_Y + Image_Height;



    	// ################################## Use the touch data to find the color from the image data ######################################


		// ------------ For touch point 0 (tag0 == 10) -----------------------

		// Remove any offset from the top-left position of image
		Img_touch_X = Point0_X - Image_X;
		Img_touch_Y = Point0_Y - Image_Y;

		// read through the image data to find byte corresponding to the coordinate
		// start address in RAM_G + (Y * length of line) + X
		ColorBytesLoc = Image_Data_RAM_G + (Img_touch_X*2 + (Img_touch_Y*Image_Width*2));

		// Read the 2 bytes from this address (2 bytes because it is ARGB1555)
		// These are encoded as ARRR RRGG GGGB BBBB
		ColorBytes = EVE_LIB_MemRead16(ColorBytesLoc);

		// Find the R, G and B components within the bits of these 2 bytes
		// Shift so that the 5 bits are in the upper bits of the byte
		ColorBytes_CHA_R = (((ColorBytes & 0x7C00) >> 10)<<3);		// 00000000 00000000 00000000 RRRRR000
		ColorBytes_CHA_G = (((ColorBytes & 0x03D0) >> 5)<<3);		// 00000000 00000000 00000000 GGGGG000
		ColorBytes_CHA_B = (((ColorBytes & 0x001F))<<3);			// 00000000 00000000 00000000 BBBBB000

		// Set the PWM outputs
		pwm_compare(0, ColorBytes_CHA_R<<8);						// RRRRR000 00000000
		pwm_compare(1, ColorBytes_CHA_G<<8);						// GGGGG000 00000000
		pwm_compare(2, ColorBytes_CHA_B<<8);						// BBBBB000 00000000




		// ------------ For touch point 1 (tag1 == 11) -----------------------

		// Remove any offset from the top-left position of image
		Img_touch_X = Point1_X - Image_X;
		Img_touch_Y = Point1_Y - Image_Y;

		// read through the image data to find byte corresponding to the coordinate
		// start address in RAM_G + (Y * length of line) + X
		ColorBytesLoc = Image_Data_RAM_G + (Img_touch_X*2 + (Img_touch_Y*Image_Width*2));

		// Read the 2 bytes from this address (2 bytes because it is ARGB1555)
		// These are encoded as ARRR RRGG GGGB BBBB
		ColorBytes = EVE_LIB_MemRead16(ColorBytesLoc);

		// Find the R, G and B components within the bits of these 2 bytes
		// Shift so that the 5 bits are in the upper bits of the byte
		ColorBytes_CHB_R = (((ColorBytes & 0x7C00) >> 10)<<3);		// 00000000 00000000 00000000 RRRRR000
		ColorBytes_CHB_G = (((ColorBytes & 0x03D0) >> 5)<<3);		// 00000000 00000000 00000000 GGGGG000
		ColorBytes_CHB_B = (((ColorBytes & 0x001F))<<3);			// 00000000 00000000 00000000 BBBBB000

		// Set the PWM outputs
		//
		pwm_compare(4, ColorBytes_CHB_R<<8);						// RRRRR000 00000000
		pwm_compare(5, ColorBytes_CHB_G<<8);						// GGGGG000 00000000
		pwm_compare(6, ColorBytes_CHB_B<<8);						// BBBBB000 00000000



    }
}

#endif







////// ===========================================================================================================================
////// ===========================================================================================================================
////// ===========================================================================================================================
////// ====================================== SQUARE COLOR PICKER WITH PALETTED 8 ================================================
////// ===========================================================================================================================
////// ===========================================================================================================================
////// ===========================================================================================================================

#if defined ColorPicker_RectangularPaletted8

void eve_display_ColorPicker_RectangularPaletted8(void)
{
	#define Image_Data_RAM_G_Pal8_LUT	 		0		// Address to put LUT in RAM_G
	#define Image_Data_RAM_G_Pal8_Index	 		1024	// Address to put Index in RAM_G
	#define Image_Width_Paletted8				790
	#define Image_Height_Paletted8				470

	#define	Image_X					20		// Image X position on screen
	#define	Image_Y					20		// Image Y position on screen

    uint32_t Img_touch_X = 0;
    uint32_t Img_touch_Y = 0;

	uint32_t Touch_Pointer_X = 0;
	uint32_t Touch_Pointer_Y = 0;

    uint32_t TouchXY = 0;
    uint32_t TouchX = 0;
    uint32_t TouchY = 0;
    uint8_t TagVal = 0;

    uint32_t ColorBytesLoc = 0;
    uint32_t ColorBytes = 0;

    uint32_t ColorByteR = 0;
    uint32_t ColorByteG = 0;
    uint32_t ColorByteB = 0;

    uint32_t ColorByteIndex = 0;


	// ################################## Enable PWM ######################################

	// Set up the PWM channels on the FT900 for the LEDs
	SetUp_PWM();

	// ################################## Copy Images from Flash to RAM_G ######################################

	// Connect to the flash at full speed
	ConnectToFlash();

	// Copy Look Up Table (LUT) from flash to RAM
	// Copy to address Image_Data_RAM_G_Pal8_LUT (defined as address 0 above)		(Must be 4-byte aligned)
	// Source Data at address 872000 in Flash 											(Must be 64-byte aligned)
	// Copy length of 1024 bytes														(Must be multiple of 4)
	EVE_LIB_BeginCoProList();
	EVE_CMD_FLASHREAD(Image_Data_RAM_G_Pal8_LUT, 872000  , 1024);
	EVE_LIB_EndCoProList();
	EVE_LIB_AwaitCoProEmpty();

	// Copy Index from flash to RAM
	// Copy to address Image_Data_RAM_G_Pal8_Index (defined as address 1024 above)		(Must be 4-byte aligned)
	// Source Data at address 873024 in Flash 							(Must be 64-byte aligned)
	// Copy length of 371328 bytes										(Must be multiple of 4)

	EVE_LIB_BeginCoProList();
	EVE_CMD_FLASHREAD(Image_Data_RAM_G_Pal8_Index, 873024   , 371328);
	EVE_LIB_EndCoProList();
	EVE_LIB_AwaitCoProEmpty();

	while(1)
	{
		// ################################## Create the display content ######################################

		EVE_LIB_BeginCoProList();														// Begin a new co-processor list
		EVE_CMD_DLSTART();																// Begin a display list
		EVE_CLEAR_TAG(255);																// STag will read as 255 for any untagged areas of the screen when touched
		EVE_CLEAR_COLOR_RGB(0, 0, 0);
		EVE_CLEAR(1,1,1);																// Clear the screen

		EVE_CMD_SETBITMAP(Image_Data_RAM_G_Pal8_Index, EVE_FORMAT_PALETTED8, Image_Width_Paletted8, Image_Height_Paletted8);	// Set the bitmap properties of the rectangular color picker

		// Set color to white for following objects.
		EVE_COLOR_RGB(255,255,255);

		EVE_TAG_MASK(0);																// Disable Tagging

		// Load the Paletted 8 image of the color picker
		// See the BT81x Programmers Guide for details of how to load a Paletted 8 image
		// Blending is used to build the image up using the different color components

		EVE_BEGIN(EVE_BEGIN_BITMAPS);
		EVE_BLEND_FUNC(EVE_BLEND_ONE, EVE_BLEND_ZERO);
		//Draw Alpha channel
		EVE_COLOR_MASK(0,0,0,1);
		EVE_PALETTE_SOURCE(Image_Data_RAM_G_Pal8_LUT+3);
		EVE_VERTEX2II(Image_X, Image_Y, 0, 0);
		//Draw Red channel
		EVE_BLEND_FUNC(EVE_BLEND_DST_ALPHA, EVE_BLEND_ONE_MINUS_DST_ALPHA);
		EVE_COLOR_MASK(1,0,0,0);
		EVE_PALETTE_SOURCE (Image_Data_RAM_G_Pal8_LUT+2);
		EVE_VERTEX2II (Image_X, Image_Y, 0, 0);
		//Draw Green channel
		EVE_COLOR_MASK(0,1,0,0);
		EVE_PALETTE_SOURCE(Image_Data_RAM_G_Pal8_LUT + 1);
		EVE_VERTEX2II(Image_X, Image_Y, 0, 0);
		//Draw Blue channel
		EVE_COLOR_MASK(0,0,1,0);
		EVE_PALETTE_SOURCE(Image_Data_RAM_G_Pal8_LUT);
		EVE_VERTEX2II(Image_X, Image_Y, 0, 0);

		// Return the color mask to all-enabled and the blend function and color setting
		// This ensures the actions used above to load the Paletted 8 image do not affect subsequent items on the screen
		EVE_COLOR_MASK(1,1,1,1);
		EVE_BLEND_FUNC(EVE_BLEND_SRC_ALPHA, EVE_BLEND_ONE_MINUS_SRC_ALPHA);
		EVE_COLOR_RGB(255,255,255);

		// print the details
		EVE_CMD_TEXT(830,130, 26, EVE_OPT_FORMAT , "X is %d ", TouchX);		// Touch X coordinate
		EVE_CMD_TEXT(830,150, 26, EVE_OPT_FORMAT , "Y is %d ", TouchY);		// Touch Y coordinate
		EVE_CMD_TEXT(830,170, 26, EVE_OPT_FORMAT  , "R is %d ", ColorByteR);	// Red color value
		EVE_CMD_TEXT(830,190, 26, EVE_OPT_FORMAT , "G is %d ", ColorByteG);		// Green color value
		EVE_CMD_TEXT(830,210, 26, EVE_OPT_FORMAT , "B is %d ", ColorByteB);		// Blue color value
		EVE_CMD_TEXT(830,230, 26, EVE_OPT_FORMAT , "TagVal is %d ", TagVal);	// Tag value

		// Draw a rectangle which will display the color chosen on the color picker
		// This rectangle and the above debug output are positioned to the right hand side of the color picker itself
		// The line width allows it to have rounded corners
		EVE_BEGIN(EVE_BEGIN_RECTS);
		EVE_LINE_WIDTH(10*16);
		EVE_COLOR_RGB((ColorByteR), (ColorByteG), (ColorByteB));
		EVE_VERTEX2F(830*16,50*16);
		EVE_VERTEX2F(890*16,80*16);
		EVE_END();

		// Make a set of cursor lines to mark the current touch point
		// Set the color to black and the line width to 1 pixel
		// Note that this could be extended to display a small bitmap arrow or target shape
		// Or to adjust the cursor color to make it most clear against the color picker background.
		EVE_BEGIN(EVE_BEGIN_LINES);
		EVE_COLOR_RGB(0, 0, 0);
		EVE_LINE_WIDTH(1*16);
		// Vertical line
		EVE_VERTEX2F((Touch_Pointer_X)*16,(Touch_Pointer_Y-10)*16);		// 10 pixels below the center of the cursor point
		EVE_VERTEX2F((Touch_Pointer_X)*16,(Touch_Pointer_Y+10)*16);		// to 10 pixels above the center of the cursor point
		// Horizontal line
		EVE_VERTEX2F((Touch_Pointer_X-10)*16,(Touch_Pointer_Y)*16);		// 10 pixels to the left of the center of the cursor point
		EVE_VERTEX2F((Touch_Pointer_X+10)*16,(Touch_Pointer_Y)*16);		// 10 pixels to the right of the center of the cursor point
		EVE_END();


		// Draw an invisible rectangle tagged with tag 10
		// You could tag the image instead when placing it on the screen but this allows greater control over the area which you want to have active
		// In this case, the tagged area is 2 pixels smaller than the image but this can be adjusted as desired
		// This code draws an invisible rectangle tagged with tag 10
		EVE_TAG_MASK(1);												// Enable tagging
		EVE_TAG(10);													// Tag 10
		EVE_BEGIN(EVE_BEGIN_RECTS);
		EVE_COLOR_MASK(0,0,0,255);										// Disable writes to Red, Green and Blue components
		EVE_VERTEX2F((Image_X+2)*16,(Image_Y + 2)*16);					// Top left corner is 2 pixels inside the image
		EVE_VERTEX2F((Image_X+Image_Width_Paletted8 - 2)*16,(Image_Y + Image_Height_Paletted8 - 2)*16); // Bottom Right corner is 2 pixels inside the image
		EVE_TAG_MASK(0);												// Disable tagging

		EVE_COLOR_MASK(255,255,255,255);								// Restore writes to all color channels so that if any items are added afterwards they are drawn correctly

		EVE_DISPLAY();													// Finish the display list
		EVE_CMD_SWAP();													// Swap onto the screen to make it visible
		EVE_LIB_EndCoProList();											// Finish the list of commands
		EVE_LIB_AwaitCoProEmpty();										// Await for the commands to be executed

    	// ################################## Read the touch data ######################################

		// Read the current Tag value and the coordinates used to find it
        TagVal = EVE_LIB_MemRead8(EVE_REG_TOUCH_TAG);                                   // Get Tag value
		TouchXY = EVE_LIB_MemRead32(EVE_REG_TOUCH_TAG_XY);								// Get coordinates
		TouchY = (TouchXY & 0x0000FFFF);											// Get the X and Y values of the coordinate
		TouchX = ((TouchXY & 0xFFFF0000) >> 16);

    	// ################################## Process the touch data ######################################

		if(TagVal == 10)
		{
			// Remove any offset from the top-left position of image
			Img_touch_X = TouchX - Image_X;
			Img_touch_Y = TouchY - Image_Y;

	    	// Make sure that any coordinates obtained by the code above are restricted to within the color picker area
	    	// If any coordinate is outside, then force it to the respective boundary so the cursor cannot be dragged outside the color picker area
	    	if(Img_touch_X < Image_X)
	    		Img_touch_X = Image_X;
			else if (Img_touch_X > (Image_X + Image_Width_Paletted8))
				Img_touch_X = Image_X + Image_Width_Paletted8;

	    	if(Img_touch_Y < Image_Y)
	    		Img_touch_Y = Image_Y;
			else if (Img_touch_Y > (Image_Y + Image_Height_Paletted8))
				Img_touch_Y = Image_Y + Image_Height_Paletted8;

	    	// ################# Use the touch data to find the color from the image data #####################

			// Look through the image data to find the pixel being touched
			// Reading this will return the 8-bit index value which we can look up
			// Start address in RAM_G + X + (Y * length of line)
			ColorBytesLoc = Image_Data_RAM_G_Pal8_Index + (Img_touch_X + (Img_touch_Y*Image_Width_Paletted8));

			// Can add additional error checking here
			//if(ColorBytesLoc > (Image_Width_Paletted8*Image_Height_Paletted8))
			//	ColorBytesLoc = (Image_Width_Paletted8*Image_Height_Paletted8);
			ColorByteIndex = EVE_LIB_MemRead8(ColorBytesLoc);

			// Now read the actual color value from the LUT.
			// From the start of the LUT data, add the index value * 4 (as the index is for 32-bit values)
			ColorBytes = EVE_LIB_MemRead32(Image_Data_RAM_G_Pal8_LUT + (ColorByteIndex*4));
			// ARRR RRGG GGGB BBBB
			// find the R, G and B components which are 8-bit in this case as it is Paletted 8
			ColorByteB = (uint8_t)(ColorBytes);
			ColorByteG = (uint8_t)(ColorBytes >> 8);
			ColorByteR = (uint8_t)(ColorBytes >>16);

			// Also set the value to be used for the cursor
			Touch_Pointer_X = TouchX;
			Touch_Pointer_Y = TouchY;

			// output to PWM channels 0, 1, 2
			pwm_compare(0, ColorByteR<<8);
			pwm_compare(1, ColorByteG<<8);
			pwm_compare(2, ColorByteB<<8);

			// Also mirror the same result on PWM channels 4, 5, 6 (second LED strip)
			pwm_compare(4, ColorByteR<<8);
			pwm_compare(5, ColorByteG<<8);
			pwm_compare(6, ColorByteB<<8);

		}
	}

}

#endif













////// ===========================================================================================================================
////// ===========================================================================================================================
////// ===========================================================================================================================
////// ====================================== COLOR PICKER WITH PALETTED 8 IMAGE =================================================
////// ===========================================================================================================================
////// ===========================================================================================================================
////// ===========================================================================================================================

#if defined ColorPicker_ImagePaletted8

void eve_display_ColorPicker_ImagePaletted8(void)
{
#define Image_Data_RAM_G_Pal8_LUT	 		0		// Address to put LUT in RAM_G
#define Image_Data_RAM_G_Pal8_Index	 		1024	// Address to put Index in RAM_G
#define Image_Width_Paletted8				602
#define Image_Height_Paletted8				401

#define	Image_X					20		// Image X position on screen
#define	Image_Y					20		// Image Y position on screen

uint32_t Img_touch_X = 0;
uint32_t Img_touch_Y = 0;

uint32_t Touch_Pointer_X = 0;
uint32_t Touch_Pointer_Y = 0;

uint32_t TouchXY = 0;
uint32_t TouchX = 0;
uint32_t TouchY = 0;
uint8_t TagVal = 0;

uint32_t ColorBytesLoc = 0;
uint32_t ColorBytes = 0;

uint32_t ColorByteR = 0;
uint32_t ColorByteG = 0;
uint32_t ColorByteB = 0;

uint32_t ColorByteIndex = 0;


// ################################## Enable PWM ######################################

// Set up the PWM channels on the FT900 for the LEDs
SetUp_PWM();

// ################################## Copy Images from Flash to RAM_G ######################################

// Connect to the flash at full speed
ConnectToFlash();

// Copy Look Up Table (LUT) from flash to RAM
// Copy to address Image_Data_RAM_G_Pal8_LUT (defined as address 0 above)		(Must be 4-byte aligned)
// Source Data at address 1244352 in Flash 											(Must be 64-byte aligned)
// Copy length of 1024 bytes														(Must be multiple of 4)
EVE_LIB_BeginCoProList();
EVE_CMD_FLASHREAD(Image_Data_RAM_G_Pal8_LUT, 1244352  , 1024);
EVE_LIB_EndCoProList();
EVE_LIB_AwaitCoProEmpty();

// Copy Index from flash to RAM
// Copy to address Image_Data_RAM_G_Pal8_Index (defined as address 1024 above)		(Must be 4-byte aligned)
// Source Data at address 1245376 in Flash 							(Must be 64-byte aligned)
// Copy length of 241408 bytes										(Must be multiple of 4)

EVE_LIB_BeginCoProList();
EVE_CMD_FLASHREAD(Image_Data_RAM_G_Pal8_Index, 1245376   , 241408);
EVE_LIB_EndCoProList();
EVE_LIB_AwaitCoProEmpty();

while(1)
{
	// ################################## Create the display content ######################################

	EVE_LIB_BeginCoProList();														// Begin a new co-processor list
	EVE_CMD_DLSTART();																// Begin a display list
	EVE_CLEAR_TAG(255);																// STag will read as 255 for any untagged areas of the screen when touched
	EVE_CLEAR_COLOR_RGB(0, 0, 0);
	EVE_CLEAR(1,1,1);																// Clear the screen

	EVE_CMD_SETBITMAP(Image_Data_RAM_G_Pal8_Index, EVE_FORMAT_PALETTED8, Image_Width_Paletted8, Image_Height_Paletted8);	// Set the bitmap properties of the rectangular color picker

	// Set color to white for following objects.
	EVE_COLOR_RGB(255,255,255);

	EVE_TAG_MASK(0);																// Disable Tagging

	// Load the Paletted 8 image
	// See the BT81x Programmers Guide for details of how to load a Paletted 8 image
	// Blending is used to build the image up using the different color components

	EVE_BEGIN(EVE_BEGIN_BITMAPS);
	EVE_BLEND_FUNC(EVE_BLEND_ONE, EVE_BLEND_ZERO);
	//Draw Alpha channel
	EVE_COLOR_MASK(0,0,0,1);
	EVE_PALETTE_SOURCE(Image_Data_RAM_G_Pal8_LUT+3);
	EVE_VERTEX2II(Image_X, Image_Y, 0, 0);
	//Draw Red channel
	EVE_BLEND_FUNC(EVE_BLEND_DST_ALPHA, EVE_BLEND_ONE_MINUS_DST_ALPHA);
	EVE_COLOR_MASK(1,0,0,0);
	EVE_PALETTE_SOURCE (Image_Data_RAM_G_Pal8_LUT+2);
	EVE_VERTEX2II (Image_X, Image_Y, 0, 0);
	//Draw Green channel
	EVE_COLOR_MASK(0,1,0,0);
	EVE_PALETTE_SOURCE(Image_Data_RAM_G_Pal8_LUT + 1);
	EVE_VERTEX2II(Image_X, Image_Y, 0, 0);
	//Draw Blue channel
	EVE_COLOR_MASK(0,0,1,0);
	EVE_PALETTE_SOURCE(Image_Data_RAM_G_Pal8_LUT);
	EVE_VERTEX2II(Image_X, Image_Y, 0, 0);

	// Return the color mask to all-enabled and the blend function and color setting
	// This ensures the actions used above to load the Paletted 8 image do not affect subsequent items on the screen
	EVE_COLOR_MASK(1,1,1,1);
	EVE_BLEND_FUNC(EVE_BLEND_SRC_ALPHA, EVE_BLEND_ONE_MINUS_SRC_ALPHA);
	EVE_COLOR_RGB(255,255,255);

	// print the details
	EVE_CMD_TEXT(830,130, 26, EVE_OPT_FORMAT , "X is %d ", TouchX);		// Touch X coordinate
	EVE_CMD_TEXT(830,150, 26, EVE_OPT_FORMAT , "Y is %d ", TouchY);		// Touch Y coordinate
	EVE_CMD_TEXT(830,170, 26, EVE_OPT_FORMAT  , "R is %d ", ColorByteR);	// Red color value
	EVE_CMD_TEXT(830,190, 26, EVE_OPT_FORMAT , "G is %d ", ColorByteG);		// Green color value
	EVE_CMD_TEXT(830,210, 26, EVE_OPT_FORMAT , "B is %d ", ColorByteB);		// Blue color value
	EVE_CMD_TEXT(830,230, 26, EVE_OPT_FORMAT , "TagVal is %d ", TagVal);	// Tag value

	// Draw a rectangle which will display the color chosen on the color picker
	// This rectangle and the above debug output are positioned to the right hand side of the color picker itself
	// The line width allows it to have rounded corners
	EVE_BEGIN(EVE_BEGIN_RECTS);
	EVE_LINE_WIDTH(10*16);
	EVE_COLOR_RGB((ColorByteR), (ColorByteG), (ColorByteB));
	EVE_VERTEX2F(830*16,50*16);
	EVE_VERTEX2F(890*16,80*16);
	EVE_END();

	// Make a set of cursor lines to mark the current touch point
	// Set the color to black and the line width to 1 pixel
	// Note that this could be extended to display a small bitmap arrow or target shape
	// Or to adjust the cursor color to make it most clear against the color picker background.
	EVE_BEGIN(EVE_BEGIN_LINES);
	EVE_COLOR_RGB(0, 0, 0);
	EVE_LINE_WIDTH(1*16);
	// Vertical line
	EVE_VERTEX2F((Touch_Pointer_X)*16,(Touch_Pointer_Y-10)*16);		// 10 pixels below the center of the cursor point
	EVE_VERTEX2F((Touch_Pointer_X)*16,(Touch_Pointer_Y+10)*16);		// to 10 pixels above the center of the cursor point
	// Horizontal line
	EVE_VERTEX2F((Touch_Pointer_X-10)*16,(Touch_Pointer_Y)*16);		// 10 pixels to the left of the center of the cursor point
	EVE_VERTEX2F((Touch_Pointer_X+10)*16,(Touch_Pointer_Y)*16);		// 10 pixels to the right of the center of the cursor point
	EVE_END();


	// Draw an invisible rectangle tagged with tag 10
	// You could tag the image instead when placing it on the screen but this allows greater control over the area which you want to have active
	// In this case, the tagged area is 2 pixels smaller than the image but this can be adjusted as desired
	// This code draws an invisible rectangle tagged with tag 10
	EVE_TAG_MASK(1);												// Enable tagging
	EVE_TAG(10);													// Tag 10
	EVE_BEGIN(EVE_BEGIN_RECTS);
	EVE_COLOR_MASK(0,0,0,255);										// Disable writes to Red, Green and Blue components
	EVE_VERTEX2F((Image_X+2)*16,(Image_Y + 2)*16);					// Top left corner is 2 pixels inside the image
	EVE_VERTEX2F((Image_X+Image_Width_Paletted8 - 2)*16,(Image_Y + Image_Height_Paletted8 - 2)*16); // Bottom Right corner is 2 pixels inside the image
	EVE_TAG_MASK(0);												// Disable tagging

	EVE_COLOR_MASK(255,255,255,255);								// Restore writes to all color channels so that if any items are added afterwards they are drawn correctly

	EVE_DISPLAY();													// Finish the display list
	EVE_CMD_SWAP();													// Swap onto the screen to make it visible
	EVE_LIB_EndCoProList();											// Finish the list of commands
	EVE_LIB_AwaitCoProEmpty();										// Await for the commands to be executed

	// ################################## Read the touch data ######################################

	// Read the current Tag value and the coordinates used to find it
    TagVal = EVE_LIB_MemRead8(EVE_REG_TOUCH_TAG);                                   // Get Tag value
	//TouchXY = EVE_LIB_MemRead32(EVE_REG_TOUCH_SCREEN_XY);
	TouchXY = EVE_LIB_MemRead32(EVE_REG_TOUCH_TAG_XY);								// Get coordinates
	TouchY = (TouchXY & 0x0000FFFF);											// Get the X and Y values of the coordinate
	TouchX = ((TouchXY & 0xFFFF0000) >> 16);

	// ################################## Process the touch data ######################################

	if(TagVal == 10)
	{
		// Remove any offset from the top-left position of image
		Img_touch_X = TouchX - Image_X;
		Img_touch_Y = TouchY - Image_Y;

    	// Make sure that any coordinates obtained by the code above are restricted to within the color picker area
    	// If any coordinate is outside, then force it to the respective boundary so the cursor cannot be dragged outside the color picker area
    	if(Img_touch_X < Image_X)
    		Img_touch_X = Image_X;
		else if (Img_touch_X > (Image_X + Image_Width_Paletted8))
			Img_touch_X = Image_X + Image_Width_Paletted8;

    	if(Img_touch_Y < Image_Y)
    		Img_touch_Y = Image_Y;
		else if (Img_touch_Y > (Image_Y + Image_Height_Paletted8))
			Img_touch_Y = Image_Y + Image_Height_Paletted8;

    	// ################# Use the touch data to find the color from the image data #####################

		// Look through the image data to find the pixel being touched
		// Reading this will return the 8-bit index value which we can look up
		// Start address in RAM_G + X + (Y * length of line)
		ColorBytesLoc = Image_Data_RAM_G_Pal8_Index + (Img_touch_X + (Img_touch_Y*Image_Width_Paletted8));

		// Can add additional error checking here
		//if(ColorBytesLoc > (Image_Width_Paletted8*Image_Height_Paletted8))
		//	ColorBytesLoc = (Image_Width_Paletted8*Image_Height_Paletted8);
		ColorByteIndex = EVE_LIB_MemRead8(ColorBytesLoc);

		// Now read the actual color value from the LUT.
		// From the start of the LUT data, add the index value * 4 (as the index is for 32-bit values)
		ColorBytes = EVE_LIB_MemRead32(Image_Data_RAM_G_Pal8_LUT + (ColorByteIndex*4));
		// ARRR RRGG GGGB BBBB
		// find the R, G and B components which are 8-bit in this case as it is Paletted 8
		ColorByteB = (uint8_t)(ColorBytes);
		ColorByteG = (uint8_t)(ColorBytes >> 8);
		ColorByteR = (uint8_t)(ColorBytes >>16);

		// Also set the value to be used for the cursor
		Touch_Pointer_X = TouchX;
		Touch_Pointer_Y = TouchY;

		// output to PWM channels 0, 1, 2
		pwm_compare(0, ColorByteR<<8);
		pwm_compare(1, ColorByteG<<8);
		pwm_compare(2, ColorByteB<<8);

		// Also mirror the same result on PWM channels 4, 5, 6 (second LED strip)
		pwm_compare(4, ColorByteR<<8);
		pwm_compare(5, ColorByteG<<8);
		pwm_compare(6, ColorByteB<<8);

	}
}

}

#endif











void eve_example(void)
{


//	unified.blob                                    : 0       : 4096
//	EDF Block                                       : 4096    : 192
//	Color_Wheel_250x250_ARGB1555.raw                : 4288    : 125056
//	Color_Picker_Square_790x470_ARGB1555.raw        : 129344  : 742656
//	Color_Picker_Square_790x470_PALETTED8_lut.raw   : 872000  : 1024
//	Color_Picker_Square_790x470_PALETTED8_index.raw : 873024  : 371328
//	Frog_602x401_PALETTED8_lut.raw                  : 1244352 : 1024
//	Frog_602x401_PALETTED8_index.raw                : 1245376 : 241408


	// Initialise the display
	EVE_Init();

	// Calibrate the display
	eve_calibrate();

	// Start example code
	// Enable only one of the following at a time using the defines at the top of example.c

#if defined ColorPicker_CircularARGB1555
	eve_display_ColorPicker_CircularARGB1555();
#elif defined ColorPicker_RectangularARGB1555_MultiTouch
	eve_display_ColorPicker_RectangularARGB1555_MultiTouch();
#elif defined ColorPicker_RectangularPaletted8
	eve_display_ColorPicker_RectangularPaletted8();
#elif defined ColorPicker_ImagePaletted8
	eve_display_ColorPicker_ImagePaletted8();
#endif

	while(1)
	{

	}
}































