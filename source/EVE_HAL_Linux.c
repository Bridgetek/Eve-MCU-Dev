/**
 * @file EVE_HAL_Linux.c
 * @details Function implementations for EVE-MCU-Dev library HAL_Linux layer.
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

/* Only compile for linux platforms. Do not compile if libMPSSE/libFT4222 are being used. */
#if defined(USE_LINUX_SPI_DEV) && !defined(USE_MPSSE) && !defined(USE_FT4222)

/* EVE HAL INCLUDES */

#include <string.h>
#include <stdint.h> // for Uint8/16/32 and Int8/16/32 data types
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

/* Include functions for EVE-MCU-Dev library API layer */
#include <EVE.h>
/* Include functions for EVE-MCU-Dev Hardware Abstraction layer */
#include <HAL.h>
#include <Platform.h>

/* EVE HAL INCLUDES END */

/* EVE HAL CONSTANTS */

#ifdef EVE_HAL_DEBUG
#define dbg_printf(...) printf(__VA_ARGS__)
#else
#define dbg_printf(...)
#endif

#ifdef EVE_HAL_INFO
#define info_printf printf
#else
#define info_printf(...)
#endif

#ifdef EVE_HAL_ERROR
#define err_printf(...) fprintf(stderr, __VA_ARGS__)
#else
#define err_printf(...)
#endif

// Used to navigate command ring buffer on FT800 and when EVE_USE_CMDB_METHOD
// is not defined.
#if !defined(EVE_USE_CMDB_METHOD)
static uint16_t writeCmdPointer = 0;
static uint16_t readCmdPointer = 0;
#endif // defined(EVE_USE_CMDB_METHOD)

// Used for co-processor list profiling. 
#if defined(EVE_COPROC_PROFILE)
static uint16_t profileCmdPointer = 0x0000;
#endif

/* EVE HAL Library functions */

