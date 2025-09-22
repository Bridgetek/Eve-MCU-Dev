# EVE-MCU-Dev Simple Arduino Example

[Back](../README.md)

The sketch needs to be additional files copied to setup it up in the Arduino IDE. Since the name of the sketch needs to match the directory name where te sketch is located the example sketch is kept in a directory named [`simple_arduino`](simple_arduino).

## Setting Up the Arduino Example Sketch

The following files **must** be copied into the `simple_arduino` directory to be compiled into the sketch.

- From the `include` directory add the library header files:
  - EVE.h
  - MCU.h
  - HAL.h
  - EVE_config.h
  - FT8xx.h
  - BT81x.h
  - BT82x.h
  - FT80x.h
  - FT81x.h
- From the `source` directory add the library files:
  - EVE_API.c
  - EVE_HAL.c
- From the `ports\eve_bt82x` directory add the BT82x base patch (if BT82x will be used):
  - patch_base.c
  - patch_base.h
- From the `ports\eve_arch_arduino` directory add the MCU layer sketch for Arduino:
  - eve_arch_arduino.ino
- From the `examples\simple\common` directory the files for the simple example functionality:
  - eve_calibrate.c
  - eve_example.c
  - eve_example.h
  - eve_fonts.c
  - eve_helper.c
  - eve_images.c

The directory will look similar to this:

```
 Directory of <path>EVE-MCU-Dev\examples\simple\arduino\simple_arduino

BT81x.h
BT82x.h
EVE.h
EVE_API.c
eve_arch_arduino.ino
eve_calibrate.c
EVE_config.h
eve_example.c
eve_example.h
eve_fonts.c
EVE_HAL.c
eve_helper.c
eve_images.c
FT80x.h
FT81x.h
FT8xx.h
HAL.h
MCU.h
patch_base.c
patch_base.h
simple_arduino.ino
```
The files above will appear as tabs in the sketch in the Arduino IDE.

Finally, in the file `patch_base.c`, edit the file so that the variable `patchdata` is a global. This ensures that the data in the patch file is not stored on the stack but can be read from program memory.

```
const uint8_t patchdata[] =
{
    ...
};

int eve_loadpatch(void)
{
```

## Compiling the Simple Arduino Example

The sketch can be verified and uploaded to the Arduino device. Please follow the instructions in the Arduino IDE and web pages on Arduino web site.

## Reusing the Example Code

This code can be reused by altering the code in the "example" files `eve_example.c`, `eve_helper.c`, `eve_images.c`, `eve_fonts.c`, and `eve_calibrate.c`.

## Enhancements

The EVE library code can be made into an Arduino Library. The "example" files can be turned into a simple sketch rather than imported C code.
