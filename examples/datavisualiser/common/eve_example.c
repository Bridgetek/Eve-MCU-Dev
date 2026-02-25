/**
 @file eve_example.c
 */
 /*
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
#include <stdlib.h>
#include <stdbool.h>

#include <EVE.h>
//#include <MCU.h> // For DEBUG_PRINTF only

#include "eve_example.h"

#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))

// Define gloabal varibles for use in example

//--------------------------------------------------------------------------------------------------------
// colour related variables
//--------------------------------------------------------------------------------------------------------
const uint32_t colourBGBox = 0x000000; // black
const uint32_t colourBG = 0x2C1048; // purple
const uint32_t colour1 = 0x9D45D0; // purple 2
const uint32_t colour2 = 0xFFC337; // yellow
const uint32_t colour3 = 0xF90099; // pink
const uint32_t colour4 = 0xE1341E; // orange

//--------------------------------------------------------------------------------------------------------
// screen data related variables
//--------------------------------------------------------------------------------------------------------

// declare arrays for the x and y axis labels
char * x_axis_labels [x_axis_labels_size];
uint8_t y_axis_labels [y_axis_labels_size];

// declare arrays for the line plot data
uint8_t line_plot1_data [plot_data_size];
uint8_t line_plot2_data [plot_data_size];
uint8_t line_plot3_data [plot_data_size];

// varibles for changing the on-screen bar readouts
uint8_t bar_value [bargauge_num_bars];
uint8_t bar_dir [bargauge_num_bars];

// variables for changing the on-screen circle readouts
uint16_t circle_value = 0;
uint8_t circle_dir = 0xFF; // used in demo mode

// used to update the line plot in demo mode
uint64_t count = 0; 

//--------------------------------------------------------------------------------------------------------
// mode and backlight related variables
//--------------------------------------------------------------------------------------------------------

// mode for demo variable
bool demoMode = true;

// variable for backlight level (0-100)
int8_t backlight_value = 100;

//--------------------------------------------------------------------------------------------------------
// touch input related vairbales
//--------------------------------------------------------------------------------------------------------

// booleans for button press states
bool mode_button_1_press = false;
bool mode_button_2_press = false;
bool menu_item_1_press = true;
bool menu_item_2_press = false;

// variables for touch detection
uint8_t TagVal = 0;
uint8_t LastTagVal = 0;
uint8_t Pen_Down_Tag = 0;
uint8_t Pen_Up_Tag = 0;
// for CMD_TRACKER
int32_t TrackValue = 0;

// for angle on backlight LCD dial arc
uint16_t angle = 0;
// have this start at the 'full' value as we are starting backlight_value at 'full'
uint16_t last_valid_angle = backlight_arc_end_deg; 

//--------------------------------------------------------------------------------------------------------
// static screen content variables
//--------------------------------------------------------------------------------------------------------

// define global variables for static screen content size and RAM_G location
uint32_t static_screen_size = 0;
uint32_t static_screen_location = 0;

//--------------------------------------------------------------------------------------------------------
// pixel precision variable
//--------------------------------------------------------------------------------------------------------

// set pixel precision variable based on EVE version, used in VERTEX2F() calls
#if IS_EVE_API(2,3,4,5)
    const uint8_t pix_precision = 8; // 1/8 th
#elif IS_EVE_API(1)
    const uint8_t pix_precision = 16; // 1/16 th
#endif


// ######################################################################################################################################################################################################
// #######################################################                     Code for gradient helper functions                     ###################################################################
// ######################################################################################################################################################################################################

/**
 @brief Function to dynamically create a gradient from a bitmap and a rectangle which can also be used to colour alpha blended shapes.
 @details This can be used after a shape has been created in the alpha buffer to colour the shape with using the 
 EVE_BLEND_FUNC(EVE_BLEND_DST_ALPHA, EVE_BLEND_ONE_MINUS_DST_ALPHA); blend function, it works by using the inverse alpha values from a inbuilt 
 gradient bitmap and layering colour based on these alpha values of a rectangle shape drawn beneath the bitmap. Otherwise it can be used to 
 create a normal rectangular gradient.
 @param grad_x x value on screen for the gradient
 @param grad_y y value on screen for the gradient
 @param width width of the gradient
 @param height height of the gradient
 @param colour1 colour used in the gradient bitmap 
 @param colour2 base colour used for the rectangle the gradeint is applied on top of
 @param alpha_compositing boolean to determine if the gradient is being used in a alpha composited shape
 @param mirror boolen to determine if the graident bitmap needs mirrored
 @param vert boolen to determine if we wish the gradient to run vertically or horizontally 
 */
void addBlendedGradient(uint16_t grad_x, uint16_t grad_y, uint16_t width, uint16_t height, uint32_t colour1, uint32_t colour2, bool alpha_compositing, bool mirror, bool vert)
{
    // scissor to the size of the gradient we want to create
    // this is required as we are using the wrapx/wrapy = REPEAT in the BITMAP_SIZE call
    // which requires the values to be a power of 2 for height or width or the bitmap data
    // becomes undefined past the wrap section. If we use a scissor here it will prevent the 
    // undefined data being rendered if the height/width arent a power of 2
    EVE_SCISSOR_XY(grad_x, grad_y);
    EVE_SCISSOR_SIZE(width, height);

    // are we using this gradient in a alpha composited shape?
    // if we arent we need to draw it before the gradient
    if(!alpha_compositing){
        // save context
        EVE_SAVE_CONTEXT();

        // draw a rectangle of the colour we want to blend the graident into
        EVE_BEGIN(EVE_BEGIN_RECTS);
        EVE_COLOR_RGB(((uint8_t)(colour2 >> 16)), ((uint8_t)(colour2 >> 8)), ((uint8_t)(colour2)));
        EVE_VERTEX2F((grad_x * pix_precision), (grad_y * pix_precision));
        EVE_VERTEX2F(((grad_x + width) * pix_precision), ((grad_y + height) * pix_precision));
        // end rectangles
        EVE_END();

    }

    // we want to use the inbuilt gradient bitmap utilised in CMD_GRADIENT to apply a gradient to our arc gauges
    EVE_BEGIN(EVE_BEGIN_BITMAPS);
    // assign this the bitmap handle that CMD_GRADIENT would use for it
    EVE_BITMAP_HANDLE(15);

    if (!vert){
        #if IS_EVE_API (1)
        EVE_BITMAP_SOURCE(-261517);
        EVE_BITMAP_LAYOUT(EVE_FORMAT_L8, 512, 1);
        #elif IS_EVE_API (2,3,4)
        // configure bitmap with standard parameters
        EVE_CMD_SETBITMAP(2097256, EVE_FORMAT_L8, 512, 1);
        #elif IS_EVE_API (5)
        // configure bitmap with standard parameters
        EVE_CMD_SETBITMAP(0, EVE_FORMAT_L8, 512, 1);
        EVE_BITMAP_SOURCE_H(7);
        EVE_BITMAP_SOURCE(15728672);
        #endif
        // set the width/height to the input value and wrap along the y axis (to repeat bitmap data)
        if (width > 511 || height > 511){
            #if IS_EVE_API (2,3,4,5)
            EVE_BITMAP_SIZE_H((width >> 9), (height >> 9));
            #endif
            EVE_BITMAP_SIZE(EVE_FILTER_NEAREST, EVE_WRAP_BORDER, EVE_WRAP_REPEAT, width, height);
        }
        else{
            EVE_BITMAP_SIZE(EVE_FILTER_NEAREST, EVE_WRAP_BORDER, EVE_WRAP_REPEAT, width, height);
        }
        // scale the bitmap width to the size we need
        EVE_CMD_LOADIDENTITY();
        // scale by width in the x axis to mirror the image and scale to the height we need
        // mirror with a  multiplication of -1 if required
        if (!mirror){
            EVE_CMD_SCALE(((width * 65536)/512), 0); // 512 is the size of the orignal bitmap
        }else{
            // adjust the translate where the bitmap will start render from when scaled
            EVE_CMD_TRANSLATE((width * 65536), 0); 
            // scale
            // add 1 here as the mirror scale can sometimes shrink the desired bitmap slightly
            EVE_CMD_SCALE((((width + 1) * 65536)/512) * -1, 0); // 512 is the size of the orignal bitmap
        }
        // set new bitmap transform matrix
        EVE_CMD_SETMATRIX(); 
    }
    else{
        // normally this bitmap is 512 x 1, but as we want ot use it vertically we can set it to be 1 x 512
        #if IS_EVE_API (1)
        EVE_BITMAP_SOURCE(-261517);
        EVE_BITMAP_LAYOUT(EVE_FORMAT_L8, 1, 512);
        #elif IS_EVE_API (2,3,4)
        EVE_CMD_SETBITMAP(2097256, EVE_FORMAT_L8, 1, 512);
        #elif IS_EVE_API (5)
        // configure bitmap with standard parameters
        EVE_CMD_SETBITMAP(0, EVE_FORMAT_L8, 1, 512);
        EVE_BITMAP_SOURCE_H(7);
        EVE_BITMAP_SOURCE(15728672);
        #endif
        // set the width/height to the input value and wrap along the x axis (to repeat bitmap data)
        if (width > 511 || height > 511){
            #if IS_EVE_API (2,3,4,5)
            EVE_BITMAP_SIZE_H((width >> 9), (height >> 9));
            #endif
            EVE_BITMAP_SIZE(EVE_FILTER_NEAREST, EVE_WRAP_REPEAT, EVE_WRAP_BORDER, width, height);
        }
        else{
            EVE_BITMAP_SIZE(EVE_FILTER_NEAREST, EVE_WRAP_REPEAT, EVE_WRAP_BORDER, width, height);
        }
        // scale the bitmap height to the size we need
        EVE_CMD_LOADIDENTITY();

        // scale by grad_size in the y axis to mirror the image and scale to the height we need
        // mirror with a multiplication of -1 if required
        if (!mirror){
            // scale
            EVE_CMD_SCALE(0, ((height * 65536)/512)); // 512 is the size of the orignal bitmap
        }else{
            // adjust the translate where the bitmap will start render from when scaled
            EVE_CMD_TRANSLATE(0, (height * 65536)); 
            // scale
            // add 1 here as the mirror scale can sometimes shrink the desired bitmap slightly
            EVE_CMD_SCALE(0, (((height + 1) * 65536)/512) * -1); // 512 is the size of the original bitmap
        }
        // set new bitmap transform matrix
        EVE_CMD_SETMATRIX(); 
    }

    // place the gradient on screen using the input colour
    EVE_COLOR_RGB(((uint8_t)(colour1 >> 16)), ((uint8_t)(colour1 >> 8)), ((uint8_t)(colour1)));
    EVE_VERTEX2F((grad_x * pix_precision), (grad_y * pix_precision));

    // end bitmaps
    EVE_END();

    // are we using this gradient in a alpha composited shape?
    // if we are we need to draw it after the gradient
    if(alpha_compositing){
        // draw a rectangle of the colour we want to blend the gradient into
        // NOTE: because the preceding BLEND_FUNC call is EVE_BLEND_FUNC(EVE_BLEND_DST_ALPHA, EVE_BLEND_ONE_MINUS_DST_ALPHA);
        // this rectangle has to be rendered after the gradient
        EVE_BEGIN(EVE_BEGIN_RECTS);
        EVE_COLOR_RGB(((uint8_t)(colour2 >> 16)), ((uint8_t)(colour2 >> 8)), ((uint8_t)(colour2)));
        EVE_VERTEX2F((grad_x * pix_precision), (grad_y * pix_precision));
        EVE_VERTEX2F(((grad_x + width) * pix_precision), ((grad_y + height) * pix_precision));

        // end rectangles
        EVE_END();
    } else{
        // restore context
        EVE_RESTORE_CONTEXT();
    }

    // reset scissor
    EVE_SCISSOR_XY(0,0);
#if IS_EVE_API(1) // max values for command are different for FT80x
    EVE_SCISSOR_SIZE(512,512);
#else
    EVE_SCISSOR_SIZE(2048,2048);
#endif

}

