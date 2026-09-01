# Set minimum required version of CMake
cmake_minimum_required(VERSION 3.12)

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

# Set C/C++ Standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Setup command line options for build - feeds into EVE_config.h
# A module type implies both a panel and eve type
# - panel type, display resolution and eve device type are ignored
# A panel configures a display resolution
# - display resolution is ignored
option(EVE_MODULE "EVE Module Name" )
option(EVE_DEVICE "EVE Device Type Name" )
option(EVE_PANEL "EVE Panel Type" )
option(EVE_DISPLAY_RES "Panel Display Resolution" )

# Set a module type
IF(EVE_MODULE)
    ADD_DEFINITIONS(-DEVE_MODULE=${EVE_MODULE})
    message(EVE_MODULE="${EVE_MODULE}")
ELSE(EVE_MODULE)
# Set the eve device type
IF(EVE_DEVICE)
    ADD_DEFINITIONS(-DEVE_DEVICE=${EVE_DEVICE})
    message(EVE_DEVICE="${EVE_DEVICE}")
ENDIF(EVE_DEVICE)
# Set the panel type
IF(EVE_PANEL)
    ADD_DEFINITIONS(-DEVE_PANEL=${EVE_PANEL})
    message(EVE_PANEL="${EVE_PANEL}")
ELSE(EVE_PANEL)
# Set the display resolution
IF(EVE_DISPLAY_RES)
    ADD_DEFINITIONS(-DEVE_DISPLAY_RES=${EVE_DISPLAY_RES})
    message(EVE_DISPLAY_RES="${CMAKE_DISPLAY_RESSYSTEM_NAME}")
ENDIF(EVE_DISPLAY_RES)
ENDIF(EVE_PANEL)
ENDIF(EVE_MODULE)

# Add environment for example code
set(EXAMPLE_COMMON "${CMAKE_CURRENT_SOURCE_DIR}/../common")
set(EXAMPLE_SNIPPETS "${CMAKE_CURRENT_SOURCE_DIR}/../../snippets")

# Point to where to find the platform source file
add_executable(${PROJECT_NAME}
        main/main.c
)
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${API_DIRECTORY}/include
        ${EXAMPLE_COMMON}
)
# Tell EVE library to use Raspberry Pi as base class
# Select the Raspberry Pi platform
add_compile_definitions(PLATFORM_RASPBERRYPI)
# Select the Linux /dev/spidev0 as a character device for transfers
add_compile_definitions(USE_LINUX_SPI_DEV)

# Source code for EVE library (targtted at Raspberry Pi)
add_library(eve_library # EVE library name
        # Source code for EVE library
        ${API_DIRECTORY}/source/EVE_API.c
        ${API_DIRECTORY}/source/EVE_HAL_Linux.c
        ${API_DIRECTORY}/ports/eve_arch_rpi/EVE_Linux_RPi.c
        ${API_DIRECTORY}/source/extensions/bt82x_patch.c # Only applicable to BT82x
        ${API_DIRECTORY}/source/extensions/custom_touch_fw.c # Only applicable to FT81x/BT88x/BT81x
)
# Header files used for EVE library
target_include_directories(eve_library PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${API_DIRECTORY}/include       
)

# Source for example code
add_library(eve_example # Example code library name
        # Source files for example code library
        ${EXAMPLE_COMMON}/eve_example.c
        ${EXAMPLE_COMMON}/eve_fonts.c
        ${EXAMPLE_COMMON}/eve_images.c
        ${EXAMPLE_SNIPPETS}/touch.c
)

# Header files used for example code library
target_include_directories(eve_example PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${API_DIRECTORY}/include # Headers for EVE library
        ${EXAMPLE_COMMON}/ # Headers for example code
        ${EXAMPLE_SNIPPETS}/ # Headers for snippets 
)

# Example code dependencies
target_link_libraries(eve_example
        eve_library # Example code depends on EVE library
        gpiod
)

# Project dependencies
target_link_libraries(${PROJECT_NAME} 
        eve_example # Project depends on example code library
)
