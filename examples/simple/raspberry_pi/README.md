# EVE-MCU-Dev Simple Raspberry Pi Example

[Back](../README.md)

## Compiling the Simple Raspberry Pi Example

The build environment depends on the presence of the build tools, GCC compiler and make utility. These can be esily installed by the following command.

```
sudo apt-get install build-essential
```

To configure the project with the default C compiler:
```
    cmake -B build -S .
```
Then to compile:
```
    cmake --build build
```
Default builds are compiled with debugging enabled. 

The executable file is `build/simple_raspberry_pi`. This can be run from the command line. 

## Running the Simple Raspberry Pi Example

The executable image is found in the file `build/simple_raspberry_pi` in the current directory. To run this file simple call it from the command line.
```
./build/simple_raspberry_pi
```
The program will output:
```
Calibrating display...
Loading font...
Loading images...
Starting demo:
```