// ######################################################################################################################################################################################################
// #######################################################                           Code for the Widgets                            ###################################################################
// ######################################################################################################################################################################################################

/**
 @brief Function to draw a circle gauge whose filled section blends in a gradient dynamically.
 @details This function draws a circular guage whos unfilled section is semi transparent, and whose filled section
 has the ability to colour with a gradient fill (or solid colour)
 @param centerx x position for the center of the circle
 @param centery y position for the center of the circle
 @param radius radius value we wish to use to draw the circle
 @param thickness thickness value for the circle
 @param user_value input value to the circle to determine current reading (range 0-360)
 */
void circleGaugeShadow(uint16_t centerx, uint16_t centery, uint16_t radius, uint16_t thickness, int16_t user_value)
{
    // local variable for arc fill value
    int16_t arc_value = user_value;

    // Ensure the input value is within limits (0 - 360)
    arc_value = max(arc_value, 0);
    arc_value = min(arc_value, 360);

    // change this value to alter how transparent the inactive section of the arc is
    uint8_t alpha_value = 50;

    // variables for current arc fill
    int32_t arc_fill_x = 0;
    int32_t arc_fill_y = 0;

    //--------------------------------------------------------------------------------------------------------
    // Process the angle data which will be used to make a uniform motion of the arc
    //--------------------------------------------------------------------------------------------------------
 
    // Calculate the coordinates of the starting point, the gauge arc and the point at the tip of the arc
 
    // for the arc gauge value itself
    // we want to draw this in the middle of the arc so we take the width and divide it by 2, then negate this from the raidus
    // multiply by 8 (or 16 for FT80x) so we can feed this number directly int the VERTEX2F command with our desired pixel precision
    arc_fill_x = CIRC_X((radius * pix_precision + 8), DEG2FURMAN(arc_value));
    arc_fill_y = CIRC_Y((radius * pix_precision + 8), DEG2FURMAN(arc_value));
    
    //--------------------------------------------------------------------------------------------------------
    // Write to the alpha buffer and disable writing colours to the screen to make an invisible arc
    //--------------------------------------------------------------------------------------------------------

    // save current graphics context
    EVE_SAVE_CONTEXT();

    // scissor the area which we wish to draw the arc in
    EVE_SCISSOR_SIZE((radius * 2) + 1, (radius * 2) + 1);
    EVE_SCISSOR_XY((centerx - radius), (centery - radius));

    // set desried pixel precision format
    // EVE_VERTEX_FORMAT(3);
    /*
    set in main display list in this example if required and carried through to this function
    */
 
    // firstly we want ot paint the fill and outline shapes into the alpah buffer, and use a stencil
    // disable all colours bar the alpha channel
    EVE_COLOR_MASK(0, 0, 0, 1);
    EVE_CLEAR(1, 1, 0);

    // we want to add the fill into the alpha buffer
    EVE_BLEND_FUNC(EVE_BLEND_ONE, EVE_BLEND_ONE_MINUS_SRC_ALPHA);
    EVE_COLOR_A(255);
    //--------------------------------------------------------------------------------------------------------
    // Draw the edge strips (or circle) which will fill in the arc
    //--------------------------------------------------------------------------------------------------------

    //here we want to start incrementing the stencil buffer 
    EVE_STENCIL_OP(EVE_STENCIL_INCR, EVE_STENCIL_INCR);

    if (arc_value >= 360){

        EVE_BEGIN(EVE_BEGIN_POINTS);
        EVE_POINT_SIZE(radius * 16);
        EVE_VERTEX2F(((centerx) * pix_precision), (centery) * pix_precision);

    }else{
        // These are drawn per quadrant as each edge strip will only work well on an angle up to 90 deg
        // 0 - 89 Deg
        if ((arc_value > 0) && (arc_value < 90))
        {
            // Edge strip to draw the arc
            EVE_BEGIN(EVE_BEGIN_EDGE_STRIP_B);
            EVE_VERTEX2F(((centerx) * pix_precision), (centery) * pix_precision);
            EVE_VERTEX2F(((centerx * pix_precision) - arc_fill_x) , ((centery * pix_precision) + arc_fill_y));
        }
        else if (arc_value > 0){
            // Edge strip to draw the arc
            EVE_BEGIN(EVE_BEGIN_EDGE_STRIP_B);
            EVE_VERTEX2F(((centerx) * pix_precision), (centery) * pix_precision);
            EVE_VERTEX2F(((centerx - radius) * pix_precision), (centery) * pix_precision);
        }

        // 90 - 179 deg
        if ((arc_value >= 90) && (arc_value < 180))
        {
            // Edge strip to draw the arc
            EVE_BEGIN(EVE_BEGIN_EDGE_STRIP_L);
            EVE_VERTEX2F(((centerx) * pix_precision), (centery) * pix_precision);
            EVE_VERTEX2F(((centerx * pix_precision) - arc_fill_x ), (centery * pix_precision) + arc_fill_y);
        }
        else if (arc_value > 90)
        {
            // Edge strip to draw the arc
            EVE_BEGIN(EVE_BEGIN_EDGE_STRIP_L);
            EVE_VERTEX2F(((centerx) * pix_precision), (centery) * pix_precision);
            EVE_VERTEX2F(((centerx) * pix_precision), (centery - radius) * pix_precision);
        }

        // 180 - 269 deg
        if ((arc_value >= 180) && (arc_value < 270))
        {
            // Edge strip to draw the arc
            EVE_BEGIN(EVE_BEGIN_EDGE_STRIP_A);
            EVE_VERTEX2F(((centerx - 1) * pix_precision), (centery) * pix_precision);
            EVE_VERTEX2F(((centerx * pix_precision) - arc_fill_x), (centery * pix_precision) + arc_fill_y);
        }
        else if (arc_value > 180)
        {
            // Edge strip to draw the arc
            EVE_BEGIN(EVE_BEGIN_EDGE_STRIP_A);
            EVE_VERTEX2F(((centerx - 1) * pix_precision), (centery) * pix_precision);
            EVE_VERTEX2F(((centerx + radius + 1) * pix_precision), (centery) * pix_precision);
        }

        // 270 - 359 deg
        if ((arc_value >= 270) && (arc_value < 360))
        {
            // Edge strip to draw the arc
            EVE_BEGIN(EVE_BEGIN_EDGE_STRIP_R);
            EVE_VERTEX2F(((centerx) * pix_precision), (centery -1) * pix_precision);
            EVE_VERTEX2F((centerx * pix_precision) - arc_fill_x, (centery * pix_precision) + arc_fill_y);
        }
    } 


    // draw the outer radius here and blend destination alpha to source alpha, this is so the fill edge strips above only render across this point
    // and only effect shapes drawn within this point
    EVE_BLEND_FUNC(EVE_BLEND_DST_ALPHA, EVE_BLEND_SRC_ALPHA);
    EVE_BEGIN(EVE_BEGIN_POINTS);
    EVE_POINT_SIZE(radius * 16);
    EVE_VERTEX2F((centerx * pix_precision), (centery * pix_precision));

    // here we want to keep the stencil buffer 
    EVE_STENCIL_OP(EVE_STENCIL_KEEP, EVE_STENCIL_KEEP);

    //--------------------------------------------------------------------------------------------------------
    // Draw the outline section for the arc
    //--------------------------------------------------------------------------------------------------------
 
    // begin drawing circles for our gauge
    //-------------------------------------------------------------------------
    EVE_BEGIN(EVE_BEGIN_POINTS);
    // set alpha to a lower value so this is see through
    EVE_COLOR_A(alpha_value);
    // add to alpha buffer
    EVE_BLEND_FUNC(EVE_BLEND_ONE, EVE_BLEND_ONE_MINUS_SRC_ALPHA);
    // circle outer size
    EVE_POINT_SIZE(radius * 16);
    EVE_VERTEX2F((centerx * pix_precision), (centery * pix_precision));

    // reset alpha to full
    EVE_COLOR_A(255);
    // remove from alpha buffer
    EVE_BLEND_FUNC(EVE_BLEND_ZERO, EVE_BLEND_ONE_MINUS_SRC_ALPHA);
    // circle inner size
    EVE_POINT_SIZE((radius - thickness) * 16);
    EVE_VERTEX2F((centerx * pix_precision), (centery * pix_precision));

    //--------------------------------------------------------------------------------------------------------
    // Draw shapes which will fill the arc
    //--------------------------------------------------------------------------------------------------------
 
    // re-enable colours
    EVE_COLOR_MASK(1, 1, 1, 1);
    // blend in colour
    EVE_BLEND_FUNC(EVE_BLEND_DST_ALPHA, EVE_BLEND_ONE_MINUS_DST_ALPHA);

    // draw only where the stencil value is <=1 (unfilled section of arc)
    EVE_STENCIL_FUNC(EVE_TEST_EQUAL, 1, 255);

    // colour based on input colour for unfilled section
    EVE_BEGIN(EVE_BEGIN_POINTS);
    EVE_COLOR_RGB(((uint8_t)(colour1 >> 16)), ((uint8_t)(colour1 >> 8)), ((uint8_t)(colour1)));
    EVE_POINT_SIZE(radius * 16);
    EVE_VERTEX2F((centerx * pix_precision), (centery * pix_precision));

    // draw only where the stencil value is >=2 (section of arc that has been filled)
    EVE_STENCIL_FUNC(EVE_TEST_GEQUAL, 2, 255);

    //--------------------------------------------------------------------------------------------------------
    // dynamic gradient section to fill based on input fill value
    //--------------------------------------------------------------------------------------------------------

    // we can use a trick here to blend in a gradient to our fill for the arc
    // we can utilise a L8 bitmap which goes from full alpha (255) to 0, and lay this on top
    // of our main colour fill to add in a transtion from one colour to the main fill
    // this gradient is generated in the addBlendedGradient() function

    // we want to blend a differnt colour on each side of the arc, so we can use one gradient blend on each side
    // figure out how tall and wide we need the grad to actually be for our arc
    int32_t grad_size_height = ((radius +1) * 2); // + 4 to account for aliased edge
    int32_t grad_size_width = (radius +1);

    // call the addGradient function to dynamically create a gradient for each side of the arc
    // position these accordingly to account for aliased edge of the arc (as we want to ensure we cover this)
    // we can add a vertical or horizontal gradient shapes via the function parameters, and mirror the direction of the gradient if required
    addBlendedGradient((centerx - radius - 1), (centery - radius - 1), grad_size_width, grad_size_height, colour2, colour1, true,true, true);
    addBlendedGradient(centerx, (centery - radius - 1), grad_size_width, grad_size_height, colour3, colour1, true, true, true);
    
    // NOTE: this trick will not work with the unfilled section of the ARC, as the unfillsed sections alpha value will make alter the colour blends
    // so this can only be used when we are filling the unfilled section of the arc with a non blended colour . To over come this we could utilise a
    // bitmap here to 'fill' the alpha composited shape

    // end drawing
    EVE_END();

    //--------------------------------------------------------------------------------------------------------
    // Add 0 line on bottom of cricle
    //--------------------------------------------------------------------------------------------------------
    
    // Revert to always drawing for the subsequent items
    EVE_STENCIL_FUNC(EVE_TEST_ALWAYS, 0, 255); 
    // set the belnd function back to the default
    EVE_BLEND_FUNC(EVE_BLEND_SRC_ALPHA, EVE_BLEND_ONE_MINUS_SRC_ALPHA); 

    EVE_BEGIN(EVE_BEGIN_LINES);
    EVE_COLOR_RGB(255,255,255);
    // draw start finish line marker 
    EVE_LINE_WIDTH(thickness); 
    EVE_VERTEX2F((centerx * pix_precision), ((centery + radius - thickness) * pix_precision));
    EVE_VERTEX2F((centerx * pix_precision), ((centery + radius) * pix_precision));

    //restore previous graphics context
    EVE_RESTORE_CONTEXT();
}

