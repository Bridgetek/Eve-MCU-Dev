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
    message(EVE_DISPLAY_RES="${EVE_DISPLAY_RES}")
ENDIF(EVE_DISPLAY_RES)
ENDIF(EVE_PANEL)
ENDIF(EVE_MODULE)

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
        ${API_DIRECTORY}/source/extensions/bt82x_patch.c # Only applicable to BT82x
        ${API_DIRECTORY}/source/extensions/custom_touch_fw.c # Only applicable to FT81x/BT88x/BT81x
)
# Header files used for EVE library
target_include_directories(eve_library PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${API_DIRECTORY}/include
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
