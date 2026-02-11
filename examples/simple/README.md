# EVE-MCU-Dev Simple Example

[Back](../README.md)

## Simple Example

The `simple` example demonstrates detection of a touch event on a display item, and loading both a custom font and a bitmap into RAM_G. 

The custom font provides a character set that contains the numbers 0 to 9 resembling a 7 segment LED. The font is called DS-FONT. A touch event detected on the numbers will increase the number displayed. A BridgeTek logo is loaded as a bitmap and displayed above the counter.

This example supports the following platforms:

| Port Name | Port Directory | 
| --- | --- | 
|Bridgetek FT9xx | [ft900](ft900/README.md) | 
|Beaglebone Black | [BeagleBone](BeagleBone/README.md) (1) | 
|Expressif ESP32 | [ESP32](ESP32/README.md) | 
|TI MSP430 | [MSP430](MSP430/README.md) (1) |
|TI MSPM0 | [MSPM0](MSPM0/README.md) (1) | 
|Microchip PIC18F | [PIC18F](PIC18F/README.md) (2) | 
|NXP K64 | [NXP_K64](NXP_K64/README.md) (2) | 
|ST STM32 (Keil) | [STM32](STM32/README.md) | 
|ST STM32Cube | [STM32CUBE](STM32CUBE/README.md) | 
|Raspberry Pi | [raspberry_pi](raspberry_pi/README.md) | 
|Raspberry Pi Pico | [pico](pico/README.md) | 
|Generic using libMPSSE CMake | [libft4222](libft4222/README.md) | 
|Generic using libMPSSE Visual Studio | [libft4222](libft4222/README.md) | 
|Generic using libFT4222 CMake | [libmpsse](libmpsse/README.md) | 
|Generic using libFT4222 Visual Studio | [libmpsse](libmpsse/README.md) | 

- (1) Hardware testing ongoing.
- (2) Build environment incomplete.
 
Supported EVE APIs in this example:

| EVE API 1 | EVE API 2 | EVE API 3 | EVE API 4 | EVE API 5 |
| --- | --- | --- | --- | --- |
| Yes | Yes | Yes | Yes | Yes |

The following is an screenshot of the simple example.

![Simple Example](docs/simple.png)

### `main.c`

The application starts up in the file `main.c` which provides initial MCU configuration and then calls `eve_example.c` where the remainder of the application will be carried out. 

The `main.c` code is platform specific. It must provide any functions that rely on a platform's operating system, or built-in non-volatile storage mechanism. The required functions store and recall previous touch screen calibration settings:
- **platform_calib_init** initialise a platform's non-volatile storage system.
- **platform_calib_read** read a previous touch screen calibration or return a value indicating that there are no stored calibration setting.
- **platform_calib_write** write a touch screen calibration to the platform's non-volatile storage.

The example program in the common code is then called.

### `eve_example.c`

In the function `eve_example` the basic format is as follows:

```
void eve_example(void)
{
    uint32_t font_end;
    // Initialise the display
    EVE_Init();
    // Calibrate the display
    eve_calibrate();
    // Load fonts and images
    font_end = eve_init_fonts();
    eve_load_images(font_end);
    // Start example code
    eve_display();
 }
```
The call to `EVE_Init()` is made which sets up the EVE environment on the platform. This will initialise the SPI communications to the EVE device and set-up the device ready to receive communication from the host.

Next, the function `eve_calibrate()` is then called which uses the calibration co-processor command to display the calibration screen and asks the user to tap the three dots (see `touch.c` below).

Once calibration is complete, the font for the counter and the image for the logo are both loaded  (see `eve_fonts.c` and `eve_images.c` below).
Finally, the main program sits in a continuous loop within `eve_display()`. Each time round the loop, a screen is created using a co-processor list. 

### `touch.c`

This function is used to show the touchscreen calibration screen and prompt the user to touch the screen at the required positions to generate an accurate transformation matrix. This matrix is used to translate the raw touch input into precise points on the screen.

The platform specific functions in `main.c` are called from this routine to store and read touchscreen calibration settings so that the user only needs to perform the action once.

Another function of this file is to read a single touch tag from the screen.

```
    Read_tag = EVE_LIB_MemRead32(EVE_REG_TOUCH_TAG);
    if ((EVE_LIB_MemRead32(EVE_REG_TOUCH_RAW_XY) & 0xffff) != 0xffff)
    {
        key_detect = 1;
        *key = Read_tag;
    }
```

A TAG event is read from the EVE_REG_TOUCH_TAG register. This is verified by reading the EVE_REG_TOUCH_RAW_XY register. 
If that register indicates a valid touch then this is flagged to the calling program.

### `eve_images.c`

The BridgeTek logo is stored in an array in this file as a JPEG image. The data from the array is sent to the
co-processor which converts it into a bitmap image suitable for use by the EVE device.

To convert the JPG to bitmap format the `EVE_CMD_LOADIMAGE` command is sent followed by the JPG file data.
Note that the JPG file data must be padded to the next 32-bit boundary. The bitmap image is written into the 
RAM_G address specified in the command.

In the following code snippet from the example the `while` loop will look for the end marker of the JPG
data (0xff 0xd9) and finish the transmission of the data to the co-processor.

```
    EVE_CMD_LOADIMAGE(start_addr, 0);
    while (flag != 2) {
        for (i = 0; i < sizeof(buf); i++) {
            buf[i] = *img++;
            if (buf[i] == 0xff) {
                flag = 1;
            } else {
                if (flag == 1) {
                    if (buf[i] == 0xd9) {
                        flag = 2;
                        i++;
                        break;
                    }
                }
                flag = 0;
            }
        }
        EVE_LIB_WriteDataToCMD(buf, (i + 3) & (~3));
    }
```