/**
 @brief Function to draw graph lines.
 @details This function draws a series of graph lines, along the mian X and Y axis for the graph. It dynamically draws additional lines along
 both axis as determined by input variables.
 @param input_x x position for top left of the graph lines
 @param input_y x position for top left of the graph lines
 @param width total width in px of the graph lines chart
 @param height total height in px of the graph lines chart
 @param num_x_lines number of additional lines to draw along the x axis
 @param num_y_lines number of additional lines to draw along the y axis
 @param line_width line width desired for the main chart lines
 @param font_handle font handle to be use for the labels
 @param y_axis_labels pointer to integer array for y axis labels
 @param x_axis_labels pointer to array of chars for x axis labels
 */
void addGraphLinesAndLabels(uint16_t input_x, uint16_t input_y, uint16_t width, uint16_t height, uint8_t num_x_lines, uint8_t num_y_lines, uint8_t line_width, uint8_t font_handle, uint8_t *y_axis_labels, char **x_axis_labels){

    // declare local variables
    // multiply these by 8 initially so we can feed these values straight into the VERTEX2F call
    uint16_t x_line_spacing = ((width * pix_precision)/num_x_lines);
    uint16_t y_line_spacing = ((height * pix_precision)/num_y_lines);

    // for label text offset
    int16_t text_offset = (width/40);
    // ensure its at least 2
    text_offset = max(text_offset, 2);

    // save context
    EVE_SAVE_CONTEXT();

    // set line colour
    EVE_COLOR_RGB(255, 255, 255); // white

    // set line width
    EVE_LINE_WIDTH((line_width * 16)/2);

    // draw X and Y axis main lines
    EVE_BEGIN(EVE_BEGIN_LINE_STRIP);    
    EVE_VERTEX2F((input_x * pix_precision), (input_y * pix_precision));
    EVE_VERTEX2F((input_x * pix_precision), ((input_y + height) * pix_precision));
    EVE_VERTEX2F(((input_x + width) * pix_precision), ((input_y + height) * pix_precision));
    EVE_END();

    // draw additional x and y axis shadow lines
    // set line width
    EVE_LINE_WIDTH((line_width * 16)/4);  
    EVE_BEGIN(EVE_BEGIN_LINES);
    // for x axis
    // dont need to re-draw the first line so start int i =1 
    for (int i = 1; i <= num_x_lines; i++){
        EVE_VERTEX2F(((input_x * pix_precision) + (i * x_line_spacing)), (input_y * pix_precision));
        EVE_VERTEX2F(((input_x * pix_precision) + (i * x_line_spacing)), ((input_y + height) * pix_precision));
    }
    // for y axis
    for (int i = 0; i < num_y_lines; i++){
        EVE_VERTEX2F((input_x * pix_precision), ((input_y * pix_precision) + (i * y_line_spacing)));
        EVE_VERTEX2F(((input_x + width) * pix_precision), ((input_y * pix_precision) + (i * y_line_spacing)));
    }
    EVE_END();

    // add x and y axis labels onto the chart
    for (int i = 0; i <= num_x_lines; i++){
        EVE_CMD_TEXT((input_x + (i * (x_line_spacing/pix_precision))), (input_y + height + text_offset) ,font_handle ,EVE_OPT_CENTER, x_axis_labels[i]);
    }
    // for y axis
    for (int i = 0; i < num_y_lines; i++){
        EVE_CMD_NUMBER((input_x - (text_offset/2)), (input_y + (i * (y_line_spacing/pix_precision))) ,font_handle ,EVE_OPT_RIGHTX | EVE_OPT_CENTERY, y_axis_labels[i]);
    }


    // restore context
    EVE_RESTORE_CONTEXT();

}

