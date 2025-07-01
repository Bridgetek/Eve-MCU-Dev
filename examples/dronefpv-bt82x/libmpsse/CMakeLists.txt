# Set minimum required version of CMake
cmake_minimum_required(VERSION 3.12)

# Set name of project (as current working directory)
get_filename_component(PARENT_DIR ${CMAKE_CURRENT_LIST_DIR} DIRECTORY)
get_filename_component(ProjectName ${PARENT_DIR} NAME)
get_filename_component(ProjectId ${CMAKE_CURRENT_LIST_DIR} NAME)
string(REPLACE " " "_" ProjectId ${ProjectId})
project(${ProjectName}_${ProjectId} C CXX)

message(ProjectId="${PROJECT_SOURCE_DIR}")
message(PROJECT_NAME="${PROJECT_NAME}")
message(CMAKE_C_COMPILER_ID="${CMAKE_C_COMPILER_ID}")
message(CMAKE_SYSTEM_NAME="${CMAKE_SYSTEM_NAME}")

# Set C/C++ Standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

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
        ../../../include
        ../common/
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
        ../../../source/EVE_API.c
        ../../../source/EVE_HAL.c
        ../../../ports/eve_libmpsse/EVE_libmpsse.c
)
# Header files used for EVE library
target_include_directories(eve_library PUBLIC
        ../../../include
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
        # Source files for example code library
        ../common/eve_calibrate.c
        ../common/eve_example.c
        ../common/eve_helper.c
        ../../snippets/flightcontrols/flightalt.c
        ../../snippets/flightcontrols/flightatt.c
        ../../snippets/trig_furman/trig_furman.c
)
# Header files used for example code library
target_include_directories(eve_example PUBLIC
        ../common/ # Headers for example code
        ../../../include # Headers for EVE library
        ../../snippets/flightcontrols # Headers for flight controls
        ../../snippets/trig_furman # Headers for trigonometry in furmans
)
# Example code dependencies
target_link_libraries(eve_example
        eve_library # Example code depends on EVE library
)

# Project dependencies
target_link_libraries(${PROJECT_NAME} 
        eve_example # Project depends on example code library
)
