/**
 * @file EVE_MCU_MSP430.c
 * @details MCU-specific code for controlling EVE on MSP430 devices.
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
#if defined(PLATFORM_MSP430)

/* EVE MCU HEADER */

#pragma diag_push
#pragma diag_remark 1181
#warning "Compiling " __FILE__ " for TI MSP430"
#pragma diag_pop

/* Replace with header file for target MCU */
#include <msp430g2553.h>

#include <stdint.h> // for Uint8/16/32 and Int8/16/32 data types

/* Include functions for EVE-MCU-Dev library API layer */
#include <EVE.h> 
/* Include functions for EVE-MCU-Dev library MCU layer */
#include <MCU.h>

#define MISO            BIT1                // P1.1 is SPI MISO
#define MOSI            BIT2                // P1.2 is SPI MOSI
#define PD              BIT3                // P1.3 is Power Down, active low
#define SCLK            BIT4                // P1.4 is SPI clock
#define CS              BIT5                // P1.5 is Chip Select, active lwo
#define INT             BIT6                // P1.7 is EVE interrupt, active low

#define DCO_FREQUENCY_HZ 8000000UL

/* EVE MCU HEADER END */

/* EVE MCU */

// This is the MSP430 platform specific section and contains the functions which
// enable the GPIO and SPI interfaces.

static void initClock(void)
{
    /* Select the lowest DCO and modulation settings first. */
    DCOCTL = 0U;

    /* Configure the DCO for its calibrated 8 MHz frequency. */
    BCSCTL1 = CALBC1_8MHZ;
    DCOCTL = CALDCO_8MHZ;

    /*
     * MCLK  = DCOCLK / 1 = 8 MHz
     * SMCLK = DCOCLK / 1 = 8 MHz
     */
    BCSCTL2 = 0U;
}

void initGpio(){

    /*
     * Configure INT (P1.6) as an active-low GPIO input
     * with the internal pull-up resistor enabled.
     */
    P1SEL  &= ~INT;
    P1SEL2 &= ~INT;
    P1DIR  &= ~INT;
    P1REN  |= INT;
    P1OUT  |= INT;   // Select pull-up resistor

    /*
     * Configure CS (P1.5) and PD (P1.3) as active-low
     * GPIO outputs, initially inactive (HIGH).
     *
     * Set P1OUT before P1DIR to avoid briefly driving
     * either signal LOW during initialisation.
     */
    P1SEL  &= ~(CS | PD);
    P1SEL2 &= ~(CS | PD);

    P1OUT |= (CS | PD);  // Inactive HIGH
    P1DIR |= (CS | PD);  // Configure as outputs

}

void initSPI()
{

    /* set SPI pins in both P1SEL and P1SEL2 as to work with Universal Serial Communication Interface*/
    P1SEL = MISO | MOSI | SCLK;             // p1.1 MISO, p1.2 MOSI, P1.4 SCLK
    P1SEL2 = MISO | MOSI | SCLK;

    /* Set SPI clock speed to 1 MHz - See the notes for MCU_SPI_TIMEOUT in the MCU.h file. */

    /* configure UCA0 for SPI */;
    UCA0CTL0 = UCCKPH | UCMSB | UCMST | UCSYNC;     // 3-pin, 8-bit SPI master (mode 0)
    UCA0CTL1 |= UCSSEL_2;                   // USCI source = 8 MHz SMCLK
    UCA0BR0 = 8U;                           // divide SMCLK by 8 in baud rate control register
    UCA0BR1 = 0U;                           // = 1 mhz spi
    UCA0MCTL = 0U;                          // No modulation
    UCA0CTL1 &= ~UCSWRST;                   // **Initialise USCI state machine** - restart module

    IE2 |= UCA0RXIE;                        // Enable USCI0 RX 
    
    UCA0CTL1 &= ~UCSWRST;
}

