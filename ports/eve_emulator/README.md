# EVE-MCU-Dev Ports for the EVE Emulator

[Back](../README.md)

The port for the EVE Emulator comprises a Windows host PC with the EVE Emulator library. The emulator simulates an EVE device on the Windows display. 

## Windows

Important Information about Windows builds.

To compile this you will have to download the **EVE_Emulator** library for Windows. It is available from the GitHub repository:

https://github.com/Bridgetek/EVE_Emulator

Download or clone the EVE_Emulator repository to a new directory.

### Install the EVE_Emulator EVE Emulator in EVE-MCU-Dev library

The EVE_Emulator repository directory will have a subfolders called `include`, `bin`, and `lib` . The repository contains library files for `x64` (64-bit Windows) architectures.

The subfolders include:
- A DLL file for `bt8xxemu`.
- LIB files for connecting the application to the `bt8xxemu` DLLs.
- Header files for the `bt8xxemu` API.
- A DLL file for `mx25lemu`. (flash emulator)
- A DLL file for `zlib`. (compression library)

A Windows Command Line BAT file `install_emulator.bat` is included in this folder. This will copy the correct files from the EVE_Emulator repository directory to the 
current directory (`ports\eve_emulator`).

To run the BAT file change directory to the `ports\eve_emulator` directory. The path to the EVE_Emulator repository directory in the extracted distribution folder is passed as the first parameter to the BAT file. For example:

```
> cd ports\eve_emulator
> install_emulator.bat ..\..\..\GitHub\EVE_Emulator
Installing AMD64 libraries from "..\..\..\GitHub\EVE_Emulator"
Copying "..\..\..\EVE_Emulator\bin\bt8xxemu.dll" to "bt8xxemu.dll"
        1 file(s) copied.
Copying "..\..\..\EVE_Emulator\lib\bt8xxemu.lib" to "bt8xxemu.lib"
        1 file(s) copied.
Copying "..\..\..\EVE_Emulator\bin\mx25lemu.dll" to "mx25lemu.dll"
        1 file(s) copied.
Copying "..\..\..\EVE_Emulator\bin\zlib.dll" to "zlib.dll"
        1 file(s) copied.
Copying "..\..\..\EVE_Emulator\include\bt8xxemu_inttypes.h" to "bt8xxemu_inttypes.h"
        1 file(s) copied.
Copying "..\..\..\EVE_Emulator\include\bt8xxemu.h" to "bt8xxemu.h"
        1 file(s) copied.
```

The example application will load and use the `bt8xxemu.dll` file at runtime. This file must be available either locally (in the same directory as the example executable) or on the system path (recommended "C:\Windows\System32"). The `CMakeLists.txt` file in each example will copy the DLL to the same output directory as the executable.

A **bash** script file `install_emulator.sh` is also included in this folder. It has the same functionality as `install_emulator.bat` but can run in a bash shell to help with setting up the environment.

## Linux

Linux builds are not supported as the emulator is not distributed for Linux.