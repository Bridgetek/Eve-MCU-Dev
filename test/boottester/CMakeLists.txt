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

# Source code for EVE library (targetted at FT4222)
add_library(eve_library # EVE library name
        # Source code for EVE library
        ${API_DIRECTORY}/source/EVE_API.c
        ${API_DIRECTORY}/source/EVE_HAL.c
        ${API_DIRECTORY}/test/EVE_simulator.c
        ${API_DIRECTORY}/ports/eve_bt82x/patch_base.c # Only applicable to BT82x
)
# Header files used for EVE library
target_include_directories(eve_library PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${API_DIRECTORY}/include
        ${API_DIRECTORY}/ports/eve_bt82x # BT82x base patch
        ${API_DIRECTORY}/ports/eve_tester # Tester Library headers
)

# Project dependencies
target_link_libraries(${PROJECT_NAME}
        eve_library # Example code depends on EVE library
)
