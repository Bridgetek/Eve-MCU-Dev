/* Generated file by extensionutil.py */
/* This file can be included in an EVE-MCU-Dev project */
/* See https://github.com/Bridgetek/Eve-MCU-Dev/ */

#include "patch_medinfo.h"
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

void EVE_CMD_MESSAGEBOX(int16_t p1, uint16_t p2, const char * p3)
{
	uint32_t StringLength;

	HAL_Write32(0xffffff99);
	HAL_Write32((p1 & 0xffff) | ((uint32_t)p2<<16));
	StringLength = EVE_LIB_SendString(p3);
	HAL_IncCmdPointer(StringLength + 8);
}

void EVE_CMD_TOOLTIP(int16_t p1, int16_t p2, int16_t p3, uint16_t p4, const char * p5)
{
	uint32_t StringLength;

	HAL_Write32(0xffffff9a);
	HAL_Write32((p1 & 0xffff) | ((uint32_t)p2<<16));
	HAL_Write32((p3 & 0xffff) | ((uint32_t)p4<<16));
	StringLength = EVE_LIB_SendString(p5);
	HAL_IncCmdPointer(StringLength + 12);
}

void EVE_CMD_TEXTSIZE(int16_t p1, uint16_t p2, const char * p3, uint16_t p4, uint16_t p5)
{
	uint32_t StringLength;

	HAL_Write32(0xffffffaa);
	HAL_Write32((p1 & 0xffff) | ((uint32_t)p2<<16));
	StringLength = EVE_LIB_SendString(p3);
	HAL_Write32((p4 & 0xffff) | ((uint32_t)p5<<16));
	HAL_IncCmdPointer(StringLength + 12);
}

void EVE_LIB_TextSize(int16_t p1, uint16_t p2, const char * p3, uint16_t *p4, uint16_t *p5)
{
	EVE_LIB_BeginCoProList();
	// Send the command to the CoProcessor.
	EVE_CMD_TEXTSIZE(p1, p2, p3, 0, 0);
	// Wait for it to finish.
	EVE_LIB_EndCoProList();
	EVE_LIB_AwaitCoProEmpty();
	// Obtain the results from the EVE_RAM_CMD in the CoProcessor.
	uint32_t tmp4 = EVE_LIB_GetResult(1);
	*p4 = (uint16_t)(tmp4>>16);
	*p5 = (uint16_t)(tmp4&0xffff);
}

void EVE_CMD_FEEDBACKICON(int16_t p1, int16_t p2, uint16_t p3, uint16_t p4, int16_t p5)
{
	HAL_Write32(0xffffffa8);
	HAL_Write32((p1 & 0xffff) | ((uint32_t)p2<<16));
	HAL_Write32((p3 & 0xffff) | ((uint32_t)p4<<16));
	HAL_Write32((p5 & 0xffff) | ((uint32_t)0<<16));
	HAL_IncCmdPointer(16);
}

void EVE_CMD_PLOTDRAW(uint32_t p1, uint16_t p2, uint16_t p3, int16_t p4, int16_t p5, uint32_t p6, uint32_t p7, uint32_t p8)
{
	HAL_Write32(0xffffffab);
	HAL_Write32(p1);
	HAL_Write32((p2 & 0xffff) | ((uint32_t)p3<<16));
	HAL_Write32((p4 & 0xffff) | ((uint32_t)p5<<16));
	HAL_Write32(p6);
	HAL_Write32(p7);
	HAL_Write32(p8);
	HAL_IncCmdPointer(28);
}

void EVE_CMD_PLOTSTREAM(uint16_t p1, uint16_t p2, int16_t p3, int16_t p4, uint16_t p5, uint16_t p6, uint32_t p7)
{
	HAL_Write32(0xffffffac);
	HAL_Write32((p1 & 0xffff) | ((uint32_t)p2<<16));
	HAL_Write32((p3 & 0xffff) | ((uint32_t)p4<<16));
	HAL_Write32((p5 & 0xffff) | ((uint32_t)p6<<16));
	HAL_Write32(p7);
	HAL_IncCmdPointer(20);
}

void EVE_CMD_PLOTBITMAP(uint32_t p1, uint16_t p2, uint16_t p3, uint32_t p4)
{
	HAL_Write32(0xffffffad);
	HAL_Write32(p1);
	HAL_Write32((p2 & 0xffff) | ((uint32_t)p3<<16));
	HAL_Write32(p4);
	HAL_IncCmdPointer(16);
}

void EVE_CMD_TEXTTICKER(int16_t p1, int16_t p2, uint16_t p3, uint16_t p4, int16_t p5, uint16_t p6, uint32_t p7, const char * p8)
{
	uint32_t StringLength;

	HAL_Write32(0xffffff97);
	HAL_Write32((p1 & 0xffff) | ((uint32_t)p2<<16));
	HAL_Write32((p3 & 0xffff) | ((uint32_t)p4<<16));
	HAL_Write32((p5 & 0xffff) | ((uint32_t)p6<<16));
	HAL_Write32(p7);
	StringLength = EVE_LIB_SendString(p8);
	HAL_IncCmdPointer(StringLength + 20);
}

void EVE_CMD_SEVENSEG(int16_t p1, int16_t p2, int16_t p3, uint16_t p4)
{
	HAL_Write32(0xffffff98);
	HAL_Write32((p1 & 0xffff) | ((uint32_t)p2<<16));
	HAL_Write32((p3 & 0xffff) | ((uint32_t)p4<<16));
	HAL_IncCmdPointer(12);
}

#endif /* IS_EVE_API(5) */
