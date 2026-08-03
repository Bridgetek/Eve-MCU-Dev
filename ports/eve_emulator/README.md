# EVE-MCU-Dev Ports for the EVE Emulator

[Back](../README.md)

The EVE Emulator port consists of a Windows host PC running the EVE_Emulator library. The emulator simulates an EVE device on the Windows display, and supports `x64` (64-bit) architectures only. 

To compile this you will have to download the **EVE_Emulator** library, it is available from the following GitHub repository:

https://github.com/Bridgetek/EVE_Emulator

Download or clone the EVE_Emulator repository to a new directory.

## Installing the EVE_Emulator in EVE-MCU-Dev library

The EVE_Emulator repository directory will have subfolders named `include`, `bin`, and `lib`. The repository contains library files for `x64` (64-bit Windows) architectures only.

The subfolders include:
- A DLL file for `bt8xxemu`.
- LIB files for connecting the application to the `bt8xxemu` DLLs.
- Header files for the `bt8xxemu` API.
- A DLL file for `mx25lemu`. (flash emulator)
- A DLL file for `zlib`. (compression library)

A Windows Command Line BAT file `install_emulator.bat` is included in this folder. This will copy the correct files from the EVE_Emulator repository directory to the 
current directory (`ports\eve_emulator`).

To run the BAT file, change directory to the `ports\eve_emulator` directory. The path to the EVE_Emulator repository directory in the extracted distribution folder is passed as the first parameter to the BAT file. For example:

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

A **bash** script file `install_emulator.sh` is also included in this folder. It has the same functionality as `install_emulator.bat` but can run in a bash shell to help with setting up the environment.

## Using EVE_Emulator in EVE-MCU-Dev Projects

The `PLATFORM_EMULATOR` macro enables EVE-MCU-Dev examples to run using the EVE_Emulator. This can be enabled in the example source code or during project compilation.

In `VisualStudio` this can be configured in `Project Properties -> Configuration Properties -> C/C++ -> Preprocessor -> Preprocessor Definitions` by adding the following:
```
PLATFORM_EMULATOR
```

Or in `CMakeLists.txt`:

```
# Tell EVE library to use the EVE Emulator as base class
add_compile_definitions(PLATFORM_EMULATOR)
```

In gcc compilers via the `command line`:
```
 -DPLATFORM_EMULATOR
```

Example applications load and use the `bt8xxemu.dll` file at runtime. This DLL must be available in the same directory as the example executable.

If the application requires the `mx25lemu.dll` and `zlib.dll` files, these DLLs must also be available in the same directory as the executable.

The `Visual Studio` project and the `CMakeLists.txt` file provided with each example automatically copy the required DLLs to the executable output directory. In most cases, only `bt8xxemu.dll` is required; however, `mx25lemu.dll` and `zlib.dll` are also copied to simplify deployment and ensure all runtime dependencies are available.

### Visual Studio

The Visual Studio project files for examples are found in the `\emulator\VisualStudio` folder. Object files are placed into the `VisualStudio\obj` folder. Program executables are placed into the `VisualStudio\x64\{Debug|Release}` folder.

When an example application is built in Visual Studio, a post-build event copies `bt8xxemu.dll`, `mx25lemu.dll`, and `zlib.dll` from the `ports\eve_emulator` folder to the executable output directory.

### Command Line Compilation

To configure the project with the default C compiler:
```
    cmake -B build -S .
```
On Windows, if an MSVC compiler is not the default compiler on your platform then choose a suitable Visual Studio compiler explicitly:
```
    cmake -B build -S . -G "Visual Studio 17 2022"
```
On Windows, to compile the project use the MinGW C compiler:
```
    cmake -B build -S . -G "MinGW Makefiles"
```
Then to compile:
```
    cmake --build build
```
The executable file is in the build directory. The exact location is defined by the compiler defaults.

## Specifying a Flash Image (BT81x, BT82x)

Storage devices such as attached flash (if applicable) must be configured before launching the emulator:

- Flash image (BT81x, BT820): initialize `BT8XXEMU_FlashParameters`, set `DataFilePath`
  or `Data`, create the Flash object, and assign it to the emulator parameters'
  `Flash` field.

On Windows, file paths in the native API use `eve_tchar_t`, which is
`wchar_t`. Flash writes are temporary by default; set `Persistent` in
`BT8XXEMU_FlashParameters` when changes should be written back to the file.

