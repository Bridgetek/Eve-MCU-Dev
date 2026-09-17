# Common CMake include file for all examples using libmpsse

# Tell EVE library to use libMPSSE as base class:
# Choose the MPSSE device (0 for first, 1 for second etc)
add_compile_definitions(USE_MPSSE=0)

# Add in flags for MPSSE header file.
add_compile_definitions(FTDIMPSSE_STATIC)
add_compile_definitions(FT_VER_MAJOR=1)
add_compile_definitions(FT_VER_MINOR=0)
add_compile_definitions(FT_VER_BUILD=9)

# Add environment for example code
set(FTDI_SUBMODULE "${API_DIRECTORY}/ports/external/ftdi_libraries/")

# EVE library dependencies
if(NOT EXISTS "${FTDI_SUBMODULE}/libMPSSE")
    message(FATAL_ERROR "The ftdi_libraries submodule needs to be loaded. Follow the instructions in \"\\ports\\eve_libmpsse\\README.md\"")
endif()

target_include_directories(eve_library PUBLIC
        ${FTDI_SUBMODULE}/libMPSSE/include
        ${FTDI_SUBMODULE}/libFTD2XX/include
)

if(CMAKE_SYSTEM_NAME MATCHES "Windows")

    # Build libMPSSE directly from source.
    target_sources(eve_library PRIVATE
        ${FTDI_SUBMODULE}/libMPSSE/source/ftdi_infra.c
        ${FTDI_SUBMODULE}/libMPSSE/source/ftdi_spi.c
        ${FTDI_SUBMODULE}/libMPSSE/source/ftdi_mid.c
    )

elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")

    target_link_libraries(eve_library PRIVATE
        ${FTDI_SUBMODULE}/libMPSSE/lib-linux-x86_64/libmpsse.a
        # This is included in the MPSSE library: ${FTDI_SUBMODULE}/libFTD2XX/lib-linux-x86_64/libftd2xx.a
    )

endif()

# Source code for EVE library (targetted at FT4222):
target_sources(eve_library PUBLIC
        # Source code for EVE library
        ${API_DIRECTORY}/ports/eve_libmpsse/EVE_libmpsse.c
)
