/**
 * @file eve_example.c
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

#include <stdio.h>
#include <time.h>

/* Include EVE-MCU-Dev library API layer */
#include <EVE.h> 
/* Include the EVE debug-output macros */
#include "EVE_debug.h"

#include "eve_example.h"

// ==========================================================================
// Global variables
// ==========================================================================

const uint8_t colours[24][3] =
{
    { 255, 63, 0 },
    { 255, 127, 0 }, //  1
    { 255, 191, 0 },
    { 255, 255, 0 }, //  2
    { 191, 255, 0 },
    { 127, 255, 0 }, //  3
    { 63, 255, 0 },
    { 0, 255, 0 },   //  4
    { 0, 255, 63 },
    { 0, 255, 127 }, //  5
    { 0, 255, 191 },
    { 0, 255, 255 }, //  6
    { 0, 191, 255 },
    { 0, 127, 255 }, //  7
    { 0, 63, 255 },
    { 0, 0, 255 },   //  8
    { 63, 0, 255 },
    { 127, 0, 255 }, //  9
    { 191, 0, 255 },
    { 255, 0, 255 }, // 10
    { 255, 0, 191 },
    { 255, 0, 127 }, // 11
    { 255, 0, 63 },
    { 255, 0, 0 },   // 12
};

siObject_t invaderTable[INVADER_TOTL]; // X, Y, Type, Life state
siObject_t shieldsTable[SHIELDS_TOTL]; // X, Y, Type, Life state
siObject_t playersTable[PLAYERS_TOTL]; // X, Y, Type, Life state
siObject_t bulletsTable[BULLETS_TOTL]; // X, Y, Type, Life state

siObject_t * const xinvader = invaderTable;
siObject_t * const xshields = shieldsTable;
siObject_t * const xplayers = playersTable;
siObject_t * const xbullets = bulletsTable;

int16_t dispX;  // X coordinate of top-left of Gameplay
int16_t dispY;  // Y coordinate of top-left of Gameplay

int16_t scoreCurrent = 0;
int16_t scoreHighest = 0;

static unsigned int randseed;

// ==========================================================================
// Local functions
// ==========================================================================

int pseudo_rand(void)
{
    // Linear Congreuential RNG
    randseed = (randseed * 1103515245 + 12345) & RAND_MAX;
    return randseed;
}

/** @brief Random number generator seed setter
 */
static void pseudo_rand_seed(int seed)
{
    randseed = seed;
}

void spaced_invaders_RenderTextual(uint32_t scale, int16_t dx, int16_t dy, uint16_t xcrd, uint16_t ycrd, const char * text)
{
    EVE_COLOR_RGB(255, 255, 255);

    EVE_BITMAP_HANDLE(BITMAP_SCALED_FONT);
    // add text to display list ...
    while(*text)
    {
        EVE_CELL(*text);
        scaledVertexTranslate(scale, dx, dy, xcrd, ycrd);
        scaledVertex(scale, 0, 0);
        xcrd += 8;
        text++;
    }
    scaledVertexTranslate(scale, dx, dy, 0, 0);
}

void spaced_invaders_RenderInvader(uint32_t scale, int16_t dx, int16_t dy)
{
    siObject_t * invader = xinvader;
    uint8_t   band;

    EVE_BITMAP_HANDLE(BITMAP_INVADERS);

    // render all invaders according to life state ...
    for (uint8_t index = 0; index < INVADER_TOTL; index++)
    {
        band = (invader->ycrd - INVADER_YMIN) / 8;
        if (band > 23)
        {
            band = 23;
        }

        switch (invader->life)
        {
        case IS_ALIVE_1:
            // select and add appropriate invader sprite to display list (depending on row and animation state) ...
            EVE_COLOR_RGB(colours[band][0], colours[band][1], colours[band][2]);
            EVE_CELL(CH_INVADERS + invader->type * 2 + invaderMarch);
            scaledVertexTranslate(scale, dx, dy, invader->xcrd, invader->ycrd);
            scaledVertex(scale, 0, 0);
            break;

        case IS_TOAST:
            // select and add appropriate toastiness sprite to display list ...
            EVE_COLOR_RGB(colours[band][0], colours[band][1], colours[band][2]);
            EVE_CELL(CH_EXPLODES);
            scaledVertexTranslate(scale, dx, dy, invader->xcrd, invader->ycrd);
            scaledVertex(scale, 0, 0);

        case IS_KAPUT:
        default:
            invader->life = IS_KAPUT;
            break;
        }
        // next invader to check ...
        invader++;
    }
}

void spaced_invaders_RenderShields(uint32_t scale, int16_t dx, int16_t dy)
{
    siObject_t * shields = xshields;

    EVE_COLOR_RGB(0, 255, 0);
    EVE_BITMAP_HANDLE(BITMAP_SHIELDS);

    for (uint8_t index = 0; index < SHIELDS_TOTL; index++)
    {
        switch (shields->life)
        {
        case IS_ALIVE_1:
        case IS_ALIVE_2:
        case IS_ALIVE_3:
        case IS_ALIVE_4:
            // select and add appropriate shield sprite to display list ...
            EVE_CELL(shields->type + (5 - shields->life));
            scaledVertexTranslate(scale, dx, dy, shields->xcrd, shields->ycrd);
            scaledVertex(scale, 0, 0);
            break;

        case IS_TOAST:
        case IS_KAPUT:
        default:
            // nothing to see here!
            break;
        }
        // next bit of shield to check ...
        shields++;
    }
}

