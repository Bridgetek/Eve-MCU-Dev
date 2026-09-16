# Common CMake include file common to all builds of the example

# Add environment for example code
set(EXAMPLE_COMMON "${CMAKE_CURRENT_SOURCE_DIR}/../common")
set(EXAMPLE_SNIPPETS "${CMAKE_CURRENT_SOURCE_DIR}/../../snippets")
set(EXAMPLE_ASSETS "${CMAKE_CURRENT_SOURCE_DIR}/../assets")

# Point to where to find the platform source file
add_executable(${EXAMPLE_PROJECT_NAME}
)

# Source for example code
add_library(eve_example # Example code library name
        # Source files for example code library
        ${EXAMPLE_COMMON}/eve_example.c
        ${EXAMPLE_SNIPPETS}/touch.c
        ${EXAMPLE_SNIPPETS}/maths/trig_furman.c
        ${EXAMPLE_SNIPPETS}/dials/compass_binnacle.c
        ${EXAMPLE_SNIPPETS}/dials/compass_bulkhead.c
        ${EXAMPLE_SNIPPETS}/dials/sub_depth.c
        ${EXAMPLE_ASSETS}/clownfish_102x54_ARGB1555.c
        ${EXAMPLE_ASSETS}/parrotfish_116x43_ARGB1555.c
        ${EXAMPLE_ASSETS}/surgeonfish_133x56_ARGB1555.c
        ${EXAMPLE_ASSETS}/yellowtang_128x81_ARGB1555.c
        ${EXAMPLE_ASSETS}/prop1_128x128_ARGB2.c
)
# Header files used for example code library
target_include_directories(eve_example PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${API_DIRECTORY}/include # Headers for EVE library
        ${EXAMPLE_COMMON}/ # Headers for example code
        ${EXAMPLE_SNIPPETS}/ # Headers for snippets 
)

# Example code library
target_link_libraries(eve_example)
