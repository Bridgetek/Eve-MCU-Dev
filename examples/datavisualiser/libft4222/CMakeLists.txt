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
    message(EVE_DISPLAY_RES="${EVE_DISPLAY_RES}")
ENDIF(EVE_DISPLAY_RES)
ENDIF(EVE_PANEL)
ENDIF(EVE_MODULE)

# Setup Quad-SPI from command line
option(EVE_QSPI_ENABLE "Enable Quad-SPI")
IF(EVE_QSPI_ENABLE)
    ADD_DEFINITIONS(-DEVE_QSPI_ENABLE)
    message(EVE_QSPI_ENABLE=TRUE)
ENDIF(EVE_QSPI_ENABLE)

# Setup Co-processor transfer method from command line
option(EVE_COPRO_METHOD "Set co-processor transfer method")
IF(EVE_COPRO_METHOD)
    ADD_DEFINITIONS(-DEVE_COPRO_METHOD=${EVE_COPRO_METHOD})
    message(EVE_COPRO_METHOD="${EVE_COPRO_METHOD}")
ENDIF(EVE_COPRO_METHOD)

# Add environment for example code
set(EXAMPLE_COMMON "${CMAKE_CURRENT_SOURCE_DIR}/../common")
set(EXAMPLE_SNIPPETS "${CMAKE_CURRENT_SOURCE_DIR}/../../snippets")
set(FTDI_SUBMODULE "${API_DIRECTORY}/ports/external/ftdi_libraries/")

# Point to where to find the platform source file
add_executable(${PROJECT_NAME}
        main.c
)
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${API_DIRECTORY}/include
        ${EXAMPLE_COMMON}
)
# Tell EVE library to use libFT4222 as base class
# Choose the FT4222 device (0 for first, 1 for second etc)
add_compile_definitions(USE_FT4222=0)

# Source code for EVE library (targetted at FT4222)
add_library(eve_library # EVE library name
        # Source code for EVE library
        ${API_DIRECTORY}/source/EVE_API.c
        ${API_DIRECTORY}/source/EVE_HAL.c
        ${API_DIRECTORY}/ports/eve_libft4222/EVE_libft4222.c
        ${API_DIRECTORY}/source/extensions/bt82x_patch.c # Only applicable to BT82x
        ${API_DIRECTORY}/source/extensions/custom_touch_fw.c # Only applicable to FT81x/BT88x/BT81x
        ${API_DIRECTORY}/source/extensions/lcd_panel_init.c # Only applicable LCD panels that require driver initialisation
)
# Header files used for EVE library
target_include_directories(eve_library PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${API_DIRECTORY}/include
        ${API_DIRECTORY}/ports/eve_libft4222 # FT4222 Library headers
)

# EVE library dependencies
if(NOT EXISTS "${FTDI_SUBMODULE}/libFT4222")
    message(FATAL_ERROR "The ftdi_libraries submodule needs to be loaded. Follow the instructions in \"\\ports\\eve_libft4222\\README.md\"")
endif()

target_include_directories(eve_library PUBLIC
        ${FTDI_SUBMODULE}/libFT4222/include
        ${FTDI_SUBMODULE}/libFTD2XX/include
)

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    ADD_DEFINITIONS(-DWINAPI=)
    target_link_libraries(eve_library
            ${FTDI_SUBMODULE}/libFT4222/lib-linux-x86_64/libft4222.a
            # This is included in the FT4222 library: ${FTDI_SUBMODULE}/libFTD2XX/lib-linux-x86_64/libftd2xx.a
            dl pthread rt stdc++
    )
endif()
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    target_link_libraries(eve_library
            ${FTDI_SUBMODULE}/libFT4222/lib-windows/LibFT4222-64.lib
            ${FTDI_SUBMODULE}/libFTD2XX/lib-windows/libftd2xx.lib
    )

    # Copy the DLL to beside the executable for Windows
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${FTDI_SUBMODULE}/libFT4222/lib-windows/LibFT4222-64.dll" $<TARGET_FILE_DIR:${PROJECT_NAME}>
            COMMAND_EXPAND_LISTS
    )
endif()

# Source for example code
add_library(eve_example # Example code library name
        # Source files for example code library
        ${EXAMPLE_COMMON}/eve_example.c
        ${EXAMPLE_SNIPPETS}/touch.c
        ${EXAMPLE_SNIPPETS}/controls/sound.c
        ${EXAMPLE_SNIPPETS}/maths/trig_furman.c
        ${EXAMPLE_SNIPPETS}/controls/arcs.c
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
)

# Project dependencies
target_link_libraries(${PROJECT_NAME} 
        eve_example # Project depends on example code library
)
