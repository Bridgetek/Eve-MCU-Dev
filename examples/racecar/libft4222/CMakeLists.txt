# Set minimum required version of CMake:
cmake_minimum_required(VERSION 3.12)

# Include the global CMake settings common to all examples:
include(../../examples.cmake)

project(${EXAMPLE_PROJECT_NAME} C CXX)

# Include the common CMake recipie for this example:
include(../common.cmake)

# Include any port CMake additions for this example:
include(../../../ports/eve_libft4222/libft4222.cmake)

# Add platform specific build files:
target_sources(${EXAMPLE_PROJECT_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/main.c)

# Trig tables are used instead of maths functions
add_compile_definitions(USE_TRIG_TABLE)

# Turn on profiling of co-processor lists
add_compile_definitions(EVE_COPROC_PROFILE)

# BY DEFAULT: On libft4222 use the files in the "assets" directory
# Comment out this line to set the method in common/eve_example.h
add_compile_definitions(ASSETS=USE_FILES)

# Build Final Executable:
target_link_libraries(${PROJECT_NAME} 
        eve_example # Depends on example code library
        eve_library # Depends on EVE library
)
