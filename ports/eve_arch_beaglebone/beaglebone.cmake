# Common CMake include file for all examples using Beaglebone

# Tell EVE library to use BeagleBone as base class:
# Select the BeagleBone platform:
add_compile_definitions(PLATFORM_BEAGLEBONE)
# Select the Linux /dev/spidev0 as a character device for transfers:
add_compile_definitions(USE_LINUX_SPI_DEV)

# Source code for EVE library (targetted at Beaglebone):
target_sources(eve_library PUBLIC
        # Source code for EVE library
        ${API_DIRECTORY}/ports/eve_arch_beaglebone/EVE_Linux_BBB.c
)
