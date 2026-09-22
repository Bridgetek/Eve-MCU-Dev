/**
 * @file invaders_demo.c
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
#include <string.h>

#include "eve_example.h"

#define PAUSE_TEXT 6
#define PAUSE_TITLE 50
#define PAUSE_DEMO 250

typedef struct title_s
{
    const char *title;
    int16_t x; int16_t y;
} title_t;

const title_t title1a = {"PLAV", 12, 8};
const title_t title1 = {"PLA", 12, 8};
const title_t title1b = {"PLAY", 12, 8};
const title_t title2 = {"SPACED INVADERS", 7, 11};
const title_t table = {"*SCORE ADVANCE TABLE*", 4, 14};
const title_t table1 = {"=30 POINTS", 10, 16};
const title_t table2 = {"=20 POINTS", 10, 18};
const title_t table3 = {"=10 POINTS", 10, 20};
const title_t ready = {"READY PLAYER 1", 7, 11};

void spaced_invaders_WaitForStart(uint32_t scale, int16_t dx, int16_t dy)
{
    int8_t tagTouch;
    uint8_t march = 0;
    int16_t xinv = INVADER_XMAX;
    enum {
        demo_start, // Start state - no display
        demo_title_1, // First line animation
        demo_title_2, // Second line animation
        demo_table_1, demo_table_2, demo_table_3, // Scoring table animations
        demo_inv_in, demo_inv_V, demo_inv_Y, demo_inv_out, // Replacement title animations
        demo_title_end, // End of titles pause
        demo_game, // Gameplay demo
        demo_end, // End of titles
        demo_ready, // Before the game starts
        demo_play,
    };
    int demo = demo_start;
    int16_t i;
    char chanim[4] = {0,0,0,0};
    int brakes;
    int16_t move = 0;

    // Reset all positions
    spaced_invaders_BeginGame(GAME_START_NEW);

    do
    {
        const char *str = NULL;
        uint32_t time_start = platform_get_time();

        // start new display list ...
        spaced_invaders_StartDisplay();
        EVE_SAVE_CONTEXT();
        spaced_invaders_ScissorDisplay(scale, dispX, dispY);
        scaledVertexFormat(0);
        // bitmaps [START] ...
        EVE_BEGIN(EVE_BEGIN_BITMAPS);
        scaledBitmap(scale);
        spaced_invaders_RenderPlayers(scale, dispX, dispY);
        
        if (demo == demo_start)
        {
            // no action
        }

        // Title first line
        if (demo == demo_title_1)
        {
            str = title1a.title;
            for (i = 0; i < xinv; i++)
            {
                chanim[0] = str[i];
                spaced_invaders_RenderTextual(scale, dx, dy, (title1a.x + i) * 8, title1a.y * 8, chanim);
            }
        }
        else if ((demo >= demo_title_2) && (demo <= demo_inv_in))
        {
            spaced_invaders_RenderTextual(scale, dx, dy, title1a.x * 8, title1a.y * 8, title1a.title);
        }
        else if ((demo == demo_inv_V) || (demo == demo_inv_Y))
        {
            spaced_invaders_RenderTextual(scale, dx, dy, title1.x * 8, title1.y * 8, title1.title);
        }
        else if ((demo >= demo_inv_out) && (demo <= demo_title_end))
        {
            spaced_invaders_RenderTextual(scale, dx, dy, title1b.x * 8, title1b.y * 8, title1b.title);
        }

        // Title second line
        if (demo == demo_title_2)
        {
            str = title2.title;
            for (i = 0; i < xinv; i++)
            {
                chanim[0] = str[i];
                spaced_invaders_RenderTextual(scale, dx, dy, (title2.x + i) * 8, title2.y * 8, chanim);
            }
        }
        else if ((demo > demo_title_2) && (demo <= demo_title_end))
        {
            spaced_invaders_RenderTextual(scale, dx, dy, title2.x * 8, title2.y * 8, title2.title);
        }
        
        // Table top line
        if ((demo >= demo_table_1) && (demo <= demo_title_end))
        {
            spaced_invaders_RenderTextual(scale, dx, dy, 4 * 8, 14 * 8, table.title);
            EVE_BITMAP_HANDLE(BITMAP_INVADERS);
            EVE_CELL(CH_INVADERS + 1);
            scaledVertexTranslate(scale, dx, dy, 8 * 8, 16 * 8);
            scaledVertex(scale, 0, 0);
            EVE_CELL(CH_INVADERS + 2);
            scaledVertexTranslate(scale, dx, dy, 8 * 8, 18 * 8);
            scaledVertex(scale, 0, 0);
            EVE_CELL(CH_INVADERS + 7);
            scaledVertexTranslate(scale, dx, dy, 8 * 8, 20 * 8);
            scaledVertex(scale, 0, 0);
        }
        
        // Table second line
        if (demo == demo_table_1)
        {
            str = table1.title;
            for (i = 0; i < xinv; i++)
            {
                chanim[0] = str[i];
                spaced_invaders_RenderTextual(scale, dx, dy, (table1.x + i) * 8, table1.y * 8, chanim);
            }
        }
        else if ((demo > demo_table_1) && (demo <= demo_title_end))
        {
            spaced_invaders_RenderTextual(scale, dx, dy, table1.x * 8, table1.y * 8, table1.title);
        }

        // Table third line
        if (demo == demo_table_2)
        {
            str = table2.title;
            for (i = 0; i < xinv; i++)
            {
                chanim[0] = str[i];
                spaced_invaders_RenderTextual(scale, dx, dy, (table2.x + i) * 8, table2.y * 8, chanim);
            }
        }
        else if ((demo > demo_table_2) && (demo <= demo_title_end))
        {
            spaced_invaders_RenderTextual(scale, dx, dy, table2.x * 8, table2.y * 8, table2.title);
        }

        // Table fourth line
        if (demo == demo_table_3)
        {
            str = table3.title;
            for (i = 0; i < xinv; i++)
            {
                chanim[0] = str[i];
                spaced_invaders_RenderTextual(scale, dx, dy, (table3.x + i) * 8, table3.y * 8, chanim);
            }
        }
        else if ((demo > demo_table_3) && (demo <= demo_title_end))
        {
            spaced_invaders_RenderTextual(scale, dx, dy, table3.x * 8, table3.y * 8, table3.title);
        }

        if (demo == demo_inv_V)
        {
            spaced_invaders_RenderTextual(scale, dx, dy, xinv - 8, 8 * 8, "V");
        }
        if (demo == demo_inv_Y)
        {
            spaced_invaders_RenderTextual(scale, dx, dy, xinv - 8, 8 * 8, "Y");
        }

        if ((demo >= demo_inv_in) && (demo <= demo_inv_out))
        {
            EVE_BITMAP_HANDLE(BITMAP_INVADERS);
            EVE_CELL(CH_INVADERS + 0 * 2 + march);
            scaledVertexTranslate(scale, dx, dy, xinv, 8 * 8);
            scaledVertex(scale, 0, 0);
        }

        if ((demo == demo_game) || (demo == demo_end))
        {
            spaced_invaders_RenderInvader(scale, dispX, dispY);
            spaced_invaders_RenderBullets(scale, dispX, dispY);
        }

        if (demo == demo_ready)
        {
            str = ready.title;
            for (i = 0; i < xinv; i++)
            {
                chanim[0] = str[i];
                spaced_invaders_RenderTextual(scale, dx, dy, (ready.x + i) * 8, ready.y * 8, chanim);
            }
        }
        else if (demo == demo_play)
        {
            spaced_invaders_RenderTextual(scale, dx, dy, ready.x * 8, ready.y * 8, ready.title);
        }

        spaced_invaders_RenderShields(scale, dispX, dispY);
        spaced_invaders_RenderScoring(scale, dispX, dispY);
        // bitmaps [END] ...
        EVE_END();
        EVE_RESTORE_CONTEXT();
        spaced_invaders_RenderButtons(scale, dispX, dispY);
        // finish display list and send to FT800 ...
        spaced_invaders_FlushDisplay();

        // switch invader images to use (for simple animation) ...
        march = !march;

        // Animation state machine:
        // part 1 - change PLAV to PLAY
        if (demo == demo_start)
        {
            xinv = 0;
            demo = demo_title_1;
            brakes = PAUSE_TEXT;
        }
        else if (demo == demo_title_1)
        {
            if (--brakes == 0)
            {
                xinv++;
                if (str[xinv] == 0)
                {
                    xinv = 0;
                    demo = demo_title_2;
                }
                brakes = PAUSE_TEXT;
            }
        }
        else if (demo == demo_title_2)
        {
            if (--brakes == 0)
            {
                xinv++;
                if (str[xinv] == 0)
                {
                    xinv = 0;
                    demo = demo_table_1;
                }
                brakes = PAUSE_TEXT;
            }
        }
        else if (demo == demo_table_1)
        {
            if (--brakes == 0)
            {
                xinv++;
                if (str[xinv] == 0)
                {
                    xinv = 0;
                    demo = demo_table_2;
                }
                brakes = PAUSE_TEXT;
            }
        }
        else if (demo == demo_table_2)
        {
            if (--brakes == 0)
            {
                xinv++;
                if (str[xinv] == 0)
                {
                    xinv = 0;
                    demo = demo_table_3;
                }
                brakes = PAUSE_TEXT;
            }
        }
        else if (demo == demo_table_3)
        {
            if (--brakes == 0)
            {
                xinv++;
                if (str[xinv] == 0)
                {
                    xinv = INVADER_XMAX;
                    demo = demo_inv_in;
                }
                brakes = PAUSE_TEXT;
            }
        }
        else if ((demo == demo_inv_in) || (demo == demo_inv_Y))
        {
            xinv -= INVADER_XSPD;
            if (xinv <= (12 + 4) * 8)
            {
                demo++;
            }
        }
        else if ((demo == demo_inv_out) || (demo == demo_inv_V))
        {
            xinv += INVADER_XSPD;
            if (xinv >= INVADER_XMAX + 24)
            {
                demo++;
                brakes = PAUSE_TITLE;
            }
        }
        else if (demo == demo_title_end)
        {
            if (--brakes == 0)
            {
                demo = demo_game;
                brakes = PAUSE_DEMO;

                // Reset all positions
                spaced_invaders_BeginGame(GAME_START_NEW);
            }
        }
        else if (demo == demo_game)
        {
            uint8_t tagAuto = 0;
            if (move == 0)
            {
                move = (pseudo_rand() % DISPLAY_XPIX) - (DISPLAY_XPIX / 2);
            }
            if (move > 0)
            {
                tagAuto = BUTTON_RIGHT;
                move--;
            }
            else if (move < 0)
            {
                tagAuto = BUTTON_LEFT;
                move++;
            }
            if ((pseudo_rand() & 15) == 0)
            {
                tagAuto = BUTTON_FIRE1;
            }
            spaced_invaders_CheckButtons(tagAuto);
            spaced_invaders_UpdateInvader();
            spaced_invaders_UpdateShields();
            spaced_invaders_UpdatePlayers();
            spaced_invaders_UpdateBullets();

            if (--brakes == 0)
            {
                demo = demo_end;
                brakes = PAUSE_TITLE;
            }
        }
        else if (demo == demo_end)
        {
            if (--brakes == 0)
            {
                demo = demo_start;
            }
        }
        else if (demo == demo_ready)
        {
            if (--brakes == 0)
            {
                xinv++;
                if (str[xinv] == 0)
                {
                    xinv = 0;
                    demo = demo_play;
                    brakes = PAUSE_TITLE;
                }
                else
                {
                    brakes = PAUSE_TEXT;
                }
            }
        }
        else if (demo == demo_play)
        {
            if (--brakes == 0)
            {
                // Start game
                break;
            }
        }

        // 20 ms delay between frames
        while (platform_get_time() - time_start < 20)
        {
            // CPU sleep
        }
        
        if (demo < demo_ready)
        {
            tagTouch = 0;
            eve_read_tag(&tagTouch);
            if ((tagTouch == BUTTON_FIRE1) || (tagTouch == BUTTON_FIRE2))
            {
                demo = demo_ready;
                xinv = 0;
                brakes = PAUSE_TEXT;
            }
        }
    } while (1);
}
