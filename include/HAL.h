/**
 * @file HAL.h
 * @brief Definitions for accessing the abstraction layer from the API layer.
 *
 * @details @details This layer provides the interface between the EVE API layer and
 *      the underlying MCU or platform-specific hardware implementation. 
 *      There is an option of using an MCU style interface implemented in EVE_HAL.c or
 *      a Linux style interface implemented in EVE_HAL_Linux.c. These HAL files
 *      standardise the methods to access the hardware layer and perform some low-level
 *      helper functions to simplify the API layer code.
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

#ifndef _EVE_HAL_HEADER_H
#define _EVE_HAL_HEADER_H

/* EVE HAL INCLUDES */

#include <stdint.h> // for Uint8/16/32 and Int8/16/32 data types

/*
 * Include the EVE settings required by the HAL layer, including API
 * selection macros and device-specific configuration.
 */
#include "EVE_settings.h"

/*
 * Support deprecated MCU SPI configuration macros.
 * User-supplied deprecated values take precedence over the replacement
 * macros and are retained for one release cycle.
 */
#if defined(HAL_MAX_CHUNK_SIZE)
#undef EVE_HAL_CHUNK_SIZE
#define EVE_HAL_CHUNK_SIZE HAL_MAX_CHUNK_SIZE
#endif // defined(HAL_MAX_CHUNK_SIZE)

/* EVE HAL */

/**
 * @brief Maximum size of an EVE data transfer chunk.
 * @details Defines the maximum number of data bytes passed to the EVE read
 *      or write functions in a single chunk. This value must always be less
 *      than 65535 bytes.
 */
#if !defined(EVE_HAL_CHUNK_SIZE)
#define EVE_HAL_CHUNK_SIZE (1024 - sizeof(uint32_t))
#endif // !defined(EVE_HAL_CHUNK_SIZE)

/**
 * @brief Initialise EVE HAL Layer.
 * @details Power cycle and start the EVE display in a controlled manner.
 *      This will call the MCU-specific initialisation routine and check for
 *      the presence of a supported FT8xx/BT8xx device on the SPI bus.
 * @return 0 for success or -1 for failure (device not found or unsupported).
 */
int HAL_EVE_Init(void);

/**
 * @brief De-Initialise EVE HAL Layer.
 * @details This will call the MCU-specific de-initialisation routines.
 * @return 0 for success or -1 for failure.
 */
int HAL_EVE_Deinit(void);

/**
 * @brief Chip Select Control
 * @details Abstract the low-level MCU chip select control to either
 *      enable or disable. A call to this function to enable chip select
 *      will result in a logic low on the SPI CS line.
 * @param enable - Non-zero to enable chip select
 *      Zero to deselect.
 */
void HAL_ChipSelect(int8_t enable);

/**
 * @brief Power Down Control
 * @details Abstract the low-level MCU power down control line to
 *      control the EVE display. A call to this function to enable
 *      power down will result in a logic low on the PD line.
 * @param enable - Non-zero to enable power down
 *      Zero to disable (normal operating state of EVE).
 */
void HAL_PowerDown(int8_t enable);

/**
 * @brief Increment command memory write pointer
 * @details The command memory write pointer stores the current
 *      location where co-processor commands are written. This is
 *      kept internally in the HAL and is NOT written to the
 *      REG_CMD_WRITE register on EVE until the HAL_WriteCmdPointer
 *      function is called. This allows multiple commands (up to
 *      the size of the command memory) to be stored and executed
 *      when required.
 * @param commandSize - The number of bytes to advance the command
 *      memory write pointer. This will wrap at the end of the
 *      command memory.
 */
void HAL_IncCmdPointer(uint16_t commandSize);

/**
 * @brief Get the current command memory write pointer
 * @details Obtains the current command memory write pointer from
 *      the value stored internally in the HAL. It is not the value
 *      read from the REG_CMD_WRITE register.
 * @returns Command memory write pointer
 */
uint16_t HAL_GetCmdPointer(void);

#if !defined(EVE_USE_CMDB_METHOD)
/**
 * @brief Reset the current command memory write pointer
 * @details Sets the current command memory write pointer to zero
 *      internally in the HAL. It is not the value read from the
 *      REG_CMD_WRITE register.
 */
void HAL_ResetCmdPointer(void);

/**
 * @brief Commits the current command memory write pointer
 * @details Stores the internal HAL command memory write pointer to
 *      the REG_CMD_WRITE register. This will start the co-processor
 *      working through items in the display list.
 */
void HAL_WriteCmdPointer(void);
#endif // !defined(EVE_USE_CMDB_METHOD)

