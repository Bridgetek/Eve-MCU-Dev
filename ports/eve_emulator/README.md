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


### Specifying Flash Image

Storage devices such as attached flash (if applicable) must be configured before launching the emulator:

- Flash image (BT81x, BT820): initialize `BT8XXEMU_FlashParameters`, set `DataFilePath`
  or `Data`, create the Flash object, and assign it to the emulator parameters'
  `Flash` field.

On Windows, file paths in the native API use `eve_tchar_t`, which is
`wchar_t`. Flash writes are temporary by default; set `Persistent` in
`BT8XXEMU_FlashParameters` when changes should be written back to the file.


Users can generate flash images (.bin) files to use in applications via the [EVE Asset Builder](https://brtchip.com/eab/) toolchain.

The `EVE_EMULATOR_FLASH_FILE` macro can be used in the project to define the name of the flash image to be attached to the emulator. It should be formed in the following manner:

- EVE_EMULATOR_FLASH_FILE=TEXT("**flash-image-name.bin**")

And can be defined in either the visual Studio project settings under `Configuration Properties > C\C++ > Preprocessor > Preprocessor Definitions`, or the `CMakeLists.txt` file.


In Visual Studio project settings:

![VS Project Settings](/docs/emulator_flash_macro_define.png)

In `CMakeLists.txt`:

```
# Tell EVE library to use the EVE Emulator as base class
add_compile_definitions(USE_EMULATOR=0)

# Tell the EVE Emulator to use the flash image "flash-image-name.bin"
add_compile_definitions(EVE_EMULATOR_FLASH_FILE=TEXT("flash-image-name.bin"))
```

The flash image file must be available locally (in the same directory as the example executable). The `CMakeLists.txt` file can be utilised to copy the .bin file to the same output directory as the executable, or the file can be copied manually to this location.

A similar macro `EVE_EMULATOR_FLASH_FILE_SIZE` can be used to define the size of the attached flash, by default the flash size is **8MiB**.

### Specifying SD Card Images

- TODO: add support for SD card function calls into EVE_Emulator.c

## Linux

Linux builds are not supported as the emulator is not distributed for Linux.
