# EVE-MCU-Dev Ports for PlatformIO

[Back](../README.md)

The port for PlatformIO allows the selection of a range of hardware to control the EVE device. Only the "Arduino" framework and "ESP-IDF" framework are supported at present.
The PlatformIO device manages the EVE device SPI bus. 

## PlatformIO Arduino Framework

The PlatformIO Arduino framework port was developed using Arduino UNO, Arduino Zero and Arduino Leonardo Arduino modules; and also Raspberry Pi pico and Sparkfun ESP32 Thing modules. 

The modules can be connected via short wires to the corresponding signals of an EVE module. 

### Arduino Modules

Refer to the [Arduino port](../eve_arch_arduino/README.md) for wiring diagrams for standard Arduino modules. 

### Other Modules

For other modules the standard Arduino pinout should be overridden to match the module.
In the `platformio.ini` file for the project the pins for the module can be changed by setting compiler definitions in the `build_flags` section of an environment.

If the `PIN_REDEFINE=` macro is defined (it can be defined to any value) then the 
`PIN_SPICLOCK`, `PIN_DATAOUT`, `PIN_DATAIN`, `PIN_CHIPSELECT`, `PIN_POWERDOWN` and `PIN_INTERRUPT`
 **must** all be defined for the environment.

```
[env:custom_board]
build_flags =
    ${env.build_flags}
    -D PIN_REDEFINE=1
    -D PIN_SPICLOCK=18
    -D PIN_DATAOUT=23
    -D PIN_DATAIN=19
    -D PIN_CHIPSELECT=22
    -D PIN_POWERDOWN=15
    -D PIN_INTERRUPT=2
```

There is support for overridding the default pinout according to this list:
- ARDUINO_ARCH_SAMD (SAMD) cannot override due to fixed I/O pins. This is boards such as the Arduino UNO, Zero, Mega.
- ARDUINO_ARCH_MBED (ARM Cortex) can override. This is platforms such as the Raspberry Pi pico.
- ARDUINO_ARCH_ESP32 (ESP32) can override. For example, the Sparkfun ESP32 Thing.

### Software

Please refer to the [PlatformIO Arduino Simple example](../../examples/simple/platformio_arduino/README.md) for instructions on using PlatformIO in an EVE-MCU-Dev project.

## PlatformIO ESP-IDF Framework

The PlatformIO ESP-IDF framework port was developed using an ESP32 Thing module and a ESP32 DevKitC module.

The modules can be connected via short wires to the corresponding signals of an EVE module. 

### ESP32 Modules

Refer to the [ESP32 port](../eve_arch_esp32/README.md) for wiring diagrams for standard ESP32 modules. 

### Other Modules

For other modules the standard pinout should be overridden to match the module.
In the `platformio.ini` file for the project the pins for the module can be changed by setting compiler definitions in the `build_flags` section of an environment.

If the `PIN_REDEFINE=` macro is defined (it can be defined to any value) then the 
`PIN_SPICLOCK`, `PIN_DATAOUT`, `PIN_DATAIN`, `PIN_CHIPSELECT`, `PIN_POWERDOWN` and `PIN_INTERRUPT`
 **must** all be defined for the environment.

```
[env:custom_board]
build_flags =
    ${env.build_flags}
    -D PIN_REDEFINE=1
    -D PIN_SPICLOCK=18
    -D PIN_DATAOUT=23
    -D PIN_DATAIN=19
    -D PIN_CHIPSELECT=22
    -D PIN_POWERDOWN=15
    -D PIN_INTERRUPT=2
```

### Software

Please refer to the [PlatformIO ESP-IDF Simple example](../../examples/simple/platformio_esp32/README.md) for instructions on using PlatformIO in an EVE-MCU-Dev project.

## Reference 

Reference [PlatformIO Project](https://platformio.org/)