#if defined(EVE_COPROC_PROFILE)
/**
 * @brief Gets the current co-processor list size for profiling
 * @details Returns the current length of the co-processor instruction
 *      list. This is reset using HAL_ResetProfilePointer and increments
 *      when a command is added using HAL_IncCmdPointer.
 */
uint16_t HAL_GetProfilePointer(void);

/**
 * @brief Resets current co-processor list profiling size
 * @details This will set the current length of the co-processor instruction
 *      list to zero. Call when a new co-processor list is started.
 */
void HAL_ResetProfilePointer(void);
#endif // defined(EVE_COPROC_PROFILE)

/**
 * @brief Wait for display list to complete
 * @details Polls the REG_CMD_READ register until it matches the
 *      current command memory write pointer. This will indicate that
 *      the co-processor has completed working through the items in the
 *      display list.
 * @param timeout - timeout in milliseconds (zero for NO timeout)
 * @returns Zero for normal completion, 0xff for an error condition, 0xfe for a timeout.
 */
uint8_t HAL_WaitCmdFifoEmpty(uint32_t timeout);

/**
 * @brief Calculate free space in the command memory.
 * @details Works out how many bytes of command memory is available
 *      to receive display list instructions. There will always be
 *      4 bytes (one display list entry) reserved in the command memory
 *      to ensure that the display list can never create a loop.
 * @returns Number of free bytes.
 */
uint16_t HAL_CheckCmdFreeSpace(void);

/**
 * @brief Write a 32-bit value to an EVE memory location
 * @details Formats a memory space write to EVE. This can be any register
 *      or mapped memory on the device (display list or command list).
 *      This function will control chip select.
 * @param address - 24-bit address on EVE 
 * @param val32 - value to write
 */
void HAL_MemWrite32(uint32_t address, uint32_t val32);

#if IS_EVE_API(1, 2, 3, 4) // Not supported on BT82x
/**
 * @brief Write a 16-bit value to an EVE memory location
 * @details Formats a memory space write to EVE. This can be any register
 *      or mapped memory on the device (display list or command list).
 *      This function will control chip select.
 * @param address - 24-bit address on EVE
 * @param val16 - value to write
 */
void HAL_MemWrite16(uint32_t address, uint16_t val16);

/**
 * @brief Write an 8-bit value to an EVE memory location
 * @details Formats a memory space write to EVE. This can be any register
 *      or mapped memory on the device (display list or command list).
 *      This function will control chip select.
 * @param address - 24-bit address on EVE
 * @param val8 - value to write
 */
void HAL_MemWrite8(uint32_t address, uint8_t val8);
#endif // IS_EVE_API(1, 2, 3, 4)

/**
 * @brief Read a 32-bit value from an EVE memory location
 * @details Formats a memory space read to EVE. This can be any register
 *      or mapped memory on the device (display list or command list).
 *      This function will control chip select.
 * @param address - 24-bit address on EVE
 * @returns value read from EVE
 */
uint32_t HAL_MemRead32(uint32_t address);

#if IS_EVE_API(1, 2, 3, 4) // Not supported on BT82x
/**
 * @brief Read a 16-bit value from an EVE memory location
 * @details Formats a memory space read to EVE. This can be any register
 *      or mapped memory on the device (display list or command list).
 *      This function will control chip select.
 * @param address - 24-bit address on EVE
 * @returns value read from EVE
 */
uint16_t HAL_MemRead16(uint32_t address);

/**
 * @brief Read an 8-bit value from an EVE memory location
 * @details Formats a memory space read to EVE. This can be any register
 *      or mapped memory on the device (display list or command list).
 *      This function will control chip select.
 * @param address - 24-bit address on EVE
 * @returns value read from EVE
 */
uint8_t HAL_MemRead8(uint32_t address);
#endif // IS_EVE_API(1, 2, 3, 4) 

/**
 * @brief Sends an address for writing to EVE
 * @details Formats a memory address for writing to the EVE.
 *      This function will not control chip select.
 * @param address - 24-bit address on EVE
 */
void HAL_SetWriteAddress(uint32_t address);

/**
 * @brief Sends an address for reading to EVE
 * @details Formats a memory address for reading from the EVE.
 *      This function will not control chip select.
 * @param address - 24-bit address on EVE
 */
void HAL_SetReadAddress(uint32_t address);

/**
 * @brief Sends a host command to EVE
 * @details Sends a special host command and parameters using SPI to the EVE.
 *      This function will control chip select.
 */
#if IS_EVE_API(1, 2, 3, 4) // Different host commands on BT82x
void HAL_HostCmdWrite(uint8_t cmd, uint8_t param);
#else // EVE 5
void HAL_HostCmdWrite(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4, uint8_t b5);
#endif

/**
 * @brief Sends a block of data to EVE
 * @details Sends a block of data using SPI to the EVE.
 *      This function will not control chip select.
 * @param buffer - Pointer to the data buffer to write.
 * @param length - Number of bytes to write.
 */
