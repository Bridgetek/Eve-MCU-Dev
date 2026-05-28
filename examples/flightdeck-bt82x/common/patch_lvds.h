/* Generated file by extension.py */

#include <stdint.h>
#include <stdarg.h>

int eve_loadpatch(void);
/* From patch1 */
void EVE_CMD_REGION();
void EVE_CMD_ENDREGION(int16_t p1, int16_t p2, int16_t p3, int16_t p4);
void EVE_CMD_TOUCHOFFSET(int16_t p1, int16_t p2);
void EVE_CMD_ENDTOUCHOFFSET();
/* From patch-lvds */
void EVE_CMD_LVDSSETUP(uint16_t p1, uint16_t p2);
void EVE_CMD_LVDSCONN(uint32_t p1);
void EVE_LIB_LVDSConn(uint32_t *p1);
void EVE_CMD_LVDSSTOP();
void EVE_CMD_LVDSSTART();
/* From patch-sevenseg */
#define EVE_OPT_DECIMAL 16
#define EVE_OPT_TIMECOLON 32
#define EVE_OPT_NUMBER 15
#define EVE_OPT_SEGMENTMASK 8240
void EVE_CMD_SEVENSEG(int16_t p1, int16_t p2, int16_t p3, uint16_t p4);
