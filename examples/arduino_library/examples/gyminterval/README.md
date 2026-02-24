# Bridgetek ### EVE LIB NAME ### GymInterval Example

## GymInterval Example

## GymInterval Example

The `gyminterval` example demonstrates drawing a countdown timer for interval training in a gym. 

An animated countdown timer is drawn using arcs, text scaling and simple animation. The tracker feature is used for setting up the example. 

There are two pages displayed during the example. The first is the setup screen showing the number and length of the intervals, the second is the countdown timer.

The interval timer allows a variable number of "sets" which are repeated a by the number of "cycles". Rest times between sets and cycles can be changed to suit the workout.

### Setup Screen

The number of sets and cycles, along with the length of each set and the reset times between each set and cycle is setup. To change one of the 5 parameters click on the setting to change then adjust the counter or time using the scroll-wheel surrounding the "GO" button.

The following is an screenshot of the gyminterval setup screen.

![GymInterval Setup Screen Example](docs/setup.png)

### Timer Screen

Once the timer has started then the countdown will be shown as a time in the centre of the timer and as a decreasing light blue arc. The sets are represented by the green arc segments and the cycles by the red arc settings.

![GymInterval Start Example](docs/start.png)

![GymInterval Timer Example](docs/timer.png)

### `gyminterval.ino`

The application starts up in the file `gyminterval.ino` which provides initial MCU configuration and the program code for the example. 

The `gyminterval.ino` code must provide any functions that rely on a platform's operating system, or built-in non-volatile storage mechanism. The required functions store and recall previous touch screen calibration settings:
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
void eve_example(void) {
  uint32_t font_end;

  // Setup the EVE library (### EVE RES ###)
  eve.setup(### EVE RES ###);
  // Initialise the display
  eve.Init();

  // Get a cache of the ROM font we want to use.
  font_getfontinforom(&clockfont, EVE_ROMFONT_MAX);
    
    <...more font initialisation...>
    
  // Calibrate the display
  Serial.print("Calibrating display...\n");
  if (eve_calibrate() != 0) {
    Serial.print("Exception...\n");
    while (1)
      ;
  }

  Serial.print("Starting demo...\n");
  eve_display();
}
```

The call to `EVE_Init()` is made which sets up the EVE environment on the platform. This will initialise the SPI communications to the EVE device and set-up the device ready to receive communication from the host.

Next, the function `eve_calibrate()` is then called which uses the calibration co-processor command to display the calibration screen and asks the user to tap the three dots (see `eve_calibrate.c` below).

Finally, the main program sits in a continuous loop within `eve_display()`. Each time round the loop, a screen is created using a co-processor list. 

### `touch.ino`

This function is used to show the touchscreen calibration screen and prompt the user to touch the screen at the required positions to generate an accurate transformation matrix. This matrix is used to translate the raw touch input into precise points on the screen.

The platform specific functions in `simple.ino` are called from this routine to store and read touchscreen calibration settings so that the user only needs to perform the action once.

## Files and Folders

The example contains a common directory with several files which comprises all the demo functionality.

| File/Folder | Description |
| --- | --- |
| [gyminterval.ino](simple.ino) | Example source code file |
| [eve_example.ino](eve_example.ino) | Sketch file for example |
| [eve_example.h](eve_example.h) | Header file for examaple |
| [touch.ino](touch.ino) | Calibrations routines |
| [touch.h](touch.h) | Calibrations routines header file |
| [sound.ino](sound.ino) | Sound routines |
| [sound.h](sound.h) | Sound routines header file |
| [fonts.ino](fonts.ino) | Font routines |
| [fonts.h](fonts.h) | Font routines header file |
| [arcs.ino](arcs.ino) | Arc drawing routines |
| [arcs.h](arcs.h) | Arc drawing routines header file |
| [trig_furman.ino](trig_furman.ino) | Trigonometry routines |
| [trig_furman.h](trig_furman.h) | Trigonometry routines header file |
