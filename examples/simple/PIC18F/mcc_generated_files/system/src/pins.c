/**
 * Generated Driver File
 * 
 * @file pins.c
 * 
 * @ingroup  pinsdriver
 * 
 * @brief This is generated driver implementation for pins. 
 *        This file provides implementations for pin APIs for all pins selected in the GUI.
 *
 * @version Driver Version 3.1.1
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

#include "../pins.h"


void PIN_MANAGER_Initialize(void)
{
   /**
    LATx registers
    */
    LATA = 0x0;
    LATB = 0x0;
    LATC = 0x0;
    LATD = 0x0;
    LATE = 0x0;
    /**
    ODx registers
    */

    /**
    TRISx registers
    */
    TRISA = 0xFF;
    TRISB = 0xFF;
    TRISC = 0x17;
    TRISD = 0xFF;
    TRISE = 0x87;

    /**
    ANSELx registers
    */
    ANSELA = 0x2F;
    ANSELB = 0x3F;
    ANSELC = 0x4;
    ANSELD = 0xFF;
    ANSELE = 0x7;

    /**
    WPUx registers
    */
    WPUB = 0xFF;


    /**
    SLRCONx registers
    */

    /**
    INLVLx registers
    */

   /**
    RxyI2C | RxyFEAT registers   
    */
    /**
    PPS registers
    */

   /**
    IOCx registers 
    */
    IOCB = 0xF0;


    // Enable INTCONbits.RBIE interrupt 
    INTCONbits.RBIE = 1; 
}
  
void PIN_MANAGER_IOC(void)
{
}
/**
 End of File
*/