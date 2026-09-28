# EVE-MCU-Dev API Reference

This callable layer is implemented in `EVE.h` and `EVE_API.c` and is called by the main loop of the application. 

Its purpose is to allow the program to use the same syntax as the EVE Programmers Guide when writing to the co-processor and so make programming of the display simpler and more easily maintained. 

The file contains several types of helper function including: 
- Functions which are used to begin, finish and check execution of co-processor lists. 
- Functions for writing data to `RAM_G` and `RAM_CMD`.
- A function for calling each Display List instruction and each Co-Processor command from the EVE programmers guide.

## Contents

- [Initialising EVE](#initialising-eve)
- [Co-Processor Lists](#co-processor-lists)
- [Co-Processor Profiling](#co-processor-profiling)
- [Co-Processor Helpers](#co-processor-helpers)
- [EVE Display List Commands](#eve-display-list-commands)
- [EVE Co-processor Commands](#eve-co-processor-commands)


## Initialising EVE

This section covers API functions that deal with initialising and closing the library.

| Function | Overview |
| --- | --- |
| [EVE_Init](#eve_init) | Initialise the EVE library. |
| [EVE_Deinit](#eve_deinit) | Close the EVE library. |

### EVE_Init

Initialise EVE library.

**Detailed description:**

Initialise the EVE API layer, HAL layer and MCU-specific hardware layer.

Before using the library to send instructions to the EVE device the `EVE_Init` function **must** be called.

This function will initialise the GPIO and SPI interface through the port file for the target MCU specific library. It will also write the display settings registers to the values defined in `EVE_config.h`. Note that these **must** be adjusted to suit your display. It then sets up the GPIO and other registers such as PWM (for the backlight) and sound on the EVE device.

A short co-processor list is used to clear the screen.

One additional step performed in `EVE_API.c` is to clear the bitmap handle properties (including BITMAP_LAYOUT_H and BITMAP_SIZE_H). It is important that this code is executed after the GPU is running and rendering the screen and therefore after the REG_PCLK has been set to the required value.

**Returns:**

0 for success or -1 for failure (device not found or unsupported).

**Format:**

`int EVE_Init(void)`

### EVE_Deinit

De-Initialise EVE library.

**Detailed description:**

This will call the HAL layer and MCU-specific de-initialisation routines.

**Returns:**

0 for success or -1 for failure (device not found or unsupported).

**Format:**

`int EVE_Deinit(void)`

## Co-Processor Lists

These functions perform the necessary tasks to begin and execute co-processor lists.

| Function | Overview |
| --- | --- |
| [EVE_LIB_BeginCoProList](#eve_lib_begincoprolist) | Begin a co-processor list. |
| [EVE_LIB_EndCoProList](#eve_lib_endcoprolist) | End the current co-processor list. |
| [EVE_LIB_AwaitCoProEmpty](#eve_lib_awaitcoproempty) | Waits for co-processor list to end. |
| [EVE_LIB_AwaitCoProEmptyTimeout](#eve_lib_awaitcoproemptytimeout) | Waits for co-processor list to end with a millisecond timeout. |
| [EVE_LIB_RecoverCoPro](#eve_lib_recovercopro) | Recovers the co-processor in the event of an exception. |
| [EVE_LIB_GetCoProSpace](#eve_lib_getcoprospace) | Returns the space remaining for further commands to be sent to the co-processor. |

### EVE_LIB_BeginCoProList

Begin a co-processor list.

| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---  | ---  | ---  | ---  | ---  |
| Yes  | Yes  | Yes  | Yes  | Yes  |

**Detailed description:**

Starts a co-processor list. Initialises the API and HAL ready to start
transmitting a co-processor list to the EVE.

This will typically assert chip select to allow the SPI interface to
send data to the EVE.

This must be called before any co-processor commands, display list commands,
and command buffer data is sent to the EVE device.

Certain API commands cannot be sent during a co-processor list as they 
perform standalone functions with EVE RAM_G or registers. These are clearly
marked in "details" section.

**Format:**

`void EVE_LIB_BeginCoProList(void)`

### EVE_LIB_EndCoProList

End the current co-processor list.
 
| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---  | ---  | ---  | ---  | ---  |
| Yes  | Yes  | Yes  | Yes  | Yes  |

**Detailed Description:** 

Ends a co-processor list. This will perform any operations in the API
and HAL to finish a co-processor list.

This will typically deasserts chip select after updating any registers
on the EVE device that will signal the end of the co-processor list.

This must be called after a call to [`EVE_LIB_BeginCoProList`](#eve_lib_begincoprolist) followed by
the co-processor command buffer data.

**Format:**

`void EVE_LIB_EndCoProList(void)`

### EVE_LIB_AwaitCoProEmpty

Waits for co-processor list to end.

| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---  | ---  | ---  | ---  | ---  |
| Yes  | Yes  | Yes  | Yes  | Yes  |

**Detailed Description:**

Will poll the co-processor command list until it has been completed.

This must be called after [`EVE_LIB_EndCoProList`](#eve_lib_endcoprolist) for the program to wait for the completion of the co-processor actions.

If the co-processor method is set to `EVE_COPROC_INT` then it will wait for an interrupt signal on the INT# line before testing for correct completion.

**Returns:** 

- 0 for successful completion.
- 0xff for co-processor exception.

**Format:**

`int EVE_LIB_AwaitCoProEmpty(void)`

Waits for the completion of the current commands sent to the co-processor.

### EVE_LIB_AwaitCoProEmptyTimeout

Waits for co-processor list to end with a millisecond timeout.

| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---  | ---  | ---  | ---  | ---  |
| Yes  | Yes  | Yes  | Yes  | Yes  |

**Detailed Description:**

Will poll the co-processor command list until it has been completed or a timeout has occurred.

This must be called after [`EVE_LIB_EndCoProList`](#eve_lib_endcoprolist) for the program to wait for the completion of the co-processor actions.

If the co-processor method is set to `EVE_COPROC_INT` then it will wait for an interrupt signal on the INT# line before testing for correct completion.

**Returns:** 

- 0 for successful completion.
- 0xfe for a timeout error.
- 0xff for co-processor exception.

**Format:**

`int EVE_LIB_AwaitCoProEmptyTimeout(uint32_t timeout)`

**Parameters:**

- *timeout* - milliseconds to wait until the function returns with a timeout error.

### EVE_LIB_RecoverCoPro

Recovers the co-processor in the event of an exception.

| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---  | ---  | ---  | ---  | ---  |
| Yes  | Yes  | Yes  | Yes  | Yes  |

**Detailed Description:**

Will reset the co-processor after an exception is reported by [`EVE_LIB_AwaitCoProEmpty`](#eve_lib_awaitcoproempty) or [`EVE_LIB_AwaitCoProEmptyTimeout`](#eve_lib_awaitcoproemptytimeout).

**Format:** 

`void EVE_LIB_RecoverCoPro(void)`

### EVE_LIB_GetCoProSpace

Returns the space remaining for further commands to be sent to the co-processor.

| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---  | ---  | ---  | ---  | ---  |
| Yes  | Yes  | Yes  | Yes  | Yes  |

**Detailed Description:**

Obtains the free space in the co-processor circular buffer. 
This operation may have an effect on the performance of the device.

This must be called during a co-processor list:
after a call to [`EVE_LIB_BeginCoProList`](#eve_lib_begincoprolist) and before a call to 
[`EVE_LIB_EndCoProList`](#eve_lib_endcoprolist).

**Returns:**

The number of free bytes in the co-processor circular buffer.

**Format:** 

`uint16_t EVE_LIB_GetCoProSpace(void)`

## Co-Processor Profiling

| Function | Overview |
| --- | --- |
| [EVE_Init](#eve_init) | Initialise the EVE library. |
| [EVE_Deinit](#eve_deinit) | Close the EVE library. |

### EVE_LIB_BeginCoProProfile

Resets the co-processor list profiling length.

| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---  | ---  | ---  | ---  | ---  |
| Yes  | Yes  | Yes  | Yes  | Yes  |

**NOTE:** This is only compiled if the macro `EVE_COPROC_PROFILE` is defined.

**Detailed Description:**

Sets the profiling pointer to zero to restart profiling.

This function can be called at any time.

**Format:** 

`void EVE_LIB_BeginCoProProfile(void)`

### EVE_LIB_GetCoProProfile

Size of co-processor list since last reset.

| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---  | ---  | ---  | ---  | ---  |
| Yes  | Yes  | Yes  | Yes  | Yes  |

**NOTE:** This is only compiled if the macro `EVE_COPROC_PROFILE` is defined.

**Detailed Description:**

Obtains the current profiling pointer for the co-processor list.

This function can be called at any time.

**Returns:** The number of instructions added to the co-processor list since 
the last reset of the profiling pointer.

**Format:** 

`uint16_t EVE_LIB_GetCoProProfile(void)`

### EVE_LIB_GetDlProfile

Size of display list.

| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---  | ---  | ---  | ---  | ---  |
| Yes  | Yes  | Yes  | Yes  | Yes  |

**NOTE:** This is only compiled if the macro `EVE_COPROC_PROFILE` is defined.

**Detailed Description:**

Obtains the current size of the display list.

This function cannot be used within a co-processor list.

**Returns:** The number of instructions instructions currently in the display list.

**Format:** 

`uint16_t EVE_LIB_GetDlProfile(void)`

## Co-Processor Helpers

| Function | Overview |
| --- | --- |
| [EVE_LIB_Int](#eve_lib_int) | Test interrupt input line. |
| [EVE_LIB_GetInterrupt](#eve_lib_getinterrupt) | Test if an interrupt flag is set. |
| [EVE_LIB_GetResult](#eve_lib_getresult) | Returns a result from the co-processor command buffer. |
| [EVE_LIB_GetCoProException](#eve_lib_getcoproexception) | Get co-processor exception description. |
| [EVE_LIB_ReadDataFromRAMG](#eve_lib_readdatafromramg) | Read a buffer from memory mapped RAM. |
| [EVE_LIB_WriteDataToRAMG](#eve_lib_writedatatoramg) | Write a buffer to memory mapped RAM. |
| [EVE_LIB_WriteDataToCMD](#eve_lib_writedatatocmd) | Write a buffer to the co-processor command memory. |
| [EVE_LIB_SendString](#eve_lib_sendstring) | Write a string the co-processor command memory. |
| [EVE_LIB_GetProps](#eve_lib_getprops) | Get properties of an CMD_LOADIMAGE operation. |
| [EVE_LIB_GetPtr](#eve_lib_getptr) | Get current allocation pointer. |
| [EVE_LIB_GetMatrix](#eve_lib_getmatrix) | Get the touchscreen transformation matrix. |
| [EVE_LIB_MemCrc](#eve_lib_memcrc) | Calculate the CRC of a memory area. |
| [EVE_LIB_BitmapTransform](#eve_lib_bitmaptransform) | Computes a bitmap transformation matrix. |
| [EVE_LIB_GetImage](#eve_lib_getimage) | Get image properties. |
| [EVE_LIB_RegRead](#eve_lib_regread) | Read a register. |
| [EVE_LIB_MemWrite8](#eve_lib_memwrite8) | Write an 8-bit memory location. |
| [EVE_LIB_MemWrite16](#eve_lib_memwrite16) | Write a 16-bit memory location. |
| [EVE_LIB_MemWrite32](#eve_lib_memwrite32) | Write a 32-bit memory location. |
| [EVE_LIB_MemRead8](#eve_lib_memread8) | Read an 8-bit memory location. |
| [EVE_LIB_MemRead16](#eve_lib_memread16) | Read a 16-bit memory location. |
| [EVE_LIB_MemRead32](#eve_lib_memread32) | Read a 32-bit memory location. |
| [EVE_CMD](#eve_cmd) | Send a single 32-bit value to the command buffer. |

### EVE_LIB_Int

Test interrupt input line.

| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---  | ---  | ---  | ---  | ---  |
| Yes  | Yes  | Yes  | Yes  | Yes  |

 **Detailed Description:**
 
 This function will check the interrupt input INT# from the EVE device.
 
This function can be called at any time.

**Returns:**

- zero if there is no interrupt.
- >0 if the EVE device is asserting an interrupt.
- -1 if the MCU or Platform does not support reading the interrupt line.

**Format:** 

`int EVE_LIB_Int(void)`

### EVE_LIB_GetInterrupt

Test if an interrupt flag is set.
 
| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---  | ---  | ---  | ---  | ---  |
| Yes  | Yes  | Yes  | Yes  | Yes  |

**NOTE:** This is only compiled if the co-processor method is set to `EVE_COPROC_INT`.

**Detailed Description:**

Will read the interrupt flag register `REG_INT_FLAGS` and add any newly pending to
a status value. The flag register will clear any pending interrupt
when read so the cumulative flagged bits are kept until they are
cleared by the mask in this function.

This function cannot be used within a co-processor list.

**Returns:**

- 0 for no interrupts in the mask being set.
- if any interrupts are set then the return value will contain bits set from the mask parameter.

**Format:** 

`uint8_t EVE_LIB_GetInterrupt(uint8_t mask)`

**Parameters:**

- *mask* - bit mask of interrupts to test. Unmasked interrupts are not modified.

### EVE_LIB_GetResult

Returns a result from the co-processor command buffer.

| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---  | ---  | ---  | ---  | ---  |
| Yes  | Yes  | Yes  | Yes  | Yes  |

**Detailed Description:**

Will return a result value from "offset" words back in the command buffer.
If the value of offset is 1 then the previous value from the co-processor
command buffer is returned.

This function cannot be used within a co-processor list.

**Returns:**

Result of a previous co-processor command.

**Format:** 

`uint32_t EVE_LIB_GetResult(int offset)`

**Parameters:**

- *offset* - number of 32-bit words to go back in the command buffer for the result.

### EVE_LIB_GetCoProException

Get co-processor exception description.

| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---  | ---  | ---  | ---  | ---  |
| *No* | *No* | Yes  | Yes  | Yes  |

**Detailed Description:**

Will query the co-processor exception description to a string.

This function cannot be used within a co-processor list.

**Returns:**

Co-processor exception description. This is a pointer to a string
and must be sufficient to hold 128 characters.

**Format:** 

`void EVE_LIB_GetCoProException(char *desc)`

**Parameters:**

- *desc* - buffer to receive the text of the exception description.

### EVE_LIB_WriteDataToRAMG

Write a buffer to memory mapped RAM.

| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---  | ---  | ---  | ---  | ---  |
| Yes  | Yes  | Yes  | Yes  | Yes  |

**Detailed Description:**

Writes a block of data via SPI to the EVE.

This function cannot be used within a co-processor list.

**Format:** 

`void EVE_LIB_WriteDataToRAMG(const uint8_t *ImgData, uint32_t DataSize, uint32_t DestAddress)`

**Parameters:**

- *ImgData* - pointer to start of data buffer.
- *DataSize* - number of bytes in buffer.
- *DestAddress* - 24-bit/32-bit memory mapped address on EVE.

### EVE_LIB_ReadDataFromRAMG

Read a buffer from memory mapped RAM.

| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---  | ---  | ---  | ---  | ---  |
| Yes  | Yes  | Yes  | Yes  | Yes  |

**Detailed Description:**

Reads a block of data via SPI from the EVE.

This function cannot be used within a co-processor list.

**Format:** 

`void EVE_LIB_ReadDataFromRAMG(uint8_t *ImgData, uint32_t DataSize, uint32_t SrcAddress)`

**Parameters:**

- *ImgData* - pointer to start of receive data buffer.
- *DataSize* - number of bytes to read (rounded up to be 32-bit aligned).
- *DestAddress* - 24-bit/32-bit memory mapped address on EVE.

### EVE_LIB_WriteDataToCMD

Write a buffer to the co-processor command memory.

| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---  | ---  | ---  | ---  | ---  |
| Yes  | Yes  | Yes  | Yes  | Yes  |

**Detailed Description:**

Writes a block of data via SPI to the EVE co-processor.

This must be part of a co-processor list. It will typically be called
after a co-processor command to provide data for the operation.
The data will be added to the co-processor command list therefore the
write will block on available space in this list.

**Format:** 

`void EVE_LIB_WriteDataToCMD(const uint8_t *ImgData, uint32_t DataSize)`

**Parameters:**

- *ImgData* - pointer to start of data buffer.
- *DataSize* - number of bytes in buffer.

### EVE_LIB_SendString

Write a string the co-processor command memory.

| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---  | ---  | ---  | ---  | ---  |
| Yes  | Yes  | Yes  | Yes  | Yes  |

**Detailed Description:**

Writes a string via SPI to the EVE co-processor.

This must be part of a co-processor list. It will typically be called
after a co-processor command to provide a string for the operation.
The data will be added to the co-processor command list therefore the
write will block on available space in this list.

**Returns:**

- size - The number of bytes of string written to the co-processor list. 
This is rounded up to the next 32-bit boundary.

**Format:** 

`uint16_t EVE_LIB_SendString(const char* string)`

**Parameters:**

- *string* - string to be sent to the co-processor memory.

### EVE_LIB_GetProps

Get properties of an CMD_LOADIMAGE operation.

| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---  | ---  | ---  | ---  | ---  |
| Yes  | Yes  | Yes  | Yes  | Yes  |

**Detailed Description:**

Obtains the details of an image decoded by the CMD_LOADIMAGE
co-processor command. The properties of the image are taken from
the co-processor command list.

This function cannot be used within a co-processor list.

**Format:** 

`void EVE_LIB_GetProps(uint32_t *addr, uint32_t *width, uint32_t *height)`

**Parameters:**

- *addr* - pointer to variable to receive the image start address.
- *width* - pointer to variable to receive the image width.
- *height* - pointer to variable to receive the image height.

### EVE_LIB_GetPtr

Get current allocation pointer.

| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---  | ---  | ---  | ---  | ---  |
| Yes  | Yes  | Yes  | Yes  | Yes  |

**Detailed Description:**

Obtains the automatic allocation pointer of the last address
used for certain co-processor operations.

This function cannot be used within a co-processor list.

**Format:** 

`void EVE_LIB_GetPtr(uint32_t *addr)`

**Parameters:**

- *addr* - last allocation address rounded up to the next 32-bit 
boundary.

### EVE_LIB_GetMatrix

Get the touchscreen transformation matrix.

| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---  | ---  | ---  | ---  | ---  |
| Yes  | Yes  | Yes  | Yes  | Yes  |

**Detailed Description:**

Obtains the transformation matrix from a CMD_CALIBRATE operation.

This function cannot be used within a co-processor list.

**Format:**

`void EVE_LIB_GetMatrix(uint32_t *a, uint32_t *b, uint32_t *c, uint32_t *d, uint32_t *e, uint32_t *f)`

**Parameters:**

- *a* -  pointer of variable to receive matrix a.
- *b* -  pointer of variable to receive matrix b.
- *c* -  pointer of variable to receive matrix c.
- *d* -  pointer of variable to receive matrix d.
- *e* -  pointer of variable to receive matrix e.
- *f* -  pointer of variable to receive matrix f.

### EVE_LIB_MemCrc

Calculate the CRC of a memory area.

| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---- | ---- | ---- | ---- | ---- |
| Yes  | Yes  | Yes  | Yes  | Yes  |

**Detailed Description:**

Obtains the CRC of a memory area.

This function cannot be used within a co-processor list.

**Format:**

`void EVE_LIB_MemCrc(uint32_t ptr, uint32_t num, uint32_t *result)`

**Parameters:**

- *ptr* - start of memory area.
- *num* - number of bytes to CRC.
- *result* - pointer to receive the CRC.

### EVE_LIB_BitmapTransform

Computes a bitmap transformation matrix.

| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---- | ---- | ---- | ---- | ---- |
| *No* | Yes  | Yes  | Yes  | Yes  |

**Detailed Description:**

It computes the transform given three corresponding points in screen space and bitmap space.

This function cannot be used within a co-processor list.

**Format:**

`void EVE_LIB_BitmapTransform( int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t tx0, int32_t ty0, int32_t tx1, int32_t ty1, int32_t tx2, int32_t ty2, uint32_t *result )`

**Parameters:**

- *x0, y0* - point 0 screen coordinate, in pixels.
- *x1, y1* - point 1 screen coordinate, in pixels.
- *x2, y2* - point 2 screen coordinate, in pixels.
- *tx0, ty0* - point 0 bitmap coordinate, in pixels.
- *tx1, ty1* - point 1 bitmap coordinate, in pixels.
- *tx2, ty2* - point 2 bitmap coordinate, in pixels.
- *result* - set to -1 on success, or 0 if it is not possible to find the solution matrix

### EVE_LIB_GetImage

Get image properties.

| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---- | ---- | ---- | ---- | ---- |
| *No* | *No* | *No* | Yes  | Yes  |

**Detailed Description:**

From the last CMD_LOADIMAGE get the address, size, format and palette of the loaded image.

This function cannot be used within a co-processor list.

**Format:**

`void EVE_LIB_GetImage(uint32_t *addr, uint32_t *fmt, uint32_t *width, uint32_t *height, uint32_t *palette)`

**Parameters:**

- *addr* - pointer to variable to receive the address the image was loaded to.
- *fmt* - pointer to variable to receive the format of the loaded image.
- *width* - pointer to variable to receive the width of the loaded image.
- *height* - pointer to variable to receive the height of the loaded image.
- *palette* - pointer to variable to receive the palette of the loaded image.

### EVE_LIB_RegRead

Read a register.

| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---- | ---- | ---- | ---- | ---- |
| *No* | *No* | *No* | *No* | Yes  |

**Detailed Description:**

Reads a register value.

This function cannot be used within a co-processor list.

**Format:**

`void EVE_LIB_RegRead(uint32_t addr, uint32_t *value)`

**Parameters:**

- *addr* - address of register to read.
- *value* - pointer to receive the contents of the register.

### EVE_LIB_MemWrite8

Write an 8-bit memory location.

| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---- | ---- | ---- | ---- | ---- |
| Yes  | Yes  | Yes  | Yes  | *No* |

**Detailed Description:**

Writes a memory location value.

This function cannot be used within a co-processor list.

**Format:**

`void EVE_LIB_MemWrite8(uint32_t addr, uint8_t value)`

**Parameters:**

- *addr* - address of memory location to write.
- *value* - value to write to memory.

### EVE_LIB_MemWrite16

Write a 16-bit memory location.

| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---- | ---- | ---- | ---- | ---- |
| Yes  | Yes  | Yes  | Yes  | *No* |

**Detailed Description:**

Writes a memory location value.

This function cannot be used within a co-processor list.

**Format:**

`void EVE_LIB_MemWrite16(uint32_t addr, uint16_t value)`

**Parameters:**

- *addr* - address of memory location to write.
- *value* - value to write to memory.

### EVE_LIB_MemWrite32

Write a 32-bit memory location.

| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---- | ---- | ---- | ---- | ---- |
| Yes  | Yes  | Yes  | Yes  | Yes  |

**NOTE:** EVE_LIB_MemWrite32 supports only 32-bit aligned writes on EVE5.

**Detailed Description:**

Writes a memory location value.

This function cannot be used within a co-processor list.

**Format:**

`void EVE_LIB_MemWrite32(uint32_t addr, uint32_t value)`

**Parameters:**

- *addr* - address of memory location to write.
- *value* - value to write to memory.

### EVE_LIB_MemRead8

Read an 8-bit memory location.

| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---- | ---- | ---- | ---- | ---- |
| Yes  | Yes  | Yes  | Yes  | *No* |

**Detailed Description:**

Reads a memory location value.

This function cannot be used within a co-processor list.

**Returns:**

- *value* - value read from memory.

**Format:**

`uint8_t EVE_LIB_MemRead8(uint32_t address)`

**Parameters:**

- *addr* - address of memory location to read.

### EVE_LIB_MemRead16

Read a 16-bit memory location.

| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---- | ---- | ---- | ---- | ---- |
| Yes  | Yes  | Yes  | Yes  | *No* |

**Detailed Description:**

Reads a memory location value.

This function cannot be used within a co-processor list.

**Returns:**

- *value* - value read from memory.

**Format:**

`uint16_t EVE_LIB_MemRead16(uint32_t address)`

**Parameters:**

- *addr* - address of memory location to read.
- *value* - pointer to receive the contents of the register.

### EVE_LIB_MemRead32

Read a 32-bit memory location.

| EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---- | ---- | ---- | ---- | ---- |
| Yes  | Yes  | Yes  | Yes  | Yes  |

**NOTE:** EVE_LIB_MemRead32 supports only 32-bit aligned reads on EVE5.

**Detailed Description:**

Reads a memory location value.

This function cannot be used within a co-processor list.

**Returns:**

- *value* - value read from memory.

**Format:**

`uint32_t EVE_LIB_MemRead32(uint32_t address)`

**Parameters:**

- *addr* - address of memory location to read.
- *value* - pointer to receive the contents of the register.

### EVE_CMD

Send a single 32-bit value to the command buffer.

**Detailed Description:**

Sends a command or word of data to the co-processor command buffer.

This function must be used within a co-processor list.

**Format:**

`void EVE_CMD(uint32_t c)`

**Parameters:**

- `c` - 32-bit command or data to send to the command buffer.

## EVE Display List Commands

The EVE display list commands are available in the EVE API.  
They are generally renamed with the prefix of `EVE_`.

Display list commands must be used within a co-processor list.

The following table shows the display list commands which are supported by each generation of EVE with the API name and the "Command Name" used in the Programming Guide for the EVE device. The format and implementation of the commands are described only in the [Programming Guide](#programming-guides). This list is grouped alphabetically.

| API Name               | Command Name       | EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ---------------------- | ------------------ | ---- | ---- | ---- | ---- | ---- |
| EVE_ALPHA_FUNC         | ALPHA_FUNC         | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_BEGIN              | CLEAR_COLOR_RGB    | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_BITMAP_EXT_FORMAT  | BITMAP_EXT_FORMAT  | *No* | *No* | Yes  | Yes  | Yes  |
| EVE_BITMAP_HANDLE      | BITMAP_HANDLE      | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_BITMAP_LAYOUT      | BITMAP_LAYOUT      | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_BITMAP_LAYOUT_H    | BITMAP_LAYOUT_H    | *No* | Yes  | Yes  | Yes  | Yes  |
| EVE_BITMAP_SIZE        | BITMAP_SIZE        | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_BITMAP_SIZE_H      | BITMAP_SIZE_H      | *No* | Yes  | Yes  | Yes  | Yes  |
| EVE_BITMAP_SOURCE      | BITMAP_SOURCE      | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_BITMAP_SOURCE2     | BITMAP_SOURCE2     | *No* | *No* | Yes  | Yes  | *No* |
| EVE_BITMAP_SOURCE_H    | BITMAP_SOURCE_H    | *No* | *No* | *No* | *No* | Yes  |
| EVE_BITMAP_SWIZZLE     | BITMAP_SWIZZLE     | *No* | *No* | Yes  | Yes  | Yes  |
| EVE_BITMAP_TRANSFORM_A | BITMAP_TRANSFORM_A | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_BITMAP_TRANSFORM_B | BITMAP_TRANSFORM_A | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_BITMAP_TRANSFORM_C | BITMAP_TRANSFORM_A | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_BITMAP_TRANSFORM_D | BITMAP_TRANSFORM_A | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_BITMAP_TRANSFORM_E | BITMAP_TRANSFORM_A | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_BITMAP_TRANSFORM_F | BITMAP_TRANSFORM_A | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_BITMAP_ZORDER      | BITMAP_ZORDER      | *No* | *No* | *No* | *No* | Yes  |
| EVE_BLEND_FUNC         | BLEND_FUNC         | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CALL               | CALL               | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CELL               | CELL               | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CLEAR              | CLEAR              | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CLEAR_COLOR        | CLEAR_COLOR        | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CLEAR_COLOR_A      | CLEAR_COLOR_A      | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CLEAR_COLOR_RGB    | CLEAR_COLOR_RGB    | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CLEAR_STENCIL      | CLEAR_STENCIL      | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_COLOR              | COLOR              | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_COLOR_A            | COLOR_A            | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_COLOR_MASK         | COLOR_MASK         | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_COLOR_RGB          | COLOR_RGB          | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CLEAR_TAG          | CLEAR_TAG          | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_DISPLAY            | DISPLAY            | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_END                | END                | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_JUMP               | JUMP               | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_LINE_WIDTH         | LINE_WIDTH         | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_MACRO              | MACRO              | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_NOP                | NOP                | *No* | Yes  | Yes  | Yes  | Yes  |
| EVE_PALETTE_SOURCE     | PALETTE_SOURCE     | *No* | Yes  | Yes  | Yes  | Yes  |
| EVE_PALLETE_SOURCE_H   | PALLETE_SOURCE_H   | *No* | *No* | *No* | *No* | Yes  |
| EVE_POINT_SIZE         | POINT_SIZE         | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_REGION             | REGION             | *No* | *No* | *No* | *No* | Yes  |
| EVE_RESTORE_CONTEXT    | RESTORE_CONTEXT    | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_RETURN             | RETURN             | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_SAVE_CONTEXT       | SAVE_CONTEXT       | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_SCISSOR_SIZE       | SCISSOR_SIZE       | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_SCISSOR_XY         | SCISSOR_XY         | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_STENCIL_FUNC       | STENCIL_FUNC       | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_STENCIL_MASK       | STENCIL_MASK       | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_STENCIL_OP         | STENCIL_OP         | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_TAG                | TAG                | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_TAG_MASK           | TAG_MASK           | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_VERTEX2F           | VERTEX2F           | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_VERTEX2II          | VERTEX2II          | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_VERTEX_FORMAT      | VERTEX_FORMAT      | *No* | Yes  | Yes  | Yes  | Yes  |
| EVE_VERTEX_TRANSLATE_X | VERTEX_TRANSLATE_X | *No* | Yes  | Yes  | Yes  | Yes  |
| EVE_VERTEX_TRANSLATE_Y | VERTEX_TRANSLATE_Y | *No* | Yes  | Yes  | Yes  | Yes  |

## EVE Co-processor Commands

The EVE co-processor commands are available in the EVE API.  
Like the display list commands they are generally renamed with the prefix of `EVE_` and must be used within a co-processor list.

The following table shows the co-processor commands which are supported by each generation of EVE with the API name and the "Command Name" used in the Programming Guide for the EVE device. The format and implementation of the commands are described only in the [Programming Guide](#programming-guides). This list is grouped by function and API support.

| API Name                 | Command Name         | EVE1 | EVE2 | EVE3 | EVE4 | EVE5 |
| ------------------------ | -------------------- | ---- | ---- | ---- | ---- | ---- |
| EVE_CMD_KEYS             | CMD_KEYS             | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_NUMBER           | CMD_NUMBER           | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_LOADIDENTITY     | CMD_LOADIDENTITY     | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_GAUGE            | CMD_GAUGE            | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_REGREAD          | CMD_REGREAD          | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_GETPROPS         | CMD_GETPROPS         | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_MEMCPY           | CMD_MEMCPY           | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_SPINNER          | CMD_SPINNER          | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_BGCOLOR          | CMD_BGCOLOR          | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_SWAP             | CMD_SWAP             | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_TRANSLATE        | CMD_TRANSLATE        | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_STOP             | CMD_STOP             | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_SLIDER           | CMD_SLIDER           | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_INTERRUPT        | CMD_INTERRUPT        | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_FGCOLOR          | CMD_FGCOLOR          | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_ROTATE           | CMD_ROTATE           | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_MEMWRITE         | CMD_MEMWRITE         | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_SCROLLBAR        | CMD_SCROLLBAR        | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_GETMATRIX        | CMD_GETMATRIX        | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_SKETCH           | CMD_SKETCH           | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_MEMSET           | CMD_MEMSET           | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_GRADCOLOR        | CMD_GRADCOLOR        | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_BITMAP_TRANSFORM | CMD_BITMAP_TRANSFORM | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_CALIBRATE        | CMD_CALIBRATE        | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_INFLATE          | CMD_INFLATE          | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_SETFONT          | CMD_SETFONT          | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_LOGO             | CMD_LOGO             | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_APPEND           | CMD_APPEND           | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_MEMZERO          | CMD_MEMZERO          | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_SCALE            | CMD_SCALE            | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_CLOCK            | CMD_CLOCK            | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_GRADIENT         | CMD_GRADIENT         | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_SETMATRIX        | CMD_SETMATRIX        | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_TRACK            | CMD_TRACK            | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_GETPTR           | CMD_GETPTR           | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_PROGRESS         | CMD_PROGRESS         | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_COLDSTART        | CMD_COLDSTART        | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_DIAL             | CMD_DIAL             | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_LOADIMAGE        | CMD_LOADIMAGE        | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_DLSTART          | CMD_DLSTART          | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_SNAPSHOT         | CMD_SNAPSHOT         | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_SCREENSAVER      | CMD_SCREENSAVER      | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_MEMCRC           | CMD_MEMCRC           | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_TEXT             | CMD_TEXT             | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_BUTTON           | CMD_BUTTON           | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_TOGGLE           | CMD_TOGGLE           | Yes  | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_CSKETCH          | CMD_CSKETCH          | *No* | Yes  | *No* | *No* | *No* |
| EVE_CMD_SETROTATE        | CMD_SETROTATE        | *No* | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_MEDIAFIFO        | CMD_MEDIAFIFO        | *No* | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_SYNC             | CMD_SYNC             | *No* | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_ROMFONT          | CMD_ROMFONT          | *No* | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_PLAYVIDEO        | CMD_PLAYVIDEO        | *No* | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_VIDEOFRAME       | CMD_VIDEOFRAME       | *No* | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_VIDEOSTART       | CMD_VIDEOSTART       | *No* | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_SETBASE          | CMD_SETBASE          | *No* | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_SETBITMAP        | CMD_SETBITMAP        | *No* | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_SETSCRATCH       | CMD_SETSCRATCH       | *No* | Yes  | Yes  | Yes  | Yes  |
| EVE_CMD_SETFONT2         | CMD_SETFONT2         | *No* | Yes  | Yes  | Yes  | *No* |
| EVE_CMD_SNAPSHOT2        | CMD_SNAPSHOT2        | *No* | Yes  | Yes  | Yes  | *No* |
| EVE_CMD_INFLATE2         | CMD_INFLATE2         | *No* | *No* | Yes  | Yes  | *No* |
| EVE_CMD_CLEARCACHE       | CMD_CLEARCACHE       | *No* | *No* | Yes  | Yes  | *No* |
| EVE_CMD_INTRAMSHARED     | CMD_INTRAMSHARED     | *No* | *No* | Yes  | Yes  | *No* |
| EVE_CMD_VIDEOSTARTF      | CMD_VIDEOSTARTF      | *No* | *No* | Yes  | Yes  | *No* |
| EVE_CMD_ANIMSTART        | CMD_ANIMSTART        | *No* | *No* | Yes  | Yes  | Yes  |
| EVE_CMD_ANIMSTOP         | CMD_ANIMSTOP         | *No* | *No* | Yes  | Yes  | Yes  |
| EVE_CMD_ANIMXY           | CMD_ANIMXY           | *No* | *No* | Yes  | Yes  | Yes  |
| EVE_CMD_ANIMDRAW         | CMD_ANIMDRAW         | *No* | *No* | Yes  | Yes  | Yes  |
| EVE_CMD_ANIMFRAME        | CMD_ANIMFRAME        | *No* | *No* | Yes  | Yes  | Yes  |
| EVE_CMD_APPENDF          | CMD_APPENDF          | *No* | *No* | Yes  | Yes  | Yes  |
| EVE_CMD_ANIMFRAMERAM     | CMD_ANIMFRAMERAM     | *No* | *No* | *No* | Yes  | *No* |
| EVE_CMD_ANIMSTARTRAM     | CMD_ANIMSTARTRAM     | *No* | *No* | *No* | Yes  | *No* |
| EVE_CMD_APILEVEL         | CMD_APILEVEL         | *No* | *No* | *No* | Yes  | *No* |
| EVE_CMD_FONTCACHE        | CMD_FONTCACHE        | *No* | *No* | *No* | Yes  | *No* |
| EVE_CMD_FONTCACHEQUERY   | CMD_FONTCACHEQUERY   | *No* | *No* | *No* | Yes  | *No* |
| EVE_CMD_HSF              | CMD_HSF              | *No* | *No* | *No* | Yes  | *No* |
| EVE_CMD_PCLKFREQ         | CMD_PCLKFREQ         | *No* | *No* | *No* | Yes  | *No* |
| EVE_CMD_RUNANIM          | CMD_RUNANIM          | *No* | *No* | *No* | Yes  | Yes  |
| EVE_CMD_TESTCARD         | CMD_TESTCARD         | *No* | *No* | *No* | Yes  | Yes  |
| EVE_CMD_WAIT             | CMD_WAIT             | *No* | *No* | *No* | Yes  | Yes  |
| EVE_CMD_NEWLIST          | CMD_NEWLIST          | *No* | *No* | *No* | Yes  | Yes  |
| EVE_CMD_ENDLIST          | CMD_ENDLIST          | *No* | *No* | *No* | Yes  | Yes  |
| EVE_CMD_CALLLIST         | CMD_CALLLIST         | *No* | *No* | *No* | Yes  | Yes  |
| EVE_CMD_RETURN           | CMD_RETURN           | *No* | *No* | *No* | Yes  | Yes  |
| EVE_CMD_NOP              | CMD_NOP              | *No* | *No* | Yes  | Yes  | Yes  |
| EVE_CMD_FILLWIDTH        | CMD_FILLWIDTH        | *No* | *No* | Yes  | Yes  | Yes  |
| EVE_CMD_ROTATEAROUND     | CMD_ROTATEAROUND     | *No* | *No* | Yes  | Yes  | Yes  |
| EVE_CMD_RESETFONTS       | CMD_RESETFONTS       | *No* | *No* | Yes  | Yes  | Yes  |
| EVE_CMD_GRADIENTA        | CMD_GRADIENTA        | *No* | *No* | Yes  | Yes  | Yes  |
| EVE_CMD_FLASHERASE       | CMD_FLASHERASE       | *No* | *No* | Yes  | Yes  | Yes  |
| EVE_CMD_FLASHWRITEEXT    | CMD_FLASHWRITEEXT    | *No* | *No* | Yes  | Yes  | Yes  |
| EVE_CMD_FLASHWRITE       | CMD_FLASHWRITE       | *No* | *No* | Yes  | Yes  | Yes  |
| EVE_CMD_FLASHUPDATE      | CMD_FLASHUPDATE      | *No* | *No* | Yes  | Yes  | Yes  |
| EVE_CMD_FLASHREAD        | CMD_FLASHREAD        | *No* | *No* | Yes  | Yes  | Yes  |
| EVE_CMD_FLASHPROGRAM     | CMD_FLASHPROGRAM     | *No* | *No* | Yes  | Yes  | Yes  |
| EVE_CMD_FLASHSOURCE      | CMD_FLASHSOURCE      | *No* | *No* | Yes  | Yes  | Yes  |
| EVE_CMD_FLASHSPITX       | CMD_FLASHSPITX       | *No* | *No* | Yes  | Yes  | Yes  |
| EVE_CMD_FLASHFAST        | CMD_FLASHFAST        | *No* | *No* | Yes  | Yes  | Yes  |
| EVE_CMD_FLASHSPIRX       | CMD_FLASHSPIRX       | *No* | *No* | Yes  | Yes  | Yes  |
| EVE_CMD_FLASHATTACH      | CMD_FLASHATTACH      | *No* | *No* | Yes  | Yes  | Yes  |
| EVE_CMD_FLASHDETATCH     | CMD_FLASHDETATCH     | *No* | *No* | Yes  | Yes  | Yes  |
| EVE_CMD_FLASHSPIDESEL    | CMD_FLASHSPIDESEL    | *No* | *No* | Yes  | Yes  | Yes  |
| EVE_CMD_GETIMAGE         | CMD_GETIMAGE         | *No* | *No* | *No* | Yes  | Yes  |
| EVE_CMD_CALIBRATESUB     | CMD_CALIBRATESUB     | *No* | *No* | *No* | Yes  | Yes  |
| EVE_CMD_COPYLIST         | CMD_COPYLIST         | *No* | *No* | *No* | *No* | Yes  |
| EVE_CMD_CGRADIENT        | CMD_CGRADIENT        | *No* | *No* | *No* | *No* | Yes  |
| EVE_CMD_TEXTDIM          | CMD_TEXTDIM          | *No* | *No* | *No* | *No* | Yes  |
| EVE_CMD_ARC              | CMD_ARC              | *No* | *No* | *No* | *No* | Yes  |
| EVE_CMD_RENDERTARGET     | CMD_RENDERTARGET     | *No* | *No* | *No* | *No* | Yes  |
| EVE_CMD_ENABLEREGION     | CMD_ENABLEREGION     | *No* | *No* | *No* | *No* | Yes  |
| EVE_CMD_FENCE            | CMD_FENCE            | *No* | *No* | *No* | *No* | Yes  |
| EVE_CMD_GRAPHICSFINISH   | CMD_GRAPHICSFINISH   | *No* | *No* | *No* | *No* | Yes  |
| EVE_CMD_REGWRITE         | CMD_REGWRITE         | *No* | *No* | *No* | *No* | Yes  |
| EVE_CMD_APBWRITE         | CMD_APBWRITE         | *No* | *No* | *No* | *No* | Yes  |
| EVE_CMD_APBREAD          | CMD_APBREAD          | *No* | *No* | *No* | *No* | Yes  |
| EVE_CMD_LOADWAV          | CMD_LOADWAV          | *No* | *No* | *No* | *No* | Yes  |
| EVE_CMD_LOADASSET        | CMD_LOADASSET        | *No* | *No* | *No* | *No* | Yes  |
| EVE_CMD_LOADPATCH        | CMD_LOADPATCH        | *No* | *No* | *No* | *No* | Yes  |
| EVE_CMD_GLOW             | CMD_GLOW             | *No* | *No* | *No* | *No* | Yes  |
| EVE_CMD_SDATTACH         | CMD_SDATTACH         | *No* | *No* | *No* | *No* | Yes  |
| EVE_CMD_FSOPTIONS        | CMD_FSOPTIONS        | *No* | *No* | *No* | *No* | Yes  |
| EVE_CMD_FSREAD           | CMD_FSREAD           | *No* | *No* | *No* | *No* | Yes  |
| EVE_CMD_FSSIZE           | CMD_FSSIZE           | *No* | *No* | *No* | *No* | Yes  |
| EVE_CMD_FSSOURCE         | CMD_FSSOURCE         | *No* | *No* | *No* | *No* | Yes  |
| EVE_CMD_FSDIR            | CMD_FSDIR            | *No* | *No* | *No* | *No* | Yes  |
| EVE_CMD_SDBLOCKREAD      | CMD_SDBLOCKREAD      | *No* | *No* | *No* | *No* | Yes  |
| EVE_CMD_WAITCHANGE       | CMD_WAITCHANGE       | *No* | *No* | *No* | *No* | Yes  |
| EVE_CMD_WAITCOND         | CMD_WAITCOND         | *No* | *No* | *No* | *No* | Yes  |
| EVE_CMD_RESULT           | CMD_RESULT           | *No* | *No* | *No* | *No* | Yes  |
| EVE_CMD_I2SSTARTUP       | CMD_I2SSTARTUP       | *No* | *No* | *No* | *No* | Yes  |