// Initialise EVE HAL Layer.
int HAL_EVE_Init(void)
{
    if (Platform_Init() < 0)
    {
        err_printf("Platform_Init() Failed.\n");
        return -1;
    }

    // Set Chip Select OFF.
    HAL_ChipSelect(0);

#if IS_EVE_API(1, 2, 3, 4)
    // Reset the display
    Platform_Delay_20ms();
    HAL_PowerDown(1);
    Platform_Delay_20ms();
    Platform_Delay_20ms();
    HAL_PowerDown(0);
    Platform_Delay_20ms();
    Platform_Delay_20ms();

#if IS_EVE_API(1)
    // FT80x_selection - FT80x modules from BRT generally use external crystal.
    // You can also send the host command to set the PLL here if you want to 
    // change it from the default of 48MHz (FT80x) or 60MHz (FT81x).
    // Clock selection and clock rate selection will put EVE to sleep and so 
    // must be before the Active command.
    // For example:
    HAL_HostCmdWrite(0x44, 0x00); // 0x44 = HostCMD_CLKEXT
    HAL_HostCmdWrite(0x62, 0x00); // 0x64 = HostCMD_CLK48M
#endif

#if IS_EVE_API(3, 4)
    // Can optionally set to 72MHz system clock here.
    // Can also adjust EVE_REG_FREQUENCY after the host commands are finished.
    HAL_HostCmdWrite(0x44, 0x00); // 0x44 = HostCMD_CLKEXT
    HAL_HostCmdWrite(0x61, 0x46);
#endif

#if IS_EVE_API(2, 3, 4)
    HAL_HostCmdWrite(0x68, 0x00); // Reset
#endif

#if IS_EVE_API(1, 2, 3, 4)
    // Set active.
    HAL_HostCmdWrite(0, 0x00);

    // Read REG_ID register (0x302000) until reads 0x7C.
    info_printf("[Waiting for REG_ID...]\n");
    uint8_t val;
    while ((val = HAL_MemRead8(EVE_REG_ID)) != 0x7C)
    {
        Platform_Delay_20ms();
        (void)val;
    }

    // Ensure CPUreset register reads 0 and so FT8xx/BT88x/BT81x is ready 
    info_printf("[Waiting for REG_CPURESET...]\n");
    while (HAL_MemRead8(EVE_REG_CPURESET) != 0x00)
    {
        Platform_Delay_20ms();
    }
#endif

#if IS_EVE_API(3, 4)
    HAL_MemWrite32(EVE_REG_FREQUENCY, 72000000);
#endif
 
    EVE_DEBUG_PRINTF("[Boot complete]\n");
 
#endif  //IS_EVE_API(1, 2, 3, 4)

#if IS_EVE_API(5)

    while (1)
    {
        unsigned char bb[128];
        unsigned int i;

        memset(bb, 0, sizeof(bb));

        // Reset the display
        Platform_Delay_20ms();
        HAL_PowerDown(1);
        Platform_Delay_20ms();
        HAL_PowerDown(0);
        Platform_Delay_20ms();

        // Set System PLL NS = 15 for 576MHz.
        HAL_HostCmdWrite(0xFF, 0xE4, 0x0F, 0x00, 0x00);
        // Set System clock divider to 0x17 for 72MHz.
        HAL_HostCmdWrite(0xFF, 0xE6, 0x17, 0x00, 0x00);
        // Set bypass BOOT_BYPASS_OTP, DDRTYPT_BYPASS_OTP and set BootCfgEn.
        HAL_HostCmdWrite(0xFF, 0xE9, 0xe1, 0x00, 0x00);
        // Set DDR Type - 1333, DDR3L, 4096.
        HAL_HostCmdWrite(0xFF, 0xEB, 0x08, 0x00, 0x00);
        // Set DDR, JT, AUD and WD in Boot Control.
        HAL_HostCmdWrite(0xFF, 0xE8, 0xf0, 0x00, 0x00);
        // Clear BootCfgEn.
        HAL_HostCmdWrite(0xFF, 0xE9, 0xc0, 0x00, 0x00);
        // Perform a reset pulse.
        HAL_HostCmdWrite(0xFF, 0xE7, 0x00, 0x00, 0x00) ; 
        // Set ACTIVE.
        HAL_HostCmdWrite(0x00, 0x00, 0x00, 0x00, 0x00) ; 

        // Delay ~100 mS.
        for (i = 0; i < 5; i++)
        {
            Platform_Delay_20ms();
        }   

        HAL_ChipSelect(1);
        // Write 4 zeros.
        HAL_Write32(0);
        // Read 128 bytes response. Raw read from SPI bus.
        struct spi_ioc_transfer xfer[1];

        xfer[0].tx_buf = (uintptr_t)NULL;
        xfer[0].rx_buf = (uintptr_t)bb;
        xfer[0].len = sizeof(bb);
        xfer[0].cs_change = 0;

        if (Platform_SPI_transfer(xfer, 1) < 0)
        {
            err_printf("HAL_Read: Transfer Failed \n");
            return -1;
        }
        HAL_ChipSelect(0);

        for (i = 0; i < sizeof(bb); i++)
        {
            if (bb[i] == 1)
            {
                uint32_t boot;

                // Wait for the REG_ID register to be set to 0x7c.
                dbg_printf("[Waiting for REG_ID...]\n");
                while (HAL_MemRead32(EVE_REG_ID) != 0x7c)
                {
                    Platform_Delay_20ms();
                }

                boot = HAL_MemRead32(EVE_REG_BOOT_STATUS);
                if (boot != 0x522e2e2e)
                {
                    dbg_printf("[Timeout waiting for BOOT_STATUS, stuck at 0x%08x, retrying...]\n", boot);
                }
                else if (HAL_MemRead32(EVE_REG_FREQUENCY) != 72000000)
                {
                    dbg_printf("[frequency %d, retrying...]\n", HAL_MemRead32(EVE_REG_FREQUENCY));
                }
                Platform_Delay_20ms();
                break;
            }
        }
        if (i < sizeof(bb)) break;

        info_printf("[Boot fail after reset, retrying...]\n");
    }

#if 0 // If we need to disable sequential reads.
    // Disable QSPI burst mode
    HAL_MemWrite32(EVE_REG_SYS_CFG, 1 << 10);
#endif

    info_printf("[Boot complete]\n");

#endif  //IS_EVE_API(5)

    // Perform any additional Platform functions. This is when the SPI interface
    // could be switched to QuadSPI in the Platform code.
    if (Platform_Setup() != 0)
    {
        err_printf("Platform_Setup() Failed.\n");
        return -1;
    }

#if defined(EVE_USE_INTERRUPT_METHOD)
    // Enable only the INT_CMDEMPTY interrupt. Other interrupt sources
    // may be added later but this bit must be set to detect command
    // buffer completion.
    HAL_MemWrite32(EVE_REG_INT_MASK, EVE_INT_CMDEMPTY);
    // Enable global interrupts.
    HAL_MemWrite32(EVE_REG_INT_EN, 1);

#endif // defined(EVE_USE_INTERRUPT_METHOD)

    // This function will not return unless an EVE device is present.
    return 0;
}

int HAL_EVE_Deinit(void)
{
    if (Platform_Deinit() < 0)
    {
        err_printf("Platform_Deinit() Failed.\n");
        return -1;
    }

    return 0;
}

