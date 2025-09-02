/**
 * SPI1 Generated Driver File
 *
 * @file mssp1.c
 *
 * @ingroup spi1
 *
 * @brief This file contains the driver code for the SPI1 module.
 *
 * @version SPI1 Driver Version v5.0.0
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
#include "../mssp1.h"
#include "../spi_polling_types.h"

const struct SPI_INTERFACE SPI1_Host = {
    .Initialize = SPI1_Initialize,
    .Deinitialize = SPI1_Deinitialize,
    .Open = SPI1_Open,
    .Close = SPI1_Close,
    .BufferExchange = SPI1_BufferExchange,
    .BufferWrite = SPI1_BufferWrite,
    .BufferRead = SPI1_BufferRead,
    .ByteExchange = SPI1_ByteExchange,
    .ByteWrite = SPI1_ByteWrite,
    .ByteRead = SPI1_ByteRead,
    .IsRxReady = SPI1_IsRxReady,
    .IsTxReady = SPI1_IsTxReady,
    .RxCompleteCallbackRegister = NULL,
    .TxCompleteCallbackRegister = NULL
};

static const spi_configuration_t spi1_configuration[] = {
    { 0x40, 0xa, 0x10, 0x1f },
    { 0x64, 0x0, 0x10, 0x1 }
};

void SPI1_Initialize(void)
{
    // Return mssp1 registers to reset state
    PIE1bits.SSP1IE  = 0;
    PIR1bits.SSP1IF    = 0;

    SSP1STAT = 0x00;
    SSP1CON1 = 0x00;
    SSP1CON3 = 0x00;
    SSP1ADD  = 0x00;
}

void SPI1_Deinitialize(void)
{
    // Return mssp1 registers to reset state
    SSP1STAT = 0x00;
    SSP1CON1 = 0x00;
    SSP1CON3 = 0x00;
    SSP1ADD  = 0x00;
}

bool SPI1_Open(uint8_t spiConfigIndex)
{
    bool returnValue = false;
    if (SSP1CON1bits.SSPEN == false)
    {
        SSP1STAT = spi1_configuration[spiConfigIndex].stat;
        SSP1CON1 = spi1_configuration[spiConfigIndex].con1;
        SSP1CON3 = spi1_configuration[spiConfigIndex].con3;
        SSP1ADD  = spi1_configuration[spiConfigIndex].baud;

        SSP1CON1bits.SSPEN = 1;

        returnValue = true;
    }
    else
    {
        returnValue = false;
    }
    return returnValue;
}

void SPI1_Close(void)
{
    SSP1CON1bits.SSPEN = 0;
}

void SPI1_BufferExchange(void *bufferData, size_t bufferSize)
{
    uint8_t *bufferInput = bufferData;
    size_t bufferInputSize = bufferSize;
    while (0U != bufferInputSize)
    {
        SSP1BUF = *bufferInput;
        while (!PIR1bits.SSP1IF)
        {
            // Wait for flag to get set
        }
        PIR1bits.SSP1IF = 0;
        *bufferInput = SSP1BUF;
        bufferInput++;
        bufferInputSize--;
    }
}

void SPI1_BufferWrite(void *bufferData, size_t bufferSize)
{
    uint8_t *bufferInput = bufferData;
    size_t bufferInputSize = bufferSize;
    while (0U != bufferInputSize)
    {
        SSP1BUF = *bufferInput;
        while (!PIR1bits.SSP1IF)
        {
            // Wait for flag to get set
        }
        PIR1bits.SSP1IF = 0;
        bufferInput++;
        bufferInputSize--;
    }
}

void SPI1_BufferRead(void *bufferData, size_t bufferSize)
{
    uint8_t *bufferInput = bufferData;
    size_t bufferInputSize = bufferSize;
    while (0U != bufferInputSize)
    {
        SSP1BUF = 0x00;
        while (!PIR1bits.SSP1IF)
        {
            // Wait for flag to get set
        }
        PIR1bits.SSP1IF = 0;
        *bufferInput = SSP1BUF;
        bufferInput++;
        bufferInputSize--;
    }
}

uint8_t SPI1_ByteExchange(uint8_t byteData)
{
    SSP1BUF = byteData;
    while (!PIR1bits.SSP1IF)
    {
        // Wait for flag to get set
    }
    PIR1bits.SSP1IF = 0;
    return SSP1BUF;
}

void SPI1_ByteWrite(uint8_t byteData)
{
    SSP1BUF = byteData;
}

uint8_t SPI1_ByteRead(void)
{
    if (1U == PIR1bits.SSP1IF)
    {
        PIR1bits.SSP1IF = 0;
    }
    return SSP1BUF;
}

bool SPI1_IsRxReady(void)
{
    bool returnValue = false;
    if (SSP1CON1bits.SSPEN == 1)
    {
        returnValue = ((PIR1bits.SSP1IF != 0U) ? true: false);
    }
    else
    {
        returnValue = false;
    }
    return returnValue;
}

bool SPI1_IsTxReady(void)
{
    bool returnValue = false;
    if (SSP1CON1bits.SSPEN == 1)
    {
        returnValue = ((PIR1bits.SSP1IF != 0U) ? false: true);
    }
    else
    {
        returnValue = false;
    }
    return returnValue;
}