# EVE-MCU-Dev Ports for the EVE Emulator

[Back](../README.md)

The `EVE Emulator` port allows EVE-MCU-Dev applications to run on a Windows host using the `EVE_Emulator` library. The emulator simulates a selected EVE device and displays its output in a **Windows** desktop window. The port supports `x64` (64-bit) Windows applications only. Linux builds are not supported because as the EVE Emulator library is not currently distributed for Linux.

## Requirements 
| Requirement | Details | 
| --- | --- | 
| Host operating system | Windows |
| Application architecture | `x64` | 
| Emulator library | Bridgetek `EVE_Emulator` |
| Build environment | Visual Studio or a CMake-compatible toolchain | 
| Platform definition | `PLATFORM_EMULATOR` |

## Installing the EVE Emulator Library 

The **EVE_Emulator** library is available from the following repository: [Bridgetek EVE_Emulator](https://github.com/Bridgetek/EVE_Emulator) 

The repository is included in EVE-MCU-Dev as a Git submodule at:

```text
ports/eve_emulator/EVE_Emulator
```

When cloning EVE-MCU-Dev, initialise the submodule using:

```console
git clone --recurse-submodules https://github.com/Bridgetek/Eve-MCU-Dev.git
```

For an existing clone, run:

```console
git submodule update --init --recursive
```

If the EVE-MCU-Dev repository is distributed without its submodules, the complete EVE_Emulator repository can instead be copied manually into:

```text
ports/eve_emulator/EVE_Emulator
```

The emulator header files, import library, and runtime DLLs should all be taken from the same EVE_Emulator revision. Mixing files from different releases may cause build or runtime compatibility problems.


## Using the EVE Emulator in EVE-MCU-Dev Projects

### Enabling the Emulator Port

The `PLATFORM_EMULATOR` macro enables an EVE-MCU-Dev application to use the `EVE Emulator` port.

#### Visual Studio

In Visual Studio, add the following definition under:

```text
Project Properties
    -> Configuration Properties
    -> C/C++
    -> Preprocessor
    -> Preprocessor Definitions
```

```text
PLATFORM_EMULATOR
```

Ensure that the project is configured to build for the `x64` platform.

#### CMake

Add the following definition to the project's `CMakeLists.txt` file:

```cmake
# Use the EVE Emulator platform port.
add_compile_definitions(PLATFORM_EMULATOR)
```

#### GCC or MinGW

Add the following compiler definition:

```console
-DPLATFORM_EMULATOR
```

The selected GCC or MinGW toolchain must generate a 64-bit Windows application.

### Runtime DLLs

The `EVE Emulator` runtime DLLs are supplied in:

```text
ports/eve_emulator/EVE_Emulator/bin
```

The required DLLs must be available in the same output directory as the application executable.

| File | Purpose |
| --- | --- |
| `bt8xxemu.dll` | Main EVE Emulator runtime library |
| `mx25lemu.dll` | Additional flash-emulation dependency |
| `zlib.dll` | Compression dependency used bbyy EVE for certain API function calls |

The provided Visual Studio projects and `CMakeLists.txt` files automatically copy the available DLLs to the executable output directory after the application is built.

In most configurations, `bt8xxemu.dll` is the primary runtime dependency. The additional DLLs are copied so that optional runtime dependencies are also available.

## Visual Studio Projects

The Visual Studio project files for an example are normally located in its:

```text
emulator/VisualStudio
```

subdirectory.

Object files are placed into the `VisualStudio\obj` folder. Program executables are placed into the `VisualStudio\x64\{Debug|Release}` folder.
The application must be built using an `x64` configuration.

When an example application is built in Visual Studio, a post-build event copies `bt8xxemu.dll`, `mx25lemu.dll`, and `zlib.dll` to the executable output directory.


### Visual Studio

The Visual Studio project files for examples are found in the `\emulator\VisualStudio` folder. .

### Command-Line Builds

To configure an example using CMake's default generator:

```console
cmake -B build -S .
```

To explicitly select Visual Studio 2022 and the `x64` platform:

```console
cmake -B build -S . -G "Visual Studio 17 2022" -A x64
```

To explicitly select MinGW Makefiles:

```console
cmake -B build -S . -G "MinGW Makefiles"
```

The selected MinGW installation must support 64-bit Windows applications.

After configuring the project, build it using:

```console
cmake --build build
```

For a multi-configuration generator such as Visual Studio, the build configuration can be selected explicitly:

```console
cmake --build build --config Debug
```

or:

```console
cmake --build build --config Release
```

The executable location depends on the selected generator and build configuration. For example, a Visual Studio build commonly places the executable in `build\Debug` or `build\Release`.

## Flash and SD Card Support

The emulator port supports the following storage features:

| Feature | Configuration macro | Supported EVE API levels |
| --- | --- | --- |
| External flash image | `EVE_EMULATOR_FLASH_FILE` | 3, 4, and 5 |
| SD-card folder | `EVE_EMULATOR_SD_FOLDER` | 5 only |

Defining one of these macros for an unsupported EVE API level causes a compilation error.
## Emulator Asset Paths for Flash and SD Cards

The flash-image and SD-card-folder macros use the same path-resolution rules.

On Windows, the native emulator API defines `eve_tchar_t` as `wchar_t`. Paths should therefore be supplied using either the Windows `TEXT("...")` macro or a wide string literal such as `L"..."`.

### Relative Paths

Relative paths are resolved from the directory containing the application executable.

They are not resolved from:

- The source-code directory.
- The directory containing the Visual Studio project.
- The current terminal working directory.

For example:

```c
TEXT("assets\\flash.bin")
```

is resolved as:

```text
<executable-directory>\assets\flash.bin
```

A path beginning with a single `/` or `\` is also treated as relative to the executable directory. The leading separator is removed before the path is resolved.

For clarity, relative paths should normally be specified without a leading separator.

### Absolute Paths

Drive-rooted Windows paths are treated as absolute:

```c
TEXT("C:\\EVE\\assets\\flash.bin")
```

UNC paths are also treated as absolute:

```c
TEXT("\\\\server\\share\\flash.bin")
```

### Path Validation

Before starting or configuring the emulator, the port verifies that:

- A flash-image path identifies an existing file.
- An SD-card path identifies an existing folder.
- The resolved path fits within the Windows `MAX_PATH` limit used by the port.

If the path is invalid, the emulator initialisation fails and an error message is reported.

## Flash Emulation (BT81x, BT82x)

External flash emulation is supported for EVE API levels 3, 4, and 5.

When `EVE_EMULATOR_FLASH_FILE` is defined, the port creates the emulated flash device and attaches it to the emulator configuration before the emulator instance is started.

Flash-image `.bin` files can be generated using the [EVE Asset Builder](https://brtchip.com/eab/) toolchain.

### Defining the Flash Image

The `EVE_EMULATOR_FLASH_FILE` macro identifies the flash-image file attached to the emulator.

The macro can specify a file in the executable directory, a path relative to the executable directory, or an absolute Windows path.

#### Local File

```c
EVE_EMULATOR_FLASH_FILE=TEXT("flash-image-name.bin")
```

This looks for the following file:

```text
<executable-directory>\flash-image-name.bin
```

#### Relative Path

```c
EVE_EMULATOR_FLASH_FILE=TEXT("assets\\flash-image-name.bin")
```

A parent-relative path can also be used:

```c
EVE_EMULATOR_FLASH_FILE=TEXT("..\\assets\\flash-image-name.bin")
```

#### Absolute Path

```c
EVE_EMULATOR_FLASH_FILE=TEXT("C:\\path\\to\\flash-image-name.bin")
```

#### UNC Path

```c
EVE_EMULATOR_FLASH_FILE=TEXT("\\\\server\\share\\flash-image-name.bin")
```

> **Note:** Do not define `EVE_EMULATOR_FLASH_FILE` without a value. The macro must expand to a valid `eve_tchar_t` string. A valueless definition will normally cause a compilation error.

### Defining the Flash Macro in Visual Studio

Add the following under the project's preprocessor definitions:

```text
EVE_EMULATOR_FLASH_FILE=TEXT("flash-image-name.bin")
```

### Defining the Flash Macro in CMake

When defining the path through CMake, use a wide string literal:

```cmake
# Use a flash image from the executable output directory.
add_compile_definitions(
    EVE_EMULATOR_FLASH_FILE=L\"flash-image-name.bin\"
)
```

Using a wide string literal avoids CMake interpreting the parentheses in `TEXT(...)` as part of a function-style preprocessor definition.

### Defining the Flash Macro with GCC or MinGW

For shells which use backslash escaping, add:

```console
-DEVE_EMULATOR_FLASH_FILE=L\"flash-image-name.bin\"
```

The exact quotation rules may vary between shells and build systems.

### Flash Image Size

The `EVE_EMULATOR_FLASH_FILE_SIZE` macro defines the size of the emulated flash device in bytes.

If the macro is not defined, the port uses a default size of 8 MiB:

```c
#define EVE_EMULATOR_FLASH_FILE_SIZE (8 * 1024 * 1024)
```

For example, the equivalent numeric preprocessor definition is:

```text
EVE_EMULATOR_FLASH_FILE_SIZE=8388608
```

The configured size should be suitable for both the selected EVE device and the supplied flash image.

### Flash Persistence

Flash writes are temporary by default and are not written back to the flash-image file.

The native `EVE Emulator` API supports persistent flash writes through the `Persistent` member of `BT8XXEMU_FlashParameters`. The EVE-MCU-Dev emulator port does not currently expose this setting through a configuration macro.

### Flash Image Recommendations

It is recommended that flash-image files be placed in the same directory as the application executable and configured using:

```c
EVE_EMULATOR_FLASH_FILE=TEXT("flash-image-name.bin")
```

Keeping the image beside the executable allows the project to be compressed, shared, and relocated without changing an absolute path.

The flash-image file can be copied to the executable output directory using:

- A CMake post-build command.
- A Visual Studio post-build event.
- A manual copy operation.

## SD-Card Folder Emulation (BT82x)

SD-card folder emulation is supported for EVE API level 5 and the BT820.

When `EVE_EMULATOR_SD_FOLDER` is defined, the port starts the emulator and then attaches the specified host folder using `BT8XXEMU_insertSDCardFolder`.

SD-card assets can be generated using the [EVE Asset Builder](https://brtchip.com/eab/) toolchain.

### Defining the SD-Card Folder

The `EVE_EMULATOR_SD_FOLDER` macro identifies the host folder attached as the emulated SD card.

The macro can specify a folder in the executable directory, a path relative to the executable directory, or an absolute Windows path.

#### Local Folder

```c
EVE_EMULATOR_SD_FOLDER=TEXT("sdFolderName")
```

This looks for the following folder:

```text
<executable-directory>\sdFolderName
```

#### Relative Path

```c
EVE_EMULATOR_SD_FOLDER=TEXT("assets\\sdFolderName")
```

A parent-relative path can also be used:

```c
EVE_EMULATOR_SD_FOLDER=TEXT("..\\assets\\sdFolderName")
```

#### Absolute Path

```c
EVE_EMULATOR_SD_FOLDER=TEXT("C:\\path\\to\\sdFolderName")
```

#### UNC Path

```c
EVE_EMULATOR_SD_FOLDER=TEXT("\\\\server\\share\\sdFolderName")
```

> **Note:** Do not define `EVE_EMULATOR_SD_FOLDER` without a value. The macro must expand to a valid `eve_tchar_t` string. A valueless definition will normally cause a compilation error.

### Defining the SD-Card Macro in Visual Studio

Add the following under the project's preprocessor definitions:

```text
EVE_EMULATOR_SD_FOLDER=TEXT("sdFolderName")
```

### Defining the SD-Card Macro in CMake

When defining the folder through CMake, use a wide string literal:

```cmake
# Attach a folder from the executable output directory as an SD card.
add_compile_definitions(
    EVE_EMULATOR_SD_FOLDER=L\"sdFolderName\"
)
```

### Defining the SD-Card Macro with GCC or MinGW

For shells which use backslash escaping, add:

```console
-DEVE_EMULATOR_SD_FOLDER=L\"sdFolderName\"
```

The exact quotation rules may vary between shells and build systems.

### Minimum SD-Card Size

The `EVE_EMULATOR_SD_FOLDER_SIZE` macro is passed to the `minimumSize` parameter of `BT8XXEMU_insertSDCardFolder`.

If the macro is not defined, the port passes zero:

```c
#define EVE_EMULATOR_SD_FOLDER_SIZE 0
```

Passing zero allows the emulator to calculate the required FAT32 image size from the contents of the folder.

A non-zero minimum size can be provided as a numeric preprocessor definition. For example, a minimum size of 64 MiB can be defined as:

```text
EVE_EMULATOR_SD_FOLDER_SIZE=67108864
```

### SD-Card Read and Write Behaviour

The EVE-MCU-Dev port currently attaches the folder-backed SD card with the native API's `readOnly` parameter set to `false`.

The emulated application can therefore write to the virtual SD card during the emulator session. 

### SD-Card Folder Recommendations

It is recommended that the SD-card folder be placed in the same directory as the application executable and configured using:

```c
EVE_EMULATOR_SD_FOLDER=TEXT("sdFolderName")
```

Keeping the folder beside the executable allows the project to be compressed, shared, and relocated without changing an absolute path.

The folder can be copied to the executable output directory using:

- A CMake post-build command.
- A Visual Studio post-build event.
- A manual copy operation.

## Emulator Limitations

### Multi-Touch Support

The `EVE Emulator` DLL provides the following API functions for simulating multiple touch points:

```c
BT8XXEMU_touchSetXY()
BT8XXEMU_touchResetXY()
```

The touch index accepted by these functions allows multiple touch points to be controlled by a host application.

Multi-touch support is not currently implemented by the EVE-MCU-Dev emulator port. Applications using the standard port are limited to a single simulated touch point controlled through the mouse.

### Display Rotation

When display rotation is performed using `EVE_REG_ROTATE` or `EVE_CMD_SETROTATE`, the dimensions of the emulator window are determined by the configured display resolution.

For rotations which swap the horizontal and vertical axes, such as a 90-degree or 270-degree rotation, the values of `EVE_DISP_WIDTH` and `EVE_DISP_HEIGHT` must also be swapped in the active display configuration.

This ensures that the emulator creates a window with the correct orientation and aspect ratio.

For example, if the normal display configuration is:

```c
#define EVE_DISP_WIDTH  800
#define EVE_DISP_HEIGHT 480
```

a configuration intended to operate permanently in a 90-degree rotated orientation should use:

```c
#define EVE_DISP_WIDTH  480
#define EVE_DISP_HEIGHT 800
```

The `EVE_REG_ROTATE` register or `EVE_CMD_SETROTATE` command controls how the EVE display is rendered, but it does not automatically resize or reshape the host window created by the emulator.

### Power-Down Behaviour

The EVE power-down sequence is not emulated by this port.

Calls to:

```c
MCU_PDlow()
MCU_PDhigh()
```

do not stop or restart the emulator instance.

This behaviour is intentional. The normal EVE-MCU-Dev initialisation sequence calls `MCU_PDlow()` and `MCU_PDhigh()` immediately after creating the emulator, which would otherwise destroy and recreate the newly started instance.

### Closing the Emulator Window

Closing the emulator window also terminates the host application.

The window state is checked when the application next begins a communication transaction with the emulator. When the closed window is detected, the port:

1. Stops and destroys the emulator instance.
2. Releases the emulator resources.
3. Exits the host application with an exit status of zero.

The application may therefore continue briefly after the window is closed if it does not immediately perform another emulator communication transaction.

## Troubleshooting

### The Application Cannot Find `bt8xxemu.dll`

Ensure that `bt8xxemu.dll` is in the same directory as the application executable.

Rebuild the project to run its post-build copy operation, or manually copy the DLL from:

```text
ports\eve_emulator\EVE_Emulator\bin
```

If required, copy `mx25lemu.dll` and `zlib.dll` to the same directory.

### The Application or DLL Has the Wrong Architecture

Ensure that:

- The application is built for `x64`.
- The selected Visual Studio configuration is `x64`.
- The selected MinGW or GCC toolchain produces 64-bit Windows applications.
- All emulator DLLs come from the same EVE_Emulator revision.

A 32-bit application cannot load the supplied 64-bit emulator DLLs.

### The Flash Image Cannot Be Opened

Check that:

- `EVE_EMULATOR_FLASH_FILE` has a valid value.
- The file exists.
- The path identifies a file rather than a folder.
- A relative path is relative to the executable directory.
- Backslashes in C string literals are escaped.
- The resolved path does not exceed the path length supported by the port.

For example:

```c
EVE_EMULATOR_FLASH_FILE=TEXT("flash-image-name.bin")
```

### The SD-Card Folder Cannot Be Attached

Check that:

- EVE API level 5 is selected.
- `EVE_EMULATOR_SD_FOLDER` has a valid value.
- The folder exists.
- The path identifies a folder rather than a file.
- A relative path is relative to the executable directory.
- Backslashes in C string literals are escaped.

For example:

```c
EVE_EMULATOR_SD_FOLDER=TEXT("sdFolderName")
```

### An Unsupported EVE API Compilation Error Is Reported

`EVE_EMULATOR_FLASH_FILE` requires EVE API level 3, 4, or 5.

`EVE_EMULATOR_SD_FOLDER` requires EVE API level 5.

Remove the unsupported macro or select a compatible EVE API level.

### The Emulator Window Has the Wrong Orientation

When using a rotation which swaps the display axes, swap the configured values of:

```c
EVE_DISP_WIDTH
EVE_DISP_HEIGHT
```

The rotation register or command changes the rendered output but does not dynamically resize the emulator window.

## Linux

Linux builds are not supported because the EVE Emulator library is not currently distributed for Linux.
