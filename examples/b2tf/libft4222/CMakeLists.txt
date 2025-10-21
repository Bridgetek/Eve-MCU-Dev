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

if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/imports)
        message(FATAL_ERROR "libFT4222 installation cannot be found.\nThe libFT4222 library \"imports\" directory must be copied to the current directory.")
endif()

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
add_compile_definitions(USE_FT4222=0)

# Source code for EVE library (targetted at FT4222)
add_library(eve_library # EVE library name
        # Source code for EVE library
        ${API_DIRECTORY}/source/EVE_API.c
        ${API_DIRECTORY}/source/EVE_HAL.c
        ${API_DIRECTORY}/ports/eve_libft4222/EVE_libft4222.c
        ${API_DIRECTORY}/ports/eve_bt82x/patch_base.c # Only applicable to BT82x
)
# Header files used for EVE library
target_include_directories(eve_library PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${API_DIRECTORY}/include
        ${API_DIRECTORY}/ports/eve_bt82x # BT82x base patch
        imports/ftd2xx
        imports/LibFT4222/inc
)

# EVE library dependencies
target_link_libraries(eve_library
        ${CMAKE_CURRENT_LIST_DIR}/imports/LibFT4222/dll/amd64/LibFT4222-64.lib
        ${CMAKE_CURRENT_LIST_DIR}/imports/ftd2xx/dll/amd64/ftd2xx.lib
)

# Copy the DLL to beside the executable for Windows
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different "../imports/LibFT4222/dll/amd64/LibFT4222-64.dll" $<TARGET_FILE_DIR:${PROJECT_NAME}>
                COMMAND_EXPAND_LISTS
        )
endif()

# Source for example code
add_library(eve_example # Example code library name
        # Source files for example code library
        ${EXAMPLE_COMMON}/eve_calibrate.c
        ${EXAMPLE_COMMON}/eve_example.c
        ${EXAMPLE_COMMON}/eve_helper.c
        ${EXAMPLE_SNIPPETS}/sevenseg/sevenseg.c
)
# Header files used for example code library
target_include_directories(eve_example PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${API_DIRECTORY}/include # Headers for EVE library
        ${API_DIRECTORY}/ports/eve_bt82x # BT82x base patch
        ${EXAMPLE_COMMON}/ # Headers for example code
        ${EXAMPLE_SNIPPETS}/sevenseg # Headers for snippets
)
# Example code dependencies
target_link_libraries(eve_example
        eve_library # Example code depends on EVE library
)

# Project dependencies
target_link_libraries(${PROJECT_NAME} 
        eve_example # Project depends on example code library
)
