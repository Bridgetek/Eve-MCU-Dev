# EVE-MCU-Dev Test Code

[Back](../README.md)

The following test code is available to verify the operation of the EVE-MCU-Dev API.

- [Boot Tester](#boot-tester) Test the API can boot the EVE device past the `EVE_Init()` call.

Each test program will return *zero* if successful or *non-zero* if it failed.

## Boot Tester

The `boottester` code will test the API is able to complete the boot process in `EVE_Init()`. This will call `HAL_EVE_Init()` which will in turn call `MCU_Init()`. The MCU layer is replaced by the SOFTWARE_SIMULATOR layer implemented in `EVE_simulator.c`.

Supported EVE APIs in this test:

| EVE API 1 | EVE API 2 | EVE API 3 | EVE API 4 | EVE API 5 |
| --- | --- | --- | --- | --- |
| Yes | Yes | Yes | Yes | Yes |

### Compiling the Boot Tester

To configure the tester with the default C compiler:
```
    cmake -B build -S .
```
Then to compile:
```
    cmake --build build
```
Default builds are compiled with debugging enabled. 

The device and panel selection can be made in the command line to override the values in `EVE_Config.h`. This is done by adding the definitions to the CMake configuration command with the `-D` option.
```
cmake --fresh -B build -S . -DFT8XX_TYPE=BT881
```

### Running the Boot Tester

To run this file simple call the executable from the command line.
```
./build/simple_raspberry_pi
```

#### Sample Output

For a boot tester compiled as follows:
```
EVE-MCU-Dev\test\boottester> cmake --fresh -B build -S . -DMODULE_TYPE=IDM20407A
EVE-MCU-Dev\test\boottester> cmake --build build
```

The following output will be observed:

```
---------------------------------------------------------------- 
Welcome to EVE-MCU-Dev Tester Program

EVE device: BT817
EVE_API: 4
EVE Module Name: IDM2040-7A
EVE Panel Type: DP-0701-11A WVGA (Capacitive)
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

## Files and Folders

The example contains a common directory with several files which comprises all the demo functionality.

| File/Folder | Description |
| --- | --- |
| [test/EVE_simulator.c](EVE_simulator.c) | Software simulator code |
| [test/boottester/main.c](boottester/main.c) | Program to run simulator to boot |
| [test/boottester/CMakeLists.txt](boottester/CMakeLists.txt) | Build file for test |
