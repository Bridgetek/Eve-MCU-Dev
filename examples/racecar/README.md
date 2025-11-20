# EVE-MCU-Dev Race Car Dashboard Example

[Back](../README.md)

## Race Car Dashboard Example

The `racecar` example demonstrates drawing a multi-function dashboard for a racing car. **This demo only works on the BT82x and BT81x devices.**

A racecar dashboard is drawn using blending, scissoring, arcs and custom fonts. The `racecar` code uses the `furman` snippet from the [snippets](../snippets) directory to calculate angles using furman trigonometry. Furman angles are an implementation of angles using only integer values to enable this demo to run on hardware which does not support floating point values. Refer to the BridgeTek Programming Guides for the EVE device for a full explanation of this method.

The example is intended to show a reimaginged dashboard for a racecar. External inputs would provide the data for the vehicle speed, engine RPM, acceleration, braking, track position, battery charge status and gear selection. This data is precomputed and stored in arrays in the program.

Graphics assets are stored in 4 different ways:
1  In the EVE device Flash. Seperate Flash images for BT81x and BT82x are required. Images are stored as ASTC compressed images. The Flash must be preprogrammed before using the example.
2  From an image of the Flash on the host PC file system.  Seperate Flash images for BT81x and BT82x are required. This method is not suitable for embedded applications where there is no file system.
3  From a C array containing ASTC compressed images. The image data are stored in C arrays and versions for BT82x and BT81x are conditionally compiled in the example.
4  From a C array containing PNG images. The image data are stored in C arrays and the example uses the EVE co-processor to decode the PNG files into uncompressed bitmaps.

The asset storage method is set in the `eve_example.c` file in the `ASSETS` macro. Change this macro in the file to choose the method. 

Each asset file for ASTC compressed images, PNG images and custom font files is a C source file with the data encoded into an array. The arrays and a counterpart constant `uint32_t` containing the size of the array are included as an `extern` in the `eve_example.c` file. This allows them to be compiled into the example. ASTC compressed images are different between BT82x and BT81x so two sets of arrays are include.

This example supports the following platforms:

| Port Name | Port Directory | Supported |
| --- | --- | --- |
|Raspberry Pi Pico | pico | Yes |
|Generic using libFT4222 | libmpsse | Yes |

Supported EVE APIs in this example:

| EVE API 1 | EVE API 2 | EVE API 3 | EVE API 4 | EVE API 5 |
| --- | --- | --- | --- | --- |
| No | No | Yes | Yes | Yes |

The following is an screenshot of the simple example.

![Race Car Dashboard Example](docs/racecar.png)

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
    EVE_Init();                 // Initialise the display

    eve_calibrate();            // Calibrate the display

    flash_full_speed();         // Ensure flash can enter Full speed

    set_asset_props();          // Configure asset properties for custom assets used in application

    eve_display_load_assets();  // Load assets into RAM_G

    eve_display();              // Run Application
 }
```
The call to `EVE_Init()` is made which sets up the EVE environment on the platform. This will initialise the SPI communications to the EVE device and set-up the device ready to receive communication from the host.

Next, the function `eve_calibrate()` is then called which uses the calibration co-processor command to display the calibration screen and asks the user to tap the three dots (see `eve_calibrate.c` below).

The `flash_full_speed()` funciton is called to ensure that any attached flash IC can sucessfuly entre FULL speed mode (this requires the .blob file to be present on the flash).

Following this the `set_asset_props()` function is called to configure specific properties for each asset (size, height, witdth, format, etc), which are then used in the `eve_display_load_assets()` funciton to load the assets into RAM_G fo use in the applciation.

Once the precceeding steps are complete, the main loop is called which sits in a continuous loop within `eve_display()`. Each time round the loop, a screen is created using a co-processor list. 

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
| [assets](assets) | Directory for storage of assets |
| [assets/eve5](assets/eve5) | Directory for storage of encoded ASTC compressed images for BT82x |
| [assets/eve34](assets/eve34) | Directory for storage of encoded ASTC compressed images for BT81x |
| [assets/png](assets/png) | Directory for storage of encoded PNG images |
| [assets/source](assets/source) | Directory for storage of source PNG files |