/**
 @brief Function to draw a line plot.
 @details This function draws a line plot line, with the number of data points determined by an input variable and the data values provided 
 provided by an input array.
 @param input_x x position for top left of the line plot
 @param input_y y position for top left of the line plot
 @param width total width in px of the line plot
 @param height total height in px of the line plot
 @param colour input colour for the lines
 @param line_width input line width for the plot lines
 @param num_points number of data points in the line plot
 @param data pointer to array containing data points
 */
void linePlot(uint16_t input_x, uint16_t input_y, uint16_t width, uint16_t height, uint32_t colour, uint16_t line_width, uint8_t num_points, uint8_t *data){
    
    // declare local variables
    uint16_t point_spacing = ((width * pix_precision)/(num_points-1));
    // used in the vertex positioning
    uint16_t value;

    // save context
    EVE_SAVE_CONTEXT();

    // set line colour from input
    EVE_COLOR_RGB(((uint8_t)(colour >> 16)), ((uint8_t)(colour >> 8)), ((uint8_t)(colour))); 

    // set line width from input
    EVE_LINE_WIDTH(line_width * 16);

    // draw main plot line
    EVE_BEGIN(EVE_BEGIN_LINE_STRIP);    
   
    for (int i = 0; i < num_points; i++){
        // set value variable to current data point
        value = data[i];
        // ensure value is within uint8_t limits
        value = min(value, 255);
        value = max(value, 0);
        // normalise value so it is within the chart y range
        value = (((value) * height) / 255);
        // add vertex into the line
        EVE_VERTEX2F(((input_x * pix_precision) + (i * point_spacing)), ((input_y + height - (value)) * pix_precision));
    }
    EVE_END();

    // draw indication points
    EVE_BEGIN(EVE_BEGIN_POINTS);    
    // set point size
    EVE_POINT_SIZE((line_width * 3) * 16);
    for (int i = 0; i < num_points; i++){
        // set value variable to current data point
        value = data[i];
        // ensure value is within uint8_t limits
        value = min(value, 255);
        value = max(value, 0);
        // normalise value so it is within the chart y range
        value = (((value) * height) / 255);
        EVE_VERTEX2F(((input_x * pix_precision) + (i * point_spacing)), ((input_y + height - value) * pix_precision));
    }
    EVE_END();

    // restore context
    EVE_RESTORE_CONTEXT();

}

/**
 @brief Function to render a vertical bar gauge widget on screen. This function will render a vertical bar gauge using rectangles, and 
 stencilling to draw a fill value for the bar based upon the input 'value' variable. 
 @param input_x x value for top left of widget
 @param input_y y value for top left of widget
 @param width width of the bar
 @param height height of the bar
 @param colour_bottom input to determine the bottom colour the bar fill and shadow
 @param colour_top input to determine the top colour bar fill (if set to 0x000000 (or 0) only colour_top will be utilised in the widget)
 @param value current value for the bar (0-255)
 */
void verticalBarGauge(uint16_t input_x, uint16_t input_y, uint16_t width, uint16_t height, uint32_t colour_bottom, uint32_t colour_top, uint8_t value){

    // ensure value is in range
    value = max(value, 0);
    value = min(value, 255);

    // normalise value to widget size, also multiple by current pixel precision so we can add this value straight into 
    // the vertex 2F call
    uint16_t reading = (((value * pix_precision) * height) / 255);

    // save graphics context
    EVE_SAVE_CONTEXT();

     // set scissor to size of widget and starting position
    EVE_SCISSOR_SIZE(width, height);
    EVE_SCISSOR_XY(input_x, input_y);

    // draw fill
    //----------------------------------------

    // draw shadow   
    EVE_COLOR_A(50);
    // use input colour for rectangles
    EVE_COLOR_RGB(((uint8_t)(colour_bottom >> 16)), ((uint8_t)(colour_bottom >> 8)), ((uint8_t)(colour_bottom)));
    // begin rectanles
    EVE_BEGIN(EVE_BEGIN_RECTS);
    // draw intial shadow bar
    EVE_VERTEX2F((input_x * pix_precision), (input_y * pix_precision));
    EVE_VERTEX2F(((input_x + width) * pix_precision), ((input_y + height) * pix_precision));
    // reset alpha
    EVE_COLOR_A(255);

    // stencil section to fill
    // clear the stencil buffer so no previous values in the effect this operation 
    EVE_CLEAR(0, 1, 0);
    // disable colours and alpha
    EVE_COLOR_MASK(0, 0, 0, 0);
    // increment the stencil
    EVE_STENCIL_OP(EVE_STENCIL_INCR, EVE_STENCIL_INCR);
    // these vertexes draw the fill stencil (first one moves with vertically with the value input)
    if(value != 0){
        EVE_VERTEX2F((input_x * pix_precision), (((input_y + height + 1) * pix_precision) - reading));
        EVE_VERTEX2F(((input_x + width) * pix_precision), ((input_y + height) * pix_precision));
    }
    // keep the stencil
    EVE_STENCIL_OP(EVE_STENCIL_KEEP, EVE_STENCIL_KEEP);
    // re-enable colours
    EVE_COLOR_MASK(1, 1, 1, 0);

    // draw the fill (this will colour all pixels where the stenicl = 1)
    EVE_STENCIL_FUNC(EVE_TEST_EQUAL, 1, 255);

    // if colour_top is set to black, then just draw a rectangle with colour bottom
    if (colour_top == 0x000000){
        EVE_VERTEX2F((input_x * pix_precision), (input_y * pix_precision));
        EVE_VERTEX2F(((input_x + width) * pix_precision), ((input_y + height) * pix_precision));
    }else{
        //else add a gradient from colour bottom to colour top
        addBlendedGradient(input_x, input_y, width, height, colour_bottom, colour_top, false, false, true);
    }
    // end drawing
    EVE_END();

    // restore graphics context
    EVE_RESTORE_CONTEXT();
}

/**
 @brief Function to draw a simple circular button.
 @details This function draws a simple circular button, assigning it a TAG value based upon the input variable,
 and altering its rendered colour based upon its current 'pressed' state.
 @param input_x x position for center of the button
 @param input_y x position for center of the button
 @param size radius in px of the button
 @param colour colour input for the buttons !pressed state
 @param colour_pressed colour input for the buttons pressed state
 @param colour_text colour input for the text printed on the button
 @param text text input for the text printed on the button
 @param tag TAG value  associated to the button
 @param pressed pressed state boolean for the button
 */
