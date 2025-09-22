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

set(EXAMPLE_COMMON "${CMAKE_CURRENT_SOURCE_DIR}/../common")
set(EXAMPLE_SNIPPETS "${CMAKE_CURRENT_SOURCE_DIR}/../../snippets")

if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/release)
        message(FATAL_ERROR "libMPSSE installation cannot be found.\nThe libMPSSE library \"release\" directory must be copied to the current directory.")
endif()

# point out the CMake, where to find the executable source file
add_executable(${PROJECT_NAME}
        release/source/ftdi_infra.c
        release/source/ftdi_spi.c
        release/source/ftdi_i2c.c
        release/source/ftdi_mid.c
        main.c
)
include_directories(
        ${API_DIRECTORY}/include
        ${EXAMPLE_COMMON}
)
# Tell EVE library to use libFT4222 as base class
add_compile_definitions(USE_MPSSE=0)
# Add in flags for MPSSE header file.
add_compile_definitions(FTDIMPSSE_STATIC)
add_compile_definitions(FT_VER_MAJOR=1)
add_compile_definitions(FT_VER_MINOR=0)
add_compile_definitions(FT_VER_BUILD=5)

# Source code for EVE library (targetted at MPSSE)
add_library(eve_library # EVE library name
        # Source code for EVE library
        ${API_DIRECTORY}/source/EVE_API.c
        ${API_DIRECTORY}/source/EVE_HAL.c
        ${API_DIRECTORY}/ports/eve_libmpsse/EVE_libmpsse.c
)
# Header files used for EVE library
target_include_directories(eve_library PUBLIC
        ${API_DIRECTORY}/include
        ${API_DIRECTORY}/ports/eve_bt82x # BT82x base patch
        release/include
        release/libftd2xx
)

# EVE library dependencies
if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        target_link_libraries(eve_library ${CMAKE_DL_LIBS})
        add_link_options(-lmpsse)
else()
        target_link_libraries(eve_library
                ${CMAKE_CURRENT_LIST_DIR}/release/build/x64/LIB/libmpsse.lib
)
endif()

# Source for example code
add_library(eve_example # Example code library name
        # Patch file to override patch_base.c
        ${EXAMPLE_COMMON}/patch_dronefpv.c
        # Source files for example code library
        ${EXAMPLE_COMMON}/eve_calibrate.c
        ${EXAMPLE_COMMON}/eve_example.c
        ${EXAMPLE_COMMON}/eve_helper.c
        ${EXAMPLE_SNIPPETS}/flightcontrols/flightalt.c
        ${EXAMPLE_SNIPPETS}/flightcontrols/flightatt.c
        ${EXAMPLE_SNIPPETS}/trig_furman/trig_furman.c
)
# Header files used for example code library
target_include_directories(eve_example PUBLIC
        ${EXAMPLE_COMMON}/ # Headers for example code
        ${API_DIRECTORY}/include # Headers for EVE library
        ${EXAMPLE_SNIPPETS}/flightcontrols # Headers for flight controls
        ${EXAMPLE_SNIPPETS}/trig_furman # Headers for trigonometry in furmans
)
# Example code dependencies
target_link_libraries(eve_example
        eve_library # Example code depends on EVE library
)

# Project dependencies
target_link_libraries(${PROJECT_NAME} 
        eve_example # Project depends on example code library
)