void spaced_invaders_RenderPlayers(uint32_t scale, int16_t dx, int16_t dy)
{
    EVE_BITMAP_HANDLE(BITMAP_INVADERS);

    // select and add appropriate player sprite to display list ...
    if (playersTable[0].life == IS_ALIVE_3)
    {
        // Green
        EVE_COLOR_RGB(0, 255, 0);
    }
    else if (playersTable[0].life == IS_ALIVE_2)
    {
        // Amber
        EVE_COLOR_RGB(255, 192, 0);
    }
    else if (playersTable[0].life == IS_ALIVE_1)
    {
        // Red
        EVE_COLOR_RGB(255, 0, 0);
    }
    EVE_CELL(playersTable[0].type + bulletsTable[BULLETS_PLAY].life);
    scaledVertexTranslate(scale, dx, dy, playersTable[0].xcrd, playersTable[0].ycrd);
    scaledVertex(scale, 0, 0);
    scaledVertexTranslate(scale, dx, dy, 0, 0);
}

void spaced_invaders_RenderBullets(uint32_t scale, int16_t dx, int16_t dy)
{
    EVE_BITMAP_HANDLE(BITMAP_INVADERS);

    // select and add appropriate bullet sprites to display list ...
    if (bulletsTable[BULLETS_PLAY].life == true)
    {
        EVE_COLOR_RGB(255, 255, 255);
        EVE_CELL(CH_BULLET_1 + bulletMarch1);
        scaledVertexTranslate(scale, dx, dy, bulletsTable[BULLETS_PLAY].xcrd, bulletsTable[BULLETS_PLAY].ycrd);
        scaledVertex(scale, 0, 0);
    }

    if (bulletsTable[BULLETS_INV1].life == true)
    {
        EVE_COLOR_RGB(255, 255, 255);
        EVE_CELL(CH_BULLET_2 + bulletMarch2);
        scaledVertexTranslate(scale, dx, dy, bulletsTable[BULLETS_INV1].xcrd, bulletsTable[BULLETS_INV1].ycrd);
        scaledVertex(scale, 0, 0);
    }

    if (bulletsTable[BULLETS_INV2].life == true)
    {
        EVE_COLOR_RGB(255, 255, 255);
        EVE_CELL(CH_BULLET_3 + bulletMarch3);
        scaledVertexTranslate(scale, dx, dy, bulletsTable[BULLETS_INV2].xcrd, bulletsTable[BULLETS_INV2].ycrd);
        scaledVertex(scale, 0, 0);
    }
    scaledVertexTranslate(scale, dx, dy, 0, 0);
}

void spaced_invaders_RenderScoring(uint32_t scale, int16_t dx, int16_t dy)
{
    static char displayText[30];

    // add score headings to display list ...
    sprintf(displayText, " SCORE<1> HI-SCORE SCORE<2>");
    spaced_invaders_RenderTextual(scale, dx, dy, 0, 0, displayText);

    // add score values to display list ...
    sprintf(displayText, "  %06d   %06d   000000 ", scoreCurrent, scoreHighest);
    spaced_invaders_RenderTextual(scale, dx, dy, 0, 12, displayText);
    scaledVertexTranslate(scale, dx, dy, 0, 0);
}

void spaced_invaders_RenderButtons(uint32_t scale, int16_t dx, int16_t dy)
{
    // add control buttons to display list ...
    EVE_BEGIN(EVE_BEGIN_POINTS);
    EVE_POINT_SIZE((dx / 3) * 16);

    EVE_COLOR_RGB(0xff, 0, 0);
    EVE_TAG(BUTTON_FIRE1);
    scaledVertexTranslate(scale, (dx / 2), EVE_DISP_HEIGHT / 2, 0, 0);
    scaledVertex(scale, 0, 0);
    EVE_TAG(BUTTON_FIRE2);
    scaledVertexTranslate(scale, EVE_DISP_WIDTH - (dx / 2), EVE_DISP_HEIGHT / 2, 0, 0);
    scaledVertex(scale, 0, 0);

    EVE_COLOR_RGB(0, 0xff, 0);
    EVE_TAG(BUTTON_LEFT);
    scaledVertexTranslate(scale, (dx / 2), (EVE_DISP_HEIGHT * 3) / 4, 0, 0);
    scaledVertex(scale, 0, 0);
    EVE_TAG(BUTTON_RIGHT);
    scaledVertexTranslate(scale, EVE_DISP_WIDTH - (dx / 2), (EVE_DISP_HEIGHT * 3) / 4, 0, 0);
    scaledVertex(scale, 0, 0);
}