void circularButton(uint16_t input_x, uint16_t input_y, uint16_t size, uint32_t colour, uint32_t colour_pressed, uint32_t colour_text, uint16_t font_handle, const char* text, uint8_t tag, bool pressed){
    
    // save context
    EVE_SAVE_CONTEXT();

    // set full alpha
    EVE_COLOR_A(255);

    // begin points
    EVE_BEGIN(EVE_BEGIN_POINTS);

    // if the button isnt pressed in, draw a highlight at the top left/bottom right edges
    if(!pressed){
        // set size to input
        EVE_POINT_SIZE((size * 16));
        // set colour to input (not pressed)
        EVE_COLOR_RGB(((uint8_t)(colour >> 16)), ((uint8_t)(colour >> 8)), ((uint8_t)(colour))); 
        // draw point
        EVE_VERTEX2F((input_x  * pix_precision), (input_y * pix_precision));
    
        // use slightly smaller white ciricles to add a highlight effect
        EVE_POINT_SIZE(((size - 1) * 16) - 8); //-1.5
        EVE_COLOR_RGB(255, 255, 255); 
        // position these up and left 1, down and right 1
        EVE_VERTEX2F(((input_x + 1)  * pix_precision), ((input_y + 1) * pix_precision));
        EVE_VERTEX2F(((input_x - 1) * pix_precision), ((input_y - 1) * pix_precision));
        
        // set size and colour for overlay/tagged point drawn further down
        EVE_POINT_SIZE((size * 16) - 8); // -0.5
        EVE_COLOR_RGB(((uint8_t)(colour >> 16)), ((uint8_t)(colour >> 8)), ((uint8_t)(colour))); 
    }
    else{
        // just draw the point with the pressed colour and input size
        // set size
        EVE_POINT_SIZE(size * 16);
        // set colour
        EVE_COLOR_RGB(((uint8_t)(colour_pressed >> 16)), ((uint8_t)(colour_pressed >> 8)), ((uint8_t)(colour_pressed)));
    } 

    // enable tagging
    EVE_TAG_MASK(1);
    // set input tag
    EVE_TAG(tag);

    // draw main point
    EVE_VERTEX2F((input_x  * pix_precision), (input_y * pix_precision));

    // end points
    EVE_END();

    // disable tagging
    EVE_TAG_MASK(0);

    // place some text in the middle of the button
    // colour based on input
    EVE_COLOR_RGB(((uint8_t)(colour_text >> 16)), ((uint8_t)(colour_text >> 8)), ((uint8_t)(colour_text))); 

    // draw text
    EVE_CMD_TEXT(input_x, input_y, font_handle, EVE_OPT_CENTER, text);

    // restore context
    EVE_RESTORE_CONTEXT();

}

// ######################################################################################################################################################################################################
// #######################################################                  Code for display list UI helper functions                 ###################################################################
// ######################################################################################################################################################################################################

/**
 @brief Helper function to add background boxes for widgets into the display list.
 @param alpha input transparency for the boxes
 @param colour input colour for the boxes
 */
void addBackgroundBoxes(uint8_t alpha, uint32_t colour){

    // save context
    EVE_SAVE_CONTEXT();

    // background boxes
    EVE_BEGIN(EVE_BEGIN_RECTS);
    // set line width
    EVE_LINE_WIDTH(1*16);    
    // set alpha and colour for boxes from input
    EVE_COLOR_A(alpha);
    // set colour from input colour
    EVE_COLOR_RGB(((uint8_t)(colour >> 16)), ((uint8_t)(colour >> 8)), ((uint8_t)(colour))); 
        
    // draw background boxes on screen    
    // top left
    EVE_VERTEX2F((line_graph_box_start_x * pix_precision), (line_graph_box_start_y * pix_precision));
    EVE_VERTEX2F((line_graph_box_end_x * pix_precision), (line_graph_box_end_y * pix_precision));
    // top right
    EVE_VERTEX2F((controls_box_start_x * pix_precision), (controls_box_start_y * pix_precision));
    EVE_VERTEX2F((controls_box_end_x * pix_precision), (controls_box_end_y * pix_precision));
    // bottom left
    EVE_VERTEX2F((bar_gauge_box_start_x * pix_precision), (bar_gauge_box_start_y * pix_precision));
    EVE_VERTEX2F((bar_gauge_box_end_x * pix_precision), (bar_gauge_box_end_y * pix_precision));
    //bottom right
    EVE_VERTEX2F((circle_gauge_box_start_x * pix_precision), (circle_gauge_box_start_y * pix_precision));
    EVE_VERTEX2F((circle_gauge_box_end_x * pix_precision), (circle_gauge_box_end_y * pix_precision));
    // end rects
    EVE_END();

    // restore context
    EVE_RESTORE_CONTEXT();

}

/**
 @brief Helper functoin add label boxes for the bar gauge widgets into the display list.
 @param colour input colour for the boxes
 */
void addBarGuageLabelBoxes(uint32_t colour){

    // save context
    EVE_SAVE_CONTEXT();

    // begin drawing rectangles
    EVE_BEGIN(EVE_BEGIN_RECTS);
    EVE_COLOR_RGB(((uint8_t)(colour >> 16)), ((uint8_t)(colour >> 8)), ((uint8_t)(colour)));
    //set line width
    EVE_LINE_WIDTH(2 * 16);

    // first bargauge
    EVE_VERTEX2F((bargauge1_x * pix_precision), (bargauge_label_y * pix_precision));
    EVE_VERTEX2F(((bargauge2_x + bargauge_width - 2) * pix_precision), ((bargauge_label_y + bargauge_label_height) * pix_precision)); // -2 is the line width set above
    // second bargauge
    EVE_VERTEX2F((bargauge3_x * pix_precision), (bargauge_label_y * pix_precision));
    EVE_VERTEX2F(((bargauge4_x + bargauge_width - 2) * pix_precision), ((bargauge_label_y + bargauge_label_height) * pix_precision)); // -2 is the line width set above
    // third bargauge
    EVE_VERTEX2F((bargauge5_x * pix_precision), (bargauge_label_y * pix_precision));
    EVE_VERTEX2F(((bargauge6_x + bargauge_width - 2) * pix_precision), ((bargauge_label_y + bargauge_label_height) * pix_precision)); // -2 is the line width set above
    // end drawing rectanlges
    EVE_END();

    // restore context
    EVE_RESTORE_CONTEXT();

}

/**
 @brief Helper function add the control menu into the display list.
 @details This function draws a simple two button menu bar, constructed using the LINES primitive,
 each button is tagged with a individual value
 @param input_x x position for the start of the menu bar line
 @param input_y x position for the start of the menu bar line
 @param lenght total length of the menu bar
 @param size total thickness of the menu bar
 @param colour input colour for the menu
 */
void controlMenu(uint16_t input_x, uint16_t input_y, uint16_t length, uint16_t size, uint32_t colour){

    // ensure the size is at least 5
    size = max(size, 5);

    // declare local varaibles
    uint16_t label_length = (length/3); // 33.333% 
    uint16_t text1_x = (input_x + (label_length/2));
    uint16_t text2_x = ((input_x + length) - (label_length/2));

    // save context
    EVE_SAVE_CONTEXT();

    // set lower alpha
    EVE_COLOR_A(50);

    // draw main menu bar
    //----------------------------------------
    // set colour to input 
    EVE_COLOR_RGB(((uint8_t)(colour >> 16)), ((uint8_t)(colour >> 8)), ((uint8_t)(colour))); 

    // set line width
    EVE_LINE_WIDTH(size * 16);

    // begin lines
    EVE_BEGIN(EVE_BEGIN_LINES);
    
    EVE_VERTEX2F((input_x  * pix_precision), (input_y * pix_precision));
    EVE_VERTEX2F(((input_x + length)  * pix_precision), (input_y * pix_precision));

    // draw menu items
    //----------------------------------------

    // make these lines smaller
    EVE_LINE_WIDTH((size-5) * 16);

    // set full transparency initially
    EVE_COLOR_A(0);
    
    // enable tagging
    EVE_TAG_MASK(1);
    
    // menu item 1
    // tag the following shape with item 1 tag
    EVE_TAG(menu_item_1_tag);
 
    // if the item is pressed set alpha to full
    if(menu_item_1_press == true)
        EVE_COLOR_A(255);
    
    // draw shape
    EVE_VERTEX2F((input_x  * pix_precision), (input_y * pix_precision));
    EVE_VERTEX2F(((input_x + label_length)  * pix_precision), (input_y * pix_precision));

    // re-set alpha to 0 (if it was pressed)
    if(menu_item_1_press == true)
        EVE_COLOR_A(0);
    
    // menu item 2
    // tag the following shape with item 2 tag
    EVE_TAG(menu_item_2_tag);
    // if the item is pressed set alpha to full
    if(menu_item_2_press == true)
        EVE_COLOR_A(255);

    // draw shape
    EVE_VERTEX2F((((input_x + length) - label_length)  * pix_precision), (input_y * pix_precision));
    EVE_VERTEX2F(((input_x + length)  * pix_precision), (input_y * pix_precision));
    
    // re-set alpha to 0 (if it was pressed)
    if(menu_item_2_press == true)
        EVE_COLOR_A(0);

    // disable tagging
    EVE_TAG_MASK(0);
 
    // add menu item text
    //----------------------------------------

    // set full alpha
    EVE_COLOR_A(255);
    // set colour to white
    EVE_COLOR_RGB(255, 255, 255);
    // add text labe;s
    EVE_CMD_TEXT(text1_x, input_y, font_med, EVE_OPT_CENTER, "Mode");
    EVE_CMD_TEXT(text2_x, input_y, font_med, EVE_OPT_CENTER, "LCD");

    // restore context
    EVE_RESTORE_CONTEXT();

}

