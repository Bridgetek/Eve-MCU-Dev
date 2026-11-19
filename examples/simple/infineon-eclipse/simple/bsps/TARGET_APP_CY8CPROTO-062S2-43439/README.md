# CY8CPROTO-062S2-43439 BSP

## Overview

The CY8CPROTO-062S2-43439 PSOC™ 62S2 Wi-Fi Bluetooth® Prototyping Kit is a low-cost hardware platform that enables design and debug of PSOC™ 6 MCUs. It comes with a Murata LBEE5KL1YN module, based on the CYW43439 combo device, industry-leading CAPSENSE™ for touch buttons and slider, on-board debugger/programmer with KitProg3, microSD card interface, 512-Mb Quad- SPI NOR flash, PDM-PCM microphone, and a thermistor. This kit is designed with a snap-away form-factor, allowing the user to separate the different components and features that come with this kit and use independently. In addition, support for Digilent's Pmod interface is also provided with this kit.     
**Note:**
CY8CPROTO-062S2-43439 shares the same GPIO for the user button (USER BTN1) and the CYW43439 host wake up pin and it is connected to CYW43439 host wake up pin by default. So, in a Wi-Fi low power application,  the pin P0[4] cannot be used as a user button. If you wish to disable the host wake up feature in your application and use the user button, add the below line to the application Makefile: DEFINES+=CY_WIFI_HOST_WAKE_SW_FORCE=0.

![](docs/html/board.png)

To use code from the BSP, simply include a reference to `cybsp.h`.

## Features

### Kit Features:

* Featuring the PSOC™ 6 CY8C62xA MCU (MPN: [CY8C624ABZI-S2D44](https://www.infineon.com/cms/en/product/microcontroller/32-bit-psoc-arm-cortex-microcontroller/psoc-6-32-bit-arm-cortex-m4-mcu/cy8c624abzi-s2d44/)): Ultra low power, high performance MCU based on the dual core CPU architecture of Arm® Cortex®-M4 and Arm® Cortex®-M0+, up to 2 MB of on-chip Flash, 1 MB of SRAM, built-in hardware and software security features, rich analog, digital, and communication peripherals
* Wireless module(Murata 1YN) based on Infineon's AIROC™ CYW43439 single-chip combo device (2.4 GHz Wi-Fi 4 (802.11n) and Bluetooth® 5.2) for evaluating cloud connected applications with PSOC™ 6 as the Wi-Fi host MCU
* Industry leading CAPSENSE™ featuring linear slider, buttons for prototyping elegant, robust capacitive touch-based user interfaces
* Integrated on-board programmer / debugger, memory expansion through 512-Mb Quad-SPI NOR Flash, microSD card interface, PDM microphone, thermistor, push buttons, LEDs

### Kit Contents:

* PSOC™ 62S2 Wi-Fi Bluetooth® Prototyping Board
* USB Type-A to Micro-B cable
* Quick Start Guide

## BSP Configuration

The BSP has a few hooks that allow its behavior to be configured. Some of these items are enabled by default while others must be explicitly enabled. Items enabled by default are specified in the bsp.mk file. The items that are enabled can be changed by creating a custom BSP or by editing the application makefile.

Components:
* Device specific category reference (e.g.: CAT1) - This component, enabled by default, pulls in any device specific code for this board.

Defines:
* CYBSP_WIFI_CAPABLE - This define, disabled by default, causes the BSP to initialize the interface to an onboard wireless chip if it has one.
* CY_USING_HAL - This define, enabled by default, specifies that the HAL is intended to be used by the application. This will cause the BSP to include the applicable header file and to initialize the system level drivers.
* CYBSP_CUSTOM_SYSCLK_PM_CALLBACK - This define, disabled by default, causes the BSP to skip registering its default SysClk Power Management callback, if any, and instead to invoke the application-defined function `cybsp_register_custom_sysclk_pm_callback` to register an application-specific callback.

### Clock Configuration

| Clock    | Source    | Output Frequency |
|----------|-----------|------------------|
| FLL      | IMO       | 100.0 MHz        |
| PLL      | IMO       | 48.0 MHz         |
| CLK_HF0  | CLK_PATH0 | 100 MHz          |

### Power Configuration

* System Active Power Mode: LP
* System Idle Power Mode: Deep Sleep
* VDDA Voltage: 3300 mV
* VDDD Voltage: 3300 mV

See the [BSP Setttings][settings] for additional board specific configuration settings.

## API Reference Manual

The CY8CPROTO-062S2-43439 Board Support Package provides a set of APIs to configure, initialize and use the board resources.

See the [BSP API Reference Manual][api] for the complete list of the provided interfaces.

## More information
* [CY8CPROTO-062S2-43439 BSP API Reference Manual][api]
* [CY8CPROTO-062S2-43439 Documentation](http://www.infineon.com/CY8CPROTO-062S2-43439)
* [Cypress Semiconductor, an Infineon Technologies Company](http://www.cypress.com)
* [Infineon GitHub](https://github.com/infineon)
* [ModusToolbox™](https://www.cypress.com/products/modustoolbox-software-environment)

[api]: https://infineon.github.io/TARGET_CY8CPROTO-062S2-43439/html/modules.html
[settings]: https://infineon.github.io/TARGET_CY8CPROTO-062S2-43439/html/md_bsp_settings.html

---
© Cypress Semiconductor Corporation (an Infineon company) or an affiliate of Cypress Semiconductor Corporation, 2019-2025.