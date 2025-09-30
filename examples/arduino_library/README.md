# EVE-MCU-Dev Arduino Library Builder

[Back](../README.md)

This directory is used to build an EVE library for Arduino. There are template files which are used to supplement the EVE-MCU-Dev library to make it accessible as a library when called by an Arduino sketch.

The script will build a library for one generation of EVE device. Each generation requires a seperate library since the API and supported options on each device is different. 

The panel settings can be chosen at runtime from a set configured into the template files.

## Setting Up the Arduino Library

The python script `libbuild.py` is run with the the following options:

```
> libbuild.py --help
usage: libbuild.py [-h] [--src SRC] [--dest DEST] [--api API] [--apisub APISUB]

Library Builder for EVE

options:
  -h, --help       show this help message and exit
  --src SRC        distribution directory for EVE-MCU-Dev
  --dest DEST      destination directory for Arduino library
  --api API        EVE API to build library for
  --apisub APISUB  EVE SUB API to build library for
```
The `--src` option tells the script where the root directory of the EVE-MCU-Dev code is located. This is the folder that contains the `source`, `include`, `ports`, and `examples` directories. The script will take files that it needs from the EVE-MCU-Dev libraries. The default is `../..` since this the relative path to the code from the normal location of this script.

The `--dest` options is the folder where the Arduino library code will be written to. This is, by default, `./BtEve<API>` where `<API>` is replaced with the API version.

The EVE API to use is set by the `--api` and `--apisub` parameters. The default is `5` for BT82x. The `--subapi` is only used for the API 2 which distinguishes between the FT81x and BT88x.

- From the `include` directory following library header files will be copied:
  - EVE.h
  - MCU.h
  - HAL.h
  - FT8xx.h
  - BT81x.h  (for EVE API 3,4 only)
  - BT82x.h  (for EVE API 5 only)
  - FT80x.h  (for EVE API 1 only)
  - FT81x.h  (for EVE API 2 only)
- From the `source` directory add the library files:
  - EVE_API.c
  - EVE_HAL.c
- From the `ports\eve_bt82x` directory add the BT82x base patch (for API 5 only):
  - patch_base.c
  - patch_base.h
- From the `ports\eve_arch_arduino` directory add the MCU layer sketch for Arduino:
  - eve_arch_arduino.ino (renamed to EVE_MCU.cpp)

Template files are added to this to form the library:
- EVE_config.h
- bteve.h
- bteve.cpp
- library.properties
- README.md
- test/test.ino

The following example projects are added:
- examples/simple

The output directory is a library specifically for the API version that was specified in the command line. It cannot be used for EVE devices that support a different API level.

### Python Script Requirements

The script needs a valid EVE-MCU-Dev directory with current files.

## Using the Arduino Library

The library can be added to a sketch by including the line in the example below. This is for the API 5 library for BT82x, the number corresponds to the API.
```
#include <bteve5.h>
```

## Distributing the Arduino Library

The generated library code can be distributed as a ZIP file and loaded into Arduino IDE or uploaded to the Arduino Library Registry.

the "Include Library ->" then "Add .ZIP Library...".

https://github.com/arduino/library-registry/blob/main/repositories.txt
