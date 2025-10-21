# EVE-MCU-Dev Examples

[Back](../README.md)

## Example File Structure

The examples are structured in a way that allows multiple platforms to be included in each example and multiple examples to be included in the library. Rather than the focus being on the example code including a copy of the library, the focus is on the library itself and the example code is structured to fit with the library. 

The structure of each example called `<demo>` would be:

- `\examples\<demo>\README.md` **Required** Information on the the example.
- `\examples\<demo>\common` **Required** Common files for the example.
- `\examples\<demo>\docs` Common documentation files for the `README.md` file.
- `\examples\<demo>\assets` Common assets which may be used in the example.
- `\examples\<demo>\ESP32` MCU specific files for the ESP32.
- `\examples\<demo>\ft900` MCU specific files for the Bridgetek FT9xx.
- `\examples\<demo>\libft4222` MCU specific files for FT4222H interface.
- `\examples\<demo>\libft4222` MCU specific files for MPSSE interface.
- `\examples\<demo>\MSP430` MCU specific files for the Ti MSP430.
- `\examples\<demo>\MSPM0` MCU specific files for the Ti MSPM0.
- `\examples\<demo>\NXP_K64` MCU specific files for the NXP K64.
- `\examples\<demo>\PIC18F` MCU specific files for the PIC18F.
- `\examples\<demo>\raspberry_pi` MCU specific files for the Raspberry Pi.
- `\examples\<demo>\STM32` MCU specific files for the STM32 using Keil.
- `\examples\<demo>\STM32CUBE` MCU specific files for the STM32 using STM32CUBE.
- `\examples\<demo>\pico` MCU specific files for the Raspberry Pi pico.

### Common Folder

In each example directory there is a folder called `common` which contains all the files which provide the self-contained code for the example. 

- `\examples\<demo>\common\eve_example.h` The header file containing the declarations for the following files and `eve_example()` function.
- `\examples\<demo>\common\eve_example.c` The C file containing the `eve_example()` function called from the platform `main()` function.
- `\examples\<demo>\common\eve_calibrate.c` A helper file that contains functions that are useful in calibrating the display touch. This will call functions in the MCU-specific part of the example to store and load touch calibration information.
- `\examples\<demo>\common\eve_helper.c` Several extra functions that are used commonly in examples.

### MCU Specific Folders

Examples may or may not have all platform support directories. The contents of the MCU specific directory vary depending on the platform supported.

The source file containing the `main()` function is not in the example code but is in the MCU specific part of example code. 

The MCU specific files including the entry point function (the `main()` function), build files (CMake, Makefile), IDE files (Eclipse, Visual Studio, Visual Studio Code, Arduino IDE, STMCube IDE, Keil, MPLab, Ti CCS).

## Runnable Examples

In the example code the EVE-MCU-Dev library is compiled with the example program. 

The example [simple](simple/README.md) is supported by all platforms and is intended to be used as a template for new projects. It contains build instructions for all the platforms as well.

The following example code is available:

| Folder | Description |
| --- | --- |
| [simple](simple/README.md) | Simple example code which demonstrates loading a bitmap and a legacy font. |
| [b2tf](b2tf/README.md) | Seven segment demo code using sevenseg code snippet. This shows a constantly updating screen with blends, scissors and gradients. |
| [cruise](cruise/README.md) | Circular click-wheel display demo code. This shows gradients and actions for user inputs. |
| [dronefpv-bt82x](dronefpv-bt82x/README.md) | Advanced BT82X demo seven segment demo code rendering video from the LVDS RX interface to the screen via a bitmap in RAM_G. |
| [flightdeck](flightdeck/README.md) | Drawing graphical representations of physical controls from an aircraft. Shows blending and gradients with smooth animation. |
| [fontmagic](fontmagic/README.md) | Simple example code demonstrating scaling and rotating letters from a font file. |

## Widgets and Utilities

In addition to runable demo code there are libraries of widgets and utilities that are used by the examples or help development.

| File/Folder | Description |
| --- | --- |
| [snippets](snippets/README.md) | Common files and widgets for the examples. |

Many of the examples load code or modules from this directory. 

TODO: this directory will be split into widgets and utilities.
