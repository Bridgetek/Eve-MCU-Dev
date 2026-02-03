/**
 @file eve_graph_calllist.c
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
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <EVE.h>

#include "eve_example.h"

#if GRAPHING_METHOD == USE_GRAPH_VERTEXES

EVE_TRACE_PROPS trace[trace_total];
uint8_t trace_data[trace_total][TRACE_POINTS];

static void graph_trace_zero(uint8_t *store)
{
    uint32_t k;
    for (k = 1; k < TRACE_POINTS; k++)
    {
        store[k] = 128;
    }
}

uint32_t graph_trace_init(uint32_t last)
{
    uint8_t i;

    trace[trace_hrm].Source = hrm;
    trace[trace_hrm].SourceSize = hrm_size;
    trace[trace_resp].Source = resp;
    trace[trace_resp].SourceSize = resp_size;
    trace[trace_sat].Source = sats;
    trace[trace_sat].SourceSize = sats_size;

    // Trace data is stored on the host not in RAM_G
    for (i = 0; i < trace_total; i++)
    {
        graph_trace_zero((uint8_t *)&trace_data[i]);
        trace[i].SourceCounter = 0;
    }
    return last;
}

void graph_update(uint8_t num, uint8_t *graph_edge, uint32_t graph_step, uint32_t pos)
{
    // Update store with new data
    memcpy(&trace_data[num][pos], graph_edge, graph_step);
}

static void graph_draw_line(uint8_t num, uint32_t pos, uint32_t count, uint32_t x, uint32_t y, uint32_t scale_x, uint32_t scale_y, int8_t filter)
{
    uint32_t k;
    uint8_t y1, y2, y3;
    uint32_t x1, x2;

    y1 = 0; y2 = trace_data[num][pos];
    x1 = 0; x2 = 0;

    // Perform simple point elimination to a deviation of "filter"
    for (k = 0; k < count; k++)
    {
        if (k + pos < TRACE_POINTS)
        {
            y3 = trace_data[num][k + pos];

            // Draw the second last point if it is not within the filter
            // Then update filter to the second last point
            if ((abs(((x2 - x1) * (y3 - y2)) + y2 - y1) > filter) || (k == 0))
            {
                y1 = y2;
                x1 = x2;
                EVE_VERTEX2F(x + SCALE(x2 + pos, scale_x), y + SCALE(255 - y2, scale_y));
            }
            // Update second last point
            x2 = k;
            y2 = y3;
        }
    }
    // Always draw the last point
    EVE_VERTEX2F(x + SCALE((k - 1) + pos, scale_x), y + SCALE(255 - y3, scale_y));
}

void graph_draw(uint8_t num, uint32_t pos, uint32_t rpos, uint32_t x, uint32_t y, uint32_t scale_x, uint32_t scale_y)
{
    if (pos > 0)
    {
        EVE_BEGIN(EVE_BEGIN_LINE_STRIP);
        graph_draw_line(num, 0, pos, x, y, scale_x, scale_y, POINTS_FILTER );
        EVE_END();
    }
    if (rpos < TRACE_POINTS)
    {
        EVE_BEGIN(EVE_BEGIN_LINE_STRIP);
        graph_draw_line(num, rpos, TRACE_POINTS - rpos, x, y, scale_x, scale_y, POINTS_FILTER );
        EVE_END();
    }
}

#endif // GRAPHING_METHOD == USE_GRAPH_VERTEXES
