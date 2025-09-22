# EVE-MCU-Dev Simple Arduino Example

[Back](../README.md)

The sketch needs to be additional files copied to setup it up in the Arduino IDE. Since the name of the sketch needs to match the directory name where te sketch is located the example sketch is kept in a directory named [`simple_arduino`](simple_arduino).

## Setting Up the Arduino Example Sketch

The following files **must** be copied into the `simple_arduino` directory to be compiled into the sketch.

- From the `include` directory:
  - EVE.h
  - MCU.h
  - HAL.h
  - EVE_config.h
  - FT8xx.h
  - BT81x.h
  - BT82x.h
  - FT80x.h
  - FT81x.h
- From the `source` directory:
  - EVE_API.c
  - EVE_HAL.c
- From the `ports\eve_bt82x` directory:
  - patch_base.c
  - patch_base.h
- From the `ports\eve_arch_arduino` directory:
  - eve_arch_arduino.ino

The directory will look similar to this (dates and sizes will vary):

```
 Directory of <path>EVE-MCU-Dev\examples\simple\arduino\simple_arduino

22/09/2025  11:27    <DIR>          .
22/09/2025  11:53    <DIR>          ..
22/09/2025  10:42    <DIR>          .theia
22/09/2025  10:45    <DIR>          .vscode
20/09/2025  16:13            38,798 BT82x.h
18/09/2025  11:24            19,975 EVE.h
22/09/2025  11:14            59,972 EVE_API.c
22/09/2025  11:50             7,583 eve_arch_arduino.ino
20/09/2025  16:15            10,743 EVE_config.h
22/09/2025  11:14            20,940 EVE_HAL.c
18/09/2025  11:24            10,717 FT8xx.h
18/09/2025  11:23            11,414 HAL.h
22/09/2025  11:09            12,408 MCU.h
20/09/2025  16:09            13,929 patch_base.c
18/09/2025  11:25               309 patch_base.h
22/09/2025  11:33             1,149 simple_arduino.ino
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

