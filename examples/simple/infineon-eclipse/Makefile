################################################################################
# Bridgetek EVE simple example - Infineon PSoC 6 / ModusToolbox 3.x
################################################################################

# ModusToolbox 3.x single-project application build flow (KBA236134).
MTB_TYPE=COMBINED

TARGET=APP_CY8CPROTO-062S2-43439
APPNAME=EVE_simple_infineon_eclipse
TOOLCHAIN = GCC_ARM
CONFIG=Debug
VERBOSE=



#CY_TOOLS_PATHS+=C:/Users/gb_so/ModusToolbox/tools_3.9
CY_TOOLS_PATHS+=C:/Users/gordon.mcnab/ModusToolbox/tools_3.9



# This project sits at examples/simple/infineon-eclipse in the EVE library tree.
EVE_ROOT=../../..

SOURCES+=source/main.c
SOURCES+=$(EVE_ROOT)/examples/simple/common/eve_example.c
SOURCES+=$(EVE_ROOT)/examples/simple/common/eve_images.c
SOURCES+=$(EVE_ROOT)/examples/simple/common/eve_fonts.c
SOURCES+=$(EVE_ROOT)/examples/snippets/touch.c
SOURCES+=$(EVE_ROOT)/examples/snippets/platform/platform_time.c
SOURCES+=$(EVE_ROOT)/examples/snippets/platform/calib_none.c
SOURCES+=$(EVE_ROOT)/source/EVE_API.c
SOURCES+=$(EVE_ROOT)/source/EVE_HAL.c
SOURCES+=$(EVE_ROOT)/ports/eve_arch_infineon/EVE_MCU_PSOC6.c

INCLUDES+=source
INCLUDES+=$(EVE_ROOT)/include
INCLUDES+=$(EVE_ROOT)/examples/simple/common
INCLUDES+=$(EVE_ROOT)/examples/snippets

DEFINES+=PLATFORM_INFINEON_PSOC6
#DEFINES+=MODULE_TYPE=IDM20407A

COMPONENTS=
DISABLE_COMPONENTS+=FREERTOS

CFLAGS=
CXXFLAGS=
ASFLAGS=
LDFLAGS=
LDLIBS=
LINKER_SCRIPT=
PREBUILD=
POSTBUILD=

# Keep downloaded shared middleware outside this application folder.
CY_GETLIBS_SHARED_PATH=../
CY_GETLIBS_SHARED_NAME=mtb_shared

################################################################################
# ModusToolbox tools discovery - Tools Package 3.8
################################################################################

# Pin this application to Tools Package 3.8. CY_TOOLS_PATHS may still be
# overridden by the caller when 3.8 is installed in a non-standard location.
CY_WIN_HOME=$(subst \,/,$(USERPROFILE))
CY_TOOLS_PATHS ?= $(wildcard \
    $(CY_WIN_HOME)/ModusToolbox/tools_3.9 \
    $(HOME)/ModusToolbox/tools_3.9 \
    /Applications/ModusToolbox/tools_3.9)

# Custom installation example (use forward slashes):
# CY_TOOLS_PATHS=C:/Infineon/Tools/ModusToolbox/tools_3.9

CY_TOOLS_DIR=$(lastword $(sort $(wildcard $(CY_TOOLS_PATHS))))

ifeq ($(strip $(CY_TOOLS_DIR)),)
$(error ModusToolbox tools_3.8 was not found. Set CY_TOOLS_PATHS to the full tools_3.8 directory using forward slashes)
endif

$(info Tools Directory: $(CY_TOOLS_DIR))

include $(CY_TOOLS_DIR)/make/start.mk
