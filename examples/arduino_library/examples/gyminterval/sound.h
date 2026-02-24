/**
 @file sound.h
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

#ifndef EVE_SOUNDS_H
#define EVE_SOUNDS_H

#define SOUND_SILENCE 0x00 // Silence Y N        
#define SOUND_SQUARE 0x01 // Square wave Y Y    
#define SOUND_SINE 0x02 // Sine wave Y Y      
#define SOUND_SAW 0x03 // Sawtooth wave Y Y  
#define SOUND_TRIANGLE 0x04 // Triangle wave Y Y  
#define SOUND_BEEPING 0x05 // Beeping Y Y        
#define SOUND_ALARM 0x06 // Alarm Y Y          
#define SOUND_WARBLE 0x07 // Warble Y Y         
#define SOUND_CAROUSEL 0x08 // Carousel Y Y       
#define SOUND_1PIP 0x10 // 1 short pips N Y   
#define SOUND_2PIP 0x11 // 2 short pips N Y   
#define SOUND_3PIP 0x12 // 3 short pips N Y   
#define SOUND_4PIP 0x13 // 4 short pips N Y   
#define SOUND_5PIP 0x14 // 5 short pips N Y   
#define SOUND_6PIP 0x15 // 6 short pips N Y   
#define SOUND_7PIP 0x16 // 7 short pips N Y   
#define SOUND_8PIP 0x17 // 8 short pips N Y   
#define SOUND_9PIP 0x18 // 9 short pips N Y   
#define SOUND_10PIP 0x19 // 10 short pips N Y  
#define SOUND_11PIP 0x1A // 11 short pips N Y  
#define SOUND_12PIP 0x1B // 12 short pips N Y  
#define SOUND_13PIP 0x1C // 13 short pips N Y  
#define SOUND_14PIP 0x1D // 14 short pips N Y  
#define SOUND_15PIP 0x1E // 15 short pips N Y  
#define SOUND_16PIP 0x1F // 16 short pips N Y  
#define SOUND_DTMFH 0x23 // DTMF # Y N         
#define SOUND_DTMFS 0x2C // DTMF * Y N         
#define SOUND_DTMF0 0x30 // DTMF 0 Y N         
#define SOUND_DTMF1 0x31 // DTMF 1 Y N         
#define SOUND_DTMF2 0x32 // DTMF 2 Y N
#define SOUND_DTMF3 0x33 // DTMF 3 Y N
#define SOUND_DTMF4 0x34 // DTMF 4 Y N
#define SOUND_DTMF5 0x35 // DTMF 5 Y N
#define SOUND_DTMF6 0x36 // DTMF 6 Y N
#define SOUND_DTMF7 0x37 // DTMF 7 Y N
#define SOUND_DTMF8 0x38 // DTMF 8 Y N
#define SOUND_DTMF9 0x39 // DTMF 9 Y N
#define SOUND_HARP 0x40 // Harp N Y
#define SOUND_XYLO 0x41 // Xylophone N Y
#define SOUND_TUBE 0x42 // Tuba N Y
#define SOUND_GLOCK 0x43 // Glockenspiel N Y
#define SOUND_ORGAN 0x44 // Organ N Y
#define SOUND_TRUMPET 0x45 // Trumpet N Y
#define SOUND_PIANO 0x46 // Piano N Y
#define SOUND_CHIMES 0x47 // Chimes N Y
#define SOUND_MUSICBOX 0x48 // Music box N Y
#define SOUND_BELL 0x49 // Bell N Y
#define SOUND_CLICK 0x50 // Click N N
#define SOUND_SWITCH 0x51 // Switch N N
#define SOUND_COWBELL 0x52 // Cowbell N N
#define SOUND_NOTCH 0x53 // Notch N N
#define SOUND_HIHAT 0x54 // Hihat N N
#define SOUND_KICK 0x55 // Kickdrum N N
#define SOUND_POP 0x56 // Pop N N
#define SOUND_CLACK 0x57 // Clack N N
#define SOUND_CHACK 0x58 // Chack N N
#define SOUND_MUTE 0x60 // Mute N N
#define SOUND_UNMUTE 0x61 // unmute N N

// MIDI note : ANSI note Freq (Hz)
#define NOTE_A0 21 // A0 27.5      
#define NOTE_A0S 22 // A#0 29.1     
#define NOTE_B0 23 // B0 30.9      
#define NOTE_C1 24 // C1 32.7      
#define NOTE_C1S 25 // C#1 34.6     
#define NOTE_D1 26 // D1 36.7      
#define NOTE_D1S 27 // D#1 38.9     
#define NOTE_E1 28 // E1 41.2      
#define NOTE_F1 29 // F1 43.7      
#define NOTE_F1S 30 // F#1 46.2     
#define NOTE_G1 31 // G1 49.0      
#define NOTE_G1S 32 // G#1 51.9     
#define NOTE_A1 33 // A1 55.0      
#define NOTE_A1S 34 // A#1 58.3     
#define NOTE_B1 35 // B1 61.7      
#define NOTE_C2 36 // C2 65.4      
#define NOTE_C2S 37 // C#2 69.3     
#define NOTE_D2 38 // D2 73.4      
#define NOTE_D2S 39 // D#2 77.8     
#define NOTE_E2 40 // E2 82.4      
#define NOTE_F2 41 // F2 87.3      
#define NOTE_F2S 42 // F#2 92.5     
#define NOTE_G2 43 // G2 98.0      
#define NOTE_G2S 44 // G#2 103.8    
#define NOTE_A2 45 // A2 110.0     
#define NOTE_A2S 46 // A#2 116.5    
#define NOTE_B2 47 // B2 123.5     
#define NOTE_C3 48 // C3 130.8     
#define NOTE_C3S 49 // C#3 138.6    
#define NOTE_D3 50 // D3 146.8     
#define NOTE_D3S 51 // D#3 155.6    
#define NOTE_E3 52 // E3 164.8     
#define NOTE_F3 53 // F3 174.6     
#define NOTE_F3S 54 // F#3 185.0    
#define NOTE_G3 55 // G3 196.0     
#define NOTE_G3S 56 // G#3 207.7    
#define NOTE_A3 57 // A3 220.0     
#define NOTE_A3S 58 // A#3 233.1    
#define NOTE_B3 59 // B3 246.9     
#define NOTE_C4 60 // C4 261.6     
#define NOTE_C4S 61 // C#4 277.2    
#define NOTE_D4 62 // D4 293.7     
#define NOTE_D4S 63 // D#4 311.1    
#define NOTE_E4 64 // E4 329.6     
#define NOTE_F4 65 // F4 349.2
#define NOTE_F4S 66 // F#4 370.0
#define NOTE_G4 67 // G4 392.0
#define NOTE_G4S 68 // G#4 415.3
#define NOTE_A4 69 // A4 440.0
#define NOTE_A4S 70 // A#4 466.2
#define NOTE_B4 71 // B4 493.9
#define NOTE_C5 72 // C5 523.3
#define NOTE_C5S 73 // C#5 554.4
#define NOTE_D5 74 // D5 587.3
#define NOTE_D5S 75 // D#5 622.3
#define NOTE_E5 76 // E5 659.3
#define NOTE_F5 77 // F5 698.5
#define NOTE_F5S 78 // F#5 740.0
#define NOTE_G5 79 // G5 784.0
#define NOTE_G5S 80 // G#5 830.6
#define NOTE_A5 81 // A5 880.0
#define NOTE_A5S 82 // A#5 932.3
#define NOTE_B5 83 // B5 987.8
#define NOTE_C6 84 // C6 1046.5
#define NOTE_C6S 85 // C#6 1108.7
#define NOTE_D6 86 // D6 1174.7
#define NOTE_D6S 87 // D#6 1244.5
#define NOTE_E6 88 // E6 1318.5
#define NOTE_F6 89 // F6 1396.9
#define NOTE_F6S 90 // F#6 1480.
#define NOTE_G6 91 // G6 1568.0
#define NOTE_G6S 92 // G#6 1661.2
#define NOTE_A6 93 // A6 1760.0
#define NOTE_A6S 94 // A#6 1864.7
#define NOTE_B6 95 // B6 1975.5
#define NOTE_C7 96 // C7 2093.0
#define NOTE_C7S 97 // C#7 2217.5
#define NOTE_D7 98 // D7 2349.3
#define NOTE_D7S 99 // D#7 2489.0
#define NOTE_E7 100 // E7 2637.0
#define NOTE_F7 101 // F7 2793.8
#define NOTE_F7S 102 // F#7 2960.0
#define NOTE_G7 103 // G7 3136.0
#define NOTE_G7S 104 // G#7 3322.4
#define NOTE_A7 105 // A7 3520.0
#define NOTE_A7S 106 // A#7 3729.3
#define NOTE_B7 107 // B7 3951.1
#define NOTE_C8 108 // C8 4186.0

void enableSound(void);
void playClick(void);
void playChimes(uint8_t note);
void playBell(uint8_t note);
void playPip(uint8_t note);
void playClack(void);

#endif // EVE_SOUNDS_H
