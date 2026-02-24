/**
    @file eve_example.h
 **/
/*
 * ============================================================================
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

#ifndef _EVE_EXAMPLE_H
#define _EVE_EXAMPLE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "touch.h"
#include "maths/trig_furman.h"
#include "controls/arcs.h"

/**
 @brief Definitions background box positioning and sizes.
 */
#define line_graph_box_start_x (EVE_DISP_WIDTH/40) // 2.5%
#define line_graph_box_start_y (EVE_DISP_HEIGHT/40) // 2.5%
#define line_graph_box_end_x ((EVE_DISP_WIDTH * 28)/40) // 70%
#define line_graph_box_end_y ((EVE_DISP_HEIGHT * 20)/40) // 50%

#define bar_gauge_box_start_x (EVE_DISP_WIDTH/40) // 2.5%
#define bar_gauge_box_start_y ((EVE_DISP_HEIGHT * 22)/40) // 55%
#define bar_gauge_box_end_x ((EVE_DISP_WIDTH * 51)/160) // 31.875%
#define bar_gauge_box_end_y ((EVE_DISP_HEIGHT * 39)/40) // 97.5%

#define controls_box_start_x ((EVE_DISP_WIDTH * 29)/40) // 72.5%
#define controls_box_start_y (EVE_DISP_HEIGHT/40) // 2.5%
#define controls_box_end_x ((EVE_DISP_WIDTH * 39)/40) // 97.5%
#define controls_box_end_y ((EVE_DISP_HEIGHT * 20)/40) // 50% 

#define circle_gauge_box_start_x ((EVE_DISP_WIDTH * 55)/160) // 34.375%
#define circle_gauge_box_start_y ((EVE_DISP_HEIGHT * 22)/40) // 55%
#define circle_gauge_box_end_x ((EVE_DISP_WIDTH * 39)/40) // 97.5%
#define circle_gauge_box_end_y ((EVE_DISP_HEIGHT * 39)/40) // 97.5%

/**
 @brief Definitions of line graph sizing and positioning.
 */
// for labels
#define line_graph_label_x ((EVE_DISP_WIDTH * 2)/40) // 5%
#define line_graph_label_y ((EVE_DISP_HEIGHT * 2)/40) // 5%
// for readout
#define line_graph_num1_x ((EVE_DISP_WIDTH * 51)/80) // 63.75%
#define line_graph_num1_y ((EVE_DISP_HEIGHT * 5)/40) // 12.5%
#define line_graph_num2_x ((EVE_DISP_WIDTH * 51)/80) // 63.75%
#define line_graph_num2_y ((EVE_DISP_HEIGHT * 21)/80) // 26.25%
#define line_graph_num3_x ((EVE_DISP_WIDTH * 51)/80) // 63.75%
#define line_graph_num3_y ((EVE_DISP_HEIGHT * 16)/40) // 40%
// for graph positioning and size
#define line_graph_x ((EVE_DISP_WIDTH * 3)/40) // 7.5%
#define line_graph_y ((EVE_DISP_HEIGHT * 5)/40) // 12.5%
#define line_graph_height ((EVE_DISP_HEIGHT * 25)/80) // 31.25%
#define line_graph_width ((EVE_DISP_WIDTH * 20)/40) // 50%
#define line_graph_extra_x_lines 6
#define line_graph_extra_y_lines 5

// for the sizeing of labels and data arrays
#define x_axis_labels_size 7 // set to (line_graph_extra_x_lines  + 1)
#define y_axis_labels_size 5 // set to line_graph_extra_y_lines 
#define plot_data_size 7 // set to (line_graph_extra_x_lines  + 1)

//set this width based on screen size
#if (EVE_DISP_WIDTH < 1000)
    #define line_graph_line_width 1
#elif (EVE_DISP_WIDTH >=1000 && EVE_DISP_WIDTH <= 1500)
    #define line_graph_line_width 2
#elif (EVE_DISP_WIDTH > 1500)
    #define line_graph_line_width 3
#endif


/**
 @brief Definitions of bar gauge size and positions.
 */
// bar guage sizing related 
#define bargauge_width (EVE_DISP_WIDTH/32) // 3.1%
#define bargauge_height ((EVE_DISP_HEIGHT*21)/80) // 26.25%
//bargauge positioning 
#define bargauge1_x ((EVE_DISP_WIDTH * 2)/40) // 5%
#define bargauge1_y ((EVE_DISP_HEIGHT * 24)/40) // 60%
#define bargauge2_x ((EVE_DISP_WIDTH * 7)/80) // // 8.75%
#define bargauge2_y ((EVE_DISP_HEIGHT * 24)/40) // 60%
#define bargauge3_x ((EVE_DISP_WIDTH * 11)/80) // 13.75%
#define bargauge3_y ((EVE_DISP_HEIGHT * 24)/40) // 60%
#define bargauge4_x ((EVE_DISP_WIDTH * 7)/40) // 17.5%
#define bargauge4_y ((EVE_DISP_HEIGHT * 24)/40) // 60%
#define bargauge5_x ((EVE_DISP_WIDTH * 18)/80) // 22.5%
#define bargauge5_y ((EVE_DISP_HEIGHT * 24)/40) // 60%
#define bargauge6_x ((EVE_DISP_WIDTH * 21)/80) // 26.25%
#define bargauge6_y ((EVE_DISP_HEIGHT * 24)/40) // 60%
// total bars
#define bargauge_num_bars 6
// for labels
#define bargauge_label_y ((EVE_DISP_HEIGHT * 36)/40) // 90%
#define bargauge_label_height ((EVE_DISP_HEIGHT * 3)/80) // 3.75% 

