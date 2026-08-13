# Set minimum required version of CMake
cmake_minimum_required(VERSION 3.12)

# Set the location of the EVE-MCU-Dev tree
set(API_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../../")

# Set name of project (as current working directory)
get_filename_component(PARENT_DIR ${CMAKE_CURRENT_LIST_DIR} DIRECTORY)
get_filename_component(ProjectName ${PARENT_DIR} NAME)
get_filename_component(ProjectId ${CMAKE_CURRENT_LIST_DIR} NAME)
string(REPLACE " " "_" ProjectId ${ProjectId})
project(${ProjectId} C CXX)

message(API_DIRECTORY="${API_DIRECTORY}")
message(ProjectId="${PROJECT_SOURCE_DIR}")
message(PROJECT_NAME="${PROJECT_NAME}")
message(CMAKE_C_COMPILER_ID="${CMAKE_C_COMPILER_ID}")
message(CMAKE_SYSTEM_NAME="${CMAKE_SYSTEM_NAME}")

# Set C/C++ Standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Directory for source code for a dummy build
option(EXAMPLE "Example directory")
# Set a dummy source code directory
IF(EXAMPLE)
    ADD_DEFINITIONS(-DEXAMPLE=${EXAMPLE})
    message(EXAMPLE="${EXAMPLE}")
ENDIF(EXAMPLE)

# Directory for assetss source code for a dummy build
option(ASSETS "Example assets flag")
# Set a dummy source code directory
IF(ASSETS)
    ADD_DEFINITIONS(-DASSETS=${ASSETS})
    message(ASSETS="${ASSETS}")
    # Set the assets flag
    add_compile_definitions(ASSETS=${ASSETS})
ENDIF(ASSETS)

# Setup command line options for build - feeds into EVE_config.h
# A module type implies both a panel and eve type
# - panel type, display resolution and eve device type are ignored
# A panel configures a display resolution
# - display resolution is ignored
option(MODULE_TYPE "EVE Module Name" )
option(FT8XX_TYPE "EVE Device Type Name" )
option(PANEL_TYPE "EVE Panel Type" )
option(DISPLAY_RES "Panel Display Resolution" )

# Set a module type
IF(MODULE_TYPE)
    ADD_DEFINITIONS(-DMODULE_TYPE=${MODULE_TYPE})
    message(MODULE_TYPE="${MODULE_TYPE}")
ELSE(MODULE_TYPE)
# Set the eve device type
IF(FT8XX_TYPE)
    ADD_DEFINITIONS(-DFT8XX_TYPE=${FT8XX_TYPE})
    message(FT8XX_TYPE="${FT8XX_TYPE}")
ENDIF(FT8XX_TYPE)
# Set the panel type
IF(PANEL_TYPE)
    ADD_DEFINITIONS(-DPANEL_TYPE=${PANEL_TYPE})
    message(PANEL_TYPE="${PANEL_TYPE}")
ELSE(PANEL_TYPE)
# Set the display resolution
IF(DISPLAY_RES)
    ADD_DEFINITIONS(-DDISPLAY_RES=${DISPLAY_RES})
    message(DISPLAY_RES="${DISPLAY_RES}")
ENDIF(DISPLAY_RES)
ENDIF(PANEL_TYPE)
ENDIF(MODULE_TYPE)

# Add environment for example code
set(EXAMPLE_SNIPPETS "${CMAKE_CURRENT_SOURCE_DIR}/../../examples/snippets")

# Point to where to find the platform source file
add_executable(${PROJECT_NAME}
        main.c
)
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${API_DIRECTORY}/include
)
# Tell EVE library to use SOFTWARE_SIMULATOR as base class
add_compile_definitions(SOFTWARE_SIMULATOR)

add_compile_definitions(DEBUG_LEVEL=1)
add_compile_definitions(MCU_SPI_TIMEOUT=8)
add_compile_definitions(MCU_SPI_TRANSFER=0x100)

# Use look up table instead of maths functions for trig
add_compile_definitions(USE_TRIG_TABLE) 

# Source code for EVE library (targetted at FT4222)
add_library(eve_library # EVE library name
        # Source code for EVE library
        ${API_DIRECTORY}/source/EVE_API.c
        ${API_DIRECTORY}/source/EVE_HAL.c
        ${API_DIRECTORY}/test/EVE_simulator.c
        ${API_DIRECTORY}/source/extensions/patch_base.c # Only applicable to BT82x
)
# Header files used for EVE library
target_include_directories(eve_library PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${API_DIRECTORY}/include
        ${API_DIRECTORY}/source/extensions # BT82x base patch & custom touch FW
        ${API_DIRECTORY}/ports/eve_tester # Tester Library headers
)

# Source for example code
file(GLOB           eve_common_sources  CONFIGURE_DEPENDS ../../examples/${EXAMPLE}/common/*.c)
file(GLOB_RECURSE   eve_assets_sources  CONFIGURE_DEPENDS ../../examples/${EXAMPLE}/assets/*.c)
file(GLOB_RECURSE   eve_snippet_sources CONFIGURE_DEPENDS ${EXAMPLE_SNIPPETS}/*.c)

add_library(eve_example # Example code library name
        # Source files for example code library
        ${eve_common_sources}
        # Source code for assets
        ${eve_assets_sources}
        # All snippets
        ${eve_snippet_sources}
)

# Header files used for example code library
target_include_directories(eve_example PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${API_DIRECTORY}/include # Headers for EVE library
        ${API_DIRECTORY}/source/extensions # BT82x base patch & custom touch FW
        ../../examples/${EXAMPLE}/common
        ../../examples/${EXAMPLE}/assets
        ${EXAMPLE_SNIPPETS}/ # Headers for snippets 

)
# Example code dependencies
target_link_libraries(eve_example
        eve_library # Example code depends on EVE library
)

# Project dependencies
target_link_libraries(${PROJECT_NAME} 
        eve_example # Project depends on example code library
)
