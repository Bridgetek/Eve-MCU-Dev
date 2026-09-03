/**
 * @file EVE_MCU_MSPM0.c
 * @details MCU-specific code for controlling EVE on MSPM0 devices.
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

// Guard against being used for incorrect platform or architecture.
#if defined(PLATFORM_MSPM0)

#pragma message "Compiling " __FILE__ " for TI MSPM0"

/* EVE MCU HEADER */

/* Replace with header file for target MCU */
#include "ti_msp_dl_config.h"
#include <stdint.h> // for Uint8/16/32 and Int8/16/32 data types

/* Include functions for EVE-MCU-Dev library API layer */
#include <EVE.h> 
/* Include functions for EVE-MCU-Dev library MCU layer */
#include <MCU.h>

#define Nop() __no_operation()
#define MISO            GPIO_SPI_0_POCI_PORT->GPIO_SPI_0_POCI_PIN  // SPI MISO
#define MOSI            GPIO_SPI_0_PICO_PORT->GPIO_SPI_0_PICO_PIN  // SPI MOSI
#define PD              GPIO_GRP_0_PORT->GPIO_GRP_0_PD_PIN         // Power Down
#define SCLK            GPIO_SPI_0_SCLK_PORT->GPIO_SPI_0_SCLK_PIN  // SPI clock
#define CS              GPIO_GRP_0_PORT->GPIO_GRP_0_CS_PIN         // Chip Select
#define CPU_FREQ        CPUCLK_FREQ                // for use in delay function

/* EVE MCU HEADER END */

/* EVE MCU */

// This is the MSPM0 platform specific section and contains the functions which
// enable the GPIO and SPI interfaces.

/* configure MCU, SPI and PD pins */
int MCU_Init(void)
{
    /* set pins to be used for CS# and PD# */
    DL_GPIO_setPins(GPIO_GRP_0_PORT, GPIO_GRP_0_CS_PIN | GPIO_GRP_0_PD_PIN);
    /* setup tick timer */
    DL_SYSTICK_init(CPUCLK_FREQ / 1000U);

    /* SPI has higher priority than the millisecond timer. */
    NVIC_SetPriority(SPI_0_INST_INT_IRQN, 0U);
    NVIC_SetPriority(SysTick_IRQn, 1U);

    /* Enable interrupt */
    DL_SYSTICK_enableInterrupt();
    /* Enable counter */
    DL_SYSTICK_enable();
    return 0;
}

int MCU_Deinit(void)
{
    /* Leave EVE in a safe state. */
    MCU_CShigh();
    MCU_PDlow();

    /* Wait for any current SPI transaction to complete. */
    while (DL_SPI_isBusy(SPI_0_INST))
    {
    }

    /* Disable SPI interrupt sources. */
    DL_SPI_disableInterrupt(
        SPI_0_INST,
        DL_SPI_INTERRUPT_RX | DL_SPI_INTERRUPT_TX);

    /* Disable the SPI interrupt at the NVIC. */
    NVIC_DisableIRQ(SPI_0_INST_INT_IRQN);

    /* Clear any pending NVIC interrupt. */
    NVIC_ClearPendingIRQ(SPI_0_INST_INT_IRQN);

    /* Clear pending SPI interrupt status. */
    DL_SPI_clearInterruptStatus(
        SPI_0_INST,
        DL_SPI_INTERRUPT_RX | DL_SPI_INTERRUPT_TX);

    /* Reset the software SPI state machine. */
    gControllerMode = IDLE_MODE;

    /* Disable the SPI peripheral. */
    DL_SPI_disable(SPI_0_INST);

    return 0;
}

int MCU_Setup(void)
{
    /* QSPI Configuration */
#ifdef EVE_QSPI_ENABLE
#error EVE_QSPI_ENABLE (QSPI interfaces to EVE) is currently not supported on MSPM0
#endif // EVE_QSPI_ENABLE

    /* Additional SPI Configuration */
    return 0;
}

/* State machine to keep track of the current SPI Controller mode */
typedef enum SPI_ControllerModeEnum {
    IDLE_MODE,
    WRITE_DATA_MODE,
    READ_DATA_MODE,
    TIMEOUT_MODE
} SPI_Controller_Mode;

/* Global used to receive data in the ISR */
uint8_t gRxVal;
/* Global used to transmit data in the ISR */
uint8_t gTxVal;

/* Used to track the state of the software state machine */
volatile SPI_Controller_Mode gControllerMode = IDLE_MODE;

/* Dummy data sent when receiving data from SPI Peripheral */
#define DUMMY_DATA (0xFF)

