/**
* @file api1.c
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

#include <stdint.h>
#include <stdio.h>

/* Include EVE-MCU-Dev library API layer */
#include <EVE.h>

#if IS_EVE_API(1)

/* CONSTANTS ***********************************************************************/

/* FUNCTIONS ***********************************************************************/

/* Function names taken directly from FT800 Series Programmer Guide Version 2.1 */

void api_test_dl(uint32_t t1, uint32_t t2)
{
    EVE_LIB_BeginCoProList();
    /* 4 Display list commands: */
    /* 4.4 ALPHA_FUNC */ EVE_ALPHA_FUNC(t1,t2);
    /* 4.5 BEGIN */ EVE_BEGIN(t1);
    /* 4.6 BITMAP_HANDLE */ EVE_BITMAP_HANDLE(t1);
    /* 4.7 BITMAP_LAYOUT */ EVE_BITMAP_LAYOUT(t1,t2,t1);
    /* 4.8 BITMAP_SIZE */ EVE_BITMAP_SIZE(t1,t2,t1,t2,t1);
    /* 4.9 BITMAP_SOURCE */ EVE_BITMAP_SOURCE(t1);
    /* 4.10 BITMAP_TRANSFORM_A */ EVE_BITMAP_TRANSFORM_A(t1);
    /* 4.11 BITMAP_TRANSFORM_B */ EVE_BITMAP_TRANSFORM_B(t1);
    /* 4.12 BITMAP_TRANSFORM_C */ EVE_BITMAP_TRANSFORM_C(t1);
    /* 4.13 BITMAP_TRANSFORM_D */ EVE_BITMAP_TRANSFORM_D(t1);
    /* 4.14 BITMAP_TRANSFORM_E */ EVE_BITMAP_TRANSFORM_E(t1);
    /* 4.15 BITMAP_TRANSFORM_F */ EVE_BITMAP_TRANSFORM_F(t1);
    /* 4.16 BLEND_FUNC */ EVE_BLEND_FUNC(t1,t2);
    /* 4.17 CALL */ EVE_CALL(t1);
    /* 4.18 CELL */ EVE_CELL(t1);
    /* 4.19 CLEAR */ EVE_CLEAR(t1,t2,t1);
    /* 4.20 CLEAR_COLOR_A */ EVE_CLEAR_COLOR_A(t1);
    /* 4.21 CLEAR_COLOR_RGB */ EVE_CLEAR_COLOR_RGB(t1,t2,t1);
    /* 4.22 CLEAR_STENCIL */ EVE_CLEAR_STENCIL(t1);
    /* 4.23 CLEAR_TAG */ EVE_CLEAR_TAG(t1);
    /* 4.24 COLOR_A */ EVE_COLOR_A(t1);
    /* 4.25 COLOR_MASK */ EVE_COLOR_MASK(t1,t2,t1,t2);
    /* 4.26 COLOR_RGB */ EVE_COLOR_RGB(t1,t2,t1);
    /* 4.27 DISPLAY */ EVE_DISPLAY();
    /* 4.28 END */ EVE_END(); /* NOTE: This will be written twice by the API */
    /* 4.29 JUMP */ EVE_JUMP(t1);
    /* 4.30 LINE_WIDTH */ EVE_LINE_WIDTH(t1);
    /* 4.31 MACRO */ EVE_MACRO(t1);
    /* 4.32 POINT_SIZE */ EVE_POINT_SIZE(t1);
    /* 4.33 RESTORE_CONTEXT */ EVE_RESTORE_CONTEXT();
    /* 4.34 RETURN */ EVE_RETURN();
    /* 4.35 SAVE CONTEXT */ EVE_SAVE_CONTEXT();
    /* 4.36 SCISSOR_SIZE */ EVE_SCISSOR_SIZE(t1,t2);
    /* 4.37 SCISSOR_XY */ EVE_SCISSOR_XY(t1,t2);
    /* 4.38 STENCIL_FUNC */ EVE_STENCIL_FUNC(t1,t2,t1);
    /* 4.39 STENCIL_MASK */ EVE_STENCIL_MASK(t1);
    /* 4.40 STENCIL_OP */ EVE_STENCIL_OP(t1,t2);
    /* 4.41 TAG */ EVE_TAG(t1);
    /* 4.42 TAG_MASK */ EVE_TAG_MASK(t1);
    /* 4.43 VERTEX2F */ EVE_VERTEX2F(t1,t2);
    /* 4.44 VERTEX2II */ EVE_VERTEX2II(t1,t2,t1,t2);
    EVE_LIB_EndCoProList();
}

