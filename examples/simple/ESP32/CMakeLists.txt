# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.10)

# Tell EVE library to use ESP32 as base class
add_compile_definitions(PLATFORM_ESP32)

set(INCLUDE_DIRECTORIES "./main" "../../../../include" "../../common" "../../../snippets/")
set(EXTRA_COMPONENT_DIRS "./main" "../../../ports/eve_arch_esp32" "../../snippets")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(simple_esp32)
