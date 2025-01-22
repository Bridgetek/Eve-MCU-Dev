# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
# Set minimum required version of CMake
cmake_minimum_required(VERSION 3.12)
#include build functions from Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# Set name of project (as current working directory)
get_filename_component(ProjectId ${CMAKE_CURRENT_LIST_DIR} NAME)
string(REPLACE " " "_" ProjectId ${ProjectId})
project(${ProjectId} C CXX)

# Set C/C++ Standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
# Set the target board to be a standard pico, you may change this for different targets
set(PICO_BOARD pico)

# Creates a pico-sdk subdirectory in our project for the libraries
pico_sdk_init()

# point out the CMake, where to find the executable source file
add_executable(${PROJECT_NAME}
        main/main.c
)
include_directories(
        ../../../include
        ../common/
)
# Tell EVE library to use RP2040 as base class
add_compile_definitions(PLATFORM_RP2040)

# Source code for EVE library (targetted at RP2040)
add_library(eve_library # EVE library name
        # Source code for EVE library
        ../../../source/EVE_API.c
        ../../../source/EVE_HAL.c
        ../../../ports/eve_arch_rpi/EVE_MCU_RP2040.c
)
# Header files used for EVE library
target_include_directories(eve_library PUBLIC
        ../../../include
)
# EVE library dependencies
target_link_libraries(eve_library
            pico_stdlib
            hardware_spi
)

# Source for application code
add_library(cruise # Code library name
        # Source files for application code library
        ../common/cruise.c
)
# Header files used for application code library
target_include_directories(cruise PUBLIC
        ../common/ # Headers for application code
        ../../../include # Headers for EVE library
)
# Example code dependencies
target_link_libraries(cruise
        eve_library # Example code depends on EVE library
)

# Source for example code
add_library(eve_helper # Helper code library name
        # Source files for helper code library
        ../common/eve_calibrate.c
        ../common/eve_helper.c
        ../../snippets/sevenseg/sevenseg.c
)
# Header files used for helper code library
target_include_directories(eve_helper PUBLIC
        ../common/ # Headers for helper code
        ../../../include # Headers for EVE library
        ../../snippets/sevenseg # Headers for snippets
)
# Example code dependencies
target_link_libraries(eve_helper
        eve_library # Example code depends on EVE library
)

# Project dependencies
target_link_libraries(${PROJECT_NAME} 
        cruise # Project depends on application library
        eve_helper # Project depends on helper code library
        hardware_flash # For non-volatile storage
        hardware_sync # For non-volatile storage
        pico_stdlib # Commonly used features (gpio, timer-delay etc)
)
# Create map/bin/hex/uf2 files.
pico_add_extra_outputs(${PROJECT_NAME})

add_custom_command(TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND arm-none-eabi-size --format=berkeley ${PROJECT_NAME}.elf
    VERBATIM
)

# Enable stdio on UART
pico_enable_stdio_uart(${PROJECT_NAME} 1)
# Enable stdio on USB
pico_enable_stdio_usb(${PROJECT_NAME} 1)