/**
 @brief Helper funciton add the LCD backlight contol menu into the display list.
 @details This function renders and arc gauge widget onto the screen which can be used to alter the backlight strength value of the display.
 */
void LCDBacklightPage(){

    // save context
    EVE_SAVE_CONTEXT();

    // set desried colour
    EVE_COLOR_RGB(((uint8_t)(colour1 >> 16)), ((uint8_t)(colour1 >> 8)), ((uint8_t)(colour1)));
    // add arc simple gauge onto the screen to act as input control (from snippets/controls/arcs.c)
    // use the last valid angle, minus the start deg angle, normalised to an 16 bit number/total deg angles as the reading value of the arc
    arc_simple_gauge(backlight_dial_x, backlight_dial_y, backlight_dial_inner_radius, backlight_dial_radius,  DEG2FURMAN(backlight_arc_start_deg), DEG2FURMAN(backlight_arc_end_deg), (((last_valid_angle - backlight_arc_start_deg) * 65535)/backlight_arc_total_deg));

    // tag and add a tracker to the arc gauge point
    EVE_TAG_MASK(1); // enable tagging
    // use dial tag
    EVE_TAG(backlight_dial_tag); 
    // draw a point slightly bigger than the gauge to be tagged, this aides in touch detection near the arcPointGauge
    EVE_BEGIN(EVE_BEGIN_POINTS);
    EVE_POINT_SIZE((backlight_dial_radius + 10) * 16);
    // make the point invisible
    EVE_COLOR_A(0);
    // place the point where the arc guage is
    EVE_VERTEX2F((backlight_dial_x  * pix_precision), (backlight_dial_y * pix_precision)); // draw point
    // add tracker in the centre of the point
    EVE_CMD_TRACK(backlight_dial_x, backlight_dial_y, 1, 1, backlight_dial_tag); 
    // disable tagging
    EVE_TAG_MASK(0); 

    // reset alpha and colour to white
    EVE_COLOR_A(255);
    EVE_COLOR_RGB(255, 255, 255);

    // add number to indicate current value to screen
    EVE_CMD_NUMBER(backlight_dial_x, backlight_dial_y, font_med_2, EVE_OPT_CENTER, backlight_value);

    // add text label
    EVE_CMD_TEXT(backlight_dial_x, (backlight_dial_y + ((backlight_dial_radius*3)/4)), font_small, EVE_OPT_CENTER, "Brightness");

    // restore context
    EVE_RESTORE_CONTEXT();
}

/**
 @brief Helper function add the mode control menu into the display list.
 @details This function renders the mode control sub-menu using the RECTS primitive for the readout, and 
 custom circle buttons for the controls.
 */
void modePage(){

    // save context
    EVE_SAVE_CONTEXT();

    // draw  rectangle  for readout
    EVE_BEGIN(EVE_BEGIN_RECTS);
    
    // set rectangle colour
    EVE_COLOR_RGB(((uint8_t)(colourBG >> 16)), ((uint8_t)(colourBG >> 8)), ((uint8_t)(colourBG)));
    // set line width
    EVE_LINE_WIDTH(5 * 16);
    // draw vertices
    EVE_VERTEX2F((mode_start_readout_x * pix_precision), (mode_start_readout_y * pix_precision));
    EVE_VERTEX2F((mode_end_readout_x * pix_precision), (mode_end_readout_y * pix_precision));
    // end  rectangles
    EVE_END();

    // set colour to white:
    EVE_COLOR_RGB(255, 255, 255);
    // add label for current mode
    if(demoMode)
        EVE_CMD_TEXT(mode_label_x, mode_lable_y, font_large, EVE_OPT_CENTER, "Demo");
    else
        EVE_CMD_TEXT(mode_label_x, mode_lable_y, font_large, EVE_OPT_CENTER, "Sensor");

    // add some custom buttons onto the screen
    circularButton(mode_button1_x, mode_button1_y, mode_button_size, colourBG, colour4, 0xFFFFFF, font_large, "<", mode_button_1_tag, mode_button_1_press);
    circularButton(mode_button2_x, mode_button2_y, mode_button_size, colourBG, colour4, 0xFFFFFF, font_large, ">", mode_button_2_tag, mode_button_2_press);

    // restore context
    EVE_RESTORE_CONTEXT();
}

// ######################################################################################################################################################################################################
// #######################################################                      Code for main display list creation                   ###################################################################
// ######################################################################################################################################################################################################

/**
 @brief Function to generate a display list containing the static screen elements and copy these into RAM_G so they
 can be appended into screen updates using the CMD_APPEND command.
 */
void generateStaticScreenComponents(){

    //--------------------------------------------------------------------------------------------------------
    // Construct display list to copy into RAM_DL
    //--------------------------------------------------------------------------------------------------------

    // start the display list
    EVE_LIB_BeginCoProList();
    EVE_CMD_DLSTART();
    // clear colour RGB to set the screen to the desired BG colour
    EVE_CLEAR_COLOR_RGB(((uint8_t)(colourBG >> 16)), ((uint8_t)(colourBG >> 8)), ((uint8_t)(colourBG)));
    // clear colour, stencil, tag
    EVE_CLEAR(1, 1, 1);

    // pix_precision = 8 (1/8th) if API level is 2,3,4,5, so we need to insert a VERTEX_FORMAT() command
    // this command will cascade through the remaining commands in the display list (such as the VERTEX2F calls)
    #if IS_EVE_API(2,3,4,5)
    // set desired vertex format for the example
    EVE_VERTEX_FORMAT(3);
    #endif

    // disable tagging, this prevents items being drawn with tag = 255 when we havent explicitly tagged them 
    EVE_TAG_MASK(0);

    // add background boxs
    //--------------------------------------------------------------------------------------------------------
    
    addBackgroundBoxes(200, colourBGBox);

    // add line graph outline onto the screen
    //--------------------------------------------------------------------------------------------------------
    
    // add main label
    EVE_CMD_TEXT(line_graph_label_x, line_graph_label_y, font_med, 0, "Temperatures");

    // add gradient behind the graph
    // set alpha for gradient
    EVE_COLOR_A(50);
    addBlendedGradient(line_graph_x, line_graph_y, line_graph_width, line_graph_height, colour4, colourBG, false, true, true);
    // reset gradient
    EVE_COLOR_A(255);

    // add graph lines and labels
    addGraphLinesAndLabels(line_graph_x, line_graph_y, line_graph_width, line_graph_height, line_graph_extra_x_lines , line_graph_extra_y_lines, line_graph_line_width, font_small, y_axis_labels, x_axis_labels);

    // add label boxes and text for bar gauges
    //--------------------------------------------------------------------------------------------------------

    // add label boxes for bar gauges
    addBarGuageLabelBoxes(colourBG);

    // add text labels for bar gauges
    EVE_CMD_TEXT(((bargauge1_x + bargauge2_x + bargauge_width)/2), (bargauge_label_y + (bargauge_label_height/2) - 1), font_small, EVE_OPT_CENTER, "Sp02");
    EVE_CMD_TEXT(((bargauge3_x + bargauge4_x + bargauge_width)/2), (bargauge_label_y + (bargauge_label_height/2) - 1), font_small, EVE_OPT_CENTER, "C02");
    EVE_CMD_TEXT(((bargauge5_x + bargauge6_x + bargauge_width)/2), (bargauge_label_y + (bargauge_label_height/2) - 1), font_small, EVE_OPT_CENTER, "Humid");

    // add label boxes and text for circle gauges
    //--------------------------------------------------------------------------------------------------------

    // add labels for circle gauges
    EVE_CMD_TEXT(circle_guage1_x, (circle_guage1_y + circle_gauge_radius + (circle_gauge_thickness * 2)), font_med, EVE_OPT_CENTER, "Main Pressure");
    EVE_CMD_TEXT(circle_guage2_x, (circle_guage2_y + circle_gauge_radius + (circle_gauge_thickness * 2)), font_med, EVE_OPT_CENTER, "Pressure 2");
    EVE_CMD_TEXT(circle_guage3_x, (circle_guage3_y + circle_gauge_radius + (circle_gauge_thickness * 2)), font_med, EVE_OPT_CENTER, "Pressure 3");
 
    // send list to the co-processor but dont display it
    EVE_LIB_EndCoProList();
    EVE_LIB_AwaitCoProEmpty();

    //--------------------------------------------------------------------------------------------------------
    // determine RAM_DL usage for the static parts of the screen by reading DL size
    //--------------------------------------------------------------------------------------------------------
    static_screen_size = EVE_LIB_MemRead32(EVE_REG_CMD_DL);

    //--------------------------------------------------------------------------------------------------------
    // copy static screen display into an unoccupied section of RAM_G
    // so we can call this with the CMD_APPEND command
    //--------------------------------------------------------------------------------------------------------
    EVE_LIB_BeginCoProList();
    // memcpy from RAM_DL to RAM_G
    EVE_CMD_MEMCPY(static_screen_location, EVE_RAM_DL, static_screen_size); // dest, src, num
    EVE_LIB_EndCoProList();
    EVE_LIB_AwaitCoProEmpty();
}

