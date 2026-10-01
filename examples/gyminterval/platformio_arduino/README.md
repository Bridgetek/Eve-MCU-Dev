# EVE-MCU-Dev GymInterval Example for the PlatformIO Arduino

Please check the pin configuration for the module being used. Instructions are available in [EVE-MCU-Dev Ports for the PlatformIO](../../../ports/eve_arch_platformio/README.md).

## Compiling using the PlatformIO VSCode Extension

Ensure that the "PlatformIO VSCode Extension" is installed on Visual Studio Code (VS Code).

The workspace file `gyminterval.code-workspace` can be loaded directly in VS Code to load the project as a workspace.

### Setting Up the GymInterval PlatformIO VSCode Example

The build environment depends on the presence of the PlatformIO VSCode Extension. This can be setup following instructions in the [PlatformIO IDE for VSCode](https://docs.platformio.org/en/latest/integration/ide/vscode.html) document on the PlatformIO website.

### Compiling the GymInterval PlatformIO VSCode Example

The instructions for compiling and programming with PlatformIO can be followed from the PlatformIO IDE for VSCode document.

## Compiling using the Command Line

The PlatformIO toolchain is also available from the command line. It can be used from the command line if the PlatformIO VSCode Extension is loaded or the PlatformIO Core is configured manually.

### Compiling the GymInterval PlatformIO Example Manually

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
