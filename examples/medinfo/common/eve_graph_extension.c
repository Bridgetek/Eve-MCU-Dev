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

#include <stdint.h>

#include <EVE.h>

#include "eve_example.h"

#if GRAPHING_METHOD == USE_GRAPH_EXTENSIONS

EVE_TRACE_PROPS trace[trace_total];

static void graph_trace_zero(EVE_TRACE_PROPS *asset)
{
    EVE_LIB_BeginCoProList();
    EVE_CMD_MEMSET(asset->RAM_G_Start, 128, asset->RAM_G_EndAddr - asset->RAM_G_Start);
    EVE_LIB_EndCoProList();
    EVE_LIB_AwaitCoProEmpty();
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

    // Graph trace data is stored in RAM_G
    for (i = 0; i < trace_total; i++)
    {
        trace[i].RAM_G_Start = last;
        // Allocate an area of RAM_G that contains all the data for the trace
        trace[i].RAM_G_EndAddr = trace[i].RAM_G_Start + TRACE_POINTS;
        graph_trace_zero(&trace[i]);
        last = trace[i].RAM_G_EndAddr;
        trace[i].SourceCounter = 0;
    }
    return last;
}

void graph_update(uint8_t num, uint8_t *graph_edge, uint32_t graph_step, uint32_t pos)
{
    EVE_LIB_WriteDataToRAMG(graph_edge, graph_step, trace[num].RAM_G_Start + pos);
}

void graph_draw(uint8_t num, uint32_t pos, uint32_t rpos, uint32_t x, uint32_t y, uint32_t scale_x, uint32_t scale_y)
{
    EVE_CMD_PLOTDRAW(trace[num].RAM_G_Start, pos, 
        EVE_OPT_PLOTFILTER, x, y, scale_x, scale_y, POINTS_FILTER );
    EVE_CMD_PLOTDRAW(trace[num].RAM_G_Start + rpos, TRACE_POINTS - rpos, 
        EVE_OPT_PLOTFILTER, x + SCALE(rpos, scale_x), y, 
        scale_x, scale_y, POINTS_FILTER );
}

#endif // GRAPHING_METHOD == USE_GRAPH_EXTENSIONS