Users can generate flash images (.bin) files to use in applications via the [EVE Asset Builder](https://brtchip.com/eab/) toolchain.

The `EVE_EMULATOR_FLASH_FILE` macro can be used in the EVE-MCU-Dev EVE Emulator projetcs to define the name of the flash image (.bin) file to be attached to the emulator. The macro allows users to specify the flash image (.bin) filename in the executable directory, a path relative to the executable directory, or an absolute path to the image.

It can be formed in the following manner:

- `Local:` EVE_EMULATOR_FLASH_FILE=TEXT("**flash-image-name.bin**")
- `Relative:` EVE_EMULATOR_FLASH_FILE=TEXT("**..\\\\path\\\\to\\\\flash-image-name.bin**")
- `Absolute:` EVE_EMULATOR_FLASH_FILE=TEXT("**C:\path\to\flash-image-name.bin**")

**NOTE:** Defining `EVE_EMULATOR_FLASH_FILE` without a value is an invalid use of the macro and may result in undefined behaviour or cause the emulator to encounter an unhandled exception.

### Defining the Macro

The macro can be defined in either the `Visual Studio` project settings under `Configuration Properties > C\C++ > Preprocessor > Preprocessor Definitions`, the `CMakeLists.txt` file , or during command line compilation.


In `VisualStudio`:

```
EVE_EMULATOR_FLASH_FILE=TEXT("flash-image-name.bin")
```

In `CMakeLists.txt`:

```
# Tell the EVE Emulator to use the flash image "flash-image-name.bin" in the executable folder
add_compile_definitions(EVE_EMULATOR_FLASH_FILE=TEXT("flash-image-name.bin"))
```

In In gcc compilers via the `command line`:
```
-DEVE_EMULATOR_FLASH_FILE=TEXT("flash-image-name.bin")
```

#### EVE_EMULATOR_FLASH_FILE_SIZE

A similar macro `EVE_EMULATOR_FLASH_FILE_SIZE` can be used to define the size of the attached flash, by default the flash size is **8 MiB**.

### Recommendations

It is reccomended to make any flash image (.bin) files available locally (in the same directory as the example executable), and to utiilse the local form of the macro `
EVE_EMULATOR_FLASH_FILE=TEXT("flash-image-name.bin")`. This allows projects to be easily compressed, shared, and relocated without requiring changes to the file path.

The `CMakeLists.txt` file can be utilised to copy the .bin file to the same output directory as the executable, the .bin file can also be copied via a post build command in `Visual Studio`, or manually copied to this location.

## Using SD Cards (BT82x) 

Storage devices such as SD Cards (if applicable) must be configured before launching the emulator:

- SD card image (BT820): set `SDCardFilePath` before calling `BT8XXEMU_run`, or use `BT8XXEMU_insertSDCardImage` or `BT8XXEMU_insertSDCardFolder` while the emulator is running.

The EVE-MCU-Dev Emulator port utilises the `BT8XXEMU_insertSDCardFolder` function call to suport SD Card Folders.

On Windows, file paths in the native API use `eve_tchar_t`, which is
`wchar_t`. SD writes are enabled by default; set `true` for the 
`readOnly` parameter in the `BT8XXEMU_insertSDCardFolder` function call when changes should be not written back to the files.

Users can generate SD Card assets to use in applications via the [EVE Asset Builder](https://brtchip.com/eab/) toolchain.

The `EVE_EMULATOR_SD_FOLDER` macro can be used in the EVE-MCU-Dev EVE Emulator projects to define the name of the SD Card Folder to be attached to the emulator. The macro allows users to specify SD Card Folder name in the executable directory, a path relative to the executable directory, or an absolute path to the image.

It can be formed in the following manner:

- `Local:` EVE_EMULATOR_SD_FOLDER=TEXT("**sdFolderName**")
- `Relative:` EVE_EMULATOR_SD_FOLDER=TEXT("**..\\\\path\\\\to\\\\sdFolderName**")
- `Absolute:` EVE_EMULATOR_SD_FOLDER=TEXT("**C:\path\to\sdFolderName**")

**NOTE:** Defining `EVE_EMULATOR_SD_FOLDER` without a value is an invalid use of the macro and may result in undefined behaviour or cause the emulator to encounter an unhandled exception.

### Defining the Macro

The macro can be defined in either the `Visual Studio` project settings under `Configuration Properties > C\C++ > Preprocessor > Preprocessor Definitions`, the `CMakeLists.txt` file , or during command line compilation.

In `VisualStudio`:

```
EVE_EMULATOR_SD_FOLDER=TEXT("sdFolderName")
```

In `CMakeLists.txt`:

```
# Tell the EVE Emulator to use the SD Card Folder "sdFolderName" in the executable folder
add_compile_definitions(EVE_EMULATOR_SD_FOLDER=TEXT("sdFolderName"))
```

In In gcc compilers via the `command line`:
```
-DEVE_EMULATOR_SD_FOLDER=TEXT("sdFolderName")
```
#### EVE_EMULATOR_SD_FOLDER_SIZE

A similar `EVE_EMULATOR_SD_FOLDER_SIZE` can be used to define the size of the attached FAT32 image size, this value is passsed into the `minimumSize` parameter of the `BT8XXEMU_insertSDCardFolder` call. By default a value of **zero** is passed in for minimumSize, which lets the emulator calculate the required FAT32 image size from the folder contents.

### Recommendations

It is reccomended to make any SD Card Folder available locally (in the same directory as the example executable), and to utiilse the local form of the macro `
EVE_EMULATOR_SD_FOLDER_=TEXT("sdFolderName")`. This allows projects to be easily compressed, shared, and relocated without requiring changes to the file path.

The `CMakeLists.txt` file can be utilised to copy the dessired folder to the same output directory as the executable, the folder file can also be copied via a post build command in `Visual Studio`, or manually copied to this location.

# Linux

Linux builds are not supported as the emulator is not distributed for Linux.
