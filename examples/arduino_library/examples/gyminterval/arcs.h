/**
 @file arcs.h
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

#ifndef EVE_ARCS_H
#define EVE_ARCS_H

#include "trig_furman.h"

/**
 @brief Function to draw a simple arc gauge, using a indicator point with 
        blanking.
 @details This function will draw and arc guage and fill it based upon the 
          user_value input vairble, utilising a simple indicator point which 
          has a blanking outline within the gauge fill.
 @param x x position for the center of the arc.
 @param y y position for the center of the arc.
 @param r0 radius value of inner part of the arc.
 @param r1 radius value of outer part of the arc.
 @param a0 degrees clockwise from the bottom of the circle where 
                      we want the arc to start.
 @param a1 degrees clockwise from the bottom of the circle where 
                    we want the arc to end.
 */
void arc_simple(int16_t x, int16_t y, 
    uint16_t r0, uint16_t r1,
    uint16_t a0, uint16_t a1
);

/**
 @brief Function to draw a simple arc gauge, using a indicator point with 
        blanking.
 @details This function will draw and arc guage and fill it based upon the 
          user_value input vairble, utilising a simple indicator point which 
          has a blanking outline within the gauge fill.
 @param x x position for the center of the arc.
 @param y y position for the center of the arc.
 @param r0 radius value of inner part of the arc.
 @param r1 radius value of outer part of the arc.
 @param a0 degrees clockwise from the bottom of the circle where 
                      we want the arc to start.
 @param a1 degrees clockwise from the bottom of the circle where 
                    we want the arc to end.
 @param user_value position of indicator on the arc.
 */
void arc_simple_gauge(int16_t x, int16_t y, 
    uint16_t r0, uint16_t r1,
    uint16_t a0, uint16_t a1,
    uint16_t val
);

#endif // EVE_ARCS_H
