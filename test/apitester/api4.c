/**
* @file api4.c
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

#if IS_EVE_API(4)

/* CONSTANTS ***********************************************************************/

/* FUNCTIONS ***********************************************************************/

/* Function names taken directly from BRT_AN_033 BT81X Series Programming Guide Version 2.4 */

void api_test_dl(uint32_t t1, uint32_t t2)
{
    EVE_LIB_BeginCoProList();
    /* 4 Display list commands: */
    /* 4.4 ALPHA_FUNC */ EVE_ALPHA_FUNC(t1,t2);
    /* 4.5 BEGIN */ EVE_BEGIN(t1);
    /* 4.6 BITMAP_EXT_FORMAT */ EVE_BITMAP_EXT_FORMAT(t1);
    /* 4.7 BITMAP_HANDLE */ EVE_BITMAP_HANDLE(t1);
    /* 4.8 BITMAP_LAYOUT */ EVE_BITMAP_LAYOUT(t1,t2,t1);
    /* 4.9 BITMAP_LAYOUT_H */ EVE_BITMAP_LAYOUT_H(t1,t2);
    /* 4.10 BITMAP_SIZE */ EVE_BITMAP_SIZE(t1,t2,t1,t2,t1);
    /* 4.11 BITMAP_SIZE_H */ EVE_BITMAP_SIZE_H(t1,t2);
    /* 4.12 BITMAP_SOURCE */ EVE_BITMAP_SOURCE(t1);
    /* 4.13 BITMAP_SWIZZLE */ EVE_BITMAP_SWIZZLE(t1,t2,t1,t2);
    /* 4.14 BITMAP_TRANSFORM_A */ EVE_BITMAP_TRANSFORM_A(t1);
    /* 4.15 BITMAP_TRANSFORM_B */ EVE_BITMAP_TRANSFORM_B(t1);
    /* 4.16 BITMAP_TRANSFORM_C */ EVE_BITMAP_TRANSFORM_C(t1);
    /* 4.17 BITMAP_TRANSFORM_D */ EVE_BITMAP_TRANSFORM_D(t1);
    /* 4.18 BITMAP_TRANSFORM_E */ EVE_BITMAP_TRANSFORM_E(t1);
    /* 4.19 BITMAP_TRANSFORM_F */ EVE_BITMAP_TRANSFORM_F(t1);
    /* 4.20 BLEND_FUNC */ EVE_BLEND_FUNC(t1,t2);
    /* 4.21 CALL */ EVE_CALL(t1);
    /* 4.22 CELL */ EVE_CELL(t1);
    /* 4.23 CLEAR */ EVE_CLEAR(t1,t2,t1);
    /* 4.24 CLEAR_COLOR_A */ EVE_CLEAR_COLOR_A(t1);
    /* 4.25 CLEAR_COLOR_RGB */ EVE_CLEAR_COLOR_RGB(t1,t2,t1);
    /* 4.26 CLEAR_STENCIL */ EVE_CLEAR_STENCIL(t1);
    /* 4.27 CLEAR_TAG */ EVE_CLEAR_TAG(t1);
    /* 4.28 COLOR_A */ EVE_COLOR_A(t1);
    /* 4.29 COLOR_MASK */ EVE_COLOR_MASK(t1,t2,t1,t2);
    /* 4.30 COLOR_RGB */ EVE_COLOR_RGB(t1,t2,t1);
    /* 4.31 DISPLAY */ EVE_DISPLAY();
    /* 4.32 END */ EVE_END(); /* NOTE: This will be written twice by the API */
    /* 4.33 JUMP */ EVE_JUMP(t1);
    /* 4.34 LINE_WIDTH */ EVE_LINE_WIDTH(t1);
    /* 4.35 MACRO */ EVE_MACRO(t1);
    /* 4.36 NOP */ EVE_NOP();
    /* 4.37 PALETTE_SOURCE */ EVE_PALETTE_SOURCE(t1);
    /* 4.38 POINT_SIZE */ EVE_POINT_SIZE(t1);
    /* 4.39 RESTORE_CONTEXT */ EVE_RESTORE_CONTEXT();
    /* 4.40 RETURN */ EVE_RETURN();
    /* 4.41 SAVE CONTEXT */ EVE_SAVE_CONTEXT();
    /* 4.42 SCISSOR_SIZE */ EVE_SCISSOR_SIZE(t1,t2);
    /* 4.43 SCISSOR_XY */ EVE_SCISSOR_XY(t1,t2);
    /* 4.44 STENCIL_FUNC */ EVE_STENCIL_FUNC(t1,t2,t1);
    /* 4.45 STENCIL_MASK */ EVE_STENCIL_MASK(t1);
    /* 4.46 STENCIL_OP */ EVE_STENCIL_OP(t1,t2);
    /* 4.47 TAG */ EVE_TAG(t1);
    /* 4.48 TAG_MASK */ EVE_TAG_MASK(t1);
    /* 4.49 VERTEX2F */ EVE_VERTEX2F(t1,t2);
    /* 4.50 VERTEX2II */ EVE_VERTEX2II(t1,t2,t1,t2);
    /* 4.51 VERTEX_FORMAT */ EVE_VERTEX_FORMAT(t1);
    /* 4.52 VERTEX_TRANSLATE_X */ EVE_VERTEX_TRANSLATE_X(t1);
    /* 4.53 VERTEX_TRANSLATE_Y */ EVE_VERTEX_TRANSLATE_Y(t1);
    EVE_LIB_EndCoProList();
}

