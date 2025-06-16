# EVE-MCU-Dev Cruise Example

[Back](../README.md)

## Cruise Example

The `cruise.py` example demonstrates drawing multiple seven segment displays on the same screen, blending, scissoring, and trackers. The `cruise.py` code uses the `sevensegment.py` widget from the [common](../common) directory to perform some of the drawing. 

The example is intended to show a reimaginged cruise control for a vehicle. The physical control for which would be a rotating turn wheel (with presumably a shaft encoder), an integrated click button (the whole rotating wheel physically registers a press) and a touchscreen display. An external input would provide a signal for when the vehicle would disengage cruise control, e.g. when the brakes are depressed. The touchscreen would be overridden and disabled in software by the click button.

The display comprises two parts: the left side is a circular display which is the operational part of the touchscreen on the physical control; the right side are touchscreen controls to mimic the function of the turn wheel, button click and brake input.

![Cruise Example](docs/cruise.png)

### `main.c`

The application starts up in the file `main.c` which provides initial MCU configuration and then calls `cruise.c` where the remainder of the application will be carried out. 

The `main.c` code is platform specific. It must provide any functions that rely on a platform's operating system, or built-in non-volatile storage mechanism. The required functions store and recall previous touch screen calibration settings:
- **platform_calib_init** initialise a platform's non-volatile storage system.
- **platform_calib_read** read a previous touch screen calibration or return a value indicating that there are no stored calibration setting.
- **platform_calib_write** write a touch screen calibration to the platform's non-volatile storage.

The example program in the common code is then called.

### `cruise.c`

In the function `cruise` the basic format is as follows:

```
void cruise(void)
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

Next, the function `eve_calibrate()` is then called which uses the calibration co-processor command to display the calibration screen and asks the user to tap the three dots (see `eve_calibrate.c` below).

Once calibration is complete, the main loop is called which sits in a continuous loop within `eve_display()`. Each time round the loop, a screen is created using a co-processor list. 

### `eve_calibrate.c`

This function is used to show the touchscreen calibration screen and prompt the user to touch the screen at the required positions to generate an accurate transformation matrix. This matrix is used to translate the raw touch input into precise points on the screen.

The platform specific functions in `main.c` are called from this routine to store and read touchscreen calibration settings so that the user only needs to perform the action once.

## Files and Folders

The example contains a common directory with several files which comprises all the demo functionality.

| File/Folder | Description |
| --- | --- |
| [common/cruise.c](common/exe_example.c) | Example source code file |
| [common/exe_calibrate.c](common/exe_calibrate.c) | Calibrations routines |
| [common/exe_helper.c](common/exe_helper.c) | General helper routines (touch detection) |
| [docs](docs) | Documentation support files |
