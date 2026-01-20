# EVE-MCU-Dev Simple PIC18F Example

[Back](../README.md)



## Compiling using the MPLAB Tools for VSCode Extension

Ensure that the "MPLAB Tools for VS Code Extension" is installed on Visual Studio Code (VS Code).

The workspace file `simple.code-workspace` can be loaded directly in VS Code to load the project as a workspace.

### Setting Up the Simple Raspberry Pi pico VSCode Example

The build environment depends on the presence of the MPLAB Tools for VS Code Extension. This can be setup following instructions in the [MPLAB Tools for VS Code](https://www.microchip.com/en-us/tools-resources/develop/mplab-tools-vs-code) document from the Microchip website.

To configure a new project based on the MPLAB Tools the device and the device family need to be set in the project configuration.

Select "Configure Projects" - "OverView" to configure the project. This is accessed through the "PROJECTS" sidebar by clicking the settings (gear symbol) button. The tooltip is "Configure Projects".

In the "Overview" section the device is selected from the box in "Device". The tested device is the "PIC18F46K22".

---
Device
The microcontroller or device that the configuration is targeting.

| Device |
| --- |
| PIC18F46K22 |
---

Below the "Device" section the "Packs" section sets the device support pack for the microcontroller. This may need to be downloaded. The tested pack is called "PIC18F-K_DFP". 

---
**Packs**
The device support packs needed for configuration. These packs provide device-specific information and drivers needed to build and program the device.

| Name | Version | Vendor |
| --- | --- | --- |
| PIC18F-K_DFP | 1.14.301 | Microchip |

---

## Compiling the Simple PIC18F Example

To compile the project start the build task. In the menu bar choose "Terminal" then "Run Build Task...". Choose the following option to build a complete image for the device.

"PIC18F44K22: default - Full Build"

## Running the Simple PIC18F Example

The executable image is found in the file `simple_PIC18F.elf` in the `out/PIC18F44K22` directory. Follow the instructions in the MPLAB Tools for VS Code to run the output on the device.
