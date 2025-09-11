# EVE-MCU-Dev Cruise Example

[Back](../README.md)

## Cruise Example

The `cruise` example demonstrates drawing multiple seven segment displays on the same screen, blending, scissoring, and trackers. The `cruise` code uses the `sevenseg` snippet from the [snippets](../snippets) directory to perform some of the drawing. 

The example is intended to show a reimaginged cruise control for a vehicle. The physical control for which would be a rotating turn wheel (with presumably a shaft encoder), an integrated click button (the whole rotating wheel physically registers a press) and a touchscreen display. An external input would provide a signal for when the vehicle would disengage cruise control, e.g. when the brakes are depressed. The touchscreen would be overridden and disabled in software by the click button.

The display comprises two parts: the left side is a circular display which is the operational part of the touchscreen on the physical control; the right side are touchscreen controls to mimic the function of the turn wheel, button click and brake input.

On screens of width/height 480 pixels only the circular control is shown. On wider screens the control buttons will be shown.

This example supports the following platforms:

| Port Name | Port Directory | Supported |
| --- | --- | --- |
|Bridgetek FT90x | ft900 | Yes |
|Bridgetek FT93x | ft900 | Yes |
|Raspberry Pi Pico | pico | Yes |
|Generic using libFT4222 | libmpsse | Yes |

Supported EVE APIs in this example:

| EVE API 1 | EVE API 2 | EVE API 3 | EVE API 4 | EVE API 5 |
| --- | --- | --- | --- | --- |
| Yes | Yes | Yes | Yes | Yes |

The following is an screenshot of the simple example.

![Cruise Example](docs/cruise.png)

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
| [common/eve_example.c](common/eve_example.c) | Example source code file |
| [common/eve_calibrate.c](common/eve_calibrate.c) | Calibrations routines |
| [common/eve_helper.c](common/eve_helper.c) | General helper routines (touch detection) |
| [docs](docs) | Documentation support files |
