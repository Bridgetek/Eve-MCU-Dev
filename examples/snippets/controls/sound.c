DP156102A/**
 @file sound.c
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
#include <stdint.h>
#include <stddef.h>

#include <EVE.h>

#include "sound.h"

void enableSound(void)
{ 
#if IS_EVE_API(1)
    uint8_t regGpio;
	uint8_t regGpioDir;
 
	// Read GPIOX_DIR register
	regGpioDir = EVE_LIB_MemRead8(EVE_REG_GPIO_DIR);
	// Set bit 1 of GPIO_DIR register  to output (GPIO1)
	regGpioDir = regGpioDir | 0x02;
	// Enable GPIO2 as an output
	EVE_LIB_MemWrite8(EVE_REG_GPIO_DIR, regGpioDir);
 
	// Read REG_GPIOX
	regGpio = EVE_LIB_MemRead8(EVE_REG_GPIO);
	// Set bit 1 of GPIO register (GPIO1) high
	regGpio = regGpio | 0x02;
	// Enable the GPIO2 signal to the Audio Driver
	EVE_LIB_MemWrite8(EVE_REG_GPIO, regGpio);
 
	// Turn synthesizer volume up
	EVE_LIB_MemWrite8(EVE_REG_VOL_SOUND, 255);
	// Set synthesizer to mute
	EVE_LIB_MemWrite8(EVE_REG_SOUND, SOUND_MUTE);
	// Play sound
	EVE_LIB_MemWrite8(EVE_REG_PLAY, 1);

#elif IS_EVE_API(2,3,4)
    uint16_t regGpiox;
	uint16_t regGpioxDir;

	// VM810C and VM880C modules use GPIO1
	#if (MODULE_TYPE == VM810C) || (MODULE_TYPE == VM880C)

	// Read GPIOX_DIR register
	regGpioxDir = EVE_LIB_MemRead16(EVE_REG_GPIOX_DIR);
	// Set bit 2 of  GPIO_DIR register  to output (GPIO1)
	regGpioxDir = regGpioxDir | 0x0002;
	// Enable GPIO2 as an output
	EVE_LIB_MemWrite16(EVE_REG_GPIOX_DIR, regGpioxDir);
 
	// Read REG_GPIOX
	regGpiox = EVE_LIB_MemRead16(EVE_REG_GPIOX);
	// Set bit 2 of GPIOX register (GPIO1) high
	regGpiox = regGpiox | 0x0002;
	// Enable the GPIO2 signal to the Audio Driver
	EVE_LIB_MemWrite16(EVE_REG_GPIOX, regGpiox);

	//all other modules use GPIO2
	#else
	// Read GPIOX_DIR register
	regGpioxDir = EVE_LIB_MemRead16(EVE_REG_GPIOX_DIR);
	// Set bit 2 of  GPIO_DIR register  to output (GPIO2)
	regGpioxDir = regGpioxDir | 0x0004;
	// Enable GPIO2 as an output
	EVE_LIB_MemWrite16(EVE_REG_GPIOX_DIR, regGpioxDir);
 
	// Read REG_GPIOX
	regGpiox = EVE_LIB_MemRead16(EVE_REG_GPIOX);
	// Set bit 2 of GPIOX register (GPIO2) high
	regGpiox = regGpiox | 0x0004;
	// Enable the GPIO2 signal to the Audio Driver
	EVE_LIB_MemWrite16(EVE_REG_GPIOX, regGpiox);

	#endif

	// Turn synthesizer volume up
	EVE_LIB_MemWrite8(EVE_REG_VOL_SOUND, 255);
	// Set synthesizer to mute
	EVE_LIB_MemWrite8(EVE_REG_SOUND, SOUND_MUTE);
	// Play sound
	EVE_LIB_MemWrite8(EVE_REG_PLAY, 1);
#else

	// if we have purposely defined AUDIO_I2S or we ar using a module with I2S support
	#if defined(AUDIO_I2S) || (MODULE_TYPE == VM820B10A) || (MODULE_TYPE == VM820B15A)
	// send I2S startup command to co-processor
    EVE_LIB_BeginCoProList();
    EVE_CMD_I2SSTARTUP(44100);
    EVE_LIB_EndCoProList();
    EVE_LIB_AwaitCoProEmpty();
	
	// write I2S control register, and set L/R volume
	EVE_LIB_MemWrite32(EVE_REG_I2S_CTL, 0x0A);  
    EVE_LIB_MemWrite32(EVE_REG_VOL_L_PB, 255);
    EVE_LIB_MemWrite32(EVE_REG_VOL_R_PB, 255);
	#endif

    // Turn synthesizer volume up
	EVE_LIB_MemWrite32(EVE_REG_VOL_SOUND, 255);
	// Set synthesizer to mute
	EVE_LIB_MemWrite32(EVE_REG_SOUND, SOUND_MUTE);
	// Play sound
	EVE_LIB_MemWrite32(EVE_REG_PLAY, 1);
#endif
}

int checkRegPlay(void)
{
#if IS_EVE_API(1,2,3,4)
	uint8_t play; 
	// play sound
	play = EVE_LIB_MemRead8(EVE_REG_PLAY);
#else
	uint32_t play;
	// play sound
	play = EVE_LIB_MemRead32(EVE_REG_PLAY);
#endif

return play;
} 

void playSound(uint8_t sound, uint8_t note)
{
#if IS_EVE_API(1,2,3,4)
	// set synthesizer to note
	EVE_LIB_MemWrite16(EVE_REG_SOUND, (note << 8) | sound);
	// play sound
	EVE_LIB_MemWrite8(EVE_REG_PLAY, 1);
#else
	// set synthesizer to note
	EVE_LIB_MemWrite32(EVE_REG_SOUND, (note << 8) | sound);
	// play sound
	EVE_LIB_MemWrite32(EVE_REG_PLAY, 1);
#endif
} 

void playMute(void)
{
#if IS_EVE_API(1,2,3,4)
	// set synthesizer to note
	EVE_LIB_MemWrite16(EVE_REG_SOUND, SOUND_MUTE);
	// play sound
	EVE_LIB_MemWrite8(EVE_REG_PLAY, 1);
#else
	// set synthesizer to note
	EVE_LIB_MemWrite32(EVE_REG_SOUND, SOUND_MUTE);
	// play sound
	EVE_LIB_MemWrite32(EVE_REG_PLAY, 1);
#endif
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