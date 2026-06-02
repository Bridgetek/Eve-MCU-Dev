# EVE-MCU-Dev Ports for the EVE Emulator

[Back](../README.md)

The port for the EVE Emulator comprises a Windows host PC with the EVE Emulator library. The emulator simulates an EVE device on the Windows display. Only EVE APIs 1,2,3 and 4 are supported (FT80x, FT81x, BT88x and BT81x). Devices with EVE API 5 (BT820) are not currently supported.

## Windows

Important Information about Windows builds.

To compile this you will have to download the **EVE-Apps** library for Windows. The recommended version is v1.7.0 or later. It is available from the GitHub repository:

https://github.com/Bridgetek/EveApps

Download or clone the EveApps repository to a new directory.

### Install the EVE-Apps EVE Emulator in EVE-MCU-Dev library

The EveApps repository directory will have a subfolder called `common/eve_hal/Bin/Simulation`. The repository contains libraries for various CPU architectures. On v1.7.0 these architectures are `x64` (64-bit Windows) and `x86` (32- bit Windows).

Each architecture has:
- A DLL file for `bt8xxemu`.
- LIB files for connecting the application to the `bt8xxemu` DLLs.
- Header files for the `bt8xxemu` API.

A Windows Command Line BAT file `install_emulator.bat` is included in this folder. This will copy the correct files from the EveApps repository directory to the 
current directory (`ports\eve_emulator`).

To run the BAT file change directory to the `ports\eve_emulator` directory. The path to the EveApps repository directory in the extracted distribution folder is passed as the first parameter to the BAT file. For example:

```
> cd ports\eve_emulator
> install_emulator.bat ..\..\..\GitHub\EveApps
Installing AMD64 libraries from "..\..\..\GitHub\EveApps"
Copying "..\..\..\GitHub\EveApps\common\eve_hal\Bin\Simulation\x64\bt8xxemu.dll" to Simulation\bt8xxemu.dll
        1 file(s) copied.
Copying "..\..\..\GitHub\EveApps\common\eve_hal\Bin\Simulation\x64\bt8xxemu.lib" to Simulation\bt8xxemu.lib
        1 file(s) copied.
Copying "..\..\..\GitHub\EveApps\common\eve_hal\Bin\Simulation\x64\mx25lemu.dll" to Simulation\mx25lemu.dll
        1 file(s) copied.
Copying "..\..\..\GitHub\EveApps\common\eve_hal\Bin\Simulation\x64\mx25lemu.lib" to Simulation\mx25lemu.lib
        1 file(s) copied.
Copying "..\..\..\GitHub\EveApps\common\eve_hal\Hdr\bt8xxemu_inttypes.h" to Hdr\bt8xxemu_inttypes.h
        1 file(s) copied.
Copying "..\..\..\GitHub\EveApps\common\eve_hal\Hdr\bt8xxemu.h" to Hdr\bt8xxemu.h
        1 file(s) copied.
```

The example application will load and use the `bt8xxemu.dll` file at runtime. This file must be available either locally (in the same directory as the example executable) or on the system path (recommended "C:\Windows\System32"). The `CMakeLists.txt` file in each example will copy the DLL to the same output directory as the executable.

A **bash** script file `install_emulator.sh` is also included in this folder. It has the same functionality as `install_emulator.bat` but can run in a bash shell to help with setting up the environment.

## Linux

Linux builds are not supported as the emulator is not distributed for Linux.