void initTimer()
{
    /* Stop and clear Timer_A before configuring it. */
    TACTL = MC_0 | TACLR;

    /* Disable and clear the CCR0 control register. */
    TACCTL0 = 0U;

    /* SMCLK = 8 MHz, Timer_A divider = 8, Timer_A clock = 1 MHz
     * 1 ms requires 1000 timer counts. Up mode counts from 0 through TACCR0, inclusive. */
    TACCR0 = 999U;

    /* Enable the Timer_A CCR0 interrupt. */
    TACCTL0 = CCIE;

    /*  TASSEL_2 = SMCLK, ID_3     = divide by 8, MC_1 = up mode */
    TACTL = TASSEL_2 | ID_3 | MC_1 | TACLR;
}

/* configure MCU, SPI and PD pins */
int MCU_Init(void){

    WDTCTL = WDTPW | WDTHOLD;               // stop watch dog timer

    initClock();                            // set Clocks
    initGpio();                             // vonfigure GPIO (CS#,PD#,INT#)
    initSPI();                              // configure SPI
    initTimer();                            // configure timer

    __enable_interrupt();                   // enable interrupts

    return 0;
}

int MCU_Deinit(void)
{
    /* Deselect EVE and assert power-down. */
    P1OUT |= CS;
    P1OUT &= ~PD;

    /* Disable SPI interrupts and hold USCI A0 in reset. */
    IE2 &= ~(UCA0RXIE | UCA0TXIE);
    UCA0CTL1 |= UCSWRST;

    /* Return SPI pins to GPIO inputs. */
    P1SEL  &= ~(MISO | MOSI | SCLK);
    P1SEL2 &= ~(MISO | MOSI | SCLK);
    P1DIR  &= ~(MISO | MOSI | SCLK);

    /* Leave INT as a pulled-up GPIO input. */
    P1SEL  &= ~INT;
    P1SEL2 &= ~INT;
    P1DIR  &= ~INT;
    P1REN  |= INT;
    P1OUT  |= INT;
}

int MCU_Setup(void)
{
    /* QSPI Configuration */
#ifdef EVE_QSPI_ENABLE
#error EVE_QSPI_ENABLE (QSPI interfaces to EVE) is currently not supported on MSP430
#endif // EVE_QSPI_ENABLE

    /* Additional SPI Configuration */

    return 0;
}


// ########################### SPI Send and Receive ####################################
// ----------------- Global variables for SPI data ---------------------
static volatile uint8_t DataRead;
static volatile uint8_t SpiRxComplete;

// ----------------------- SPI RX ISR ----------------------------------

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI_SPIRX_ISR(void)
{
    if ((IFG2 & UCA0RXIFG) != 0U)
    {
        /*
         * Reading UCA0RXBUF clears UCA0RXIFG.
         */
        DataRead = UCA0RXBUF;
        SpiRxComplete = 1U;
    }
}

// ---------------------- SPI Read/Write 8 bits -------------------------
uint8_t MCU_SPIReadWrite8(uint8_t DataToWrite)
{
    uint8_t timerWasEnabled;

    timerWasEnabled = ((TACCTL0 & CCIE) != 0U);

    /*
     * Defer Timer_A0 until this SPI byte has been received.
     * Do not disable global interrupts because the SPI ISR
     * still needs to execute.
     */
    TACCTL0 &= ~CCIE;

    SpiRxComplete = 0U;

    while ((IFG2 & UCA0TXIFG) == 0U)
    {
    }

    UCA0TXBUF = DataToWrite;

    /*
     * UCA0RXIFG is set by the hardware when the transfer
     * completes. Do not set it manually.
     */
    while (SpiRxComplete == 0U)
    {
    }

    /*
     * If CCIFG became set during the transfer, restoring
     * CCIE allows Timer_A0 to run now, after the SPI ISR.
     */
    if (timerWasEnabled != 0U)
    {
        TACCTL0 |= CCIE;
    }

    return DataRead;
}

// --------------------- SPI Read/Write 16 bits -------------------------
uint16_t MCU_SPIReadWrite16(uint16_t DataToWrite)
{
    uint16_t DataRead = 0;
    DataRead = MCU_SPIReadWrite8((DataToWrite) >> 8) << 8;
    DataRead |= MCU_SPIReadWrite8((DataToWrite) & 0xff);

    return DataRead;
}

// -------------------- SPI Read/Write 24 bits --------------------------
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

