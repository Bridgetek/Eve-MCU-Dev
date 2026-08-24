# EVE-MCU-Dev Simple Raspberry Pi Example

[Back](../README.md)

## Compiling the Simple Raspberry Pi Example

The build environment depends on the presence of the build tools, GCC compiler and make utility. These can be easily installed by the following command.

```console
sudo apt-get install build-essential
```

### Using Make (Makefile):
Then to compile:
```console
make
```
### Using CMake (CmakeLists.txt):

Cmake can easily installed by the following command.

```console
sudo apt-get install cmake
```

To configure the project with the default C compiler:
```console
cmake -B build -S .
```
Then to compile:
```console
cmake --build build
```
Default builds are compiled with debugging enabled. 

The executable file is `build/simple_raspberry_pi`. This can be run from the command line. 

## Running the Simple Raspberry Pi Example

The executable image is found in the file `build/simple_raspberry_pi` in the current directory. To run this file simple call it from the command line.
```console
./build/simple_raspberry_pi
```
The program will output:
```text
Calibrating display...
Loading font...
Loading images...
Starting demo:
```