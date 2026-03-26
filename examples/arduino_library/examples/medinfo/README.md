# Bridgetek ### EVE LIB NAME ### MedInfo Example

## MedInfo Example

The `medinfo` example demonstrates detection of a touch event on a display item, and loading both a custom font and a bitmap into RAM_G. 

The custom font provides a character set that contains the numbers 0 to 9 resembling a 7 segment LED. The font is called DS-FONT. A touch event detected on the numbers will increase the number displayed. A BridgeTek logo is loaded as a bitmap and displayed above the counter.

The following is an screenshot of the medinfo example.

![Medinfo Example](docs/medinfo.png)

### `medinfo.ino`

The application starts up in the file `medinfo.ino` which provides initial MCU configuration and the program code for the example. 

The `medinfo.ino` code must provide any functions that rely on a platform's operating system, or built-in non-volatile storage mechanism. The required functions store and recall previous touch screen calibration settings:
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

The platform specific functions in `medinfo.ino` are called from this routine to store and read touchscreen calibration settings so that the user only needs to perform the action once.

## Files and Folders

The example contains a common directory with several files which comprises all the demo functionality.

| File/Folder | Description |
| --- | --- |
| [medinfo.ino](medinfo.ino) | Example source code file |
| [eve_example.ino](eve_example.ino) | Sketch file for example |
| [eve_example.h](eve_example.h) | Header file for examaple |
| [touch.ino](touch.ino) | Calibrations routines |
| [touch.h](touch.h) | Calibrations routines header file |
| [bad.ino](bad.ino) | Encoded PNG file |
| [dialogs.h](dialogs.h) | Dialog routines header file |
| [dialogs.ino](dialogs.ino) | Dialog routines |
| [sevenseg.h](sevenseg.h) | Seven segment routines header file |
| [sevenseg.ino](sevenseg.ino) | Seven segment routines |
| [ecg_heart.ino](ecg_heart.ino) | Encoded PNG file |
| [eurostile_150_L8.ino](eurostile_150_L8.ino) | Encoded font file |
| [eve_assetload_array.ino](eve_assetload_array.ino) | Routines to load assets from arrays |
| [eve_graph_vertexes.ino](eve_graph_vertexes.ino) | Routines to draw graphs |
| [eve_tracedata.ino](eve_tracedata.ino) | Encoded trace data for graphs |
| [good.ino](good.ino) | Encoded PNG file |
| [lock.ino](lock.ino) | Encoded PNG file |
| [lock_open_right.ino](lock_open_right.ino) | Encoded PNG file |
| [off_on.ino](off_on.ino) | Encoded PNG file |
| [patch_medinfo2.ino](patch_medinfo2.ino) | Encoded patch file for BT82x |
| [pause_circle.ino](pause_circle.ino) | Encoded PNG file |
| [play_circle.ino](play_circle.ino) | Encoded PNG file |
| [sick.ino](sick.ino) | Encoded PNG file |
| [silence.ino](silence.ino) | Encoded PNG file |
| [volume_down.ino](volume_down.ino) | Encoded PNG file |
| [volume_off.ino](volume_off.ino) | Encoded PNG file |
| [volume_up.ino](volume_up.ino) | Encoded PNG file |