void SPI_0_INST_IRQHandler(void)
{
    switch (DL_SPI_getPendingInterrupt(SPI_0_INST)) {
        case DL_SPI_IIDX_TX:
            switch (gControllerMode) {
                case IDLE_MODE:
                case TIMEOUT_MODE:
                    break;
                case READ_DATA_MODE:
                    /*  Send dummy data to get read more bytes */
                    DL_SPI_transmitData8(SPI_0_INST, DUMMY_DATA);
                    break;
                case WRITE_DATA_MODE:
                    /* Transmit data until all expected data is sent */
                    DL_SPI_transmitData8(SPI_0_INST, gTxVal);
                    gControllerMode = IDLE_MODE;
                    break;
            }
            break;
        case DL_SPI_IIDX_RX:
            switch (gControllerMode) {
                case IDLE_MODE:
                case TIMEOUT_MODE:
                    break;
                case READ_DATA_MODE:
                    gRxVal = DL_SPI_receiveData8(SPI_0_INST);
                    /* All data is received, reset state machine */
                    gControllerMode = IDLE_MODE;
                    break;
                case WRITE_DATA_MODE:
                    /* Ignore the data while transmitting */
                    DL_SPI_receiveData8(SPI_0_INST);
                    break;
            }
            break;
        default:
            break;
    }
}

// --------------------- SPI Read/Write 8 bits ----------------------------------
uint8_t MCU_SPIReadWrite8(uint8_t DataToWrite)
{
    gControllerMode = WRITE_DATA_MODE;

    /*
     * TX interrupts are disabled and RX interrupts are enabled by default.
     * TX interrupts will be enabled after sending the command, and they will
     * trigger after the FIFO has more space to send all subsequent bytes.
     */
    DL_SPI_clearInterruptStatus(SPI_0_INST, DL_SPI_INTERRUPT_TX);
    DL_SPI_transmitData8(SPI_0_INST, DataToWrite);
    DL_SPI_enableInterrupt(SPI_0_INST, DL_SPI_INTERRUPT_TX);

    /* Go to sleep until all data is received */
    while (gControllerMode != IDLE_MODE) {
        __WFI();
    }

    /* Disable TX interrupts after the command is complete */
    DL_SPI_disableInterrupt(SPI_0_INST, DL_SPI_INTERRUPT_TX);
    DL_SPI_clearInterruptStatus(SPI_0_INST, DL_SPI_INTERRUPT_TX);

    return gRxVal;
}

// --------------------- SPI Read/Write 16 bits ----------------------------------
uint16_t MCU_SPIReadWrite16(uint16_t DataToWrite)
{
    uint16_t DataRead = 0;
    DataRead = MCU_SPIReadWrite8((DataToWrite) >> 8) << 8;
    DataRead |= MCU_SPIReadWrite8((DataToWrite) & 0xff);

    return DataRead;
}

// --------------------- SPI Read/Write 24 bits ----------------------------------
uint32_t MCU_SPIReadWrite24(uint32_t DataToWrite)
{
    uint32_t DataRead = 0;
    uint32_t temp;

    temp = (MCU_SPIReadWrite8((DataToWrite) >> 24));
    DataRead |= (temp<<24);
    temp = (MCU_SPIReadWrite8((DataToWrite) >> 16));
    DataRead |= (temp<<16);
    temp = (MCU_SPIReadWrite8((DataToWrite) >> 8));
    DataRead |= (temp<<8);

    return DataRead;
}

// --------------------- SPI Read/Write 32 bits ----------------------------------
uint32_t MCU_SPIReadWrite32(uint32_t DataToWrite)
{
    uint32_t DataRead = 0;
    uint32_t temp;

    temp = (MCU_SPIReadWrite8((DataToWrite) >> 24));
    DataRead |= (temp << 24);
    temp = (MCU_SPIReadWrite8((DataToWrite) >> 16));
    DataRead |= (temp << 16);
    DataRead |= (MCU_SPIReadWrite8((DataToWrite) >> 8) << 8);
    DataRead |= (MCU_SPIReadWrite8(DataToWrite) & 0xff);

    return DataRead;
}

// --------------------- SPI Read 8 bits ----------------------------------
uint8_t MCU_SPIRead8(void)
{
    uint8_t DataRead = 0;

    DataRead = MCU_SPIReadWrite8(0);

    return DataRead;
}

// --------------------- SPI Write 8 bits ----------------------------------
void MCU_SPIWrite8(uint8_t DataToWrite)
{
    MCU_SPIReadWrite8(DataToWrite);
}

// --------------------- SPI Read 16 bits ----------------------------------
uint16_t MCU_SPIRead16(void)
{
    uint16_t DataRead = 0;

    DataRead = MCU_SPIReadWrite16(0);

    return DataRead;
}