// ------------------- SPI Read/Write 32 bits --------------------------
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

// --------------------- SPI Read 8 bits -------------------------------
uint8_t MCU_SPIRead8(void)
{
    uint8_t DataRead = 0;

    DataRead = MCU_SPIReadWrite8(0);

    return DataRead;
}

// --------------------- SPI Write 8 bits -------------------------------
void MCU_SPIWrite8(uint8_t DataToWrite)
{
    MCU_SPIReadWrite8(DataToWrite);
}

// --------------------- SPI Read 16 bits -------------------------------
uint16_t MCU_SPIRead16(void)
{
    uint16_t DataRead = 0;

    DataRead = MCU_SPIReadWrite16(0);

    return DataRead;
}

// --------------------- SPI Write 16 bits ------------------------------
void MCU_SPIWrite16(uint16_t DataToWrite)
{
    MCU_SPIReadWrite16(DataToWrite);
}

// --------------------- SPI Write 24 bits ------------------------------
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

// --------------------- SPI Read/Write 32 bits -------------------------
void MCU_SPIWrite32(uint32_t DataToWrite)
{
    MCU_SPIReadWrite32(DataToWrite);
}

// --------------------- SPI burst write --------------------------------
void MCU_SPIWrite(const uint8_t *DataToWrite, uint32_t length)
{
    uint16_t DataPointer = 0;

    while(DataPointer < length)
    {
        MCU_SPIWrite8(DataToWrite[DataPointer]);  // Send data byte-by-byte from array
        DataPointer ++;
    }
}

// --------------------- SPI burst read ---------------------------------
void MCU_SPIRead(uint8_t *DataToRead, uint32_t length)
{
    uint16_t DataPointer = 0;

    while(DataPointer < length)
    {
        DataToRead[DataPointer] = MCU_SPIRead8();  // Receive data byte-by-byte to array
        DataPointer ++;
    }
}

// ########################### GPIO CONTROL #############################

// ------------------------ interrupt input ------------------------------------
int MCU_Int(void){
    
    uint8_t pinState;

    pinState = ((P1IN & INT) != 0U);

    return pinState;
}

// --------------------- Chip Select line low ---------------------------
inline void MCU_CSlow(void)
{
    P1OUT &= (~CS);                       // CS# line low
    __delay_cycles(80);
}

// --------------------- Chip Select line high --------------------------
inline void MCU_CShigh(void)
{
    __delay_cycles(80);
    P1OUT |= (CS);                        // CS# line high
}

// -------------------------- PD line low -------------------------------
inline void MCU_PDlow(void)
{
    P1OUT &= (~PD);                       // PD# line low
}

// ------------------------- PD line high -------------------------------
inline void MCU_PDhigh(void)
{
    P1OUT |= (PD);                        // PD# line high
}

// --------------------------- msec based on timer ----------------------
static volatile uint32_t ticks = 0;

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A0(void)
{
    ticks++;
}

static uint32_t readTicks(void)
{
    uint16_t gieState;
    uint32_t value;

    /* Remember whether interrupts were initially enabled. */
    gieState = __get_SR_register() & GIE;

    /*
     * Prevent Timer_A from changing the 32-bit value between
     * the two 16-bit reads required by the MSP430.
     */
    __disable_interrupt();

    value = ticks;

    /* Restore the previous interrupt state. */
    if (gieState != 0U)
    {
        __enable_interrupt();
    }

    return value;
}

uint32_t MCU_Time_ms(void)
{
    return readTicks();
}

// ------------------------- msec delay based tick -----------------------
void delay(uint32_t msec)
{
    uint32_t start = readTicks();

    while ((uint32_t)(readTicks() - start) < msec)
    {
        /* Busy wait. */
    }
}

// ----------------------------- 20ms delay ------------------------------
void MCU_Delay_20ms(void)
{
    delay(20U);
}

// ----------------------------- 500ms delay ----------------------------
void MCU_Delay_500ms(void)
{
    delay(500U);
}

// ########################### ENDIAN CONVERSION ########################

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

#endif /* defined(PLATFORM_MSP430) */
