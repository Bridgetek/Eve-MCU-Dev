# EVE-MCU-Dev Racecar Example for the PlatformIO Arduino

Please check the pin configuration for the module being used. Instructions are available in [EVE-MCU-Dev Ports for the PlatformIO](../../../ports/eve_arch_platformio/README.md).

## Compiling using the PlatformIO VSCode Extension

Ensure that the "PlatformIO VSCode Extension" is installed on Visual Studio Code (VS Code).

The workspace file `racecar.code-workspace` can be loaded directly in VS Code to load the project as a workspace.

### Setting Up the Racecar PlatformIO VSCode Example

The build environment depends on the presence of the PlatformIO VSCode Extension. This can be setup following instructions in the [PlatformIO IDE for VSCode](https://docs.platformio.org/en/latest/integration/ide/vscode.html) document on the PlatformIO website.

### Compiling the Racecar PlatformIO VSCode Example

The instructions for compiling and programming with PlatformIO can be followed from the PlatformIO IDE for VSCode document.

## Compiling using the Command Line

The PlatformIO toolchain is also available from the command line. It can be used from the command line if the PlatformIO VSCode Extension is loaded or the PlatformIO Core is configured manually.

### Compiling the Racecar PlatformIO Example Manually

The following PlatformIO Code command will build the code for the "MM2040EV" environment:

```console
pio run --environment MM2040EV
```

To upload the compiled result to the target device for the "MM2040EV" environment:

```console
pio run --target upload --environment MM2040EV 
```

## Environments

This example contains PlatformIO environments for the following boards:

| Environment | Framework | Board | Pins |
| --- | --- | --- | --- |
| zero | atmelsam | zero | Default Arduino pins | 
| MM2040EV | raspberrypi | pico | SCLK 2, MOSI 3, MISO 4, CS# 5, PD# 7, INT# 6 | 
| esp32thing | espressif32 | esp32thing | SCLK 18, MOSI 23, MISO 19, CS# 15, PD# 15, INT# 2 | 

## Running the Example

If the `ASSETS` macro is set to `USE_FLASH` then the appropriate flash image corresponding to the EVE generation must be programmed into the device flash. The options for the image are found in the `eve3`/`eve4`/`eve5` subdirectory. 

If `USE_C_ARRAYS` then no additional action is needed. However, there is not enough program memory in some MCUs to load this program in its current form. The graphics will need to be modified to fit into the flash memory of the MCU.

The `USE_FILES` or `USE_FLASHIMAGE` options are not possible on the PlatformIO platform as there is no file system built into the example.

Note: the default setting for `ASSETS` on pico is `USE_FLASH` which is set in the `platformio.ini` file. 

