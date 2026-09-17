# Common CMake include file common to all builds of the example

# Add environment for example code
set(EXAMPLE_COMMON "${CMAKE_CURRENT_SOURCE_DIR}/../common")
set(EXAMPLE_SNIPPETS "${CMAKE_CURRENT_SOURCE_DIR}/../../snippets")
set(EXAMPLE_ASSETS "${CMAKE_CURRENT_SOURCE_DIR}/../assets")

# Point to where to find the platform source file
add_executable(${EXAMPLE_PROJECT_NAME}
)

# Trig tables are used instead of maths functions
add_compile_definitions(USE_TRIG_TABLE)

# Turn on profiling of co-processor lists
add_compile_definitions(EVE_COPROC_PROFILE)

# Source for example code
add_library(eve_example # Example code library name
        # Source files for example code library
        ${EXAMPLE_COMMON}/eve_example.c
        ${EXAMPLE_COMMON}/eve_helper.c
        ${EXAMPLE_COMMON}/eve_assetload_flash.c
        ${EXAMPLE_COMMON}/eve_assetload_flash_eve3.c
        ${EXAMPLE_COMMON}/eve_assetload_flash_eve4.c
        ${EXAMPLE_COMMON}/eve_assetload_flash_eve5.c
        ${EXAMPLE_COMMON}/eve_assetload_array.c
        ${EXAMPLE_COMMON}/eve_assetload_file.c
        ${EXAMPLE_COMMON}/patch_textscale.c
        ${EXAMPLE_SNIPPETS}/maths/trig_furman.c
)

# Example code assets
# Add all C files, the EVE generation macro selects the correct data
file(GLOB EVE_ASSET_LIST CONFIGURE_DEPENDS 
        "${EXAMPLE_ASSETS}/c_array_assets/*.c")
target_sources(eve_example PRIVATE
        ${EVE_ASSET_LIST})

# Header files used for example code library
target_include_directories(eve_example PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${API_DIRECTORY}/include # Headers for EVE library
        ${EXAMPLE_COMMON}/ # Headers for example code
        ${EXAMPLE_SNIPPETS}/ # Headers for snippets 
)

# Example code library
target_link_libraries(eve_example)