void api_test_cmd(uint32_t t1, uint32_t t2)
{
    EVE_LIB_BeginCoProList();
    /* 5.1 Co-processor handling of Display list commands */
    /* 5.13 CMD_APILEVEL */
    /* 5.14 CMD_DLSTART */ EVE_CMD_DLSTART();
    /* 5.15 CMD_INTERRUPT */ EVE_CMD_INTERRUPT(t1);
    /* 5.16 CMD_COLDSTART */ EVE_CMD_COLDSTART();
    /* 5.17 CMD_SWAP */ EVE_CMD_SWAP();
    /* 5.18 CMD_APPEND */ EVE_CMD_APPEND(t1,t2);
    /* 5.19 CMD_REGREAD */ EVE_CMD_REGREAD(t1,t2);
    /* 5.20 CMD_MEMWRITE */ EVE_CMD_MEMWRITE(t1,t2);
    /* 5.21 CMD_INFLATE */ EVE_CMD_INFLATE(t1);
    /* 5.22 CMD_INFLATE2 */ EVE_CMD_INFLATE2(t1,t2);
    /* 5.23 CMD_LOADIMAGE */ EVE_CMD_LOADIMAGE(t1,t2);
    /* 5.24 CMD_MEDIAFIFO */ EVE_CMD_MEDIAFIFO(t1,t2);
    /* 5.25 CMD_PLAYVIDEO */ EVE_CMD_PLAYVIDEO(t1);
    /* 5.26 CMD_VIDEOSTART */ EVE_CMD_VIDEOSTART();
    /* 5.27 CMD_VIDEOFRAME */ EVE_CMD_VIDEOFRAME(t1,t2);
    /* 5.28 CMD_MEMCRC */ EVE_CMD_MEMCRC(t1,t2,t1);
    /* 5.29 CMD_MEMZERO */ EVE_CMD_MEMZERO(t1,t2);
    /* 5.30 CMD_MEMSET */ EVE_CMD_MEMSET(t1,t2,t1);
    /* 5.31 CMD_MEMCPY */ EVE_CMD_MEMCPY(t1,t2,t1);
    /* 5.32 CMD_BUTTON */ EVE_CMD_BUTTON(t1,t2,t1,t2,t1,t2,"1234");
    /* 5.33 CMD_CLOCK */ EVE_CMD_CLOCK(t1,t2,t1,t2,t1,t2,t1,t2);
    /* 5.34 CMD_FGCOLOR */ EVE_CMD_FGCOLOR(t1);
    /* 5.35 CMD_BGCOLOR */ EVE_CMD_BGCOLOR(t1);
    /* 5.36 CMD_GRADCOLOR */ EVE_CMD_GRADCOLOR(t1);
    /* 5.37 CMD_GAUGE */ EVE_CMD_GAUGE(t1,t2,t1,t2,t1,t2,t1,t2);
    /* 5.38 CMD_GRADIENT */ EVE_CMD_GRADIENT(t1,t2,t1,t2,t1,t2);
    /* 5.39 CMD_GRADIENTA */ EVE_CMD_GRADIENTA(t1,t2,t1,t2,t1,t2);
    /* 5.40 CMD_KEYS */ EVE_CMD_KEYS(t1,t2,t1,t2,t1,t2,"12345");
    /* 5.41 CMD_PROGRESS */ EVE_CMD_PROGRESS(t1,t2,t1,t2,t1,t2,t1);
    /* 5.42 CMD_SCROLLBAR */ EVE_CMD_SCROLLBAR(t1,t2,t1,t2,t1,t2,t1,t2);
    /* 5.43 CMD_SLIDER */ EVE_CMD_SLIDER(t1,t2,t1,t2,t1,t2,t1);
    /* 5.44 CMD_DIAL */ EVE_CMD_DIAL(t1,t2,t1,t2,t1);
    /* 5.45 CMD_TOGGLE */ EVE_CMD_TOGGLE(t1,t2,t1,t2,t1,t2,"123456");
    /* 5.46 CMD_FILLWIDTH */ EVE_CMD_FILLWIDTH(t1);
    /* 5.47 CMD_TEXT */ EVE_CMD_TEXT(t1,t2,t1,t2,"1234567");
    /* 5.48 CMD_SETBASE */ EVE_CMD_SETBASE(t1);
    /* 5.49 CMD_NUMBER */ EVE_CMD_NUMBER(t1,t2,t1,t2,t1);
    /* 5.50 CMD_NOP */ EVE_CMD_NOP();
    /* 5.51 CMD_LOADIDENTITY */ EVE_CMD_LOADIDENTITY();
    /* 5.52 CMD_SETMATRIX */ EVE_CMD_SETMATRIX();
    /* 5.53 CMD_GETMATRIX */ EVE_CMD_GETMATRIX(t1,t2,t1,t2,t1,t2);
    /* 5.54 CMD_GETPTR */ EVE_CMD_GETPTR(t1);
    /* 5.55 CMD_GETPROPS */ EVE_CMD_GETPROPS(t1,t2,t1);
    /* 5.56 CMD_SCALE */ EVE_CMD_SCALE(t1,t2);
    /* 5.57 CMD_ROTATE */ EVE_CMD_ROTATE(t1);
    /* 5.58 CMD_ROTATEAROUND */ EVE_CMD_ROTATEAROUND(t1,t2,t1,t2);
    /* 5.59 CMD_TRANSLATE */ EVE_CMD_TRANSLATE(t1,t2);
    /* 5.60 CMD_CALIBRATE */ EVE_CMD_CALIBRATE(t1);
    /* 5.61 CMD_CALIBRATESUB */
    /* 5.62 CMD_SETROTATE */ EVE_CMD_SETROTATE(t1);
    /* 5.63 CMD_SPINNER */ EVE_CMD_SPINNER(t1,t2,t1,t2);
    /* 5.64 CMD_SCREENSAVER */ EVE_CMD_SCREENSAVER();
    /* 5.65 CMD_SKETCH */ EVE_CMD_SKETCH(t1,t2,t1,t2,t1,t2);
    /* 5.66 CMD_STOP */ EVE_CMD_STOP();
    /* 5.67 CMD_SETFONT */ EVE_CMD_SETFONT(t1,t2);
    /* 5.68 CMD_SETFONT2 */ EVE_CMD_SETFONT2(t1,t2,t1);
    /* 5.69 CMD_SETSCRATCH */ EVE_CMD_SETSCRATCH(t1);
    /* 5.70 CMD_ROMFONT */ EVE_CMD_ROMFONT(t1,t2);
    /* 5.71 CMD_RESETFONTS */ EVE_CMD_RESETFONTS();
    /* 5.72 CMD_TRACK */ EVE_CMD_TRACK(t1,t2,t1,t2,t1);
    /* 5.73 CMD_SNAPSHOT */ EVE_CMD_SNAPSHOT(t1);
    /* 5.74 CMD_SNAPSHOT2 */ EVE_CMD_SNAPSHOT2(t1,t2,t1,t2,t1,t2);
    /* 5.75 CMD_SETBITMAP */ EVE_CMD_SETBITMAP(t1,t2,t1,t2);
    /* 5.76 CMD_LOGO */ EVE_CMD_LOGO();
    /* 5.77 CMD_FLASHERASE */ EVE_CMD_FLASHERASE();
    /* 5.78 CMD_FLASHWRITE */ EVE_CMD_FLASHWRITE(t1,t2);
    /* 5.79 CMD_FLASHPROGRAM */ EVE_CMD_FLASHPROGRAM(t1,t2,t1);
    /* 5.80 CMD_FLASHREAD */ EVE_CMD_FLASHREAD(t1,t2,t1);
    /* 5.81 CMD_APPENDF */ EVE_CMD_APPENDF(t1,t2);
    /* 5.82 CMD_FLASHUPDATE */ EVE_CMD_FLASHUPDATE(t1,t2,t1);
    /* 5.83 CMD_FLASHDETACH */ EVE_CMD_FLASHDETACH();
    /* 5.84 CMD_FLASHATTACH */ EVE_CMD_FLASHATTACH();
    /* 5.85 CMD_FLASHFAST */ EVE_CMD_FLASHFAST(t1);
    /* 5.86 CMD_FLASHSPIDESEL */ EVE_CMD_FLASHSPIDESEL();
    /* 5.87 CMD_FLASHSPITX */ EVE_CMD_FLASHSPITX(t1);
    /* 5.88 CMD_FLASHSPIRX */ EVE_CMD_FLASHSPIRX(t1,t2);
    /* 5.89 CMD_CLEARCACHE */ EVE_CMD_CLEARCACHE();
    /* 5.90 CMD_FLASHSOURCE */ EVE_CMD_FLASHSOURCE(t1);
    /* 5.91 CMD_VIDEOSTARTF */ EVE_CMD_VIDEOSTARTF();
    /* 5.92 CMD_ANIMSTART */ EVE_CMD_ANIMSTART(t1,t2,t1);
    /* 5.93 CMD_ANIMSTARTRAM */
    /* 5.94 CMD_RUNANIM */
    /* 5.95 CMD_ANIMSTOP */ EVE_CMD_ANIMSTOP(t1);
    /* 5.96 CMD_ANIMXY */ EVE_CMD_ANIMXY(t1,t2,t1);
    /* 5.97 CMD_ANIMDRAW */ EVE_CMD_ANIMDRAW(t1);
    /* 5.98 CMD_ANIMFRAME */ EVE_CMD_ANIMFRAME(t1,t2,t1,t2);
    /* 5.99 CMD_ANIMFRAMERAM */ 
    /* 5.100 CMD_SYNC */ EVE_CMD_SYNC();
    /* 5.101 CMD_BITMAP_TRANSFORM */ EVE_CMD_BITMAP_TRANSFORM(t1,t2,t1,t2,t1,t2,t1,t2,t1,t2,t1,t2,t1);
    /* 5.102 CMD_TESTCARD */ 
    /* 5.103 CMD_WAIT */ 
    /* 5.104 CMD_NEWLIST */ 
    /* 5.105 CMD_ENDLIST */ 
    /* 5.106 CMD_CALLLIST */ 
    /* 5.107 CMD_RETURN */ 
    /* 5.108 CMD_FONTCACHE */ 
    /* 5.109 CMD_FONTCACHEQUERY */ 
    /* 5.110 CMD_GETIMAGE */ 
    /* 5.111 CMD_HSF */ 
    /* 5.112 CMD_PCLKFREQ */ 

    /* 5.13 CMD_APILEVEL */ EVE_CMD_APILEVEL(t1);
    /* 5.61 CMD_CALIBRATESUB */ EVE_CMD_CALIBRATESUB(t1,t2,t1,t2,t1);
    /* 5.94 CMD_RUNANIM */ EVE_CMD_RUNANIM(t1,t2);
    /* 5.93 CMD_ANIMSTARTRAM */ EVE_CMD_ANIMSTARTRAM(t1,t2,t1);
    /* 5.99 CMD_ANIMFRAMERAM */ EVE_CMD_ANIMFRAMERAM(t1,t2,t1,t2);
    /* 5.102 CMD_TESTCARD */ EVE_CMD_TESTCARD();
    /* 5.103 CMD_WAIT */ EVE_CMD_WAIT(t1);
    /* 5.104 CMD_NEWLIST */ EVE_CMD_NEWLIST(t1);
    /* 5.105 CMD_ENDLIST */ EVE_CMD_ENDLIST();
    /* 5.106 CMD_CALLLIST */ EVE_CMD_CALLLIST(t1);
    /* 5.107 CMD_RETURN */ EVE_CMD_RETURN();
    /* 5.108 CMD_FONTCACHE */ EVE_CMD_FONTCACHE(t1,t2,t1);
    /* 5.109 CMD_FONTCACHEQUERY */ EVE_CMD_FONTCACHEQUERY(t1,t2);
    /* 5.110 CMD_GETIMAGE */ EVE_CMD_GETIMAGE(t1,t2,t1,t2,t1);
    /* 5.111 CMD_HSF */ EVE_CMD_HSF(t1);
    /* 5.112 CMD_PCLKFREQ */ EVE_CMD_PCLKFREQ(t1,t2,t1);

    EVE_LIB_EndCoProList();
}

#endif // IS_EVE_API(4)