void spaced_invaders_ScissorDisplay(uint32_t scale, int16_t dx, int16_t dy)
{
    EVE_SCISSOR_XY(dx, dy);
    EVE_SCISSOR_SIZE(SCALE(DISPLAY_XPIX, scale), SCALE(DISPLAY_YPIX, scale));
}

void spaced_invaders_StartDisplay(void)
{
    // start of new display list ...
    EVE_LIB_BeginCoProList();
    EVE_CMD_DLSTART();
    // clear display (using colour specified) ...
    EVE_CLEAR_COLOR_RGB(0, 0, 0);
    EVE_CLEAR(1, 1, 1);
}

void spaced_invaders_FlushDisplay(void)
{
    // end of display list ...
    EVE_DISPLAY();
    // Swap between frames
    EVE_CMD_SWAP();
    // Download the commands into fifo
    EVE_LIB_EndCoProList();
    // Wait till coprocessor completes the operation
    EVE_LIB_AwaitCoProEmpty();
}

void spaced_invaders(void)
{
    // the scale is dependent on the mapped screen dimensions
    const uint32_t scale_x = (EVE_DISP_WIDTH * 0x10000) / DISPLAY_XPIX;
    const uint32_t scale_y = (EVE_DISP_HEIGHT * 0x10000) / DISPLAY_YPIX;
    uint32_t scale = MIN(scale_x, scale_y);

    dispX = (EVE_DISP_WIDTH / 2) - SCALE(DISPLAY_XPIX / 2, scale);
    dispY = (EVE_DISP_HEIGHT / 2) - SCALE(DISPLAY_YPIX / 2, scale);
    
    spaced_invaders_LoadBitmaps(scale);
    scaledFont(scale, BITMAP_FIXED_FONT, BITMAP_SCALED_FONT);

    do
    {
        // wait for start button to be pressed then start game (reset and start first wave) ...
        spaced_invaders_WaitForStart(scale, dispX, dispY);
        
        // Reset all positions
        spaced_invaders_BeginGame(GAME_START_NEW);

        // flag to mark that the screen display should be updated
        int8_t updateScreen = true; 

        do // play game ...
        {
            do // play wave ...
            {
                uint32_t time_start = platform_get_time();

                uint8_t tagTouch = 0;
                eve_read_tag(&tagTouch);
                spaced_invaders_CheckButtons(tagTouch);
                updateScreen |= spaced_invaders_UpdateInvader();
                updateScreen |= spaced_invaders_UpdateShields();
                updateScreen |= spaced_invaders_UpdatePlayers();
                updateScreen |= spaced_invaders_UpdateBullets();

                if (updateScreen == true)
                {
                    // start new display list ...
                    spaced_invaders_StartDisplay();
                    EVE_SAVE_CONTEXT();
                    spaced_invaders_ScissorDisplay(scale, dispX, dispY);
                    scaledVertexFormat(0);
                    // bitmaps [START] ...
                    EVE_BEGIN(EVE_BEGIN_BITMAPS);
                    scaledBitmap(scale);
                    // add invaders, shields, player and bullets to display list ...
                    spaced_invaders_RenderPlayers(scale, dispX, dispY);
                    spaced_invaders_RenderShields(scale, dispX, dispY);
                    spaced_invaders_RenderInvader(scale, dispX, dispY);
                    spaced_invaders_RenderBullets(scale, dispX, dispY);
                    spaced_invaders_RenderScoring(scale, dispX, dispY);
                    // bitmaps [END] ...
                    EVE_END();
                    EVE_RESTORE_CONTEXT();
                    spaced_invaders_RenderButtons(scale, dispX, dispY);
                    // finish display list and send to FT800 ...
                    spaced_invaders_FlushDisplay();
                    // reset screen display update ...
                    updateScreen = false;
                }

                // 20 ms delay between frames
                while (platform_get_time() - time_start < 20)
                {
                    // CPU sleep
                }

            } while (((invaderAlive != 0) && (invaderLands == false)) &&
                        (playerAlive == true));

            if (playerAlive == false)
            {
                break;
            }
            if (invaderLands == false)
            {
                // reset invaders (2nd wave onwards) ...
                spaced_invaders_BeginGame(GAME_NEXT_WAVE);
            }

        } while (1);

    } while (1);
}

// Application Code begins here
void eve_example(void)
{
    // Seed the random number generator
    pseudo_rand_seed(0xDEAD);

    // Initialise the display
    EVE_DEBUG_PRINTF("Initialising display...\n");
    if (EVE_Init() != 0)
    {
        EVE_DEBUG_ERROR("ERROR: EVE_Init() failed.\n");
        return;
    }
    
    // Calibrate the display
    EVE_DEBUG_PRINTF("Calibrating display...\n");
    if (eve_calibrate() != 0)
    {
        EVE_DEBUG_ERROR("ERROR: eve_calibrate() failed.\n");
        return;
    }

    // Start example code
    EVE_DEBUG_PRINTF("Starting demo:\n");
    
    // Run Application
    spaced_invaders();
}
