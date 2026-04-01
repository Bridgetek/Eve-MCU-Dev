cmake_minimum_required(VERSION 3.22)

# Command line in STM32 Visual Studio Code extension:
#  cube-cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=C:/Users/gordon.mcnab/Documents/GitHub/EVE-MCU-Dev/examples/simple/STMCUBE_CMake_MiniSTM32H7xx/cmake/gcc-arm-none-eabi.cmake -DCMAKE_COMMAND=cube-cmake -S C:/Users/gordon.mcnab/Documents/GitHub/EVE-MCU-Dev/examples/simple/STMCUBE_CMake_MiniSTM32H7xx -B C:/Users/gordon.mcnab/Documents/GitHub/EVE-MCU-Dev/examples/simple/STMCUBE_CMake_MiniSTM32H7xx/build/Debug -G Ninja
#  cube-cmake --build build

# Command line in Ubuntu 24.04:
#  cmake -DCMAKE_BUILD_TYPE=Debug -B build -S . -G Ninja
#  cmake --build build

# Set the project name (may be overridden)
set(CMAKE_PROJECT_NAME STM32CUBE_CMake_MiniSTM32H7xx)

# Set the project configuration file (may be overridden)
if(NOT CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/gcc-arm-none-eabi.cmake")
endif()

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
message(CMAKE_PROJECT_NAME="${CMAKE_PROJECT_NAME}")
message(CMAKE_C_COMPILER_ID="${CMAKE_C_COMPILER_ID}")
message(CMAKE_SYSTEM_NAME="${CMAKE_SYSTEM_NAME}")

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

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
ELSE(MODULE_TYPE)
# Set the eve device type
IF(FT8XX_TYPE)
    ADD_DEFINITIONS(-DFT8XX_TYPE=${FT8XX_TYPE})
ENDIF(FT8XX_TYPE)
# Set the panel type
IF(PANEL_TYPE)
    ADD_DEFINITIONS(-DPANEL_TYPE=${PANEL_TYPE})
ELSE(PANEL_TYPE)
# Set the display resolution
IF(DISPLAY_RES)
    ADD_DEFINITIONS(-DDISPLAY_RES=${DISPLAY_RES})
ENDIF(DISPLAY_RES)
ENDIF(PANEL_TYPE)
ENDIF(MODULE_TYPE)

# Add environment for example code
set(EXAMPLE_COMMON "${CMAKE_CURRENT_SOURCE_DIR}/../common")
set(EXAMPLE_SNIPPETS "${CMAKE_CURRENT_SOURCE_DIR}/../../snippets")

# Tell EVE library to use STM32 CUBE Platform
# Setting this macro chooses the STM32 peripheral:
# - "1" uses the SPI peripheral (default if set to zero)
# - "4" uses the QUADSPI peripheral
add_compile_definitions(PLATFORM_STM32_CUBE=4)
# Setting this macro selects the channel in the STM32 SPI peripheral:
# - "1" will select SPI1 (default if not set or set to zero)
# - "2" will select SPI2
# - Unused for QUADSPI peripheral
add_compile_definitions(PLATFORM_STM32_CUBE_CHANNEL=1)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
    # Source code for EVE library
    ${API_DIRECTORY}/source/EVE_API.c
    ${API_DIRECTORY}/source/EVE_HAL.c
    ${API_DIRECTORY}/ports/eve_arch_stm32/EVE_MCU_STM32CUBE.c
    ${API_DIRECTORY}/ports/eve_arch_stm32/EVE_MCU_STM32CUBE_SPI.c
    ${API_DIRECTORY}/ports/eve_arch_stm32/EVE_MCU_STM32CUBE_QUADSPI.c
    ${API_DIRECTORY}/ports/eve_bt82x/patch_base.c # Only applicable to BT82x
    # Source files for example code library
    ${EXAMPLE_COMMON}/eve_example.c
    ${EXAMPLE_COMMON}/eve_fonts.c
    ${EXAMPLE_COMMON}/eve_images.c
    ${EXAMPLE_SNIPPETS}/touch.c
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${API_DIRECTORY}/include
    ${API_DIRECTORY}/ports/eve_bt82x # BT82x base patch
    ${API_DIRECTORY}/ports/eve_arch_stm32 # FT4222 Library headers
    ${EXAMPLE_COMMON}/ # Headers for example code
    ${EXAMPLE_SNIPPETS}/ # Headers for snippets 
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx
    # Add user defined libraries
)

# Convert output to hex and binary
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${CMAKE_PROJECT_NAME}.hex
)