/**
 @brief Definitions of circular gauge size and positions.
 */
// circular guage sizing related 
#define circle_gauge_radius (EVE_DISP_HEIGHT/7) // 14.2 %
#define circle_gauge_thickness (EVE_DISP_WIDTH/64) // 1.5 %
// circular gauge positioning 
#define circle_guage1_x ((EVE_DISP_WIDTH * 37)/80) // 46.25 %
#define circle_guage1_y ((EVE_DISP_HEIGHT * 59)/80) // 73.75%
#define circle_guage2_x ((EVE_DISP_WIDTH * 53)/80) // 66.25%
#define circle_guage2_y ((EVE_DISP_HEIGHT * 59)/80) // 73.75%
#define circle_guage3_x ((EVE_DISP_WIDTH * 69)/80) // 86.25 %
#define circle_guage3_y ((EVE_DISP_HEIGHT * 59)/80) // 73.75%

/**
 @brief Definitions of control menu size and position.
 */
#define menu_x ((EVE_DISP_WIDTH * 31)/40) // 77.5%
#define menu_y ((EVE_DISP_HEIGHT * 9)/80) // 11.25%
#define menu_length ((EVE_DISP_WIDTH * 49)/320) // 15.3%
#define menu_size (EVE_DISP_HEIGHT/20) // 5% 

/**
 @brief Definitions of mode menu buttons and readout, size and positions.
 */
// sizing related 
#define mode_button_size (EVE_DISP_HEIGHT/20) // 5%
// positioning related 
#define mode_button1_x ((EVE_DISP_WIDTH * 32)/40) // 
#define mode_button1_y ((EVE_DISP_HEIGHT * 17)/40) // 42.5%
#define mode_button2_x ((EVE_DISP_WIDTH * 36)/40) // 
#define mode_button2_y ((EVE_DISP_HEIGHT * 17)/40) // 42.5%
// for readout
#define mode_start_readout_x ((EVE_DISP_WIDTH * 31)/40) // 92.5%
#define mode_start_readout_y ((EVE_DISP_HEIGHT * 9)/40) // 22.5%
#define mode_end_readout_x ((EVE_DISP_WIDTH * 37)/40) // 92.5%
#define mode_end_readout_y ((EVE_DISP_HEIGHT * 13)/40) // 32.5%
// for label
#define mode_label_x ((EVE_DISP_WIDTH * 34)/40) // 85%
#define mode_lable_y ((EVE_DISP_HEIGHT * 11)/40) // 27.5%

/**
 @brief Definitions of backlight menu arc guage size, position and angles.
 */
#define backlight_dial_x ((EVE_DISP_WIDTH * 34)/40) // 85%
#define backlight_dial_y ((EVE_DISP_HEIGHT * 14)/40) // 30%
#define backlight_dial_radius (EVE_DISP_HEIGHT/7) // 14.2 %
#define backlight_dial_thickness (EVE_DISP_HEIGHT/10) // 12.5 %
#define backlight_arc_start_deg 53
#define backlight_arc_end_deg 308
#define backlight_arc_total_deg (backlight_arc_end_deg - backlight_arc_start_deg)

/**
 @brief Definitions of handles for on screen buttons.
 */
#define menu_item_1_tag 10
#define menu_item_2_tag 11
#define mode_button_1_tag 12
#define mode_button_2_tag 13
#define backlight_dial_tag 14

/**
 @brief Definitions of handles for inbuilt fonts to be used based on screen size.
 */
#if (EVE_DISP_WIDTH < 500)
    #define font_small 20
    #define font_med 26
    #define font_med_2 26
    #define font_large 29
    #define font_xl 30
#elif (EVE_DISP_WIDTH >= 500 && EVE_DISP_WIDTH <= 1200)
    #define font_small 20
    #define font_med 27
    #define font_med_2 28
    #define font_large 30
    #define font_xl 32
#elif (EVE_DISP_WIDTH > 1200)
    #define font_small 22
    #define font_med 30
    #define font_med_2 30
    #define font_large 31
    #define font_xl 34

#endif

/* Functions called from eve_example code to platform specific code */
int8_t platform_calib_init(void);
int8_t platform_calib_write(struct touchscreen_calibration *calib);
int8_t platform_calib_read(struct touchscreen_calibration *calib);
uint32_t platform_get_time(void);

/* Entry point to the example code */
void eve_example(void);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* _EVE_EXAMPLE_H */