/**
 @brief Function to issue a display list to EVE to update the screen contents.
 */
void renderScreenUpdate(){

    //--------------------------------------------------------------------------------------------------------
    // Construct display list and send to EVE
    //--------------------------------------------------------------------------------------------------------

    // start the display list
    EVE_LIB_BeginCoProList();
    EVE_CMD_DLSTART();

    // append static sections of display list that were previously generated and stored in RAM_G
    EVE_CMD_APPEND(static_screen_location, static_screen_size);
    
    //--------------------------------------------------------------------------------------------------------
    // add three line plots on top of the graph lines from the static section of the display
    //--------------------------------------------------------------------------------------------------------
    
    linePlot(line_graph_x, line_graph_y, line_graph_width, line_graph_height, colour1, line_graph_line_width, (line_graph_extra_x_lines  + 1), line_plot1_data);
    linePlot(line_graph_x, line_graph_y, line_graph_width, line_graph_height, colour2, line_graph_line_width, (line_graph_extra_x_lines  + 1), line_plot2_data);
    linePlot(line_graph_x, line_graph_y, line_graph_width, line_graph_height, colour3, line_graph_line_width, (line_graph_extra_x_lines  + 1), line_plot3_data);

    // add number readouts for the line plots
    //--------------------------------------------------------------------------

    #if IS_EVE_API(2,3,4,5) // if we arnt FT8xx
    // call ROMFONT so we can use a larger font in handle font_large
    EVE_CMD_ROMFONT(font_large, font_xl);
    #endif

    // number and colour 1
    EVE_COLOR_RGB(((uint8_t)(colour1 >> 16)), ((uint8_t)(colour1 >> 8)), ((uint8_t)(colour1)));
    EVE_CMD_NUMBER(line_graph_num1_x, line_graph_num1_y, font_large, EVE_OPT_CENTER, ((line_plot1_data[plot_data_size-1] * 100)/255));
    // number and colour 2
    EVE_COLOR_RGB(((uint8_t)(colour2 >> 16)), ((uint8_t)(colour2 >> 8)), ((uint8_t)(colour2)));
    EVE_CMD_NUMBER(line_graph_num2_x, line_graph_num2_y, font_large, EVE_OPT_CENTER, ((line_plot2_data[plot_data_size-1] * 100)/255));
    // number and colour 3
    EVE_COLOR_RGB(((uint8_t)(colour3 >> 16)), ((uint8_t)(colour3 >> 8)), ((uint8_t)(colour3)));
    EVE_CMD_NUMBER(line_graph_num3_x, line_graph_num3_y, font_large, EVE_OPT_CENTER, ((line_plot3_data[plot_data_size-1] * 100)/255));

    #if IS_EVE_API(2,3,4,5) // if we arnt FT8xx
    // call ROMFONT so we can reset the font handle back to what we originally set
    EVE_CMD_ROMFONT(font_large, font_large);
    #endif

    // reset colour
    EVE_COLOR_RGB(255, 255, 255); // white

    //--------------------------------------------------------------------------------------------------------
    // add our bar gauges onto the screen
    //--------------------------------------------------------------------------------------------------------
    
    // first gauges
    verticalBarGauge(bargauge1_x, bargauge1_y, bargauge_width, bargauge_height, colour1, colour4, bar_value[0]);
    verticalBarGauge(bargauge2_x, bargauge2_y, bargauge_width, bargauge_height, colour1, 0, bar_value[1]);
    // second guages
    verticalBarGauge(bargauge3_x, bargauge3_y, bargauge_width, bargauge_height, colour2, colour4, bar_value[2]);
    verticalBarGauge(bargauge4_x, bargauge4_y, bargauge_width, bargauge_height, colour2, 0, bar_value[3]);
    // third guages
    verticalBarGauge(bargauge5_x, bargauge5_y, bargauge_width, bargauge_height, colour3, colour4, bar_value[4]);
    verticalBarGauge(bargauge6_x, bargauge6_y, bargauge_width, bargauge_height, colour3, 0, bar_value[5]);

    //--------------------------------------------------------------------------------------------------------
    // add our circular gauges onto the screen
    //--------------------------------------------------------------------------------------------------------

    // add three circle gauges
    //--------------------------------------------------------------------------
    circleGaugeShadow(circle_guage1_x, circle_guage1_y, circle_gauge_radius, circle_gauge_thickness, circle_value);
    circleGaugeShadow(circle_guage2_x, circle_guage2_y, circle_gauge_radius, circle_gauge_thickness, circle_value);
    circleGaugeShadow(circle_guage3_x, circle_guage3_y, circle_gauge_radius, circle_gauge_thickness, circle_value);

    // add readout numbers for gauges
    //--------------------------------------------------------------------------
    // first gauge
    EVE_CMD_NUMBER(circle_guage3_x, circle_guage3_y, font_large, EVE_OPT_CENTER, ((circle_value * 100)/360)); // normalise number to 0-100
    // second guage
    EVE_CMD_NUMBER(circle_guage2_x, circle_guage2_y, font_large, EVE_OPT_CENTER, ((circle_value * 100)/360)); // normalise number to 0-100
    // Third guage
    EVE_CMD_NUMBER(circle_guage1_x, circle_guage1_y, font_large, EVE_OPT_CENTER, ((circle_value * 100)/360)); // normalise number to 0-100

    //--------------------------------------------------------------------------------------------------------
    // add control panel buttons onto the screen
    //--------------------------------------------------------------------------------------------------------

    // we always want to draw the control menu
    controlMenu(menu_x, menu_y, menu_length, menu_size, colour1);

    // decide which sub-menu we wish to render based on the menu item 1 press state
    if(menu_item_1_press == true){
        // if menu item 1 then drawn the mode sub-menu
        modePage();
    }else{
        // else draw the LCD backlight setting page
        LCDBacklightPage();
    }

    // display
    EVE_DISPLAY();
    EVE_DISPLAY(); // per BRT_TN_005
    // swap this display list inro RAM_DL
    EVE_CMD_SWAP();
    // send display list to co-processor
    EVE_LIB_EndCoProList();
    EVE_LIB_AwaitCoProEmpty();

}

// ######################################################################################################################################################################################################
// #######################################################                             Main example code                              ###################################################################
// ######################################################################################################################################################################################################

/**
 @brief Helper function to initially populate the data arrays for the line graph labels.
 */
void setAxisLabels(void){
    
    // populate y axis labels arrays
    y_axis_labels[0] = 100;
    y_axis_labels[1] = 80;
    y_axis_labels[2] = 60;
    y_axis_labels[3] = 40;
    y_axis_labels[4] = 20;

    // populate x axis labels arrays
    x_axis_labels[0] = "0:30";
    x_axis_labels[1] = "0:25";
    x_axis_labels[2] = "0:20";
    x_axis_labels[3] = "0:15";
    x_axis_labels[4] = "0:10";
    x_axis_labels[5] = "0:05";
    x_axis_labels[6] = "0:00";
}

/**
 @brief Helper function to read current touch inputs and update screen rendering variables accordingly.
 */
