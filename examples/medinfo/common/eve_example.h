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

#include "patch_medinfo.h"

/* Functions called from eve_example code to platform specific code */
uint32_t platform_get_time(void);

/* Entry point to the example code */
void eve_example(const char *assets);

// ---- Method for storing assets ----

// Use Flash storage to keep preconverted assets
#define USE_FLASH 1
// Use local file system (on host device) to keep preconverted assets
#define USE_FLASHIMAGE 2
// Use C arrays to store PNG files and raw assets
#define USE_C_ARRAYS 3
// Use local file system to store PNG files and raw assets
// (Convert to EVE assets as required)
#define USE_FILES 4

// Choose the methods for storing assets
#ifndef ASSETS
#define ASSETS USE_C_ARRAYS
#endif

// Validate asset storage method is appropriate
// Asset storage options which require a filesystem use the stdio library
#if (ASSETS == USE_FLASHIMAGE) || (ASSETS == USE_FILES)
// If the target platform does not support stdio 
// then this line will result in a compilation error
#include <stdio.h>
#endif

// ---- Method for rendering graphs ----

// Use the plotgraph extension to draw traces - available only on BT82x
#define USE_GRAPH_EXTENSIONS 1
// Use a vertexes to draw traces
#define USE_GRAPH_VERTEXES 2

#ifndef GRAPHING_METHOD
#define GRAPHING_METHOD USE_GRAPH_EXTENSIONS
#endif

// Validate graphing method
#if (!IS_EVE_API(5) && (GRAPHING_METHOD == USE_GRAPH_EXTENSIONS))
#error Extensions are only supported on BT82x.
#endif

// ---- Method for drawing digital readouts ----

// Use the sevenseg extension to draw digital readouts - available only on BT82x
#define USE_SEVENSEG_EXTENSIONS 1
// Use the sevenseg snippet to draw digital readouts
#define USE_SEVENSEG_SNIPPETS 2
// Use a scaled font to draw digital readouts
#define USE_SCALED_FONT 4

#ifndef DIGITAL_READOUT_METHOD
#define DIGITAL_READOUT_METHOD USE_SEVENSEG_SNIPPETS
#endif

// Validate graphing method
#if (!IS_EVE_API(5) && (DIGITAL_READOUT_METHOD == USE_SEVENSEG_EXTENSIONS))
#error Extensions are only supported on BT82x.
#endif

// ---- End of options

// Scale using EVE scaling method
#define SCALE(v, s) (((v) * (s))/0x10000)

// Number of points in a trace
#define TRACE_POINTS 800
// Trace step - maximum number of data points to scroll each time
#define TRACE_STEP 16

// Filter value for point elimination
#define POINTS_FILTER 10

// Asset storage information
typedef struct EVE_ASSET_PROPS_s
{
    uint8_t Handle;
    uint8_t Cell;
#if ((ASSETS == USE_FLASH) || (ASSETS == USE_FLASHIMAGE))
    uint32_t Flash_Start;
    uint32_t Flash_Size;
#elif (ASSETS == USE_C_ARRAYS) 
    const uint8_t * Array_Ptr;
    uint32_t Array_Size;
#elif (ASSETS == USE_FILES)
    const char *filename;
#endif
    uint32_t RAM_G_Start;
    uint32_t RAM_G_EndAddr;
    uint32_t Format;
    uint16_t Width;
    uint16_t Height;
    uint16_t CellHeight;
} EVE_ASSET_PROPS;
 
// Trace storage
typedef struct EVE_TRACE_PROPS_s
{
    uint32_t RAM_G_Start;
    uint32_t RAM_G_EndAddr;
    const uint8_t *Source; 
    uint32_t SourceSize;
    uint32_t SourceCounter;
} EVE_TRACE_PROPS;
 
// Trace buffer setup
enum {
    trace_hrm,
    trace_resp,
    trace_sat,
    trace_total
};

extern EVE_ASSET_PROPS patch_asset;
extern EVE_TRACE_PROPS trace[trace_total];

// Parts of icons_asset
enum { 
    icon_play,
    icon_pause,
    icon_pulse,
    icon_silence,
    icons_total_large
};

// Parts of icons_asset
enum { 
    icon_lock,
    icon_lock_open,
    icon_volume_off,
    icon_volume_down,
    icon_volume_up,
    icon_good,
    icon_bad,
    icon_sick,
    icon_off_on,
    icons_total_small
};

// 100x100 size icons
extern EVE_ASSET_PROPS icon_assets_small[icons_total_small];
// 200x200 size icons
extern EVE_ASSET_PROPS icon_assets_large[icons_total_large];

// Incoming data simulation arrays
extern const uint32_t hrm_size;
extern const uint8_t hrm[];
extern const uint32_t resp_size;
extern const uint8_t resp[];
extern const uint32_t sats_size;
extern const uint8_t sats [];

/* Asset loading function depends on the storage method in ASSETS macro */
void eve_asset_properties(const char *assets);
void eve_asset_load(EVE_ASSET_PROPS *asset, uint32_t loadimage);
int eve_loadpatch_impl(void);

/* Graphing functions depend on the GRAPHING_METHOD macro */
uint32_t graph_trace_init(uint32_t last);
void graph_update(uint8_t num, uint8_t *graph_edge, uint32_t graph_step, uint32_t addr);
void graph_draw(uint8_t num, uint32_t pos, uint32_t rpos, uint32_t x, uint32_t y, uint32_t scale_x, uint32_t scale_y);

#if ASSETS == USE_FLASH
void eve_flash_full_speed(void);
#endif

#include "touch.h"
#include "widgets/sevenseg.h"

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* _EVE_EXAMPLE_H */
