# Set minimum required version of CMake:
cmake_minimum_required(VERSION 3.22)

# Command line in STM32 Visual Studio Code extension:
#  cube-cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=cmake/gcc-arm-none-eabi.cmake -DCMAKE_COMMAND=cube-cmake -S . -B build -G Ninja
#  cube-cmake --build build

# Command line in Ubuntu 24.04:
#  cmake -DCMAKE_BUILD_TYPE=Debug -B build -S . -G Ninja
#  cmake --build build

# Set the project configuration file (may be overridden)
if(NOT CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/gcc-arm-none-eabi.cmake")
endif()

# Include the global CMake settings common to all examples:
include(../../examples.cmake)

# Set the project name (may be overridden)
set(CMAKE_PROJECT_NAME ${EXAMPLE_PROJECT_NAME})

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME} C ASM)

# Include the common CMake recipie for this example:
include(../common.cmake)

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Tell EVE library to use STM32 CUBE Platform
# Setting this macro chooses the STM32 peripheral:
# - "1" uses the SPI peripheral (default if set to zero)
# - "4" uses the QUADSPI peripheral
# NOTE: To change this setting the CubeMX must be run to change the "Core"
# source code to SPI or QuadSPI to match.
add_compile_definitions(PLATFORM_STM32_CUBE=4)
# Setting this macro selects the channel in the STM32 SPI peripheral:
# - "1" will select SPI1 (default if not set or set to zero)
# - "2" will select SPI2
# - Unused for QUADSPI peripheral
add_compile_definitions(PLATFORM_STM32_CUBE_CHANNEL=1)

# Get source files from targets and add to the stm32cubemx target
get_target_property(EVE_LIB eve_library SOURCES)
get_target_property(EVE_EXMP eve_example SOURCES)

# Add sources to stm32cubemx target
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    ${EVE_LIB} # Source code for EVE library
    ${EVE_EXMP} # Source code for EVE example
    ${API_DIRECTORY}/ports/eve_arch_stm32/EVE_MCU_STM32CUBE.c
    ${API_DIRECTORY}/ports/eve_arch_stm32/EVE_MCU_STM32CUBE_SPI.c
    ${API_DIRECTORY}/ports/eve_arch_stm32/EVE_MCU_STM32CUBE_QUADSPI.c
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${API_DIRECTORY}/include
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
)

# Convert output to hex and binary
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${CMAKE_PROJECT_NAME}.hex
)
