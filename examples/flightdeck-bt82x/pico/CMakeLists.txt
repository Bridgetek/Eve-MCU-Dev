# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (NOT EXISTS ${VSCODE_CWD})
        if (NOT DEFINED ENV{PICO_SDK_PATH})
                message(FATAL_ERROR "PICO_SDK_PATH environment variable is not set")
        endif()
endif()

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.3.0)
set(toolchainVersion 15_2_Rel1)
set(picotoolVersion 2.3.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
if (EXISTS ${VSCODE_CWD})
        include(pico_sdk_import.cmake)
else ()
        include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
endif ()

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

include(../../examples.cmake)

project(${EXAMPLE_PROJECT_NAME} C CXX)

# Include the common CMake recipie for this example:
include(../common.cmake)

# Include any port CMake additions for this example:
include(../../../ports/eve_arch_rpi/pico.cmake)

# Add platform specific build files:
target_sources(${EXAMPLE_PROJECT_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}main/main.c
        )

# Enable debug logging output to stdio:
add_compile_definitions(EVE_DEBUG_LEVEL=0)

# Default name is the project name, version 0.1
pico_set_program_name(${EXAMPLE_PROJECT_NAME} "flightdeck-bt82x")
pico_set_program_version(${EXAMPLE_PROJECT_NAME} "0.1")

# Build Final Executable:
target_link_libraries(${EXAMPLE_PROJECT_NAME}
        eve_example # Depends on example code library
        eve_library # Depends on EVE library
        hardware_flash # For non-volatile storage
        hardware_sync # For non-volatile storage
        pico_stdlib # For standard library
        )

# Add the standard include files to the build
target_include_directories(${EXAMPLE_PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

pico_add_extra_outputs(${EXAMPLE_PROJECT_NAME})

add_custom_command(TARGET ${EXAMPLE_PROJECT_NAME}
    POST_BUILD
    COMMAND arm-none-eabi-size --format=berkeley ${EXAMPLE_PROJECT_NAME}.elf
    VERBATIM
)