void HAL_Write(const uint8_t *buffer, uint32_t length);

/**
 * @brief Sends a 32-bit value to EVE command buffer
 * @details Sends a 32-bit value using SPI to the EVE.
 *      This function will not control chip select. If the EVE_USE_CMDB_METHOD
 *      is not defined then it will wrap at the end of the command buffer.
 * @param val32 - 32-bit value
 */
void HAL_WriteCmd(uint32_t val32);

/**
 * @brief Sends a 32-bit value to EVE
 * @details Sends a 32-bit value using SPI to the EVE.
 *      This function will not control chip select.
 * @param val32 - 32-bit value
 */
void HAL_Write32(uint32_t val32);

#if IS_EVE_API(1, 2, 3, 4) // Not supported on BT82x
/**
 * @brief Sends a 16-bit value to EVE
 * @details Sends a 16-bit value using SPI to the EVE.
 *      This function will not control chip select.
 * @param val16 - 16-bit value
 */
void HAL_Write16(uint16_t val16);

/**
 * @brief Sends a 8-bit value to EVE
 * @details Sends a 8-bit value using SPI to the EVE.
 *      This function will not control chip select.
 * @param val8 - 8-bit value
 */
void HAL_Write8(uint8_t val8);
#endif // IS_EVE_API(1, 2, 3, 4)

/**
* @brief Reads a block of data from EVE
* @details Reads a block of data using SPI from the EVE.
*      This function will not control chip select.
* @param buffer - Pointer to the buffer to receive the data.
* @param length - Number of bytes to read.
 */
void HAL_Read(uint8_t *buffer, uint32_t length);

/**
 * @brief Reads a 32-bit value from EVE
 * @details Sends a 32-bit dummy value using SPI to the EVE
 *      and receives the result.
 *      This function will not control chip select.
 * @returns 32-bit value read
 */
uint32_t HAL_Read32(void);

#if IS_EVE_API(1, 2, 3, 4) // Not supported on BT82x
/**
 * @brief Reads a 16-bit value from EVE
 * @details Sends a 16-bit dummy value using SPI to the EVE
 *      and receives the result.
 *      This function will not control chip select.
 * @returns 16-bit value read
 */
uint16_t HAL_Read16(void);

/**
 * @brief Reads a 8-bit value from EVE
 * @details Sends an 8-bit dummy value using SPI to the EVE
 *      and receives the result.
 *      This function will not control chip select.
 * @returns 8-bit value read
 */
uint8_t HAL_Read8(void);
#endif // IS_EVE_API(1, 2, 3, 4)

/**
 * @brief Deprecated SPI interface width type.
 * @details Retained for source compatibility for one release cycle.
 *      New code should use uint8_t with the EVE_SPI_*_CHANNEL definitions
 *      from EVE_defs.h.
 */
typedef uint8_t EVE_SPI_CHANNELS_T;

#if defined(EVE_QSPI_ENABLE)
/**
 * @brief Configures the EVE and MCU SPI interface width.
 * @details Changes the EVE SPI interface width and then configures the
 *      MCU peripheral to use the corresponding interface mode. No EVE
 *      transactions are performed between these operations.
 * @param mode - SPI mode:
 *                  0 - 1bit (Default Single mode)
 *                  1 - 2bits (Dual mode)
 *                  2 - 4bits (Quad mode)
 * @returns 0 if successful, -1 if failed.
 */
int HAL_SetSPIMode(uint8_t mode);
#endif // defined(EVE_QSPI_ENABLE)

/**
 * @brief Test interrupt input line
 * @details This function will check the interrupt input INT# from
 *      the EVE device using the MCU layer.
 * @returns zero if there is no interrupt, non-zero if the EVE device is
 *      asserting an interrupt. A negative value will be returned if
 *      the method is not supported on the MCU or platform.
 */
int HAL_Int(void);

#if defined(EVE_MANAGE_INTERRUPTS)
/**
 * @brief Test if an EVE interrupt flag is set
 * @details Reads the EVE interrupt flag register and adds any newly pending
 *      interrupt flags to an internally stored status value. Reading the
 *      interrupt flag register clears the pending hardware flags, so the
 *      accumulated flags are retained until consumed by the supplied mask.
 * @param mask - Bit mask of interrupts to query and clear.
 * @returns 0 if no interrupts in the mask are set. If any interrupts are
 *      set then the return value will contain the corresponding bits from
 *      the mask parameter.
 */
uint8_t HAL_GetInterrupt(uint8_t mask);
#endif //defined(EVE_MANAGE_INTERRUPTS)

/* EVE HAL END */

#endif /* _EVE_HAL_HEADER_H */
