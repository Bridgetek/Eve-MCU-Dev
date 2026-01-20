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
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
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

# Add executable. Default name is the project name, version 0.1

# Set the location of the EVE-MCU-Dev tree
set(API_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../../../")

# Set name of project (as current working directory)
get_filename_component(PARENT_DIR ${CMAKE_CURRENT_LIST_DIR} DIRECTORY)
get_filename_component(ProjectName ${PARENT_DIR} NAME)
get_filename_component(ProjectId ${CMAKE_CURRENT_LIST_DIR} NAME)
string(REPLACE " " "_" ProjectId ${ProjectId})
project(${ProjectName}_${ProjectId} C CXX)

message(API_DIRECTORY="${API_DIRECTORY}")
message(ProjectId="${PROJECT_SOURCE_DIR}")
message(PROJECT_NAME="${PROJECT_NAME}")
message(CMAKE_C_COMPILER_ID="${CMAKE_C_COMPILER_ID}")
message(CMAKE_SYSTEM_NAME="${CMAKE_SYSTEM_NAME}")

# Setup command line options for build - feeds into EVE_config.h
option(MODULE_TYPE "EVE Module Name" )
IF(MODULE_TYPE)
    ADD_DEFINITIONS(-DMODULE_TYPE=${MODULE_TYPE})
ENDIF(MODULE_TYPE)
option(FT8XX_TYPE "EVE Device Type Name" )
IF(FT8XX_TYPE)
    ADD_DEFINITIONS(-DFT8XX_TYPE=${FT8XX_TYPE})
ENDIF(FT8XX_TYPE)
option(PANEL_TYPE "EVE Panel Type" )
IF(PANEL_TYPE)
    ADD_DEFINITIONS(-DPANEL_TYPE=${PANEL_TYPE})
ENDIF(PANEL_TYPE)
option(DISPLAY_RES "Panel Display Resolution" )
IF(DISPLAY_RES)
    ADD_DEFINITIONS(-DDISPLAY_RES=${DISPLAY_RES})
ENDIF(DISPLAY_RES)

# Add environment for example code
set(EXAMPLE_COMMON "${CMAKE_CURRENT_SOURCE_DIR}/../common")
set(EXAMPLE_SNIPPETS "${CMAKE_CURRENT_SOURCE_DIR}/../../snippets")
set(EXAMPLE_ASSETS "${CMAKE_CURRENT_SOURCE_DIR}/../assets")

# Point to where to find the platform source file
add_executable(${PROJECT_NAME}
        main/main.c
)
pico_set_program_name(${PROJECT_NAME} "medinfo")
pico_set_program_version(${PROJECT_NAME} "0.1")
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${API_DIRECTORY}/include
        ${EXAMPLE_COMMON}
	${EXAMPLE_SNIPPETS}/widgets
)

# Tell EVE library to use RP2040 as base class
add_compile_definitions(PLATFORM_RP2040)

# The pico port can only use the EVE Flash or C arrays for storage of assets
# Comment out this line to set the method in common/eve_example.h
add_compile_definitions(ASSETS=USE_C_ARRAYS)

# Trig tables are used instead of maths functions
add_compile_definitions(USE_TRIG_TABLE)

# Enable debug logging output to stdio
add_compile_definitions(DEBUG_LEVEL=0)
# Milliseconds to wait for stdio connection
# (will also have stdio_usb_connected() call in main to wait for connection)
add_compile_definitions(PICO_STDIO_USB_CONNECT_WAIT_TIMEOUT_MS=0)

# Modify the below lines to enable/disable output over USB
pico_enable_stdio_usb(${PROJECT_NAME} 1)

# Source code for EVE library (targetted at RP2040)
add_library(eve_library # EVE library name
        # Source code for EVE library
        ${API_DIRECTORY}/source/EVE_API.c
        ${API_DIRECTORY}/source/EVE_HAL.c
        ${API_DIRECTORY}/ports/eve_arch_rpi/EVE_MCU_RP2040.c
        #${API_DIRECTORY}/ports/eve_bt82x/patch_base.c # Only applicable to BT82x
)
# Header files used for EVE library
target_include_directories(eve_library PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${API_DIRECTORY}/include
        ${API_DIRECTORY}/ports/eve_bt82x # BT82x base patch
)

# Source for example code
add_library(eve_example # Example code library name
        # Source files for example code library
        ${EXAMPLE_COMMON}/eve_example.c
        ${EXAMPLE_COMMON}/eve_assetload_array.c
        ${EXAMPLE_COMMON}/eve_assetload_file.c
        ${EXAMPLE_COMMON}/eve_assetload_flash.c
        ${EXAMPLE_COMMON}/eve_tracedata.c
        ${EXAMPLE_COMMON}/eve_graph_extension.c
        ${EXAMPLE_COMMON}/eve_graph_vertexes.c
        ${EXAMPLE_SNIPPETS}/touch.c
        ${EXAMPLE_SNIPPETS}/widgets/sevenseg.c
        ${EXAMPLE_COMMON}/patch_medinfo.c
)

# Example code assets
# Add all C files, the EVE generation macro selects the correct data
file(GLOB EVE_ASSET_LIST CONFIGURE_DEPENDS 
        # Add any assets that are compiled in
        "${EXAMPLE_ASSETS}/c_array_assets/*.c")
target_sources(eve_example PRIVATE
        # Add specific asset files that are compiled in
        ${EVE_ASSET_LIST})

# Header files used for example code library
target_include_directories(eve_example PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${API_DIRECTORY}/include # Headers for EVE library
        ${API_DIRECTORY}/ports/eve_bt82x # BT82x base patch
        ${EXAMPLE_COMMON}/ # Headers for example code
        ${EXAMPLE_SNIPPETS}/ # Headers for snippets
)
# Example code dependencies
target_link_libraries(eve_example
        eve_library # Example code depends on EVE library
)

# Add the standard library to the build
target_link_libraries(${PROJECT_NAME}
        eve_example # Project depends on example code library
        hardware_flash # For non-volatile storage
        hardware_sync # For non-volatile storage
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

# Add any user requested libraries
target_link_libraries(eve_library 
        pico_stdlib
        hardware_spi
        pico_time
        )

pico_add_extra_outputs(${PROJECT_NAME})

add_custom_command(TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND arm-none-eabi-size --format=berkeley ${PROJECT_NAME}.elf
    VERBATIM
)
