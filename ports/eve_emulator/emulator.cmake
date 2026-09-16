# Common CMake include file for all examples using the emulator

# Tell EVE library to use the EVE Emulator as base class:
add_compile_definitions(PLATFORM_EMULATOR)

# Source code for EVE library (targetted at the EVE Emulator):
target_sources(eve_library PUBLIC
        ${API_DIRECTORY}/ports/eve_emulator/EVE_emulator.c
)

# Header files used for EVE library:
target_include_directories(eve_library PUBLIC
        ${API_DIRECTORY}/ports/eve_emulator # EVE Emulator Library headers
)

# EVE library dependencies for emulator:
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    message(FATAL_ERROR "emulator not supported on Linux")
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    if(NOT EXISTS "${API_DIRECTORY}/ports/eve_emulator/EVE_Emulator/README.md")
        message(FATAL_ERROR "The Windows emulator installation has not been completed. Follow the instructions in \"\\ports\\eve_emulator\\README.md\"")
    endif()

    target_link_libraries(eve_library
            # Main emulator library to load the DLL at runtime
            ${API_DIRECTORY}/ports/eve_emulator/EVE_Emulator/lib/bt8xxemu.lib
    )

    # Copy the DLL to beside the executable for Windows 
    if(EXISTS "${API_DIRECTORY}/ports/eve_emulator/EVE_Emulator/bin/bt8xxemu.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different "${API_DIRECTORY}/ports/eve_emulator/EVE_Emulator/bin/bt8xxemu.dll" $<TARGET_FILE_DIR:${PROJECT_NAME}>
                COMMAND_EXPAND_LISTS
    )
    endif()
    if(EXISTS "${API_DIRECTORY}/ports/eve_emulator/EVE_Emulator/bin/mx25lemu.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different "${API_DIRECTORY}/ports/eve_emulator/EVE_Emulator/bin/mx25lemu.dll" $<TARGET_FILE_DIR:${PROJECT_NAME}>
                COMMAND_EXPAND_LISTS
    )
    endif()
    if(EXISTS "${API_DIRECTORY}/ports/eve_emulator/EVE_Emulator/bin/zlib.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different "${API_DIRECTORY}/ports/eve_emulator/EVE_Emulator/bin/zlib.dll" $<TARGET_FILE_DIR:${PROJECT_NAME}>
                COMMAND_EXPAND_LISTS
    )
    endif()
endif()
