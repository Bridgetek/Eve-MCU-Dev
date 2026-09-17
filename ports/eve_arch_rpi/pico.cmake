# Common CMake include file for all examples using pico/RP2040

# Tell EVE library to use RP2040 as base class:
add_compile_definitions(PLATFORM_RP2040)

# Milliseconds to wait for stdio connection:
# (will also have stdio_usb_connected() call in main to wait for connection)
add_compile_definitions(PICO_STDIO_USB_CONNECT_WAIT_TIMEOUT_MS=0)

# Modify the below lines to enable/disable output over UART/USB:
pico_enable_stdio_uart(${PROJECT_NAME} 0)
pico_enable_stdio_usb(${PROJECT_NAME} 1)

# Source code for EVE library (targetted at pico/RP2040):
target_sources(eve_library PUBLIC
        # Source code for EVE library
        ${API_DIRECTORY}/ports/eve_arch_rpi/EVE_MCU_RP2040.c
)

# Build Final Executable:
target_link_libraries(eve_library 
        pico_stdlib
        hardware_spi
        pico_time
        )
