/**
 * CLOCK Generated Driver Source File
 * 
 * @file clock.c
 * 
 * @ingroup clockdriver 
 * 
 * @brief This file contains the API implementation for the Clock driver.
 *
 * @version Driver Version 1.0.1
 *
 * @version Package Version 1.0.5 
*/

/*
© [2025] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/

#include <xc.h>
#include "../clock.h"

void CLOCK_Initialize(void)
{
    OSCCON = (0 << _OSCCON_SCS_POSN)   // SCS FOSC
        | (7 << _OSCCON_IRCF_POSN)   // IRCF 16MHz_HFINTOSC
        | (0 << _OSCCON_IDLEN_POSN);  // IDLEN disabled
    OSCCON2 = (1 << _OSCCON2_PRISD_POSN)   // PRISD enabled
        | (0 << _OSCCON2_SOSCGO_POSN)   // SOSCGO disabled
        | (0 << _OSCCON2_MFIOSEL_POSN);  // MFIOSEL disabled
    OSCTUNE = (0 << _OSCTUNE_TUN_POSN)   // TUN 0x0
        | (0 << _OSCTUNE_PLLEN_POSN)   // PLLEN disabled
        | (0 << _OSCTUNE_INTSRC_POSN);  // INTSRC INTRC
}
/**
 End of File
*/