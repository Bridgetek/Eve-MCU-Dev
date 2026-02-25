/**
 @file fonts.c
 */
/*
 * ============================================================================
 * (C) Copyright,  Bridgetek Pte. Ltd.
 * ============================================================================
 *
 * This source code ("the Software") is provided by Bridgetek Pte Ltd
 * ("Bridgetek") subject to the licence terms set out
 * https://brtchip.com/wp-content/uploads/2021/11/BRT_Software_License_Agreement.pdf ("the Licence Terms").
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
#include <stdint.h>
#include <stddef.h>

#include <### EVE CLASS ###.h>

/**
 @brief EVE library handle.
 @details This is the one instance of the EVE library. Available as a global.
 */
extern ### EVE CLASS ### eve;

#include "sound.h"

void enableSound(void)
{ 
	/* ### BEGIN API == 1 ### */
	uint8_t regGpio;
	uint8_t regGpioDir;
 
	// Read GPIOX_DIR register
	regGpioDir = eve.LIB_MemRead8(eve.REG_GPIO_DIR);
	// Set bit 1 of GPIO_DIR register  to output (GPIO1)
	regGpioDir = regGpioDir | 0x02;
	// Enable GPIO2 as an output
	eve.LIB_MemWrite8(eve.REG_GPIO_DIR, regGpioDir);
 
	// Read REG_GPIOX
	regGpio = eve.LIB_MemRead8(eve.REG_GPIO);
	// Set bit 1 of GPIO register (GPIO1) high
	regGpio = regGpio | 0x02;
	// Enable the GPIO2 signal to the Audio Driver
	eve.LIB_MemWrite8(eve.REG_GPIO, regGpio);
 
	// Turn synthesizer volume up
	eve.LIB_MemWrite8(eve.REG_VOL_SOUND, 255);
	// Set synthesizer to mute
	eve.LIB_MemWrite8(eve.REG_SOUND, 0x60);
	// Play sound
	eve.LIB_MemWrite8(eve.REG_PLAY, 1);
	/* ### END API ### */

	/* ### BEGIN API == 2, 3, 4 ### */
    uint16_t regGpiox;
	uint16_t regGpioxDir;
 
	// Read GPIOX_DIR register
	regGpioxDir = eve.LIB_MemRead16(eve.REG_GPIOX_DIR);
	// Set bit 2 of  GPIO_DIR register  to output (GPIO2)
	regGpioxDir = regGpioxDir | 0x0004;
	// Enable GPIO2 as an output
	eve.LIB_MemWrite16(eve.REG_GPIOX_DIR, regGpioxDir);
 
	// Read REG_GPIOX
	regGpiox = eve.LIB_MemRead16(eve.REG_GPIOX);
	// Set bit 2 of GPIOX register (GPIO2) high
	regGpiox = regGpiox | 0x0004;
	// Enable the GPIO2 signal to the Audio Driver
	eve.LIB_MemWrite16(eve.REG_GPIOX, regGpiox);
 
	// Turn synthesizer volume up
	eve.LIB_MemWrite8(eve.REG_VOL_SOUND, 255);
	// Set synthesizer to mute
	eve.LIB_MemWrite8(eve.REG_SOUND, 0x60);
	// Play sound
	eve.LIB_MemWrite8(eve.REG_PLAY, 1);
	/* ### END API ### */
	
	/* ### BEGIN API >= 5 ### */
	// Turn synthesizer volume up
	eve.LIB_MemWrite32(eve.REG_VOL_SOUND, 255);
	// Set synthesizer to mute
	eve.LIB_MemWrite32(eve.REG_SOUND, 0x60);
	// Play sound
	eve.LIB_MemWrite32(eve.REG_PLAY, 1);
	/* ### END API ### */
}

void playSound(uint8_t sound, uint8_t note)
{
	/* ### BEGIN API < 5 ### */
	// set synthesizer to chime c#3
	eve.LIB_MemWrite16(eve.REG_SOUND, (note << 8) | sound);
	// play sound
	eve.LIB_MemWrite8(eve.REG_PLAY, 1);
	/* ### END API ### */

	/* ### BEGIN API >= 5 ### */
	// set synthesizer to chime c#3
	eve.LIB_MemWrite32(eve.REG_SOUND, (note << 8) | sound);
	// play sound
	eve.LIB_MemWrite32(eve.REG_PLAY, 1);
	/* ### END API ### */
} 

void playClick(void) 
{
    playSound(SOUND_CLICK, 0);
}
 
void playChimes(uint8_t note) 
{
    playSound(SOUND_CHIMES, note);
}
 
void playBell(uint8_t note)
{
    playSound(SOUND_BELL, note);
}
 
void playPip(uint8_t note)
{
    playSound(SOUND_1PIP, note);
}
 
void playClack(void)
{
    playSound(SOUND_CLACK, 0);
}