# Common CMake include file for all examples using Raspberry Pi

# Tell EVE library to use Raspberry Pi as base class
# Select the Raspberry Pi platform
add_compile_definitions(PLATFORM_RASPBERRYPI)
# Select the Linux /dev/spidev0 as a character device for transfers
add_compile_definitions(USE_LINUX_SPI_DEV)

# Source code for EVE library (targetted at Raspberry Pi):
target_sources(eve_library PUBLIC
        # Source code for EVE library
        ${API_DIRECTORY}/ports/eve_arch_rpi/EVE_Linux_RPi.c
)
