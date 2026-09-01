/* Generated file by extensionutil.py */
/* This file can be included in an EVE-MCU-Dev project */
/* See https://github.com/Bridgetek/Eve-MCU-Dev/ */

#ifndef _EVE_BT82X_PATCH_H
#define _EVE_BT82X_PATCH_H

#include <stdint.h>
#include <stdarg.h>

int eve_loadpatch(void);

/* EVE PATCH */

/* From patch-base */
void EVE_CMD_REGION(void);
void EVE_CMD_ENDREGION(int16_t p1, int16_t p2, int16_t p3, int16_t p4);
void EVE_CMD_TOUCHOFFSET(int16_t p1, int16_t p2);
void EVE_CMD_ENDTOUCHOFFSET(void);

/* EVE PATCH END */

#endif /* _EVE_BT82X_PATCH_H */
