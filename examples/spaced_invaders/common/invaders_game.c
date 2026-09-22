/**
 * @file invaders_game.c
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

// ==========================================================================
// Constant Arrays
// ==========================================================================

const int16_t invaderSpeed[INVADER_TOTL + 1] = // invader speed according to how many of them are alive ...
{
    0, 1,                           //  0- 1 invaders alive
    3, 6,                           //  2- 3 invaders alive
    9, 9, 9, 9,                     //  4- 7 invaders alive
    12, 12, 12, 12, 12, 12, 12, 12, //  7-15 invaders alive
    15, 15, 15, 15, 15, 15, 15, 15, // 16-23 invaders alive
    20, 20, 20, 20, 20, 20, 20, 20, // 24-31 invaders alive
    30, 30, 30, 30, 30, 30, 30, 30, // 32-39 invaders alive
    40, 40, 40, 40, 40, 40, 40, 40, // 40-47 invaders alive
    50, 50, 50, 50, 50, 50, 50, 50  // 48-55 invaders alive
};

const uint8_t invaderScore[5] = { 30, 20, 20, 10, 10 };

uint8_t invaderMarch; // flag to determine invader animation state (only for ALIVE state)
uint8_t invaderEdges; // flag to mark that an invader has reached left/right edge of display
uint8_t invaderLands; // flag to mark that an invader has reached bottom of display

int16_t invaderAlive; // current number of invaders still alive
int16_t invaderStart; // Y position to start invader wave at
int16_t invaderMoveX; // used to move invaders backwards and forwards
int16_t invaderMoveY; // used to move invaders downwards

uint8_t playerAlive; // flag for player alive
int16_t playersXcord;
int16_t playersYcord;
int16_t playersMoveX;

uint8_t  bulletMarch1; // flag to determine bullets animation state (only for ALIVE state)
uint8_t  bulletMarch2; // flag to determine bullets animation state (only for ALIVE state)
uint8_t  bulletMarch3; // flag to determine bullets animation state (only for ALIVE state)

// timers for update of objects ...
int16_t timerInvades;
int16_t timerShields;
int16_t timerPlayers;
int16_t timerBullet2;
int16_t timerBullet3;
int16_t timerBullet1;

// ==========================================================================
// Gameplay declarations
// ==========================================================================

uint8_t spaced_invaders_BulletCollide(int16_t bx, int16_t by, int16_t cx, int16_t cy, int16_t cw, int16_t ch)
{
    int16_t xmin = cx;
    int16_t xmax = cx + cw;
    int16_t ymin = cy;
    int16_t ymax = cy + ch;
    uint8_t  hit  = false;

    if ((bx >= xmin) && (bx <= xmax))
    {
        if ((by >= ymin) && (by <= ymax))
        {
            hit = true;
        }
    }

    return (hit);
}

uint8_t spaced_invaders_HitsOnPlayers(int16_t bx, int16_t by)
{
    uint8_t alive = true;

    // check to see if bullet has hit the defender ...
    if (spaced_invaders_BulletCollide(bx, by, xplayers->xcrd, xplayers->ycrd, PLAYERS_XPIX, PLAYERS_XPIX) == true)
    {
        if (xplayers->life == IS_ALIVE_3)
        {
            // lose a base, decrement total players alive and kill bullet ...
            xplayers->life = IS_ALIVE_2;
            alive = false;
        }
        else if (xplayers->life == IS_ALIVE_2)
        {
            // lose a base, decrement total players alive and kill bullet ...
            xplayers->life = IS_ALIVE_1;
            alive = false;
        }
        else if (xplayers->life == IS_ALIVE_1)
        {
            // switch to toasted state, kill bullet ...
            xplayers->life = IS_TOAST;
            alive = false;

            playerAlive = false;
        }
    }

    return (alive);
}

uint8_t spaced_invaders_HitsOnShields(int16_t bx, int16_t by)
{
    siObject_t * check = xshields;
    int16_t index = 0;
    uint8_t  alive = true;

    // check to see if bullet has hit any shields ...
    do
    {
        if (spaced_invaders_BulletCollide(bx, by, check->xcrd, check->ycrd, SHIELDS_XPIX, SHIELDS_YPIX) == true)
        {
            if (check->life > IS_TOAST)
            {
                // reduce shield life and kill bullet ...
                check->life--;
                alive = false;
            }
        }
        check++;
    } while ((++index < SHIELDS_TOTL) && (alive == true));

    return (alive);
}

uint8_t spaced_invaders_HitsOnInvader(int16_t bx, int16_t by)
{
    siObject_t * check = xinvader;
    int16_t index = 0;
    uint8_t  alive = true;

    // check to see if bullet has hit any invaders ...
    do
    {
        if (spaced_invaders_BulletCollide(bx, by, check->xcrd, check->ycrd, INVADER_XPIX, INVADER_YPIX) == true)
        {
            if (check->life == IS_ALIVE_1)
            {
                // switch to toasted state, decrement total invaders alive and kill bullet ...
                check->life = IS_TOAST;
                invaderAlive--;
                alive = false;
                // update current score and high score ...
                scoreCurrent += invaderScore[invaderTable[index].type];
                if (scoreCurrent > scoreHighest)
                {
                    scoreHighest = scoreCurrent;
                }
            }
        }
        check++;
    } while ((++index < INVADER_TOTL) && (alive == true));

    return (alive);
}

void spaced_invaders_SetupObject(siObject_t * data, int16_t xcrd, int16_t ycrd, uint8_t type, uint8_t life)
{
    data->xcrd = xcrd;
    data->ycrd = ycrd;
    data->type = type;
    data->life = life;
}

void spaced_invaders_BeginGame(uint8_t gamestate)
{
    playerAlive = true;

    invaderEdges = false; // no invaders have reached edge of display ...
    invaderLands = false; // no invaders have reached bottom of display ...
    invaderMarch = 0;     // initial invader images to use ...

    invaderAlive = INVADER_TOTL;
    invaderMoveX = INVADER_XSPD;
    invaderMoveY = INVADER_YSPD;
    
    spaced_invaders_SetupObject(&bulletsTable[BULLETS_PLAY], 0, 0, 0, IS_KAPUT);
    spaced_invaders_SetupObject(&bulletsTable[BULLETS_INV1], 0, 0, 0, IS_KAPUT);
    spaced_invaders_SetupObject(&bulletsTable[BULLETS_INV2], 0, 0, 0, IS_KAPUT);

    bulletMarch1 = 0;
    bulletMarch2 = 0;
    bulletMarch3 = 0;

    // initialise timers for movement of objects ...
    timerInvades = 1;
    timerShields = 1;
    timerPlayers = 1;
    timerBullet1 = 1;
    timerBullet2 = 1;
    timerBullet3 = 1;
    
    siObject_t * setup;

    invaderStart = (gamestate == GAME_START_NEW) ? INVADER_YMIN : invaderStart + INVADER_YSPD;

    // Initialise invaders ...
    setup = xinvader;
    for (uint8_t rows = 0; rows < INVADER_ROWS; rows++)
    {
        for (uint8_t cols = 0; cols < INVADER_COLS; cols++)
        {
            spaced_invaders_SetupObject(setup++, INVADER_XMIN + INVADER_XGRD * cols, invaderStart + INVADER_YGRD * rows, CH_INVADERS + rows, IS_ALIVE_1);
        }
    }

    if (gamestate == GAME_START_NEW)
    {
        scoreCurrent = 0;

        int16_t xcrd = SHIELDS_XPOS;
        int16_t ycrd = SHIELDS_YPOS;

        // Initialise shields ...
        setup = xshields;
        for (uint8_t shields = 0; shields < 4; shields++)
        {
            // top row of shield ...
            spaced_invaders_SetupObject(setup++, xcrd +  0, ycrd +  0, CH_SHIELD_1, IS_ALIVE_4);
            spaced_invaders_SetupObject(setup++, xcrd +  6, ycrd +  0, CH_SHIELD_5, IS_ALIVE_4);
            spaced_invaders_SetupObject(setup++, xcrd + 12, ycrd +  0, CH_SHIELD_5, IS_ALIVE_4);
            spaced_invaders_SetupObject(setup++, xcrd + 18, ycrd +  0, CH_SHIELD_2, IS_ALIVE_4);
            // middle row of shield ...
            spaced_invaders_SetupObject(setup++, xcrd +  0, ycrd +  6, CH_SHIELD_5, IS_ALIVE_4);
            spaced_invaders_SetupObject(setup++, xcrd +  6, ycrd +  6, CH_SHIELD_3, IS_ALIVE_4);
            spaced_invaders_SetupObject(setup++, xcrd + 12, ycrd +  6, CH_SHIELD_4, IS_ALIVE_4);
            spaced_invaders_SetupObject(setup++, xcrd + 18, ycrd +  6, CH_SHIELD_5, IS_ALIVE_4);
            // bottom row of shield ...
            spaced_invaders_SetupObject(setup++, xcrd +  0, ycrd + 12, CH_SHIELD_5, IS_ALIVE_4);
            spaced_invaders_SetupObject(setup++, xcrd + 18, ycrd + 12, CH_SHIELD_5, IS_ALIVE_4);
            // x,y coords of next shield ...
            xcrd += SHIELDS_XGRD;
            ycrd += SHIELDS_YGRD;
        }

        // Initialise players ...
        spaced_invaders_SetupObject(xplayers, PLAYERS_XPOS, PLAYERS_YPOS, CH_DEFENDER, IS_ALIVE_3);
    }

    playersMoveX = 0;
}

int8_t spaced_invaders_UpdateInvader(void)
{
    int8_t updateScreen = false;
    siObject_t * entry = xinvader;
    uint8_t    edges = false;

    if (--timerInvades == 0)
    {
        // inform system that display needs refreshed ...
        updateScreen = true;

        // set delay before next update (delay decreases as invaders are destroyed) ...
        timerInvades = (invaderSpeed[invaderAlive] / 2) + 1;

        // switch invader images to use (for simple animation) ...
        invaderMarch = !invaderMarch;

        // move all invaders, determining if any have reached edges or bottom of display ...
        for (uint8_t index = 0; index < INVADER_TOTL; index++)
        {
            // update position of invaders accordingly ...
            switch (entry->life)
            {
            case IS_ALIVE_1:
                // determine if direction needs to change ...
                if (invaderEdges == false)
                {
                    entry->xcrd += invaderMoveX;

                    if ((entry->xcrd <= INVADER_XMIN) || (entry->xcrd >= INVADER_XMAX))
                    {
                        edges = true;
                    }
                }
                else
                {
                    entry->ycrd += invaderMoveY;

                    if (entry->ycrd >= INVADER_YMAX)
                    {
                        invaderLands = true;
                    }
                }
                break;

            case IS_TOAST:
                if (invaderEdges == false)
                {
                    entry->xcrd += invaderMoveX;
                }
                else
                {
                    entry->ycrd += invaderMoveY;
                }
                break;

            case IS_KAPUT:
            default:
                // nothing to see here!
                break;
            }
            // next invader ...
            entry++;
        }

        invaderEdges = edges;

        // if edges have been reached then we've dropped down this time, so reverse X direction ...
        if (invaderEdges == true)
        {
            invaderMoveX = -invaderMoveX;
        }
    }
    return updateScreen;
}

int8_t spaced_invaders_UpdateShields(void)
{
    // shields don't move!!!!!
    return false;
}

int8_t spaced_invaders_UpdatePlayers(void)
{
    int8_t updateScreen = false;

    if (--timerPlayers == 0)
    {
        // set delay before next update ...
        timerPlayers = 5;
        // inform system that display needs refreshed ...
        updateScreen = true;
        // update position of player ...
        xplayers->xcrd += playersMoveX;
        playersMoveX = 0;

        if (xplayers->xcrd <= PLAYERS_XMIN) xplayers->xcrd = PLAYERS_XMIN;
        if (xplayers->xcrd >= PLAYERS_XMAX) xplayers->xcrd = PLAYERS_XMAX;
    }

    return updateScreen;
}

int8_t spaced_invaders_UpdateBullets(void)
{
    int8_t updateScreen = false;
    siObject_t * bullet;

    if (--timerBullet1 == 0)
    {
        timerBullet1 = 2; // delay before next update
        bullet = &bulletsTable[BULLETS_PLAY];

        if (bullet->life == true)
        {
            updateScreen  = true;          // display needs refreshed
            bulletMarch1  = !bulletMarch1; // switch bullet images beings used (for simple animation) ...
            bullet->ycrd -= BULLETS_YSPD;  // update bullet position

            if (bullet->ycrd <= INVADER_YMIN)
            {
                bullet->life = false; // kill off bullet if it has reached top of display
            }
            else
            {
                bullet->life = spaced_invaders_HitsOnShields(bullet->xcrd + 5, bullet->ycrd); // check for hits on shields
                if (bullet->life == true)
                {
                    bullet->life = spaced_invaders_HitsOnInvader(bullet->xcrd + 5, bullet->ycrd); // check for hits on invaders
                }
            }
        }
    }

    if (--timerBullet2 == 0)
    {
        timerBullet2 = 2; // delay before next update
        bullet = &bulletsTable[BULLETS_INV1];

        if (bullet->life == true)
        {
            updateScreen  = true;          // display needs refreshed
            bulletMarch2  = !bulletMarch2; // switch bullet images beings used (for simple animation) ...
            bullet->ycrd += BULLETS_YSPD;  // update bullet position

            if (bullet->ycrd >= INVADER_YMAX)
            {
                bullet->life = false; // kill off bullet if it has reached bottom of display
            }
            else
            {
                bullet->life = spaced_invaders_HitsOnShields(bullet->xcrd + 5, bullet->ycrd + 8); // check for hits on shields
                if (bullet->life == true)
                {
                    bullet->life = spaced_invaders_HitsOnPlayers(bullet->xcrd + 5, bullet->ycrd + 8); // check for hits on players
                }
            }
        }
    }

    if (--timerBullet3 == 0)
    {
        timerBullet3 = 2; // delay before next update
        bullet = &bulletsTable[BULLETS_INV2];

        if (bullet->life == true)
        {
            updateScreen  = true;          // display needs refreshed
            bulletMarch3  = !bulletMarch3; // switch bullet images beings used (for simple animation) ...
            bullet->ycrd += BULLETS_YSPD;  // update bullet position

            if (bullet->ycrd >= INVADER_YMAX)
            {
                bullet->life = false; // kill off bullet if it has reached bottom of display
            }
            else
            {
                bullet->life = spaced_invaders_HitsOnShields(bullet->xcrd + 5, bullet->ycrd + 8); // check for hits on shields
                if (bullet->life == true)
                {
                    bullet->life = spaced_invaders_HitsOnPlayers(bullet->xcrd + 5, bullet->ycrd + 8); // check for hits on players
                }
            }
        }
    }
    return updateScreen;
}

void spaced_invaders_CheckButtons(uint8_t tag)
{
    // check to see if any buttons have been pressed ...
    uint8_t randInvader = 0;

    switch (tag)
    {
    case BUTTON_LEFT:
        playersMoveX = -PLAYERS_XSPD;
        break;
    case BUTTON_RIGHT:
        playersMoveX = PLAYERS_XSPD;
        break;
    case BUTTON_FIRE1:
    case BUTTON_FIRE2:
        // only allow player to fire again if no player bullet is alive ...
        if (bulletsTable[BULLETS_PLAY].life == false)
        {
            timerBullet1 = 1;

            spaced_invaders_SetupObject(bulletsTable+BULLETS_PLAY, playersTable[0].xcrd, PLAYERS_YPOS, 0, true);

            if (bulletsTable[BULLETS_INV1].life == false)
            {
                timerBullet2 = 4;
                do { randInvader = (pseudo_rand() % (INVADER_TOTL)); } while (invaderTable[randInvader].life < IS_ALIVE_1);
                spaced_invaders_SetupObject(bulletsTable+BULLETS_INV1, invaderTable[randInvader].xcrd, invaderTable[randInvader].ycrd, 0, true);
            }

            if (bulletsTable[BULLETS_INV2].life == false)
            {
                timerBullet3 = 8;
                do { randInvader = (pseudo_rand() % (INVADER_TOTL)); } while (invaderTable[randInvader].life < IS_ALIVE_1);
                spaced_invaders_SetupObject(bulletsTable+BULLETS_INV2, invaderTable[randInvader].xcrd, invaderTable[randInvader].ycrd, 0, true);
            }
        }
        break;
    }
}