// Chip Select line.
void HAL_ChipSelect(int8_t enable)
{
    if (enable)
        Platform_CSlow();
    else
        Platform_CShigh();
}

// Power Down line.
void HAL_PowerDown(int8_t enable)
{
    if (enable)
        Platform_PDlow();
    else
        Platform_PDhigh();
}

// Send register address for writing .
void HAL_SetWriteAddress(uint32_t address)
{
    struct spi_ioc_transfer xfer[1];

#if IS_EVE_API(1, 2, 3, 4) // Different addressing on BT82x.
    // Send three bytes of address to write to.
    // Ignore return values as this is an SPI write only.
    // Send high byte of address with 'read/write' bit set.
    uint32_t addr = Platform_htobe32((address << 8) | (1UL << 31));
    dbg_printf("%s: 0x%x -> 0x%x \n", __FUNCTION__, address, addr);

    memset(xfer, 0, sizeof(xfer));

    xfer[0].tx_buf = (uintptr_t)&addr;
    xfer[0].rx_buf = (uintptr_t)NULL;
    xfer[0].len = 3;
    xfer[0].cs_change = 0;
#else
    // Send 32-bit address to write to.
    // Send high byte of address with 'read/write' bit set.
    uint32_t addr = Platform_htobe32(address | (1UL << 31));
    dbg_printf("%s: 0x%x -> 0x%x \n", __FUNCTION__, address, addr);

    memset(xfer, 0, sizeof(xfer));

    xfer[0].tx_buf = (uintptr_t)&addr;
    xfer[0].rx_buf = (uintptr_t)NULL;
    xfer[0].len = 4;
    xfer[0].cs_change = 0;
#endif

    if (Platform_SPI_transfer(xfer, 1) < 0)
    {
        err_printf("HAL_SetWriteAddress: 0x%x Transfer Failed 0x%x \n", address, addr);
    }
}

// Send register address for reading.
void HAL_SetReadAddress(uint32_t address)
{
    struct spi_ioc_transfer xfer[1];

#if IS_EVE_API(1, 2, 3, 4) // Different addressing on BT82x.
    // Send three bytes of address to read from.
    // Ignore return values as this is an SPI write only.
    // Send high byte of address with 'read/write' bit unset.
    uint32_t addr = Platform_htobe32((address << 8) | (0UL << 31));
    dbg_printf("%s: 0x%x -> 0x%x \n", __FUNCTION__, address, addr);
#else
    // Send 32-bit address to read from.
    // Send high byte of address with 'read/write' bit unset.
    uint32_t addr = Platform_htobe32(address | (0UL << 31));
    dbg_printf("%s: 0x%x -> 0x%x \n", __FUNCTION__, address, addr);
#endif

    memset(xfer, 0, sizeof(xfer));

    xfer[0].tx_buf = (uintptr_t)&addr;
    xfer[0].rx_buf = (uintptr_t)NULL;
    xfer[0].len = 4;
    xfer[0].cs_change = 0;

    if (Platform_SPI_transfer(xfer, 1) < 0)
    {
        err_printf("HAL_SetReadAddress: Transfer Failed \n");
    }
}

// Send a block of data.
void HAL_Write(const uint8_t *buffer, uint32_t length)
{
    // Send multiple bytes of data after previously sending address. Ignore return
    // values as this is an SPI write only. Data must be the correct endianess
    // for the SPI bus.
    struct spi_ioc_transfer xfer[1];
    dbg_printf("%s: %d \n", __FUNCTION__, length);

    memset(xfer, 0, sizeof(xfer));

    xfer[0].tx_buf = (uintptr_t)buffer;
    xfer[0].rx_buf = (uintptr_t)NULL;
    xfer[0].len = length;
    xfer[0].cs_change = 0;

    if (Platform_SPI_transfer(xfer, 1) < 0)
    {
        err_printf("HAL_Write: Transfer Failed \n");
    }
}

// Send a 32-bit data value to command buffer.
void HAL_WriteCmd(uint32_t val32)
{
    // Send four bytes of data after previously sending address. Ignore return
    // values as this is an SPI write only.
    HAL_Write32(val32);

#if !defined(EVE_USE_CMDB_METHOD)
    // Calculate new offset.
    writeCmdPointer = (writeCmdPointer + 4) & (EVE_RAM_CMD_SIZE - 1);

    // If the command buffer has overflowed then restart the list at the start.
    if (writeCmdPointer == readCmdPointer)
    {
        EVE_DEBUG_ERROR("ERROR: Command buffer overflow.\n");
        return;
    }
#endif // defined(EVE_USE_CMDB_METHOD)
}

