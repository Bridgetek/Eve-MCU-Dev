/**
* @file api5.c
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

#if IS_EVE_API(5)

/* CONSTANTS ***********************************************************************/

/* FUNCTIONS ***********************************************************************/

/* Function names taken directly from BRT_AN_086 BT82X Series Programming Guide Version 1.2 */

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
    /* 4.13 BITMAP_SOURCE_H */ EVE_BITMAP_SOURCE_H(t1);
    /* 4.14 BITMAP_SWIZZLE */ EVE_BITMAP_SWIZZLE(t1,t2,t1,t2);
    /* 4.15 BITMAP_TRANSFORM_A */ EVE_BITMAP_TRANSFORM_A(t1);
    /* 4.16 BITMAP_TRANSFORM_B */ EVE_BITMAP_TRANSFORM_B(t1);
    /* 4.17 BITMAP_TRANSFORM_C */ EVE_BITMAP_TRANSFORM_C(t1);
    /* 4.18 BITMAP_TRANSFORM_D */ EVE_BITMAP_TRANSFORM_D(t1);
    /* 4.19 BITMAP_TRANSFORM_E */ EVE_BITMAP_TRANSFORM_E(t1);
    /* 4.20 BITMAP_TRANSFORM_F */ EVE_BITMAP_TRANSFORM_F(t1);
    /* 4.22 BITMAP_ZORDER */ EVE_BITMAP_ZORDER(t1);
    /* 4.22 BLEND_FUNC */ EVE_BLEND_FUNC(t1,t2);
    /* 4.23 CALL */ EVE_CALL(t1);
    /* 4.24 CELL */ EVE_CELL(t1);
    /* 4.25 CLEAR */ EVE_CLEAR(t1,t2,t1);
    /* 4.26 CLEAR_COLOR_A */ EVE_CLEAR_COLOR_A(t1);
    /* 4.27 CLEAR_COLOR_RGB */ EVE_CLEAR_COLOR_RGB(t1,t2,t1);
    /* 4.28 CLEAR_STENCIL */ EVE_CLEAR_STENCIL(t1);
    /* 4.29 CLEAR_TAG */ EVE_CLEAR_TAG(t1);
    /* 4.30 COLOR_A */ EVE_COLOR_A(t1);
    /* 4.31 COLOR_MASK */ EVE_COLOR_MASK(t1,t2,t1,t2);
    /* 4.32 COLOR_RGB */ EVE_COLOR_RGB(t1,t2,t1);
    /* 4.33 DISPLAY */ EVE_DISPLAY();
    /* 4.34 END */ EVE_END();
    /* 4.35 JUMP */ EVE_JUMP(t1);
    /* 4.36 LINE_WIDTH */ EVE_LINE_WIDTH(t1);
    /* 4.37 MACRO */ EVE_MACRO(t1);
    /* 4.38 NOP */ EVE_NOP();
    /* 4.39 PALETTE_SOURCE */ EVE_PALETTE_SOURCE(t1);
    /* 4.40 PALETTE_SOURCE_H */ EVE_PALETTE_SOURCE_H(t1);
    /* 4.41 POINT_SIZE */ EVE_POINT_SIZE(t1);
    /* 4.42 REGION */ EVE_REGION(t1,t2,t1);
    /* 4.43 RESTORE_CONTEXT */ EVE_RESTORE_CONTEXT();
    /* 4.44 RETURN */ EVE_RETURN();
    /* 4.45 SAVE CONTEXT */ EVE_SAVE_CONTEXT();
    /* 4.46 SCISSOR_SIZE */ EVE_SCISSOR_SIZE(t1,t2);
    /* 4.47 SCISSOR_XY */ EVE_SCISSOR_XY(t1,t2);
    /* 4.48 STENCIL_FUNC */ EVE_STENCIL_FUNC(t1,t2,t1);
    /* 4.49 STENCIL_MASK */ EVE_STENCIL_MASK(t1);
    /* 4.50 STENCIL_OP */ EVE_STENCIL_OP(t1,t2);
    /* 4.51 TAG */ EVE_TAG(t1);
    /* 4.52 TAG_MASK */ EVE_TAG_MASK(t1);
    /* 4.53 VERTEX2F */ EVE_VERTEX2F(t1,t2);
    /* 4.54 VERTEX2II */ EVE_VERTEX2II(t1,t2,t1,t2);
    /* 4.55 VERTEX_FORMAT */ EVE_VERTEX_FORMAT(t1);
    /* 4.56 VERTEX_TRANSLATE_X */ EVE_VERTEX_TRANSLATE_X(t1);
    /* 4.57 VERTEX_TRANSLATE_Y */ EVE_VERTEX_TRANSLATE_Y(t1);
    EVE_LIB_EndCoProList();
}

