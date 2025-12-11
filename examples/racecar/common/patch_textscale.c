/* Generated file by extensionutil.py */
/* This file can be included in an EVE-MCU-Dev project */
/* See https://github.com/Bridgetek/Eve-MCU-Dev/ */

#include "patch_textscale.h"
/* Include functions for EVE-MCU-Dev library API layer */
#include <EVE.h>
/* Include functions for EVE-MCU-Dev library Hardware Abstraction layer */
#include <HAL.h>

/* EVE-MCU-Dev macro to limit compilation to EVE API 5 (BT82x) */
#if IS_EVE_API(5)

void EVE_CMD_REGION()
{
	HAL_Write32(0xffffff8f);
	HAL_IncCmdPointer(4);
}

void EVE_CMD_ENDREGION(int16_t p1, int16_t p2, int16_t p3, int16_t p4)
{
	HAL_Write32(0xffffff90);
	HAL_Write32((p1 & 0xffff) | ((uint32_t)p2<<16));
	HAL_Write32((p3 & 0xffff) | ((uint32_t)p4<<16));
	HAL_IncCmdPointer(12);
}

void EVE_CMD_TOUCHOFFSET(int16_t p1, int16_t p2)
{
	HAL_Write32(0xffffffae);
	HAL_Write32((p1 & 0xffff) | ((uint32_t)p2<<16));
	HAL_IncCmdPointer(8);
}

void EVE_CMD_ENDTOUCHOFFSET()
{
	HAL_Write32(0xffffffaf);
	HAL_IncCmdPointer(4);
}

void EVE_CMD_TEXTSCALE(int16_t p1, int16_t p2, int16_t p3, uint16_t p4, uint32_t p5, const char * p6)
{
	uint32_t StringLength;

	HAL_Write32(0xffffff95);
	HAL_Write32((p1 & 0xffff) | ((uint32_t)p2<<16));
	HAL_Write32((p3 & 0xffff) | ((uint32_t)p4<<16));
	HAL_Write32(p5);
	StringLength = EVE_LIB_SendString(p6);
	HAL_IncCmdPointer(StringLength + 16);
}

#endif /* IS_EVE_API(5) */