// Send a 32-bit data value.
void HAL_Write32(uint32_t val32)
{
    uint32_t val = Platform_htole32(val32);
    // Send four bytes of data after previously sending address. Ignore return
    // values as this is an SPI write only.
    struct spi_ioc_transfer xfer[1];
    dbg_printf("%s: 0x%x \n", __FUNCTION__, val32);

    memset(xfer, 0, sizeof(xfer));

    xfer[0].tx_buf = (uintptr_t)&val;
    xfer[0].rx_buf = (uintptr_t)NULL;
    xfer[0].len = 4;
    xfer[0].cs_change = 0;

    if (Platform_SPI_transfer(xfer, 1) < 0)
    {
        err_printf("HAL_Write32: Transfer Failed \n");
    }
}

#if IS_EVE_API(1, 2, 3, 4) // Not supported on BT82x.
// Send a 16-bit data value.
void HAL_Write16(uint16_t val16)
{
    uint16_t val = Platform_htole16(val16);
    // Send two bytes of data after previously sending address. Ignore return
    // values as this is an SPI write only.
    struct spi_ioc_transfer xfer[1];
    dbg_printf("%s: 0x%x \n", __FUNCTION__, val16);

    memset(xfer, 0, sizeof(xfer));

    xfer[0].tx_buf = (uintptr_t)&val;
    xfer[0].rx_buf = (uintptr_t)NULL;
    xfer[0].len = 2;
    xfer[0].cs_change = 0;

    if (Platform_SPI_transfer(xfer, 1) < 0)
    {
        err_printf("HAL_Write16: Transfer Failed \n");
    }
}
#endif

#if IS_EVE_API(1, 2, 3, 4) // Not supported on BT82x.
// Send an 8-bit data value.
void HAL_Write8(uint8_t val8)
{
    // Send one byte of data after previously sending address. Ignore return
    // values as this is an SPI write only.
    struct spi_ioc_transfer xfer[1];
    dbg_printf("%s: 0x%x \n", __FUNCTION__, val8);

    memset(xfer, 0, sizeof(xfer));

    xfer[0].tx_buf = (uintptr_t)&val8;
    xfer[0].rx_buf = (uintptr_t)NULL;
    xfer[0].len = 1;
    xfer[0].cs_change = 0;

    if (Platform_SPI_transfer(xfer, 1) < 0)
    {
        err_printf("HAL_Write8: Transfer Failed \n");
    }
}
#endif

// Read a block of data.
void HAL_Read(uint8_t *buffer, uint32_t length)
{
    // Send multiple bytes of data after previously sending address. Ignore return
    // values as this is an SPI write only. Data must be the correct endianess
    // for the SPI bus.
    struct spi_ioc_transfer xfer[1];
    memset(xfer, 0, sizeof(xfer));
    dbg_printf("%s: %d \n", __FUNCTION__, length);

#if IS_EVE_API(1, 2, 3, 4)

    xfer[0].tx_buf = (uintptr_t)NULL;
    xfer[0].rx_buf = (uintptr_t)buffer;
    xfer[0].len = length;
    xfer[0].cs_change = 0;

    if (Platform_SPI_transfer(xfer, 1) < 0)
    {
        err_printf("HAL_Read: Transfer Failed \n");
        return;
    }
#else
    unsigned char bb[MCU_SPI_TIMEOUT];
    uint32_t recvlen = 0;
    int i;
    // Read MCU_SPI_TIMEOUT bytes before the "0x01" that signifies data ready.
    xfer[0].tx_buf = (uintptr_t)NULL;
    xfer[0].rx_buf = (uintptr_t)bb;
    xfer[0].len = MCU_SPI_TIMEOUT;
    xfer[0].cs_change = 0;

    if (Platform_SPI_transfer(xfer, 1) < 0)
    {
        err_printf("HAL_Read: Transfer Failed \n");
        return;
    }
    for (i = 0; i < MCU_SPI_TIMEOUT; i++)
    {
        if (bb[i] == 1)
        {
            i++;
            // Number of bytes received that are valid.
            recvlen = MCU_SPI_TIMEOUT - i;
            // Number of valid bytes can range from 0 to MCU_SPI_TIMEOUT-1.
            // Only take the requested length of data from the input buffer.
            if (length < recvlen)
            {
                recvlen = length;
            }
            length -= recvlen;
            // Read first part of data in. This will always be less than 
            // MCU_SPI_TIMEOUT bytes. Do not break alignment.
            while (recvlen--)
            {
                *(buffer++) = bb[i++];
            }
            while (length > 0)
            {
                uint32_t nn = length;
                if (nn > MCU_SPI_TRANSFER)
                {
                    nn = MCU_SPI_TRANSFER;
                }
                xfer[0].tx_buf = (uintptr_t)NULL;
                xfer[0].rx_buf = (uintptr_t)buffer;
                xfer[0].len = nn;
                xfer[0].cs_change = 0;

                if (Platform_SPI_transfer(xfer, 1) < 0)
                {
                    err_printf("HAL_Read: Transfer Failed \n");
                    return;
                }
                length -= nn;
                buffer += nn;
            }
            break;
        }
    }
#endif
}