// --------------------- SPI Write 16 bits ----------------------------------
void MCU_SPIWrite16(uint16_t DataToWrite)
{
    MCU_SPIReadWrite16(DataToWrite);
}

// --------------------- SPI Write 24 bits ----------------------------------
void MCU_SPIWrite24(uint32_t DataToWrite)
{
    MCU_SPIReadWrite24(DataToWrite);
}

uint32_t MCU_SPIRead32(void)
{
    uint32_t DataRead = 0;

    DataRead = MCU_SPIReadWrite32(0);

    return DataRead;
}

// --------------------- SPI Read/Write 32 bits ----------------------------------
void MCU_SPIWrite32(uint32_t DataToWrite)
{
    MCU_SPIReadWrite32(DataToWrite);
}

// --------------------- SPI burst write ----------------------------------
void MCU_SPIWrite(const uint8_t *DataToWrite, uint32_t length)
{
    uint16_t DataPointer = 0;

    while(DataPointer < length)
    {
        MCU_SPIWrite8(DataToWrite[DataPointer]);  // Send data byte-by-byte from array
        DataPointer ++;
    }
}

// --------------------- SPI burst read -----------------------------------
void MCU_SPIRead(uint8_t *DataToRead, uint32_t length)
{
    uint16_t DataPointer = 0;

    while(DataPointer < length)
    {
        DataToRead[DataPointer] = MCU_SPIRead8();  // Receive data byte-by-byte to array
        DataPointer ++;
    }
}

// ########################### GPIO CONTROL ####################################

// --------------------- Chip Select line low ----------------------------------
inline void MCU_CSlow(void)
{
    DL_GPIO_clearPins(GPIO_GRP_0_PORT, GPIO_GRP_0_CS_PIN); // CS# line low
    delay_cycles(10);
}

// --------------------- Chip Select line high ---------------------------------
inline void MCU_CShigh(void)
{
    delay_cycles(10);
    DL_GPIO_setPins(GPIO_GRP_0_PORT, GPIO_GRP_0_CS_PIN);  // CS# line high
}

// -------------------------- PD line low --------------------------------------
inline void MCU_PDlow(void)
{
    DL_GPIO_clearPins(GPIO_GRP_0_PORT, GPIO_GRP_0_PD_PIN);  // PD# line low
}

// ------------------------- PD line high --------------------------------------
inline void MCU_PDhigh(void)
{
    DL_GPIO_setPins(GPIO_GRP_0_PORT, GPIO_GRP_0_PD_PIN);  // PD# line high
}

// ------------------------ interrupt input ------------------------------------
int MCU_Int(void) 
{
#if !defined(EVE_USE_CMDB_METHOD) && defined(EVE_USE_INTERRUPT_METHOD)
#error EVE_USE_INTERRUPT_METHOD (EVE Interrupt pin) is not supported on MSPM0
#endif
    return 1;
}

// ------------------- msec delay based on MCLK (CPU_FREQ) ----------------------
void delay(int msec){
    //could use a timer here
    while(msec)
    {
        delay_cycles(CPU_FREQ);
        msec--;
    }
}
// ----------------------------- 20ms delay --------------------------------------
void MCU_Delay_20ms(void)
{
    delay(20);
}

// ----------------------------- 500ms delay -------------------------------------
void MCU_Delay_500ms(void)
{
    delay(500);
}

// --------------------------- msec delay based on timer -------------------------
uint32_t MCU_Time_ms(void)
{
    return DL_SYSTICK_getValue();
}

// ########################### ENDIAN CONVERSION ####################################

uint32_t bswap32(uint32_t x)
{
    uint32_t s;
    s = ((x) >> 24);
    s |= (((x) & 0x00FF0000) >> 8);
    s |= (((x) & 0x0000FF00) << 8);
    s |= ((x) << 24);

    return s;
}

uint16_t bswap16(uint16_t x)
{
    uint16_t s;
    s = ((x) >> 8);
    s |= ((x) << 8);

    return s;
}

uint16_t MCU_htobe16 (uint16_t h)
{
    return h;
}

uint32_t MCU_htobe32 (uint32_t h)
{
    return h;
}

uint16_t MCU_htole16 (uint16_t h)
{

    return bswap16(h);
}

uint32_t MCU_htole32 (uint32_t h)
{

    return bswap32(h);
}

uint16_t MCU_be16toh (uint16_t h)
{
    return h;
}
uint32_t MCU_be32toh (uint32_t h)
{
    return h;
}

uint16_t MCU_le16toh (uint16_t h)
{

    return bswap16(h);
}

uint32_t MCU_le32toh (uint32_t h)
{
    return bswap32(h);

}

/* EVE MCU END */

#endif /* defined(PLATFORM_MSPM0) */
