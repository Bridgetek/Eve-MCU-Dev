# EVE-MCU-Dev Simple Raspberry Pi Example

[Back](../README.md)

## Compiling the Simple Raspberry Pi Example

The build environment depends on the presence of the build tools, GCC compiler and make utility. These can be esily installed by the following command.

```
sudo apt-get install build-essential
```
To build the example code the `Makefile` in the current directory is run using the `make` command.
```
make
```

## Running the Simple Raspberry Pi Example
The executable image is found in the file `raspberry_pi` in the current directory. To run this file simple call it from the command line.
```
./raspberry_pi
```
The program will output:
```
Calibrating display...
Loading font...
Loading images...
Starting demo:
```