// Read a 32-bit data value.
uint32_t HAL_Read32(void)
{
    // Read 4 bytes from a previously setup address. Send dummy
    // 00 bytes as only the incoming value is important.
    uint32_t val32 = 0;
    struct spi_ioc_transfer xfer[1];

    memset(xfer, 0, sizeof(xfer));
#if IS_EVE_API(1, 2, 3, 4) // Method differs on BT82x.
    xfer[0].tx_buf = (uintptr_t)NULL;
    xfer[0].rx_buf = (uintptr_t)&val32;
    xfer[0].len = 4;
    xfer[0].cs_change = 0;

    if (Platform_SPI_transfer(xfer, 1) < 0)
    {
        err_printf("HAL_Read32: Transfer Failed \n");
    }

    dbg_printf("%s: 0x%x \n", __FUNCTION__, val32);
#else
    HAL_Read((uint8_t *)&val32, sizeof(uint32_t));
#endif

    // Return combined 32-bit value
    return Platform_le32toh(val32);
}

#if IS_EVE_API(1, 2, 3, 4) // Not supported on BT82x.
// Read a 16-bit data value.
uint16_t HAL_Read16(void)
{
    // Read 2 bytes from a register has been previously addressed. Send dummy
    // 00 bytes as only the incoming value is important.
    uint16_t val16;
    struct spi_ioc_transfer xfer[1];

    memset(xfer, 0, sizeof(xfer));

    xfer[0].tx_buf = (uintptr_t)NULL;
    xfer[0].rx_buf = (uintptr_t)&val16;
    xfer[0].len = 2;
    xfer[0].cs_change = 0;

    if (Platform_SPI_transfer(xfer, 1) < 0)
    {
        err_printf("HAL_Read16: Transfer Failed \n");
    }

    dbg_printf("%s: 0x%x \n", __FUNCTION__, val16);
    // Return combined 16-bit value.
    return Platform_le16toh(val16);
}
#endif

#if IS_EVE_API(1, 2, 3, 4) // Not supported on BT82x.
// Read an 8-bit data value.
uint8_t HAL_Read8(void)
{
    // Read 1 byte from a previously setup address. Send dummy
    // 00 byte as only the incoming value is important.
    uint8_t val8;
    struct spi_ioc_transfer xfer[1];

    memset(xfer, 0, sizeof(xfer));

    xfer[0].tx_buf = (uintptr_t)NULL;
    xfer[0].rx_buf = (uintptr_t)&val8;
    xfer[0].len = 1;
    xfer[0].cs_change = 0;

    if (Platform_SPI_transfer(xfer, 1) < 0)
    {
        err_printf("HAL_Read8: Transfer Failed \n");
    }

    dbg_printf("%s: 0x%x \n", __FUNCTION__, val8);
    // Return 8-bit value read.
    return val8;
}
#endif

// COMBINED ADDRESSING AND DATA FUNCTIONS

// This section has combined calls which carry out a full write or read cycle
// including chip select, address, and data transfer.
// This would often be used for register writes and reads.

// Write a 32-bit value to specified address.
void HAL_MemWrite32(uint32_t address, uint32_t val32)
{
    dbg_printf("%s: %0x%x 0x%x \n", __FUNCTION__, address, val32);

    // CS low begins the SPI transfer.
    HAL_ChipSelect(1);
    // Send address to be written.
    HAL_SetWriteAddress(address);
    // Send the data value.
    HAL_Write32(val32);
    // CS high terminates the SPI transfer.
    HAL_ChipSelect(0);
}

#if IS_EVE_API(1, 2, 3, 4) // Not supported on BT82x.
// Write a 16-bit value to specified address.
void HAL_MemWrite16(uint32_t address, uint16_t val16)
{
    dbg_printf("%s: %0x%x 0x%x \n", __FUNCTION__, address, val16);

    // CS low begins the SPI transfer.
    HAL_ChipSelect(1);
    // Send address to be written.
    HAL_SetWriteAddress(address);
    // Send the data value.
    HAL_Write16(val16);
    // CS high terminates the SPI transfer.
    HAL_ChipSelect(0);
}
#endif

