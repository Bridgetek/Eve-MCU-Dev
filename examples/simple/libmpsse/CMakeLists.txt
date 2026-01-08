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

# point out the CMake, where to find the executable source file
add_executable(${PROJECT_NAME}
        main.c
)
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
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
        ${API_DIRECTORY}/ports/eve_bt82x/patch_base.c # Only applicable to BT82x
)
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
target_sources(eve_library PRIVATE # EVE library name
        # Source code for MPSSE library
        ${API_DIRECTORY}/ports/eve_libmpsse/ftdi_infra.c
        ${API_DIRECTORY}/ports/eve_libmpsse/ftdi_spi.c
        ${API_DIRECTORY}/ports/eve_libmpsse/ftdi_i2c.c
        ${API_DIRECTORY}/ports/eve_libmpsse/ftdi_mid.c
)
endif()
# Header files used for EVE library
target_include_directories(eve_library PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${API_DIRECTORY}/include
        ${API_DIRECTORY}/ports/eve_bt82x # BT82x base patch
        ${API_DIRECTORY}/ports/eve_libmpsse # MPSSE Library headers
)

# EVE library dependencies
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    find_library(MPSSE_LIBRARY
        NAMES mpsse
        )

    find_library(FTD2XX_LIBRARY
        NAMES ftd2xx
        )

    if (MPSSE_LIBRARY AND FTD2XX_LIBRARY)
        message(STATUS "libmpsse found")
    else()
        message(FATAL_ERROR "libmpsse not found")
    endif()
    
    if (FTD2XX_LIBRARY)
        message(STATUS "libftd2xx found")
    else()
        message(FATAL_ERROR "libftd2xx not found")
    endif()

    #target_link_libraries(eve_library ${CMAKE_DL_LIBS})
    target_link_libraries(eve_library
	mpsse
    )
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    if(NOT EXISTS "${API_DIRECTORY}/ports/eve_libmpsse/libmpsse_spi.h")
        message(FATAL_ERROR "The Windows libMPSSE installation has not been completed. Follow the instructions in \"\\ports\\eve_libmpsse\\README.md\"")
    endif()

    target_link_libraries(eve_library
            ${API_DIRECTORY}/ports/eve_libmpsse/libmpsse.lib
    )
endif()

# Source for example code
add_library(eve_example # Example code library name
        # Source files for example code library
        ${EXAMPLE_COMMON}/eve_calibrate.c
        ${EXAMPLE_COMMON}/eve_example.c
        ${EXAMPLE_COMMON}/eve_helper.c
        ${EXAMPLE_COMMON}/eve_images.c
        ${EXAMPLE_COMMON}/eve_fonts.c
)
# Header files used for example code library
target_include_directories(eve_example PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${API_DIRECTORY}/include # Headers for EVE library
        ${API_DIRECTORY}/ports/eve_bt82x # BT82x base patch
        ${EXAMPLE_COMMON}/ # Headers for example code

)
# Example code dependencies
target_link_libraries(eve_example
        eve_library # Example code depends on EVE library
)

# Project dependencies
target_link_libraries(${PROJECT_NAME} 
        eve_example # Project depends on example code library
)