void checkTouchStatus(void)
{

    // ================ Check for button presses ===========================

    // read REG_TOUCH_TAG and determine what TAG value is returned
    #if IS_EVE_API(1,2,3,4)
    TagVal = EVE_LIB_MemRead8(EVE_REG_TOUCH_TAG);   
    #else
    TagVal = EVE_LIB_MemRead32(EVE_REG_TOUCH_TAG);   
    #endif


    //-------- Check for pen up and pen down tags -------
	if ((LastTagVal == 0) && (TagVal != 0))	// if there was previously no touch but now there is
		Pen_Down_Tag = TagVal;
	if ((LastTagVal != 0) && (TagVal == 0)) // if there was previously touch but now there isnt
		Pen_Up_Tag = LastTagVal;

	LastTagVal = TagVal;

	//-------- perform logic for control menu buttons-------
    // if the pen up tag equals menu_item_1 AND the current menu press is not menu_item_1
	if ((Pen_Up_Tag == menu_item_1_tag) && menu_item_1_press == false){
		// reset variables
		Pen_Down_Tag = 0;
		Pen_Up_Tag = 0;

		// flip boolean state for menu_item_1 and menu_item_2 press
		menu_item_1_press = !menu_item_1_press;
        menu_item_2_press = !menu_item_2_press;
   	}
    // if the pen up tag equals menu_item_2 AND the current menu press is not menu_item_2
	if ((Pen_Up_Tag == menu_item_2_tag) && menu_item_2_press == false){
		// reset variables
		Pen_Down_Tag = 0;
		Pen_Up_Tag = 0;

		// flip boolean state for menu_item_2 and menu_item_1 press
		menu_item_2_press = !menu_item_2_press;
        menu_item_1_press = !menu_item_1_press;
   	}

    //-------- perform logic for mode menu buttons-------

    // if the current tag value equals mode button 1 tag
	if (TagVal == mode_button_1_tag)
		mode_button_1_press = true; // set button 1 press to true
    else
	    mode_button_1_press = false; // set button 1 press to false
    

    // if the current tag value equals mode button 2 tag
	if (TagVal == mode_button_2_tag)
		mode_button_2_press = true; // set button 2 press to true
    else
	    mode_button_2_press = false; // set button 2 press to false
    

    // if the pen up tag equals mode button 1 tag
	if (Pen_Up_Tag == mode_button_1_tag){
		//reset variables
		Pen_Down_Tag = 0;
        Pen_Up_Tag = 0;

		// flip boolean state for mode button 1 press
		mode_button_1_press = !mode_button_1_press;
        // flip demo mode varibale
        demoMode = !demoMode;
   	}

    // if the pen up tag equals mode button 2 tag
	if (Pen_Up_Tag == mode_button_2_tag){
		// reset variables
		Pen_Down_Tag = 0;
        Pen_Up_Tag = 0;

		// flip boolean state for mode button 1 press
		mode_button_2_press = !mode_button_2_press;
        // flip demo mode varibale
        demoMode = !demoMode;
   	}
  
    //-------- perform logic for lcd backlight menu buttons-------
    // if the tag value is equal to the backlight dial i.e. the item using CMD_TRACKER
    if (TagVal  == backlight_dial_tag){

        // reset variables
		Pen_Down_Tag = 0;
		Pen_Up_Tag = 0;    

        // register has a differnt name for the FT80x series
        #if IS_EVE_API(2,3,4,5)
                TrackValue = EVE_LIB_MemRead32(EVE_REG_TRACKER);
        #else
                TrackValue = EVE_LIB_MemRead32(EVE_REG_TRACK);
        #endif

        // determine the current angel in degress from the TrackValue
        angle = (((360 * (((TrackValue >> 16)) & 0xffff)) / 0x10000));

        // pefrom some simple rollover checking below
        // if angle is within range then set this to the last valid angle
        if(angle >= backlight_arc_start_deg && angle <= backlight_arc_end_deg ){
            last_valid_angle = angle;
        }
        else if ((angle > backlight_arc_end_deg || angle < backlight_arc_start_deg) && (last_valid_angle == backlight_arc_end_deg || last_valid_angle > (backlight_arc_end_deg - 10))){
            // if we are in an invalid section of the arc, and the last valid angle is near the end of the arc
            // set last valid angle to the end of the arc
            last_valid_angle = backlight_arc_end_deg;
        }else if ((angle < backlight_arc_start_deg || angle > backlight_arc_end_deg) && (last_valid_angle == backlight_arc_start_deg || last_valid_angle < (backlight_arc_start_deg + 10))){
            // if we are in an invalid section of the arc, and the last valid angle is near the start of the arc
            // set last valid angle to the start of the arc
            last_valid_angle = backlight_arc_start_deg;
        }

        // update the backlight level variable
        // nomralise this to a rang of 0-100 based on the angles used
        backlight_value = (((last_valid_angle - backlight_arc_start_deg) * 100)/backlight_arc_total_deg);

        // write the backlight strength register based upon the current angle
        // add 27 here to the backlight value so the value range becomes 27-127
        EVE_LIB_MemWrite32(EVE_REG_PWM_DUTY, (backlight_value + 27));
    
    }
    return;

}

/**
 @brief Helper function to update data arrays for the readouts if we are in demo mode.
 */
void demoDataUpdates(){

    //--------------------------------------------------------------------------------------------------------
    // logic to move the readings for bar gauges
    //--------------------------------------------------------------------------------------------------------

    for (uint8_t i = 0; i < (bargauge_num_bars); i++){ 
        // check if we need to change the bar_value direction
        if(bar_value [i] == 0){
            bar_dir [i] = 0x00;
        }            
        else if (bar_value [i] == 255)
        {
            bar_dir [i] = 0xFF;
        }         
        
        // add or remove from value based on the dir
        if (bar_dir[i] == 0x00){
            // account for when loop var is at 0
            if(i < 1)
                bar_value[i] = min((bar_value[i] + 1), 255); // plus 1 if this the case, also ensure were not out of range
            else
                bar_value[i] = min((bar_value[i] + (i+1)), 255); // plus i + 1 if i > 0, also ensure were not out of range
        }
        else  {
            // accouunt for when loop var is at 0
            if(i < 1)
                bar_value[i] = max((bar_value[i] - 1), 0); // minus 1 if this the case, also ensure were not out of range
            else
                bar_value[i] = max((bar_value[i] - (i+1)), 0); // minus i + 1 if i > 0, also ensure were not out of range
        }        
    } 

    //--------------------------------------------------------------------------------------------------------
    // logic to move the reading counter for circle gauges
    //--------------------------------------------------------------------------------------------------------

    if(circle_dir == 0)
    {            
        if(circle_value == 0)            
            circle_dir = 0xFF;            
        else            
            circle_value -= 1;            
    }
    else
    {
        if(circle_value == 360)             
            circle_dir = 0x00;            
        else        
            circle_value += 1;        
    }

    //--------------------------------------------------------------------------------------------------------
    // logic to move the line graph values around
    //--------------------------------------------------------------------------------------------------------

    // shift the values if count is % 50 == 0
    if (count % 50 == 0){
        // declar some temps
        uint8_t temp1 = (rand() % (220 + 1 - 180) + 180); // between 220 and 180
        uint8_t temp2 = (rand() % (250 + 1 - 70) + 70); // between 250 and 70
        uint8_t temp3 = (rand() % (140 + 1 - 90) + 90); // between 140 and 190
        // shift the other values right
        for(int i = 0; i < (plot_data_size - 1); i++){
            line_plot1_data[i]=line_plot1_data[i+1];
            line_plot2_data[i]=line_plot2_data[i+1];
            line_plot3_data[i]=line_plot3_data[i+1];
        }
        // add the temps back into the end of the arrays
        line_plot1_data[plot_data_size -1] = temp1;
        line_plot2_data[plot_data_size -1] = temp2;
        line_plot3_data[plot_data_size -1] = temp3;

    }

    // increment count
    count ++;
}

/**
 @brief Function to send display lists to EVE within a while(1) 'main' loop. This function constructs a display list to render screens to EVE
 within a while (1) loop, calling functions to add widgets onto the screen. It also contains the data arrays and variables used to update 
 readouts and widgets on the screen. Finally it will perform some logic to loop through applicable data arrays or change variables.
 */
void eve_display(void)
{

    // set the axis labels we want to use for the line graph
    setAxisLabels();

    // generate display list entries for static sections of the screen
    generateStaticScreenComponents();

    // main loop
    while(1)
    {            
        
        // check if any of our buttons have been pressed
        // we are polling this for simplicity, but we can use the INT_N pin to trigger an  interrupt
        // for touch input and use this to call the checkTouchStatus() function. We could also call
        // renderScreenUpdate() if required based upon touch inputs
        checkTouchStatus();

        //--------------------------------------------------------------------------------------------------------
        // Update the screen with either demo data or sensor data
        //--------------------------------------------------------------------------------------------------------

        // again we are simply issuing screen updates continuously, but we can gate these by time elapsed, or sensor
        // data rates and use either to determine when to call renderScreenUpdate()
        if(demoMode){
            // call the helper function to update data arrays
            demoDataUpdates();

            // call render screen funciton to update the screen
            renderScreenUpdate();
        }else{
            // else we want to read some data from our attached sensors
            // TODO: add code to read sensor values 

            // call render screen funciton to update the screen
            renderScreenUpdate();
        } 
          
    }
}

// ######################################################################################################################################################################################################
// #######################################################                        Application Code begins here                        ###################################################################
// ######################################################################################################################################################################################################

/**
 @brief Function to start the EVE application, called from main.c. This function will call separate functions to initialize EVE, and calibrate
 touch for the screen. Finally it will call eve_display() to run the main display loop and update the screen.  
 */
void eve_example(void)
{

    EVE_Init();             // Initialise the display

    eve_calibrate();        // Calibrate the display

    eve_display();          // Run Application
}
