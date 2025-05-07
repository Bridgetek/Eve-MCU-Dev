# EVE-MCU-BRT_AN_025-Example-Pico
Example code for Application Note BRT_AN_025 for the Raspberry Pi Pico Board

Refer to the Bridgetek Application Note document BRT_AN_025 for details on the example code.

To build use CMake. Firstly initialise the build directory into a directory called `build`. This only needs to be done once.

```
$ cmake -B build -S .
```

Next, build the code.

```
$ cmake --build build
```

The build will end with the following messages. Places where compilation steps are not shown are marked in the output.

```
[  1%] Built target bs2_default
[  2%] Built target bs2_default_padded_checksummed_asm
Scanning dependencies of target eve_library
<<<< SKIPPED COMPILATIONS >>>>
[  3%] Building C object CMakeFiles/eve_library.dir/mnt/c/Users/gordon.mcnab/Documents/GitHub/EVE-MCU-Examples-BRT_AN_025/ports/eve_arch_rpi/EVE_MCU_RP2040.c.obj
/mnt/c/Users/gordon.mcnab/Documents/GitHub/EVE-MCU-Examples-BRT_AN_025/ports/eve_arch_rpi/EVE_MCU_RP2040.c:53:9: note: #pragma message: Compiling /mnt/c/Users/gordon.mcnab/Documents/GitHub/EVE-MCU-Examples-BRT_AN_025/ports/eve_arch_rpi/EVE_MCU_RP2040.c for Raspberry Pi pico RP2040
   53 | #pragma message "Compiling " __FILE__ " for Raspberry Pi pico RP2040"
      |         ^~~~~~~
<<<< SKIPPED COMPILATIONS >>>>
[  4%] Linking CXX static library libeve_library.a
[ 29%] Built target eve_library
[ 57%] Built target eve_example
[ 58%] Performing build step for 'ELF2UF2Build'
[100%] Built target elf2uf2
[ 59%] No install step for 'ELF2UF2Build'
[ 59%] Completed 'ELF2UF2Build'
[ 61%] Built target ELF2UF2Build
[ 62%] Linking CXX executable simple_pico.elf
   text    data     bss     dec     hex filename
  35228      40    3476   38744    9758 simple_pico.elf
[100%] Built target simple_pico
```

There will be the following files in the `build` directory:

```
$ ls build/
CMakeCache.txt  Makefile             elf2uf2    libeve_example.a  pico-sdk  simple_pico.dis  simple_pico.elf.map  simple_pico.uf2
CMakeFiles      cmake_install.cmake  generated  libeve_library.a  simple_pico.bin  simple_pico.elf  simple_pico.hex
```

To run the code on the Pico, power-up the pico with the BOOTSEL button depressed. It will instantiate a removable drive. Now copy the file `simple_pico.uf2` to the removable drive.
