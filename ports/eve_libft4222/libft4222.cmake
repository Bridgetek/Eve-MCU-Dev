# Common CMake include file for all examples using libft4222

# Tell EVE library to use libFT4222 as base class:
# Choose the FT4222 device (0 for first, 1 for second etc)
add_compile_definitions(USE_FT4222=0)

# Add environment for example code
set(FTDI_SUBMODULE "${API_DIRECTORY}/ports/external/ftdi_libraries/")

# EVE library dependencies
if(NOT EXISTS "${FTDI_SUBMODULE}/libFT4222")
    message(FATAL_ERROR "The ftdi_libraries submodule needs to be loaded. Follow the instructions in \"\\ports\\eve_libft4222\\README.md\"")
endif()

# Source code for EVE library (targetted at FT4222):
target_sources(eve_library PUBLIC
        # Source code for EVE library
        ${API_DIRECTORY}/ports/eve_libft4222/EVE_libft4222.c
)

# Header files used for EVE library:
target_include_directories(eve_library PUBLIC
        ${API_DIRECTORY}/ports/eve_libft4222 # FT4222 Library headers
)

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

