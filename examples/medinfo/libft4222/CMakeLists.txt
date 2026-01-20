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

# Add environment for example code
set(EXAMPLE_COMMON "${CMAKE_CURRENT_SOURCE_DIR}/../common")
set(EXAMPLE_SNIPPETS "${CMAKE_CURRENT_SOURCE_DIR}/../../snippets")
set(EXAMPLE_ASSETS "${CMAKE_CURRENT_SOURCE_DIR}/../assets")

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

# Enable debug logging output to stdio
add_compile_definitions(DEBUG_LEVEL=1)

# Source code for EVE library (targetted at FT4222)
add_library(eve_library # EVE library name
        # Source code for EVE library
        ${API_DIRECTORY}/source/EVE_API.c
        ${API_DIRECTORY}/source/EVE_HAL.c
        ${API_DIRECTORY}/ports/eve_libft4222/EVE_libft4222.c
        #${API_DIRECTORY}/ports/eve_bt82x/patch_base.c # Only applicable to BT82x
)
# Header files used for EVE library
target_include_directories(eve_library PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${API_DIRECTORY}/include
        ${API_DIRECTORY}/ports/eve_bt82x # BT82x base patch
        ${API_DIRECTORY}/ports/eve_libft4222 # FT4222 Library headers
)

# EVE library dependencies
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    find_library(FT4222_LIBRARY
        NAMES ft4222
        )

    find_library(FTD2XX_LIBRARY
        NAMES ftd2xx
        )

    if (FT4222_LIBRARY AND FTD2XX_LIBRARY)
        message(STATUS "libft4222 found")
    else()
        message(FATAL_ERROR "libft4222 not found")
    endif()
    
    if (FTD2XX_LIBRARY)
        message(STATUS "libftd2xx found")
    else()
        message(FATAL_ERROR "libftd2xx not found")
    endif()

    target_link_libraries(eve_library
	ft4222
	ftd2xx        
    )
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    if(NOT EXISTS "${API_DIRECTORY}/ports/eve_libft4222/libft4222.h")
        message(FATAL_ERROR "The Windows LibFT4222 installation has not been completed. Follow the instructions in \"\\ports\\eve_libft4222\\README.md\"")
    endif()

    target_link_libraries(eve_library
            ${API_DIRECTORY}/ports/eve_libft4222/libft4222.lib
            ${API_DIRECTORY}/ports/eve_libft4222/libftd2xx.lib
    )

    # Copy the DLL to beside the executable for Windows
    if(EXISTS "${API_DIRECTORY}/ports/eve_libft4222/LibFT4222-64.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different "${API_DIRECTORY}/ports/eve_libft4222/LibFT4222-64.dll" $<TARGET_FILE_DIR:${PROJECT_NAME}>
                COMMAND_EXPAND_LISTS
    )
    elseif(EXISTS "${API_DIRECTORY}/ports/eve_libft4222/LibFT4222.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different "${API_DIRECTORY}/ports/eve_libft4222/LibFT4222.dll" $<TARGET_FILE_DIR:${PROJECT_NAME}>
                COMMAND_EXPAND_LISTS
    )
    endif()
endif()


# Source for example code
add_library(eve_example # Example code library name
        # Source files for example code library
        ${EXAMPLE_COMMON}/eve_example.c
        ${EXAMPLE_COMMON}/eve_assetload_array.c
        ${EXAMPLE_COMMON}/eve_assetload_file.c
        ${EXAMPLE_COMMON}/eve_assetload_flash.c
        ${EXAMPLE_COMMON}/eve_tracedata.c
        ${EXAMPLE_COMMON}/eve_graph_extension.c
        ${EXAMPLE_COMMON}/eve_graph_vertexes.c
        ${EXAMPLE_SNIPPETS}/touch.c
        ${EXAMPLE_SNIPPETS}/widgets/sevenseg.c
        ${EXAMPLE_COMMON}/patch_medinfo.c
)

# Example code assets
# Add all C files, the EVE generation macro selects the correct data
file(GLOB EVE_ASSET_LIST CONFIGURE_DEPENDS 
        # Add any assets that are compiled in
        "${EXAMPLE_ASSETS}/c_array_assets/*.c")
target_sources(eve_example PRIVATE
        # Add specific asset files that are compiled in
        ${EVE_ASSET_LIST})

# Header files used for example code library
target_include_directories(eve_example PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${API_DIRECTORY}/include # Headers for EVE library
        ${API_DIRECTORY}/ports/eve_bt82x # BT82x base patch
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