#if IS_EVE_API(1, 2, 3, 4) // Not supported on BT82x.
// Write an 8-bit value to specified address.
void HAL_MemWrite8(uint32_t address, uint8_t val8)
{
    dbg_printf("%s: %0x%x 0x%x \n", __FUNCTION__, address, val8);

    // CS low begins the SPI transfer.
    HAL_ChipSelect(1);
    // Send address to be written.
    HAL_SetWriteAddress(address);
    // Send the data value.
    HAL_Write8(val8);
    // CS high terminates the SPI transfer.
    HAL_ChipSelect(0);
}
#endif

// Read a 32-bit value from specified address.
uint32_t HAL_MemRead32(uint32_t address)
{
    uint32_t val32;
    dbg_printf("%s: 0x%x \n", __FUNCTION__, address);

    // CS low begins the SPI transfer.
    HAL_ChipSelect(1);
    // Send address to be read.
    HAL_SetReadAddress(address);
    // Read the data value.
    val32 = HAL_Read32();
    // CS high terminates the SPI transfer.
    HAL_ChipSelect(0);

    dbg_printf("%s: 0x%x \n", __FUNCTION__, val32);
    // Return 32-bit value read.
    return Platform_le32toh(val32);
}

#if IS_EVE_API(1, 2, 3, 4) // Not supported on BT82x.
// Read a 16-bit value from specified address.
uint16_t HAL_MemRead16(uint32_t address)
{
    uint16_t val16;
    dbg_printf("%s: 0x%x \n", __FUNCTION__, address);

    // CS low begins the SPI transfer.
    HAL_ChipSelect(1);
    // Send address to be read.
    HAL_SetReadAddress(address);
    // Read the data value.
    val16 = HAL_Read16();
    // CS high terminates the SPI transfer.
    HAL_ChipSelect(0);

    dbg_printf("%s: 0x%x \n", __FUNCTION__, val16);
    // Return 16-bit value read.
    return Platform_le16toh(val16);
}
#endif

#if IS_EVE_API(1, 2, 3, 4) // Not supported on BT82x.
// Read an 8-bit value from specified address.
uint8_t HAL_MemRead8(uint32_t address)
{
    uint8_t val8;
    dbg_printf("%s: 0x%x \n", __FUNCTION__, address);

    // CS low begins the SPI transfer.
    HAL_ChipSelect(1);
    // Send address to be read.
    HAL_SetReadAddress(address);
    // Read the data value.
    val8 = HAL_Read8();
    // CS high terminates the SPI transfer.
    HAL_ChipSelect(0);

    dbg_printf("%s: 0x%x \n", __FUNCTION__, val8);
    // Return 8-bit value read.
    return val8;
}
#endif

// HOST COMMANDS

//  Write a host command.
#if IS_EVE_API(1, 2, 3, 4) // Different host commands on BT82x.
void HAL_HostCmdWrite(uint8_t cmd, uint8_t param)
{
    struct spi_ioc_transfer xfer[3];
    uint8_t zero = 0;

    // CS low begins the SPI transfer.
    HAL_ChipSelect(1);

    memset(xfer, 0, sizeof(xfer));

    // Send command.
    xfer[0].tx_buf = (uintptr_t)&cmd;
    xfer[0].rx_buf = (uintptr_t)NULL;
    xfer[0].len = 1;
    xfer[0].cs_change = 0;
    // followed by parameter.
    xfer[1].tx_buf = (uintptr_t)&param;
    xfer[1].rx_buf = (uintptr_t)NULL;
    xfer[1].len = 1;
    xfer[1].cs_change = 0;
    // and a dummy 00 byte.
    xfer[2].tx_buf = (uintptr_t)&zero;
    xfer[2].rx_buf = (uintptr_t)NULL;
    xfer[2].len = 1;
    xfer[2].cs_change = 0;

    if (Platform_SPI_transfer(xfer, 3) < 0)
    {
        err_printf("HAL_HostCmdWrite: Transfer Failed \n");
    }

    // CS high terminates the SPI transfer.
    HAL_ChipSelect(0);
}
#else
void HAL_HostCmdWrite(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4, uint8_t b5)
{
    struct spi_ioc_transfer xfer[1];
    uint8_t command[5];

    memset(xfer, 0, sizeof(xfer));
    command[0] = b1;
    command[1] = b2;
    command[2] = b3;
    command[3] = b4;
    command[4] = b5;
    
    xfer[0].tx_buf = (uintptr_t)command;
    xfer[0].rx_buf = (uintptr_t)NULL;
    xfer[0].len = 5;
    xfer[0].cs_change = 0;

    // CS low begins the SPI transfer.
    HAL_ChipSelect(1);
    // Send command.
    if (Platform_SPI_transfer(xfer, 1) < 0)
    {
        err_printf("HAL_HostCmdWrite: Transfer Failed \n");
    }
    // CS high terminates the SPI transfer.
    HAL_ChipSelect(0);
}
#endif

