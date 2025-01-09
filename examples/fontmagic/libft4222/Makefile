# Makefile for building FT8xx-SPI Sample code for libmpsse on Linux and Windows.

APP = fontmagic

# Use MPSSE. This tells BRT_AN_025 to use a specific interface.
CFLAGS += -DUSE_FT4222=0
#CFLAGS += -Wall -Wextra 

ifeq ($(OS),Windows_NT)

# FT4222H link library and D2XX link library.
LIBPATHFT4222 = imports/LibFT4222/dll/amd64/LibFT4222-64.lib
LIBPATHD2XX = imports/ftd2xx/dll/amd64/ftd2xx.lib 
DLLRUNTIMEFT4222 = $(basename $(notdir $(LIBPATHFT4222))).dll
DLLPATHFT4222 = $(dir $(LIBPATHFT4222))$(DLLRUNTIMEFT4222)

# Add in include directories for BRT_AN_025.
CFLAGS += -Iimports/ftd2xx -Iimports/LibFT4222/inc -I../include -I../../../include  -I../common
CFLAGS += -DWIN32 -DNDEBUG -D_WINDOWS

# Add sources from the HAL and the example common code.
C_SOURCES = $(wildcard ../common/*.c) \
	$(wildcard ../../../ports/eve_libft4222/*.c) \
	$(wildcard ../../../source/*.c) \
	main.c

# Object files.
OBJ_FILES = $(C_SOURCES:.c=.o) 

# Link libraries and search paths.
LIBFILES = -L$(dir $(LIBPATHFT4222)) -l:$(notdir $(LIBPATHFT4222)) -L$(dir $(LIBPATHD2XX)) -l:$(notdir $(LIBPATHD2XX)) 

all: $(APP).exe $(DLLRUNTIMEFT4222)

$(APP).exe: $(OBJ_FILES) 
	echo $(DLLRUNTIMEFT4222)
	echo $(DLLPATHFT4222)
	$(CC) $(CFLAGS) $(DEPENDENCIES) $^ $(LIBFILES) -o $(APP).exe 

$(DLLRUNTIMEFT4222): 
	cp $(DLLPATHFT4222) $(DLLRUNTIMEFT4222)

clean:
	-rm -f *.o ; $(APP).exe; rm $(DLLRUNTIMEFT4222)
	
else # Linux builds (NOT YET SUPPORTED!)

DEPENDENCIES := -lpthread -lrt -ldl
UNAME := $(shell uname)
# Assume target is Mac OS if build host is Mac OS; any other host targets Linux.
ifeq ($(UNAME), Darwin)
	DEPENDENCIES += -lobjc -framework IOKit -framework CoreFoundation
else
	DEPENDENCIES += -lrt
endif

CFLAGS += -Wall -Wextra 
# Add in include directories for BRT_AN_025
CFLAGS += -Iimports/ftd2xx -Iimports/LibFT4222/inc -I../include -I../../../include  -I../common

DYNAMIC_LINK_OPTIONS := -Wl,-rpath /usr/local/lib

STATIC_APP = $(APP)-static
DYNAMIC_APP = $(APP)-dynamic

C_SOURCES = $(wildcard ../common/*.c) \
	$(wildcard ../../../ports/eve_libft4222/*.c) \
	$(wildcard ../../../source/*.c) \
	main.c

OBJ_FILES = $(C_SOURCES:.c=.o)

all: $(STATIC_APP) $(DYNAMIC_APP)

# TODO: adjust library path to libFT4222.lib
$(STATIC_APP): $(OBJ_FILES)
	$(CC) $^ -o $(STATIC_APP) -lFT4222 $(CFLAGS) $(DEPENDENCIES)

$(DYNAMIC_APP): $(OBJ_FILES)
	$(CC) $^ -o $(DYNAMIC_APP) -lFT4222 $(CFLAGS) $(DEPENDENCIES) $(DYNAMIC_LINK_OPTIONS)
	
clean:
	-rm -f *.o ; rm $(STATIC_APP); rm $(DYNAMIC_APP)
endif