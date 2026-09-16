# Common CMake include file common to all examples

# Set the location of the EVE-MCU-Dev tree
set(API_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../../../")

# Set name of project (as portmanteau of example name and platform name)
get_filename_component(PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
get_filename_component(ProjectName ${PARENT_DIR} NAME)
get_filename_component(PROJECT_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" PROJECT_SRC_DIR ${PROJECT_SRC_DIR})
set(EXAMPLE_PROJECT_NAME "${ProjectName}_${PROJECT_SRC_DIR}")

message(API_DIRECTORY="${API_DIRECTORY}")
message(EXAMPLE_PROJECT_NAME="${EXAMPLE_PROJECT_NAME}")

# Set C/C++ Standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Setup command line options for build - feeds into EVE_config.h
# A module type implies both a panel and eve type
# - panel type, display resolution and eve device type are ignored
# A panel configures a display resolution
# - display resolution is ignored
option(EVE_MODULE "EVE Module Name" )
option(EVE_DEVICE "EVE Device Type Name" )
option(EVE_PANEL "EVE Panel Type" )
option(EVE_DISPLAY_RES "Panel Display Resolution" )

# Set a module type
IF(EVE_MODULE)
    ADD_DEFINITIONS(-DEVE_MODULE=${EVE_MODULE})
    message(EVE_MODULE="${EVE_MODULE}")
ELSE(EVE_MODULE)
# Set the eve device type
IF(EVE_DEVICE)
    ADD_DEFINITIONS(-DEVE_DEVICE=${EVE_DEVICE})
    message(EVE_DEVICE="${EVE_DEVICE}")
ENDIF(EVE_DEVICE)
# Set the panel type
IF(EVE_PANEL)
    ADD_DEFINITIONS(-DEVE_PANEL=${EVE_PANEL})
    message(EVE_PANEL="${EVE_PANEL}")
ELSE(EVE_PANEL)
# Set the display resolution
IF(EVE_DISPLAY_RES)
    ADD_DEFINITIONS(-DEVE_DISPLAY_RES=${EVE_DISPLAY_RES})
    message(EVE_DISPLAY_RES="${EVE_DISPLAY_RES}")
ENDIF(EVE_DISPLAY_RES)
ENDIF(EVE_PANEL)
ENDIF(EVE_MODULE)

# Setup Quad-SPI from command line
option(EVE_QSPI_ENABLE "Enable Quad-SPI")
IF(EVE_QSPI_ENABLE)
    ADD_DEFINITIONS(-DEVE_QSPI_ENABLE)
    message(EVE_QSPI_ENABLE=TRUE)
ENDIF(EVE_QSPI_ENABLE)

# Setup Co-processor transfer method from command line
option(EVE_COPRO_METHOD "Set co-processor transfer method")
IF(EVE_COPRO_METHOD)
    ADD_DEFINITIONS(-DEVE_COPRO_METHOD=${EVE_COPRO_METHOD})
    message(EVE_COPRO_METHOD="${EVE_COPRO_METHOD}")
ENDIF(EVE_COPRO_METHOD)

# Source code for EVE library
add_library(eve_library # EVE library name
        # Source code for EVE library
        ${API_DIRECTORY}/source/EVE_API.c
        ${API_DIRECTORY}/source/EVE_HAL.c
        ${API_DIRECTORY}/source/EVE_HAL_Linux.c
        ${API_DIRECTORY}/source/extensions/bt82x_patch.c # Only applicable to BT82x
        ${API_DIRECTORY}/source/extensions/custom_touch_fw.c # Only applicable to FT81x/BT88x/BT81x
        ${API_DIRECTORY}/source/extensions/lcd_panel_init.c # Only applicable LCD panels that require driver initialisation
)

# Header files used for EVE library
target_include_directories(eve_library PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${API_DIRECTORY}/include
)

# EVE library
target_link_libraries(eve_library)