// SUPPORTING FUNCTIONS

// Increment co-processor address offset counter.
void HAL_IncCmdPointer(uint16_t commandSize)
{
    (void)commandSize;
    // If we are using the CMDB method then ignore.
#if !defined(EVE_USE_CMDB_METHOD)
    // Calculate new offset.
    writeCmdPointer = (writeCmdPointer + commandSize) & (EVE_RAM_CMD_SIZE - 1);
    dbg_printf("%s is now 0x%x\n", __FUNCTION__, writeCmdPointer);
#endif // defined(EVE_USE_CMDB_METHOD)

#if defined(EVE_COPROC_PROFILE)
    // Update profiling information.
    profileCmdPointer += commandSize;
#endif // defined(EVE_COPROC_PROFILE)
}

// Increment co-processor address offset counter.
uint16_t HAL_GetCmdPointer(void)
{
    // Return new offset.
#if defined(EVE_USE_CMDB_METHOD)
    // If we are using the CMDB method then get the current command pointer.
    uint16_t writeCmdPointer;
    writeCmdPointer = HAL_MemRead32(EVE_REG_CMD_WRITE) & 0xffff;
#endif // defined(EVE_USE_CMDB_METHOD)
    dbg_printf("%s 0x%x\n", __FUNCTION__, writeCmdPointer);
    // Return new offset.
    return writeCmdPointer;
}

#if !defined(EVE_USE_CMDB_METHOD)
void HAL_ResetCmdPointer(void)
{
    writeCmdPointer = 0;
    readCmdPointer = 0;
}
#endif

#if !defined(EVE_USE_CMDB_METHOD)
void HAL_WriteCmdPointer(void)
{
    dbg_printf("%s 0x%x\n", __FUNCTION__, writeCmdPointer);
#if defined(EVE_USE_INTERRUPT_METHOD)
    // Clear the interrupt flags register and reset the interrupt line.
    EVE_LIB_GetInterrupt(EVE_INT_CMDEMPTY);
#endif // defined(EVE_USE_INTERRUPT_METHOD)

    // Do nothing if no commands have been added.
    if (writeCmdPointer != readCmdPointer)
    {
        // And move write pointer to here to start the list.
        HAL_MemWrite32(EVE_REG_CMD_WRITE, writeCmdPointer);
    }
}
#endif

#if defined(EVE_COPROC_PROFILE)
uint16_t HAL_GetProfilePointer(void)
{
    return profileCmdPointer;
}
#endif

#if defined(EVE_COPROC_PROFILE)
void HAL_ResetProfilePointer(void)
{
    profileCmdPointer = 0;
}
#endif

