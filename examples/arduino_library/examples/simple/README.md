# Bridgetek ### EVE LIB NAME ### Simple Example

## Simple Example

The `simple` example demonstrates detection of a touch event on a display item, and loading both a custom font and a bitmap into RAM_G. 

The custom font provides a character set that contains the numbers 0 to 9 resembling a 7 segment LED. The font is called DS-FONT. A touch event detected on the numbers will increase the number displayed. A BridgeTek logo is loaded as a bitmap and displayed above the counter.

The following is an screenshot of the simple example.

![Simple Example](docs/simple.png)

### `simple.ino`

The application starts up in the file `simple.ino` which provides initial MCU configuration and the program code for the example. 

The `simple.ino` code must provide any functions that rely on a platform's operating system, or built-in non-volatile storage mechanism. The required functions store and recall previous touch screen calibration settings:
- **platform_calib_init** initialise a platform's non-volatile storage system.
- **platform_calib_read** read a previous touch screen calibration or return a value indicating that there are no stored calibration setting.
- **platform_calib_write** write a touch screen calibration to the platform's non-volatile storage.

The `setup` function in the sketch is as follows:

```
void setup() {
  Serial.begin(9600);
}
```
The `loop` function in the sketch is as follows:
```
void loop() {
  // Initialise the display
  Serial.print("Starting EVE...\n");
  
  eve_example();
}
```

### `eve_example.ino`

The control program for the example is in `eve_example.ino`. The sets up the EVE Library `eve` and loads fonts and images.

```
void eve_example(void) {
  uint32_t font_end;

  // Setup the EVE library (### EVE RES ###)
  eve.setup(### EVE RES ###);
  // Initialise the display
  eve.Init();

  // Calibrate the display
  Serial.print("Calibrating display...\n");
  if (eve_calibrate() != 0) {
    Serial.print("Exception...\n");
    while (1)
      ;
  }

  // Load fonts and images
  Serial.print("Loading font...\n");
  font_end = eve_init_fonts();
  Serial.print("Loading images...\n");
  eve_load_images(font_end);

  Serial.print("Starting demo...\n");
  eve_display();
}
```

The call to `EVE_Init()` is made which sets up the EVE environment on the platform. This will initialise the SPI communications to the EVE device and set-up the device ready to receive communication from the host.

Next, the function `eve_calibrate()` is then called which uses the calibration co-processor command to display the calibration screen and asks the user to tap the three dots (see `eve_calibrate.c` below).

