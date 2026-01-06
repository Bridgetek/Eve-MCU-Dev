/* Generated file by extensionutil.py */
/* This file can be included in an EVE-MCU-Dev project */
/* See https://github.com/Bridgetek/Eve-MCU-Dev/ */

#include <stdint.h>
#include <stdarg.h>

int eve_loadpatch(void);
/* From patch-base */
void EVE_CMD_REGION();
void EVE_CMD_ENDREGION(int16_t p1, int16_t p2, int16_t p3, int16_t p4);
void EVE_CMD_TOUCHOFFSET(int16_t p1, int16_t p2);
void EVE_CMD_ENDTOUCHOFFSET();
/* From patch-dialogs */
#define EVE_OPT_MSGBGALPHA 255
#define EVE_OPT_MSGTOP 512
#define EVE_OPT_MSGBOTTOM 1024
#define EVE_OPT_MSGEDGE 2048
void EVE_CMD_MESSAGEBOX(int16_t p1, uint16_t p2, const char * p3);
void EVE_CMD_TOOLTIP(int16_t p1, int16_t p2, int16_t p3, uint16_t p4, const char * p5);
void EVE_CMD_TEXTSIZE(int16_t p1, uint16_t p2, const char * p3, uint16_t p4, uint16_t p5);
void EVE_LIB_TextSize(int16_t p1, uint16_t p2, const char * p3, uint16_t *p4, uint16_t *p5);
/* From patch-feedbackicon */
void EVE_CMD_FEEDBACKICON(int16_t p1, int16_t p2, uint16_t p3, uint16_t p4, int16_t p5);
/* From patch-plotgraph */
#define EVE_OPT_PLOTFILTER 8192
#define EVE_OPT_PLOTHORIZONTAL 0
#define EVE_OPT_PLOTVERTICAL 4096
#define EVE_OPT_PLOTINVERT 16384
#define EVE_OPT_PLOTOVERLAY 32768
#define EVE_OPT_PLOTWIDTH 15
void EVE_CMD_PLOTDRAW(uint32_t p1, uint16_t p2, uint16_t p3, int16_t p4, int16_t p5, uint32_t p6, uint32_t p7, uint32_t p8);
void EVE_CMD_PLOTSTREAM(uint16_t p1, uint16_t p2, int16_t p3, int16_t p4, uint16_t p5, uint16_t p6, uint32_t p7);
void EVE_CMD_PLOTBITMAP(uint32_t p1, uint16_t p2, uint16_t p3, uint32_t p4);
/* From patch-textticker */
void EVE_CMD_TEXTTICKER(int16_t p1, int16_t p2, uint16_t p3, uint16_t p4, int16_t p5, uint16_t p6, uint32_t p7, const char * p8);
/* From patch-sevenseg */
#define EVE_OPT_DECIMAL 16
#define EVE_OPT_TIMECOLON 32
#define EVE_OPT_NUMBER 15
#define EVE_OPT_SEGMENTMASK 8240
void EVE_CMD_SEVENSEG(int16_t p1, int16_t p2, int16_t p3, uint16_t p4);
