# Bridgetek ### EVE LIB NAME ### Datavisualiser Example

## Datavisualiser Example

The `datavisualiser` example demonstrates detection of a touch event on a display item, and loading both a custom font and a bitmap into RAM_G. 

The custom font provides a character set that contains the numbers 0 to 9 resembling a 7 segment LED. The font is called DS-FONT. A touch event detected on the numbers will increase the number displayed. A BridgeTek logo is loaded as a bitmap and displayed above the counter.

The following is an screenshot of the datavisualiser example.

![Datavisualiser Example](docs/datavisualiser.png)

### `datavisualiser.ino`

The application starts up in the file `datavisualiser.ino` which provides initial MCU configuration and the program code for the example. 

The `datavisualiser.ino` code must provide any functions that rely on a platform's operating system, or built-in non-volatile storage mechanism. The required functions store and recall previous touch screen calibration settings:
- **platform_calib_init** initialise a platform's non-volatile storage system.
- **platform_calib_read** read a previous touch screen calibration or return a value indicating that there are no stored calibration setting.
- **platform_calib_write** write a touch screen calibration to the platform's non-volatile storage.
- **platform_get_time** get elapsed milliseconds since program start.

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
void eve_example(void)
{
    // Initialise the display
    EVE_DEBUG_PRINTF("Initialising display...\n");
    // Setup the EVE display (WUXGA)
    eve.setup(FULLHD);
    // Setup the EVE library
    eve.Init();

    // Enable audio amplifier
    EVE_DEBUG_PRINTF("Enabling audio amplifier...\n");
    sound_enable();

    // Calibrate the display
    Serial.print("Calibrating display...\n");
    if (eve_calibrate() != 0) {
      Serial.print("Exception...\n");
      return;
    }

    // Start example code
    EVE_DEBUG_PRINTF("Starting demo:\n");
    eve_display();          // Run Application
}
```

The call to `EVE_Init()` is made which sets up the EVE environment on the platform. This will initialise the SPI communications to the EVE device and set-up the device ready to receive communication from the host.

The `sound_enable()` function is then called from the `sound.ino` file (linked above) which configures a GPIO pin on EVE to enable the Audio Amplifier circurity commonly used on BridgeTek development modules. After which the sound synthesizer is set to play the MUTE sound.

Next, the function `eve_calibrate()` is called which uses the calibration co-processor command to display the calibration screen and asks the user to tap the three dots (see `touch.ino` below).

Once the precceeding steps are complete, the main loop is called which sits in a continuous loop within `eve_display()`. Each time round the loop, a screen is created using a co-processor list. 

### `touch.ino`

This function is used to show the touchscreen calibration screen and prompt the user to touch the screen at the required positions to generate an accurate transformation matrix. This matrix is used to translate the raw touch input into precise points on the screen.

The platform specific functions in `datavisualiser.ino` are called from this routine to store and read touchscreen calibration settings so that the user only needs to perform the action once.

## Files and Folders

The example contains a common directory with several files which comprises all the demo functionality.

| File/Folder | Description |
| --- | --- |
| [datavisualiser.ino](datavisualiser.ino) | Example source code file |
| [eve_example.ino](eve_example.ino) | Sketch file for example |
| [eve_example.h](eve_example.h) | Header file for examaple |
| [touch.ino](touch.ino) | Calibrations routines |
| [touch.h](touch.h) | Calibrations routines header file |
| [sound.ino](sound.ino) | Sound routines |
| [sound.h](sound.h) | Sound routines header file |
| [arcs.ino](arcs.ino) | Arc drawing routines |
| [arcs.h](arcs.h) | Arc drawing routines header file |
| [trig_furman.ino](trig_furman.ino) | Trigonometry routines |
| [trig_furman.h](trig_furman.h) | Trigonometry routines header file |