Once calibration is complete, the font for the counter and the image for the logo are both loaded  (see `eve_fonts.ino` and `eve_images.ino below).

Finally, the main program sits in a continuous loop within `eve_display()`. Each time round the loop, a screen is created using a co-processor list. 

### `touch.ino`

This function is used to show the touchscreen calibration screen and prompt the user to touch the screen at the required positions to generate an accurate transformation matrix. This matrix is used to translate the raw touch input into precise points on the screen.

The platform specific functions in `simple.ino` are called from this routine to store and read touchscreen calibration settings so that the user only needs to perform the action once.

### `eve_images.c`

The BridgeTek logo is stored in an array in this file as a JPEG image. The data from the array is sent to the
co-processor which converts it into a bitmap image suitable for use by the EVE device.

To convert the JPG to bitmap format the `eve.CMD_LOADIMAGE` command is sent followed by the JPG file data.
Note that the JPG file data must be padded to the next 32-bit boundary. The bitmap image is written into the 
RAM_G address specified in the command.

In the following code snippet from the example the `while` loop will look for the end marker of the JPG
data (0xff 0xd9) and finish the transmission of the data to the co-processor.

```
    eve.CMD_LOADIMAGE(start_addr, 0);
    while (flag != 2) {
        for (i = 0; i < sizeof(buf); i++) {
            buf[i] = pgm_read_byte(img++);
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
        eve.LIB_WriteDataToCMD(buf, (i + 3) & (~3));
    }
```

Once loaded into RAM_G the `eve.CMD_GETPROPS` command can find the size of the image.

```
    eve.LIB_GetProps(&eve_addr, &img_width, &img_height);
```

Using the information returned from `eve.CMD_GETPROPS` the bitmap is intialised.
The code will select bitmap mode with the `eve.BEGIN`, this tells the device that it is going to be processing bitmap instructions.
Then it will set the source address of the bitmap in RAM_G with `eve.BITMAP_SOURCE`. 
Next `eve.BITMAP_LAYOUT` and `eve.BITMAP_SIZE` commands tell the graphics device how to render the bitmaps included in the font.

```
    eve.CMD_DLSTART();
    eve.BEGIN(eve.BEGIN_BITMAPS);
    eve.BITMAP_HANDLE(FONT_CUSTOM);
    eve.BITMAP_SOURCE(start_addr & 0x3FFFFF);
    eve.BITMAP_LAYOUT(eve.FORMAT_RGB565, img_width * 2, img_height);
    eve.BITMAP_SIZE(eve.FILTER_NEAREST, eve.WRAP_BORDER, eve.WRAP_BORDER,
                    img_width, img_height);
    eve.END();
    eve.DISPLAY();
    eve.CMD_SWAP();
```

Some EVE devices have a larger address space and also have an `eve.BITMAP_SOURCE_H` command for the higher address bits.
Other EVE devices have `eve.BITMAP_LAYOUT_H` and `eve.BITMAP_SIZE_H` to cope with larger supported bitmap sizes.

A `eve.CMD_SWAP()` command **must** be called to register the bitmap handle on the device so that it can be used by 
subsequent display lists.

### `eve_fonts.ino`

This file contains the data array of a font which was produced by the Font Converter tool which is part of EVE Asset Builder. The code in this file will send the commands to the co-processor to load the font into RAM_G and set-up the font for use.

The font created from EVE Asset Builder will be located at a certain address in RAM_G. In the example here the font was at address 1000(decimal). This must be 32-bit aligned.

```
const uint32_t font0_offset = 1000; // Taken from commmand line
const uint8_t font0[] = { <data for font> };
```

The font data in the array `font0` is written into RAM_G using the API call `EVE_LIB_WriteDataToRAMG`. 
This function takes a pointer to an array in program memory and writes to RAM_G.

```
  /* Read the data from the program memory into RAM. */
  uint8_t pgm[16];
  uint32_t pgmoffset, pgmchunk;
  for (pgmoffset = 0; pgmoffset < font0_size; pgmoffset+=16)
  {
    // Maximum of pgm buffer
    uint32_t chunk = sizeof(pgm);
    if (pgmoffset + chunk > font0_size)
    {
      chunk = font0_size - pgmoffset;
    }
    // Load the pgm buffer
    memcpy_P(pgm, &font0[pgmoffset], chunk);
    eve.LIB_WriteDataToRAMG(pgm, chunk, font0_offset + pgmoffset);
  }
```

Once this has been written to RAM_G then the EVE device must be instructed how make a font use the data in RAM_G. 

```
  eve.CMD_DLSTART();
#if IS_EVE_API(5)
  eve.CMD_SETFONT(FONT_CUSTOM, font0_offset, 0);
#elif IS_EVE_API(2,3,4)
  eve.CMD_SETFONT2(FONT_CUSTOM, font0_offset, 0);
#else
  eve.BEGIN(eve.BEGIN_BITMAPS);
  eve.BITMAP_HANDLE(FONT_CUSTOM);
      eve.BITMAP_SOURCE((font0_hdr->PointerToFontGraphicsData)&(0x3FFFFF));
  eve.BITMAP_LAYOUT(font0_hdr->FontBitmapFormat,
            font0_hdr->FontLineStride, font0_hdr->FontHeightInPixels);
  eve.BITMAP_SIZE(eve.FILTER_NEAREST, eve.WRAP_BORDER, eve.WRAP_BORDER,
            font0_hdr->FontWidthInPixels,
            font0_hdr->FontHeightInPixels);
  eve.CMD_SETFONT(FONT_CUSTOM, font0_offset);
#endif
  eve.DISPLAY();
  eve.CMD_SWAP();
```

A font must be set-up on the device before the font can be used. A font is essentially a set of bitmaps, 
one for each character. The bitmap parameters are therefore needed to be setup in the same way as other
bitmaps are. The device needs the `eve.BITMAP_SOURCE`, `eve.BITMAP_LAYOUT` and `eve.BITMAP_SIZE` commands 
to properly render the font's characters. 

#if IS_EVE_API(5)
The `eve.CMD_SETFONT` command will initialise the bitmap settings for the font using information in the font
data structure and associate the font with the handle.
#elif IS_EVE_API(2)
The `eve.CMD_SETFONT2` command will initialise the bitmap settings for the font using information in the font
data structure and associate the font with the handle.
#elif IS_EVE_API(3,4)
The `eve.CMD_SETFONT2` command will initialise the bitmap settings for the font using information in the font
data structure and associate the font with the handle.
#endif
#if IS_EVE_API(2,3,4,5)
The `eve.BITMAP_SOURCE`, `eve.BITMAP_LAYOUT` and `eve.BITMAP_SIZE` commands are generated automatically by
the co-processor using data from the font structure and are therefore not required.
#else
The required information for setting up the font must be taken from the font structure. In this
example it uses a cast to the data structure cast as `font0_hdr`. 
Finally, the `eve.CMD_SETFONT` command will initialise the font on the device and associate it with the 
supplied handle.
#endif

A `eve.CMD_SWAP()` command **must** be called to register the font handle on the device so that it can be used by 
subsequent display lists.

## Files and Folders

The example contains a common directory with several files which comprises all the demo functionality.

| File/Folder | Description |
| --- | --- |
| [simple.ino](simple.ino) | Example source code file |
| [eve_example.ino](eve_example.ino) | Sketch file for example |
| [eve_example.h](eve_example.h) | Header file for examaple |
| [touch.ino](touch.ino) | Calibrations routines |
| [touch.h](touch.h) | Calibrations routines header file |
| [eve_fonts.ino](eve_fonts.ino) | Font helper routines |
| [eve_images.ino](eve_images.ino) | Image helper routines |
