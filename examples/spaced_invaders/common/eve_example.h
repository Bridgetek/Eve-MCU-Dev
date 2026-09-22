/**
 * @file eve_example.h
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

#ifndef _EVE_EXAMPLE_H
#define _EVE_EXAMPLE_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <EVE.h>

/* Headers required for this example */
#include <touch.h>

// ==========================================================================
// Defined Constants
// ==========================================================================

// Play area size and position on display ...
// This is the logical game play area in pixels
// NOTE these pixels are scaled to fit on screen
#define DISPLAY_XPIX 240                    // Gameplay width
#define DISPLAY_YPIX 240                    // Gameplay height

// Game state values ...
#define GAME_START_NEW false
#define GAME_NEXT_WAVE true

// Button tag values ...
#define BUTTON_START 1
#define BUTTON_LEFT  2
#define BUTTON_RIGHT 3
#define BUTTON_FIRE1 4
#define BUTTON_FIRE2 5

// Life states (used by all objects) ...
#define IS_KAPUT   0
#define IS_TOAST   1
#define IS_ALIVE_1 2
#define IS_ALIVE_2 3
#define IS_ALIVE_3 4
#define IS_ALIVE_4 5

// Player specific info ...
#define PLAYERS_TOTL 1
#define PLAYERS_XPOS (DISPLAY_XPIX - 12) / 2
#define PLAYERS_YPOS (DISPLAY_YPIX - 8)
#define PLAYERS_XPIX 11
#define PLAYERS_YPIX  8
#define PLAYERS_XSPD  4
#define PLAYERS_YSPD  0
#define PLAYERS_XMIN  0
#define PLAYERS_XMAX DISPLAY_XPIX - 12

// Shield specific info ...
#define SHIELDS_TOTL 4*10
#define SHIELDS_XPOS 16
#define SHIELDS_YPOS (DISPLAY_YPIX - 32)
#define SHIELDS_XPIX 6
#define SHIELDS_YPIX 6
#define SHIELDS_XGRD (SHIELDS_XPIX * 4) + 32
#define SHIELDS_YGRD 0

// Invader specific info ...
#define INVADER_COLS 11
#define INVADER_ROWS 5
#define INVADER_TOTL (INVADER_COLS * INVADER_ROWS)

#define INVADER_XMIN 0
#define INVADER_XMAX DISPLAY_XPIX - 12
#define INVADER_YMIN 32
#define INVADER_YMAX PLAYERS_YPOS

#define INVADER_XPIX 11
#define INVADER_YPIX  8
#define INVADER_XGRD 16
#define INVADER_YGRD 16
#define INVADER_XSPD  4
#define INVADER_YSPD  8

// Bullet specific info ...
#define BULLETS_YSPD  8
#define BULLETS_PLAY  0
#define BULLETS_INV1  1
#define BULLETS_INV2  2
#define BULLETS_TOTL  3

// Scaling minimum and maximum macros
#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

// Scaling calculations
#define SCALE(a, sc) (((a) * (sc)) / 0x10000)

#ifndef EVE_OPT_FORMAT
#define EVE_OPT_FORMAT 0x1000ul
#endif

/* Entry point to the example code */
void eve_example(void);

/* Functions called from eve_example code to platform specific code */
uint32_t platform_get_time(void);

/* Load Invaders data */
void spaced_invaders_LoadBitmaps(uint32_t scale);

/* sample app structure definitions */
typedef struct bitmap_info
{
    uint8_t Format;
    int16_t Width;
    int16_t Height;
    int16_t Stride;
    int32_t Arrayoffset;
    uint8_t Cells;
} bitmap_info_t;

void SAMAPP_BootupConfig();

// =================================================================================================================
// Spaced Invaders - Bits
// =================================================================================================================
typedef struct siObject
{
    int16_t xcrd;
    int16_t ycrd;
    uint8_t type;
    uint8_t life;
}siObject_t;

