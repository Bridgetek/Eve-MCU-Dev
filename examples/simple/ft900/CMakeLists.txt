# Standard CMakeLists.txt Template for FT9xx project compilation.

# To build use the command line as follows:
# cmake -G "Ninja" -S . -B build -DBUILD=Debug -DTARGET=ft90x

# Project name. Default name is the name of the current directory.
# On command line call cmake with the parameter "-DPROJECT=firmware" to override.

# Build type (case sensitive), can be Debug (default) or Release.
# On command line call cmake with the parameter "-DBUILD=Release" or "-DBUILD=Debug" to override.

# Target device type (case sensitive), can be ft90x (default) or ft93x.
# On command line call cmake with the parameter "-DTARGET=ft93x" or "-DTARGET=ft90x" to override.

# Output directories (you may override this).
# Default is one of "FT90x_Release", "FT90x_Debug", "FT93x_Release" or
#  "FT93x_Debug" depending on build type and target device.
# On command line call cmake with the parameter "-DOUTDIR=test" to override.

# Load the toolchain cmake file.
# This will look in the local directory, followed by the '..' directory,
# finally it will look in the toolchain installation.
if (EXISTS ${CMAKE_SOURCE_DIR}/toolchain_ft9xx.cmake)
    set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/toolchain_ft9xx.cmake)
elseif (EXISTS ${CMAKE_SOURCE_DIR}/../toolchain_ft9xx.cmake)
    set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/../toolchain_ft9xx.cmake)
elseif (EXISTS $ENV{FT9XX_TOOLCHAIN}/hardware/toolchain_ft9xx.cmake)
    set(CMAKE_TOOLCHAIN_FILE $ENV{FT9XX_TOOLCHAIN}/hardware/toolchain_ft9xx.cmake)
else()
    message(FATAL_ERROR "Make sure that the FT9XX is installed correctly. Can't find the 'toolchain_ft9xx.cmake' file")
endif()

cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

# Set the project name to the current directory if it is not defined in the command line.
if (NOT DEFINED ${PROJECT})
    get_filename_component(PARENT_DIR ${CMAKE_CURRENT_LIST_DIR} DIRECTORY)
    get_filename_component(PARENT_NAME ${PARENT_DIR} NAME)
    get_filename_component(PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR} NAME)
    string(REPLACE " " "_" PROJECT "${PROJECT_DIR}_${PARENT_NAME}")
    message(STATUS "Project name set to ${PROJECT}")
endif()

# Setup the cmake project name.
project(${PROJECT})

# Make a variable for the executable output. This is normally based on the project name.
set(EXECUTABLE ${PROJECT}.elf)

# Tell EVE library to use FT9xx as base class
add_compile_definitions(PLATFORM_FT9XX)

# Set the location of the EVE-MCU-Dev tree
set(API_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../../../")
set(EXAMPLE_COMMON "${CMAKE_CURRENT_SOURCE_DIR}/../common")
set(EXAMPLE_SNIPPETS "${CMAKE_CURRENT_SOURCE_DIR}/../../snippets")

# Compile a list of source files.
file(GLOB SRC_FILES CONFIGURE_DEPENDS
    Sources/main.c

    # Example project sources
    ${EXAMPLE_COMMON}/eve_example.c
    ${EXAMPLE_COMMON}/eve_helper.c
    ${EXAMPLE_COMMON}/eve_calibrate.c
    ${EXAMPLE_COMMON}/eve_images.c
    ${EXAMPLE_COMMON}/eve_fonts.c

    # API Sources
    ${API_DIRECTORY}/source/EVE_API.c
    ${API_DIRECTORY}/source/EVE_HAL.c
    ${API_DIRECTORY}/ports/eve_arch_ft9xx/EVE_MCU_FT9XX.c
    
    # Base patch include
    ${API_DIRECTORY}/ports/eve_bt82x/patch_base.c

    # GENERATED: SOURCES - DO NOT MODIFY - BEGIN
    # GENERATED: SOURCES - DO NOT MODIFY - END
)

# Find any linker scripts. Uses glob to find files.
file(GLOB LDSCRIPT_FILES CONFIGURE_DEPENDS Scripts/*.ld)

# Add any additional linker libraries or options required.
if (${TARGET} MATCHES ft90x)
    set(LIB_FILES
        # GENERATED: PRE-COMPILE LIBS FT90X - DO NOT MODIFY - BEGIN
        # GENERATED: PRE-COMPILE LIBS FT90X - DO NOT MODIFY - END
    )
else (${TARGET} MATCHES ft93x)
    set(LIB_FILES
        # GENERATED: PRE-COMPILE LIBS FT93X - DO NOT MODIFY - BEGIN
        # GENERATED: PRE-COMPILE LIBS FT93X - DO NOT MODIFY - END
    )
endif()

# Macro to perform cmake add_executable and configure the default settings for BUILD and TARGET.
ft9xx_set_executable(${EXECUTABLE} "${SRC_FILES}" "${LIB_FILES}" "${LDSCRIPT_FILES}")

# Compile a list of include directories.
target_include_directories(${EXECUTABLE} PRIVATE
    Includes
    # Example project includes
    ${EXAMPLE_COMMON}
    # API Includes
    ${API_DIRECTORY}/include
    # Base patch include
    ${API_DIRECTORY}/ports/eve_bt82x

    # GENERATED: INCLUDES - DO NOT MODIFY - BEGIN
    # GENERATED: INCLUDES - DO NOT MODIFY - END
)

# Setup any extra compiler options required.
target_compile_options(${EXECUTABLE} PRIVATE
    # GENERATED: DEFINITIONS - DO NOT MODIFY - BEGIN
    # GENERATED: DEFINITIONS - DO NOT MODIFY - END
    # GENERATED: CUSTOM DEFINITIONS - DO NOT MODIFY - BEGIN
    # GENERATED: CUSTOM DEFINITIONS - DO NOT MODIFY - END
)

target_link_libraries(${EXECUTABLE} PRIVATE
    # GENERATED: CUSTOM LINKER FLAGS - DO NOT MODIFY - BEGIN
    # GENERATED: CUSTOM LINKER FLAGS - DO NOT MODIFY - END
)

# Enable generation of a map file for the project if needed.
ft9xx_enable_map_file(${EXECUTABLE})
