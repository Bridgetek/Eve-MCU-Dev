# EVE-MCU-Dev Simple Beaglebone Black Example

[Back](../README.md)

## Compiling the Simple Beaglebone Black Example

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

The executable file is `build/simple_BeagleBone`. This can be run from the command line. 

## Running the Simple Beaglebone Black Example

The executable image is found in the file `simple_BeagleBone` in the current directory. To run this file simple call it from the command line.
```
./build/simple_BeagleBone
```
The program will output:
```
Calibrating display...
Loading font...
Loading images...
Starting demo:
```