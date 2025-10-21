# EVE-MCU-Dev Simple Arduino Example

[Back](../README.md)

The sketch needs to be additional files copied to setup it up in the Arduino IDE. Since the name of the sketch needs to match the directory name where the sketch is located the example sketch is kept in a special sketch directory named [`simple_arduino`](simple_arduino/).

## Sketch Directory

There are two methods of setting up the sketch directory. A python script is provided and manual instructions are also provided.

### Python Script

The python script `sketchbuild.py` can be used to copy the required files to the sketch automatically. It is run from the directory where the script is located and will find the API and example common directory.

The script will report the names of the files copied. There are no parameters for the script.

```
> python .\sketchbuild.py
Sketch name is simple_arduino
..\common\eve_example.c -> simple_arduino\eve_example.c
..\common\eve_example.h -> simple_arduino\eve_example.h
..\common\eve_calibrate.c -> simple_arduino\eve_calibrate.c
..\common\eve_fonts.c -> simple_arduino\eve_fonts.c
..\common\eve_helper.c -> simple_arduino\eve_helper.c
..\common\eve_images.c -> simple_arduino\eve_images.c
..\..\..\source\EVE_API.c -> simple_arduino\EVE_API.c
..\..\..\source\EVE_HAL.c -> simple_arduino\EVE_HAL.c
..\..\..\ports\eve_arch_arduino\eve_arch_arduino.ino -> simple_arduino\eve_arch_arduino.ino
..\..\..\ports\eve_bt82x\patch_base.c -> simple_arduino\patch_base.c
..\..\..\ports\eve_bt82x\patch_base.h -> simple_arduino\patch_base.h
..\..\..\include\EVE.h -> simple_arduino\EVE.h
..\..\..\include\HAL.h -> simple_arduino\HAL.h
..\..\..\include\MCU.h -> simple_arduino\MCU.h
..\..\..\include\FT8xx.h -> simple_arduino\FT8xx.h
..\..\..\include\EVE_config.h -> simple_arduino\EVE_config.h
..\..\..\include\FT80x.h -> simple_arduino\FT80x.h
..\..\..\include\FT81x.h -> simple_arduino\FT81x.h
..\..\..\include\BT81x.h -> simple_arduino\BT81x.h
..\..\..\include\BT81x.h -> simple_arduino\BT81x.h
..\..\..\include\BT82x.h -> simple_arduino\BT82x.h
```

### Copy Files Manually

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
  - README.md
- From the `examples\simple\common` directory the files for the simple example functionality:
  - eve_calibrate.c
  - eve_example.c
  - eve_example.h
  - eve_fonts.c
  - eve_helper.c
  - eve_images.c

## Sketch Directory

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
README.md
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

This code can be reused by altering the code in the "example" files `eve_example.c`, `eve_helper.c`, `eve_images.c`, `eve_fonts.c`, and `eve_calibrate.c`. If using the python script then the list of files for the example code will need updated.

## Updating the Configuration

If the sketch is to be changed to target a different display panel or EVE device then changes can be made to the file `EVE_config.h`. This is the main configuration file that is most often changed. If a display panel is changed then there are settings built-in to allow for several different popular screen types and sizes. Likewise, the EVE API is not compatible between generations of devices and the `FT8XX_TYPE` setting will have to change to any different device type.
