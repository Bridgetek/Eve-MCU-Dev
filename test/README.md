# EVE-MCU-Dev Test Code

[Back](../README.md)

The following test code is available to verify the operation of the EVE-MCU-Dev API.

- [Boot Tester](#boot-tester) Test the API can boot the EVE device past the `EVE_Init()` call.
- [Example Tester](#example-tester) Test the example code will compile successfully.

Each test program will return *zero* if successful or *non-zero* if it failed.

The software simulator is used for all boot and example tests. 
It will set a macro called `SOFTWARE_SIMULATOR` to indicate which "platform" is selected.
Adaptations can be made in example code to cope with the software simulator running. 
For example, infinite loops are not performed and a single display list is generated in most sample.

**NOTE:** The software simulator will install a BT820 patch file at startup. 
To check consistency of the patch file a version report is compared with an expected value in the patch file. 
The `EVE_RAM_REPORT` response string in function `get_response` in [`EVE_simulator.c`](EVE_simulator.c) must match the `expected` string in function `eve_loadpatch` function of [`bt82x_patch.c`](../source/extensions/bt82x_patch.c).

## Boot Tester

The `boottester` code will test the API is able to complete the boot process in `EVE_Init()`. This will call `HAL_EVE_Init()` which will in turn call `MCU_Init()`. The MCU layer is replaced by the software simulator layer implemented in `EVE_simulator.c`.

Supported EVE APIs in this test:

| EVE API 1 | EVE API 2 | EVE API 3 | EVE API 4 | EVE API 5 |
| --- | --- | --- | --- | --- |
| Yes | Yes | Yes | Yes | Yes |

### Compiling the Boot Tester

To configure the tester with the default C compiler using the configuration in [`EVE_config.h`](../include/EVE_config.h):
```
    cmake -B build -S .
```

The device and panel selection can be made in the command line to override the values in `EVE_Config.h`. This is done by adding the definitions to the CMake configuration command with the `-D` option. To compile for a specific configuration option the following may be used:

- `EVE_MODULE` specify an EVE Module Name to set the `EVE_DEVICE` and `EVE_DISPLAY_RES` options.
- `EVE_DEVICE` set the EVE target device.
- `EVE_PANEL` select a specific panel type to use, implies `EVE_DEVICE_RES` is set.
- `EVE_DISPLAY_RES` set the display panel resolution.

Therefore to test for FT810 on a WVGA panel use the following:
```
    cmake -B build -S . -DEVE_DEVICE=EVE_FT810 -DEVE_DISPLAY_RES=EVE_WVGA
```
To select the IDK_BT817_70A module (BT817 IC Development Kit with 7.0-inch display).
```
    cmake -B build -S . -DEVE_MODULE=EVE_IDK_BT817_70A
```

Then to compile:
```
    cmake --build build
```

By default, builds are compiled with debugging enabled. 

### Running the Boot Tester

To run this file simple call the executable from the command line on Linux:
```
./build/boottester
```
On Windows
```
./build/boottester.exe
```

#### Sample Output

For a boot tester compiled as follows:
```
EVE-MCU-Dev\test\boottester> cmake --fresh -B build -S . -DEVE_MODULE=EVE_IDM20407A
EVE-MCU-Dev\test\boottester> cmake --build build
```

The following output will be observed:

```
---------------------------------------------------------------- 
Welcome to EVE-MCU-Dev Tester Program

EVE device: BT817
EVE_API: 4
EVE Module Name: IDM2040-7A
EVE Panel Type: DP-0701-01A WVGA (Capacitive)
EVE display panel resolution: WVGA 800x480

delay 20 ms
reset
delay 20 ms
booting
delay 20 ms
host command: 44 00 00
host command: 61 46 00
host command: 68 00 00
host command: 00 00 00
idle
addr: read 302000 REG_ID 7c
addr: read 302020 REG_CPURESET 00
addr: write 30200c REG_FREQUENCY 044aa200
addr: write 302034 REG_HSIZE 0320
addr: write 30202c REG_HCYCLE 03a0
addr: write 302030 REG_HOFFSET 0058
addr: write 302038 REG_HSYNC0 0000
addr: write 30203c REG_HSYNC1 0030
addr: write 302048 REG_VSIZE 01e0
addr: write 302040 REG_VCYCLE 020d
addr: write 302044 REG_VOFFSET 0020
addr: write 30204c REG_VSYNC0 0000
addr: write 302050 REG_VSYNC1 0003
addr: write 30206c REG_PCLK_POL 01
addr: write 302064 REG_SWIZZLE 00
addr: write 302068 REG_CSPREAD 00
addr: write 302060 REG_DITHER 01
addr: write 300000 RAM_DL 02000000
addr: write 300004 RAM_DL 26000007
addr: write 300008 RAM_DL 00000000
addr: write 302054 REG_DLSWAP 02
addr: read 302094 REG_GPIO 00
addr: write 302094 REG_GPIO 80
addr: write 302070 REG_PCLK 02
addr: write 3020d4 REG_PWM_DUTY 7f
addr: write 302118 REG_TOUCH_RZTHRESH 04b0
addr: write 302080 REG_VOL_PB 00
addr: write 302084 REG_VOL_SOUND 00
addr: write 302088 REG_SOUND 6000
addr: read 302574 REG_CMDB_SPACE 00000ffc
addr: write 302578 REG_CMDB_WRITE ffffff00 02000000 26000007 00000000 ffffff01
addr: read 302574 REG_CMDB_SPACE 00000ffc
addr: read 302574 REG_CMDB_SPACE 00000ffc
addr: write 302578 REG_CMDB_WRITE ffffff00 02000000 26000007 05000000 ffffff43 00000000 00000000 00000000 05000001 ffffff43 00000000 00000000 00000000 05000002 ffffff43 00000000 00000000 00000000 05000003 ffffff43 00000000 00000000 00000000 05000004 ffffff43 00000000 00000000 00000000 05000005 ffffff43 00000000 00000000 00000000 05000006 ffffff43 00000000 00000000 00000000 05000007 ffffff43 00000000 00000000 00000000 05000008 ffffff43 00000000 00000000 00000000 05000009 ffffff43 00000000 00000000 00000000 0500000a ffffff43 00000000 00000000 00000000 0500000b ffffff43 00000000 00000000 00000000 0500000c ffffff43 00000000 00000000 00000000 0500000d ffffff43 00000000 00000000 00000000 0500000e ffffff43 00000000 00000000 00000000 0500000f ffffff43 00000000 00000000 00000000 00000000 ffffff01 
addr: read 302574 REG_CMDB_SPACE 00000ffc
```

## Example Tester

The `exampletester` code will compile an example using the simulator layer to replace the MCU layer. The simulator uses the software simulator which is implemented in `EVE_simulator.c`.

Supported EVE APIs in this test:

| EVE API 1 | EVE API 2 | EVE API 3 | EVE API 4 | EVE API 5 |
| --- | --- | --- | --- | --- |
| Yes | Yes | Yes | Yes | Yes |

### Compiling the Example Tester

To configure the tester with the default C compiler using the configuration in [`EVE_config.h`](../include/EVE_config.h):
```
    cmake -B build -S .
```

As with the [boottester](#compiling-the-boot-tester) the device and panel selection can be made in the command line to override the values in `EVE_Config.h`. 

      {EXAMPLE: simple, ASSETS: 0, EVE1: EVE_QVGA, EVE2: EVE_WQVGA, EVE3: EVE_WVGA, EVE4: EVE_WXGA, EVE5: EVE_WUXGA }, 
      run: cmake -B build -S . -G Ninja -DEVE_DEVICE=EVE_BT820 -DEVE_DISPLAY_RES=${{ matrix.eve.EVE5 }} -DEXAMPLE=${{ matrix.eve.EXAMPLE }} -DASSETS=${{ matrix.eve.ASSETS }}

Additional options for the example tester are required to set the target example to compile. To compile for a specific example the following is be used:

- `EXAMPLE` specify an example directory to use in the [`examples`](../examples/) directory.
- `ASSETS` sets the macro for any assets which are used in the example. Normally this is set in the example configuration.

To compile the B2TF example for the IDM2040-43A (BT883 with DP-0431-11A):
```
    cmake -B build -S . -DEVE_MODULE=EVE_IDM204043A -DEXAMPLE=b2tf
```

Then to compile:
```
    cmake --build build
```

By default, builds are compiled with debugging enabled. 

### Running the Example Tester

To run this file simple call the executable from the command line.
```
./build/boottester
```

## Files and Folders

The example contains a common directory with several files which comprises all the demo functionality.

| File/Folder | Description |
| --- | --- |
| [test/EVE_simulator.c](EVE_simulator.c) | Software simulator code |
| [test/boottester/main.c](boottester/main.c) | Program to run simulator to boot |
| [test/boottester/CMakeLists.txt](boottester/CMakeLists.txt) | Build file for test |
| [test/exampletester/main.c](exampletester/main.c) | Program to run simulator to test the example compilation |
| [test/exampletester/CMakeLists.txt](exampletester/CMakeLists.txt) | Build file for examples |