void api_test_cmd(uint32_t t1, uint32_t t2)
{
    EVE_LIB_BeginCoProList();
    /* 5 Co-processor Engine */
    /* 5.17.1 CMD_DLSTART */ EVE_CMD_DLSTART();
    /* 5.17.2 CMD_SWAP */ EVE_CMD_SWAP();
    /* 5.18.1 CMD_APPEND */ EVE_CMD_APPEND(t1,t2);
    /* 5.18.2 CMD_ARC */ EVE_CMD_ARC(t1,t2,t1,t2,t1,t2);
    /* 5.18.3 CMD_BGCOLOR */ EVE_CMD_BGCOLOR(t1);
    /* 5.18.4 CMD_BUTTON */ EVE_CMD_BUTTON(t1,t2,t1,t2,t1,t2,"1234");
    /* 5.18.5 CMD_CGRADIENT */ EVE_CMD_CGRADIENT(t1,t2,t1,t2,t1,t2,t1);
    /* 5.18.6 CMD_CLOCK */ EVE_CMD_CLOCK(t1,t2,t1,t2,t1,t2,t1,t2);
    /* 5.18.7 CMD_DIAL */ EVE_CMD_DIAL(t1,t2,t1,t2,t1);
    /* 5.18.8 CMD_FGCOLOR */ EVE_CMD_FGCOLOR(t1);
    /* 5.18.9 CMD_FILLWIDTH */ EVE_CMD_FILLWIDTH(t1);
    /* 5.18.10 CMD_GAUGE */ EVE_CMD_GAUGE(t1,t2,t1,t2,t1,t2,t1,t2);
    /* 5.18.11 CMD_GLOW */ EVE_CMD_GLOW(t1,t2,t1,t2);
    /* 5.18.12 CMD_GRADCOLOR */ EVE_CMD_GRADCOLOR(t1);
    /* 5.18.13 CMD_GRADIENT */ EVE_CMD_GRADIENT(t1,t2,t1,t2,t1,t2);
    /* 5.18.14 CMD_GRADIENTA */ EVE_CMD_GRADIENTA(t1,t2,t1,t2,t1,t2);
    /* 5.18.15 CMD_KEYS */ EVE_CMD_KEYS(t1,t2,t1,t2,t1,t2,"12345");
    /* 5.18.16 CMD_NUMBER */ EVE_CMD_NUMBER(t1,t2,t1,t2,t1);
    /* 5.18.17 CMD_PROGRESS */ EVE_CMD_PROGRESS(t1,t2,t1,t2,t1,t2,t1);
    /* 5.18.18 CMD_SCROLLBAR */ EVE_CMD_SCROLLBAR(t1,t2,t1,t2,t1,t2,t1,t2);
    /* 5.18.19 CMD_SETBASE */ EVE_CMD_SETBASE(t1);
    /* 5.18.20 CMD_SLIDER */ EVE_CMD_SLIDER(t1,t2,t1,t2,t1,t2,t1);
    /* 5.18.21 CMD_TEXT */ EVE_CMD_TEXT(t1,t2,t1,t2,"1234567");
    /* 5.18.22 CMD_TEXTDIM */ EVE_CMD_TEXTDIM(t1,t2,t1,"123");
    /* 5.18.23 CMD_TOGGLE */ EVE_CMD_TOGGLE(t1,t2,t1,t2,t1,t2,"123456");
    /* 5.19.1 CMD_MEMCPY */ EVE_CMD_MEMCPY(t1,t2,t1);
    /* 5.19.2 CMD_MEMCRC */ EVE_CMD_MEMCRC(t1,t2,t1);
    /* 5.19.3 CMD_MEMSET */ EVE_CMD_MEMSET(t1,t2,t1);
    /* 5.19.4 CMD_MEMWRITE */ EVE_CMD_MEMWRITE(t1,t2);
    /* 5.19.5 CMD_MEMZERO */ EVE_CMD_MEMZERO(t1,t2);
    /* 5.20.1 CMD_FSREAD */ EVE_CMD_FSREAD(t1,"1234567890a",t2);
    /* 5.20.2 CMD_INFLATE */ EVE_CMD_INFLATE(t1,t2);
    /* 5.20.3 CMD_LOADIMAGE */ EVE_CMD_LOADIMAGE(t1,t2);
    /* 5.20.4 CMD_LOADWAV */ EVE_CMD_LOADWAV(t1,t2);
    /* 5.20.5 CMD_MEDIAFIFO */ EVE_CMD_MEDIAFIFO(t1,t2);
    /* 5.20.6 CMD_PLAYWAV */ EVE_CMD_PLAYWAV(t1);
    /* 5.21.1 CMD_BITMAP_TRANSFORM */ EVE_CMD_BITMAP_TRANSFORM(t1,t2,t1,t2,t1,t2,t1,t2,t1,t2,t1,t2,t1);
    /* 5.21.2 CMD_GETMATRIX */ EVE_CMD_GETMATRIX(t1,t2,t1,t2,t1,t2);
    /* 5.21.3 CMD_LOADIDENTITY */ EVE_CMD_LOADIDENTITY();
    /* 5.21.4 CMD_ROTATE */ EVE_CMD_ROTATE(t1);
    /* 5.21.5 CMD_ROTATEAROUND */ EVE_CMD_ROTATEAROUND(t1,t2,t1,t2);
    /* 5.21.6 CMD_SCALE */ EVE_CMD_SCALE(t1,t2);
    /* 5.21.7 CMD_SETMATRIX */ EVE_CMD_SETMATRIX();
    /* 5.21.8 CMD_TRANSLATE */ EVE_CMD_TRANSLATE(t1,t2);
    /* 5.22.1 CMD_APPENDF */ EVE_CMD_APPENDF(t1,t2);
    /* 5.22.2 CMD_FLASHATTACH */ EVE_CMD_FLASHATTACH();
    /* 5.22.3 CMD_FLASHDETACH */ EVE_CMD_FLASHDETACH();
    /* 5.22.4 CMD_FLASHERASE */ EVE_CMD_FLASHERASE();
    /* 5.22.5 CMD_FLASHFAST */ EVE_CMD_FLASHFAST(t1);
    /* 5.22.6 CMD_FLASHPROGRAM */ EVE_CMD_FLASHPROGRAM(t1,t2,t1);
    /* 5.22.7 CMD_FLASHREAD */ EVE_CMD_FLASHREAD(t1,t2,t1);
    /* 5.22.8 CMD_FLASHSOURCE */ EVE_CMD_FLASHSOURCE(t1);
    /* 5.22.9 CMD_FLASHSPIDESEL */ EVE_CMD_FLASHSPIDESEL();
    /* 5.22.10 CMD_FLASHSPIRX */ EVE_CMD_FLASHSPIRX(t1,t2);
    /* 5.22.11 CMD_FLASHSPITX */ EVE_CMD_FLASHSPITX(t1);
    /* 5.22.12 CMD_FLASHUPDATE */ EVE_CMD_FLASHUPDATE(t1,t2,t1);
    /* 5.22.13 CMD_FLASHWRITE */ EVE_CMD_FLASHWRITE(t1,t2);
    /* 5.23.1 CMD_PLAYVIDEO */ EVE_CMD_PLAYVIDEO(t1);
    /* 5.23.2 CMD_VIDEOFRAME */ EVE_CMD_VIDEOFRAME(t1,t2);
    /* 5.23.3 CMD_VIDEOSTART */ EVE_CMD_VIDEOSTART(t1);
    /* 5.24.1 CMD_ANIMDRAW */ EVE_CMD_ANIMDRAW(t1);
    /* 5.24.2 CMD_ANIMFRAME */ EVE_CMD_ANIMFRAME(t1,t2,t1,t2);
    /* 5.24.3 CMD_ANIMSTART */ EVE_CMD_ANIMSTART(t1,t2,t1);
    /* 5.24.4 CMD_ANIMSTOP */ EVE_CMD_ANIMSTOP(t1);
    /* 5.24.5 CMD_ANIMXY */ EVE_CMD_ANIMXY(t1,t2,t1);
    /* 5.24.6 CMD_RUNANIM */ EVE_CMD_RUNANIM(t1,t2);
    /* 5.25.1 CMD_FSDIR */ EVE_CMD_FSDIR(t1,t2,"123456789abc",t1);
    /* 5.25.2 CMD_FSOPTIONS */ EVE_CMD_FSOPTIONS(t1);
    /* 5.25.3 CMD_FSSIZE */ EVE_CMD_FSSIZE("1234",t1);
    /* 5.25.4 CMD_FSSOURCE */ EVE_CMD_FSSOURCE("1234",t1);
    /* 5.25.5 CMD_SDATTACH */ EVE_CMD_SDATTACH(t1,t2);
    /* 5.25.6 CMD_SDBLOCKREAD */ EVE_CMD_SDBLOCKREAD(t1,t2,t1,t2);
    /* 5.26.1 CMD_CALLLIST */ EVE_CMD_CALLLIST(t1);
    /* 5.26.2 CMD_COPYLIST */ EVE_CMD_COPYLIST(t1);
    /* 5.26.3 CMD_ENDLIST */ EVE_CMD_ENDLIST();
    /* 5.26.4 CMD_NEWLIST */ EVE_CMD_NEWLIST(t1);
    /* 5.27.1 CMD_CALIBRATE */ EVE_CMD_CALIBRATE(t1);
    /* 5.27.2 CMD_CALIBRATESUB */ EVE_CMD_CALIBRATESUB(t1,t2,t1,t2,t1);
    /* 5.27.3 CMD_COLDSTART */ EVE_CMD_COLDSTART();
    /* 5.27.4 CMD_DDRSHUTDOWN */ EVE_CMD_DDRSHUTDOWN();
    /* 5.27.5 CMD_DDRSTARTUP */ EVE_CMD_DDRSTARTUP();
    /* 5.27.6 CMD_ENABLEREGION */ EVE_CMD_ENABLEREGION(t1);
    /* 5.27.7 CMD_FENCE */ EVE_CMD_FENCE();
    /* 5.27.8 CMD_GETIMAGE */ EVE_CMD_GETIMAGE(t1,t2,t1,t2,t1);
    /* 5.27.9 CMD_GETPROPS */ EVE_CMD_GETPROPS(t1,t2,t1);
    /* 5.27.10 CMD_GETPTR */ EVE_CMD_GETPTR(t1);
    /* 5.27.11 CMD_GRAPHICSFINISH */ EVE_CMD_GRAPHICSFINISH();
    /* 5.27.12 CMD_INTERRUPT */ EVE_CMD_INTERRUPT(t1);
    /* 5.27.13 CMD_I2SSTARTUP */ EVE_CMD_I2SSTARTUP(t1);
    /* 5.27.14 CMD_LOADASSET */ EVE_CMD_LOADASSET(t1,t2);
    /* 5.27.15 CMD_LOGO */ EVE_CMD_LOGO();
    /* 5.27.16 CMD_NOP */ EVE_CMD_NOP();
    /* 5.27.17 CMD_REGREAD */ EVE_CMD_REGREAD(t1,t2);
    /* 5.27.18 CMD_REGWRITE */ EVE_CMD_REGWRITE(t1,t2);
    /* 5.27.19 CMD_RENDERTARGET */ EVE_CMD_RENDERTARGET(t1,t2,t1,t2);
    /* 5.27.20 CMD_RESETFONTS */ EVE_CMD_RESETFONTS();
    /* 5.27.21 CMD_RESTORECONTEXT */ EVE_CMD_RESTORECONTEXT();
    /* 5.27.22 CMD_RESULT */ EVE_CMD_RESULT(t1);
    /* 5.27.23 CMD_RETURN */ EVE_CMD_RETURN();
    /* 5.27.24 CMD_ROMFONT */ EVE_CMD_ROMFONT(t1,t2);
    /* 5.27.25 CMD_SAVECONTEXT */ EVE_CMD_SAVECONTEXT();
    /* 5.27.26 CMD_SCREENSAVER */ EVE_CMD_SCREENSAVER();
    /* 5.27.27 CMD_SETBITMAP */ EVE_CMD_SETBITMAP(t1,t2,t1,t2);
    /* 5.27.28 CMD_SETFONT */ EVE_CMD_SETFONT(t1,t2,t1);
    /* 5.27.29 CMD_SETROTATE */ EVE_CMD_SETROTATE(t1);
    /* 5.27.30 CMD_SETSCRATCH */ EVE_CMD_SETSCRATCH(t1);
    /* 5.27.31 CMD_SKETCH */ EVE_CMD_SKETCH(t1,t2,t1,t2,t1,t2);
    /* 5.27.32 CMD_SKIPCOND */ EVE_CMD_SKIPCOND(t1,t2,t1,t2,t1);
    /* 5.27.33 CMD_SNAPSHOT */ EVE_CMD_SNAPSHOT(t1);
    /* 5.27.34 CMD_SPINNER */ EVE_CMD_SPINNER(t1,t2,t1,t2);
    /* 5.27.35 CMD_STOP */ EVE_CMD_STOP();
    /* 5.27.36 CMD_SYNC */ EVE_CMD_SYNC();
    /* 5.27.37 CMD_TESTCARD */ EVE_CMD_TESTCARD();
    /* 5.27.38 CMD_TRACK */ EVE_CMD_TRACK(t1,t2,t1,t2,t1);
    /* 5.27.39 CMD_WAIT */ EVE_CMD_WAIT(t1);
    /* 5.27.40 CMD_WAITCHANGE */ EVE_CMD_WAITCHANGE(t1);
    /* 5.27.41 CMD_WAITCOND */ EVE_CMD_WAITCOND(t1,t2,t1,t2);
    /* 5.27.42 CMD_WATCHDOG */ EVE_CMD_WATCHDOG(t1);
    EVE_LIB_EndCoProList();
}

#endif // IS_EVE_API(5)