Once loaded into RAM_G the `EVE_CMD_GETPROPS` command can find the size of the image.

```
    EVE_LIB_GetProps(&eve_addr, &img_width, &img_height);
```

Using the information returned from `EVE_CMD_GETPROPS` the bitmap is intialised.
The code will select bitmap mode with the `EVE_BEGIN`, this tells the device that it is going to be processing bitmap instructions.
Then it will set the source address of the bitmap in RAM_G with `EVE_BITMAP_SOURCE`. 
Next `EVE_BITMAP_LAYOUT` and `EVE_BITMAP_SIZE` commands tell the graphics device how to render the bitmaps included in the font.

```
    EVE_CMD_DLSTART();
    EVE_BEGIN(EVE_BEGIN_BITMAPS);
    EVE_BITMAP_HANDLE(FONT_CUSTOM);
    EVE_BITMAP_SOURCE(start_addr & 0x3FFFFF);
    EVE_BITMAP_LAYOUT(EVE_FORMAT_RGB565, img_width * 2, img_height);
    EVE_BITMAP_SIZE(EVE_FILTER_NEAREST, EVE_WRAP_BORDER, EVE_WRAP_BORDER,
                    img_width, img_height);
    EVE_END();
    EVE_DISPLAY();
    EVE_CMD_SWAP();
```

Some EVE devices have a larger address space and also have an `EVE_BITMAP_SOURCE_H` command for the higher address bits.
Other EVE devices have `EVE_BITMAP_LAYOUT_H` and `EVE_BITMAP_SIZE_H` to cope with larger supported bitmap sizes.

A call to the `eve.CMD_SWAP()` command **must** be made within the same co-processor list to register 
the bitmap handle on the device so that it can be used by subsequent display lists.

### `eve_fonts.c`

This file contains the data array of a font which was produced by the Font Converter tool which is part of EVE Asset Builder. The code in this file will send the commands to the co-processor to load the font into RAM_G and set-up the font for use.

The font created from EVE Asset Builder will be located at a certain address in RAM_G. In the example here the font was at address 1000(decimal). This must be 32-bit aligned.

```
const uint32_t font0_offset = 1000; // Taken from commmand line
const uint8_t font0[] = { <data for font> };
```

The font data in the array `font0` is directly written into RAM_G using the API call `EVE_LIB_WriteDataToRAMG`. 
This function takes a pointer to an array and writes to RAM_G.

```
    EVE_LIB_WriteDataToRAMG(font0, font0_size, font0_offset);
```

Once this has been written to RAM_G then the EVE device must be instructed how make a font use the data in RAM_G. 

```
    EVE_CMD_DLSTART();
#if IS_EVE_API(5)
    EVE_CMD_SETFONT(FONT_CUSTOM, font0_offset, 0);
#elif IS_EVE_API(2,3,4)
    EVE_CMD_SETFONT2(FONT_CUSTOM, font0_offset, 0);
#else
    EVE_BEGIN(EVE_BEGIN_BITMAPS);
    EVE_BITMAP_HANDLE(FONT_CUSTOM);
    EVE_BITMAP_SOURCE((font0_hdr->PointerToFontGraphicsData)&(0x3FFFFF));
    EVE_BITMAP_LAYOUT(font0_hdr->FontBitmapFormat,
            font0_hdr->FontLineStride, font0_hdr->FontHeightInPixels);
    EVE_BITMAP_SIZE(EVE_FILTER_NEAREST, EVE_WRAP_BORDER, EVE_WRAP_BORDER,
            font0_hdr->FontWidthInPixels,
            font0_hdr->FontHeightInPixels);
    EVE_CMD_SETFONT(FONT_CUSTOM, font0_offset);
#endif
    EVE_DISPLAY();
    EVE_CMD_SWAP();
```

A font must be set-up on the device before the font can be used. A font is essentially a set of bitmaps, 
one for each character. The bitmap parameters are therefore needed to be setup in the same way as other
bitmaps are. The device needs the `eve.BITMAP_SOURCE`, `eve.BITMAP_LAYOUT` and `eve.BITMAP_SIZE` commands 
to properly render the font's characters. 

On BT82x, using the `eve.CMD_SETFONT` command, and on FT81x, BT88x, BT81x, using the `eve.CMD_SETFONT2` 
command, the co-processor will initialise the bitmap settings for the font using information in the font
data structure and associate the font with the handle.

On FT80x the required information for setting up the font must be taken from the font structure. In this
example it uses a cast to the data structure cast as `font0_hdr`. 
Finally, the `eve.CMD_SETFONT` command will initialise the font on the device and associate it with the 
supplied handle.

A call to the `eve.CMD_SWAP()` command **must** be made within the same co-processor list to register 
the font handle on the device so that it can be used by subsequent display lists.

## Files and Folders

The example contains a common directory with several files which comprises all the demo functionality.

| File/Folder | Description |
| --- | --- |
| [common/eve_example.c](common/eve_example.c) | Example source code file |
| [snippets/touch.c](../snippets/touch.c) | Calibration and touch detection routines |
| [common/eve_fonts.c](common/eve_fonts.c) | Font helper routines |
| [common/eve_images.c](common/eve_images.c) | Image helper routines |
| [docs](docs) | Documentation support files |