// Wait for co-processor read and write pointers to be equal.
uint8_t HAL_WaitCmdFifoEmpty(uint32_t timeout)
{
    uint32_t starttime = 0, curtime = 0;

    if (timeout)
    {
        // Reference start point for timeout.
        starttime = Platform_Time_ms();
    }

#if !defined(EVE_USE_CMDB_METHOD)

    // Only wait for an coprocessor empty if the write pointer differs
    // from the read pointer. If they are equal then there is nothing
    // to do and therefore no action required.
    if (writeCmdPointer != readCmdPointer)
    {

#if defined(EVE_USE_INTERRUPT_METHOD)

        do 
        {
            while (Platform_Int())
            {
                // Detect a timeout.
                if (timeout)
                {
                    // Elapsed time since function call.
                    curtime = Platform_Time_ms();
                    if ((curtime - starttime) > timeout) break;
                }
            }
            // Timeout break from interrupt test.
            if ((curtime - starttime) > timeout) break;
            // Read of REG_INT_FLAGS to clear interrupt.
        } while (!EVE_LIB_GetInterrupt(EVE_INT_CMDEMPTY));

        // Read the graphics processor read pointer (contains error flag).
        readCmdPointer = HAL_MemRead32(EVE_REG_CMD_READ);
#else
        // Wait until the two registers match.
        do
        {
            // Read the graphics processor read pointer.
            readCmdPointer = HAL_MemRead32(EVE_REG_CMD_READ);
            // Detect an exception.
            if (readCmdPointer & 1) break;
            // Detect a timeout.
            if (timeout)
            {
                // Elapsed time since function call.
                curtime = Platform_Time_ms();
                if ((curtime - starttime) > timeout) break;
            }
        } while ((writeCmdPointer != readCmdPointer) && (readCmdPointer != (EVE_RAM_CMD_SIZE - 1)));
#endif
    }

#else // defined(EVE_USE_CMDB_METHOD)

    uint32_t readCmdPointer;
    // Wait until there is all the potential space free.
    do
    {
        // Read the graphics processor read pointer.
        readCmdPointer = HAL_MemRead32(EVE_REG_CMDB_SPACE);
        // Detect an exception.
        if (readCmdPointer & 1) break;
        // Detect a timeout.
        if (timeout)
        {
            // Elapsed time since function call.
            curtime = Platform_Time_ms();
            if ((curtime - starttime) > timeout) break;
        }
    } while (readCmdPointer < (EVE_RAM_CMD_SIZE - 4));

#endif // defined(EVE_USE_CMDB_METHOD)

    if (readCmdPointer & 1)
    {
        // Return 0xFF (EVE_COPRO_STATUS_EXCEPTION) if an error occurred.
#ifdef EVE_HAL_ERROR
#if IS_EVE_API(3,4,5)
        char message[256];

        memset(message, 0, sizeof(message));
        EVE_LIB_GetCoProException(message);
        err_printf("Co-processor exception: %s\n", message);
#else // IS_EVE_API(3,4,5)
        err_printf("Co-processor exception\n");
#endif // IS_EVE_API(3,4,5)
#endif // EVE_HAL_ERROR
        
        return EVE_COPRO_STATUS_EXCEPTION;
    }
    else if (timeout)
    {
        // Return 0xFE (EVE_COPRO_STATUS_TIMEOUT) if a timeout occurred.
        if ((curtime - starttime) > timeout)
        {
#if DEBUG_LEVEL > 0
            EVE_DEBUG_ERROR("Co-processor timeout\n");
#endif // DEBUG_LEVEL
            return EVE_COPRO_STATUS_TIMEOUT;
        }
    }

    // Return 0 if pointers became equal successfully.
    return EVE_COPRO_STATUS_SUCCESS;
}

// Check how much free space is available in CMD FIFO.
uint16_t HAL_CheckCmdFreeSpace(void)
{
#if !defined(EVE_USE_CMDB_METHOD)

    uint16_t Fullness, Freespace;

    // Check the graphics processor read pointer.
    readCmdPointer = (uint16_t)HAL_MemRead32(EVE_REG_CMD_READ);
    // Fullness is difference between current write pointer 
    // value and value of the REG_CMD_READ.
    Fullness = ((writeCmdPointer - readCmdPointer) & (EVE_RAM_CMD_SIZE - 1));
    // Free Space is 4K - 4 - Fullness (-4 avoids buffer wrapping round).
    Freespace = (EVE_RAM_CMD_SIZE - 4) - Fullness;

    return Freespace;
#else // defined(EVE_USE_CMDB_METHOD)
    uint16_t readCmdSpace;
    // Free space is read from the REG_CMDB_SPACE register.
    readCmdSpace = HAL_MemRead32(EVE_REG_CMDB_SPACE) & 0xffff;

    return readCmdSpace;
#endif // defined(EVE_USE_CMDB_METHOD)
}

void HAL_SetSPIMode(uint32_t mode)
{
#if IS_EVE_API(1)
    // QuadSPI is not supported on FT80x.
    (void)mode;
#elif IS_EVE_API(2,3,4)
    // Turn on EVE quad-SPI for FT81x and BT81x devices.
    // Write EVE_REG_SPI_WIDTH and mask SPI_WIDTH.
    HAL_MemWrite32(EVE_REG_SPI_WIDTH, ((uint32_t)mode) & 3);
#elif IS_EVE_API(5)
    // Turn on EVE quad-SPI for FT82x devices.
    // Read REG_SYS_CFG and mask SPI_WIDTH.
    uint32_t cfg;
    cfg = HAL_MemRead32(EVE_REG_SYS_CFG) & (~(0x3 << 8));
    cfg = cfg | (((uint32_t)mode) << 8);
    HAL_MemWrite32(EVE_REG_SYS_CFG, cfg);
#endif 
}

int HAL_Int(void)
{
    return Platform_Int();
}

/* EVE HAL END */

#endif // defined(USE_LINUX_SPI_DEV)