void api_test_cmd(uint32_t t1, uint32_t t2)
{
    EVE_LIB_BeginCoProList();
    /* 5.1 Co-processor handling of Display list commands */
    /* 5.12 CMD_DLSTART */ EVE_CMD_DLSTART();
    /* 5.13 CMD_SWAP */ EVE_CMD_SWAP();
    /* 5.14 CMD_COLDSTART */ EVE_CMD_COLDSTART();
    /* 5.15 CMD_INTERRUPT */ EVE_CMD_INTERRUPT(t1);
    /* 5.16 CMD_APPEND */ EVE_CMD_APPEND(t1,t2);
    /* 5.17 CMD_REGREAD */ EVE_CMD_REGREAD(t1,t2);
    /* 5.18 CMD_MEMWRITE */ EVE_CMD_MEMWRITE(t1,t2);
    /* 5.19 CMD_INFLATE */ EVE_CMD_INFLATE(t1);
    /* 5.20 CMD_LOADIMAGE */ EVE_CMD_LOADIMAGE(t1,t2);
    /* 5.21 CMD_MEMCRC */ EVE_CMD_MEMCRC(t1,t2,t1);
    /* 5.22 CMD_MEMZERO */ EVE_CMD_MEMZERO(t1,t2);
    /* 5.23 CMD_MEMSET */ EVE_CMD_MEMSET(t1,t2,t1);
    /* 5.24 CMD_MEMCPY */ EVE_CMD_MEMCPY(t1,t2,t1);
    /* 5.25 CMD_BUTTON */ EVE_CMD_BUTTON(t1,t2,t1,t2,t1,t2,"1234");
    /* 5.26 CMD_CLOCK */ EVE_CMD_CLOCK(t1,t2,t1,t2,t1,t2,t1,t2);
    /* 5.27 CMD_FGCOLOR */ EVE_CMD_FGCOLOR(t1);
    /* 5.28 CMD_BGCOLOR */ EVE_CMD_BGCOLOR(t1);
    /* 5.29 CMD_GRADCOLOR */ EVE_CMD_GRADCOLOR(t1);
    /* 5.30 CMD_GAUGE */ EVE_CMD_GAUGE(t1,t2,t1,t2,t1,t2,t1,t2);
    /* 5.31 CMD_GRADIENT */ EVE_CMD_GRADIENT(t1,t2,t1,t2,t1,t2);
    /* 5.32 CMD_KEYS */ EVE_CMD_KEYS(t1,t2,t1,t2,t1,t2,"12345");
    /* 5.33 CMD_PROGRESS */ EVE_CMD_PROGRESS(t1,t2,t1,t2,t1,t2,t1);
    /* 5.34 CMD_SCROLLBAR */ EVE_CMD_SCROLLBAR(t1,t2,t1,t2,t1,t2,t1,t2);
    /* 5.35 CMD_SLIDER */ EVE_CMD_SLIDER(t1,t2,t1,t2,t1,t2,t1);
    /* 5.36 CMD_DIAL */ EVE_CMD_DIAL(t1,t2,t1,t2,t1);
    /* 5.37 CMD_TOGGLE */ EVE_CMD_TOGGLE(t1,t2,t1,t2,t1,t2,"123456");
    /* 5.38 CMD_TEXT */ EVE_CMD_TEXT(t1,t2,t1,t2,"1234567");
    /* 5.39 CMD_NUMBER */ EVE_CMD_NUMBER(t1,t2,t1,t2,t1);
    /* 5.40 CMD_SETMATRIX */ EVE_CMD_SETMATRIX();
    /* 5.41 CMD_GETMATRIX */ EVE_CMD_GETMATRIX(t1,t2,t1,t2,t1,t2);
    /* 5.42 CMD_GETPTR */ EVE_CMD_GETPTR(t1);
    /* 5.43 CMD_GETPROPS */ EVE_CMD_GETPROPS(t1,t2,t1);
    /* 5.44 CMD_SCALE */ EVE_CMD_SCALE(t1,t2);
    /* 5.45 CMD_ROTATE */ EVE_CMD_ROTATE(t1);
    /* 5.46 CMD_TRANSLATE */ EVE_CMD_TRANSLATE(t1,t2);
    /* 5.47 CMD_CALIBRATE */ EVE_CMD_CALIBRATE(t1);
    /* 5.48 CMD_SPINNER */ EVE_CMD_SPINNER(t1,t2,t1,t2);
    /* 5.49 CMD_SCREENSAVER */ EVE_CMD_SCREENSAVER();
    /* 5.50 CMD_SKETCH */ EVE_CMD_SKETCH(t1,t2,t1,t2,t1,t2);
    /* 5.51 CMD_STOP */ EVE_CMD_STOP();
    /* 5.52 CMD_SETFONT */ EVE_CMD_SETFONT(t1,t2);
    /* 5.53 CMD_TRACK */ EVE_CMD_TRACK(t1,t2,t1,t2,t1);
    /* 5.54 CMD_SNAPSHOT */ EVE_CMD_SNAPSHOT(t1);
    /* 5.55 CMD_LOGO */ EVE_CMD_LOGO();
    EVE_LIB_EndCoProList();
}

#endif // IS_EVE_API(1)