// Bitmap handles ...
#define BITMAP_INVADERS    0
#define BITMAP_SHIELDS     1
// Font handles
#define BITMAP_FIXED_FONT  16
#define BITMAP_SCALED_FONT 14

// Cell handles (individual cell offsets within bitmaps) ...
#define CH_INVADERS  0
#define CH_EXPLODES 10
#define CH_BULLET_1 11
#define CH_BULLET_2 13
#define CH_BULLET_3 15
#define CH_DEFENDER 17
#define CH_SHIELD_1  0
#define CH_SHIELD_2  5
#define CH_SHIELD_3 10
#define CH_SHIELD_4 15
#define CH_SHIELD_5 20

// Global variables
extern uint8_t playerAlive; // flag for player alive
extern int16_t invaderAlive; // current number of invaders still alive
extern uint8_t invaderLands; // flag to mark that an invader has reached bottom of display
extern uint8_t invaderMarch; // flag to determine invader animation state (only for ALIVE state)
extern uint8_t bulletMarch1; // flag to determine bullets animation state (only for ALIVE state)
extern uint8_t bulletMarch2; // flag to determine bullets animation state (only for ALIVE state)
extern uint8_t bulletMarch3; // flag to determine bullets animation state (only for ALIVE state)

extern siObject_t invaderTable[INVADER_TOTL];
extern siObject_t shieldsTable[SHIELDS_TOTL];
extern siObject_t playersTable[PLAYERS_TOTL];
extern siObject_t bulletsTable[BULLETS_TOTL];

extern siObject_t * const xinvader;
extern siObject_t * const xshields;
extern siObject_t * const xplayers;
extern siObject_t * const xbullets;

extern int16_t dispX;  // X coordinate of top-left of Gameplay
extern int16_t dispY;  // Y coordinate of top-left of Gameplay

extern int16_t scoreCurrent;
extern int16_t scoreHighest;

// Random number generator
int pseudo_rand(void);

// Demo declarations
void spaced_invaders_WaitForStart(uint32_t scale, int16_t dx, int16_t dy);

// Rendering declarations
void spaced_invaders_CheckButtons(uint8_t tag);
void spaced_invaders_FlushDisplay(void);
void spaced_invaders_StartDisplay(void);
void spaced_invaders_ScissorDisplay(uint32_t scale, int16_t dx, int16_t dy);
void spaced_invaders_RenderButtons(uint32_t scale, int16_t dx, int16_t dy);
void spaced_invaders_RenderBullets(uint32_t scale, int16_t dx, int16_t dy);
void spaced_invaders_RenderScoring(uint32_t scale, int16_t dx, int16_t dy);
void spaced_invaders_RenderPlayers(uint32_t scale, int16_t dx, int16_t dy);
void spaced_invaders_RenderShields(uint32_t scale, int16_t dx, int16_t dy);
void spaced_invaders_RenderInvader(uint32_t scale, int16_t dx, int16_t dy);
void spaced_invaders_RenderTextual(uint32_t scale, int16_t dx, int16_t dy, uint16_t xcrd, uint16_t ycrd, const char * text);

// Gameplay declarations
void spaced_invaders_BeginGame(uint8_t gamestate);
int8_t spaced_invaders_UpdateInvader(void);
int8_t spaced_invaders_UpdateShields(void);
int8_t spaced_invaders_UpdatePlayers(void);
int8_t spaced_invaders_UpdateBullets(void);

// Scaling declarations
void scaledVertex(uint32_t scale, int16_t input_x, int16_t input_y);
void scaledPointSize(uint32_t scale, uint16_t size);
void scaledVertexTranslate(uint32_t scale, int16_t dx, int16_t dy, int16_t x, int16_t y);
void scaledVertexFormat(uint8_t format);
void scaledBitmap(uint32_t scale);
void scaledFont(uint32_t scale, uint8_t romfont, uint8_t mappedfont);

// Application
void spaced_invaders(void);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* _EVE_EXAMPLE_H */
