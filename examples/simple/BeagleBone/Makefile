PLATFORM?=BEAGLEBONE

CC = gcc
CFLAGS = -c -Wall -g -Os -DPLATFORM_BEAGLEBONE -DUSE_LINUX_SPI_DEV -I. -I../../../include -I../../../ports/eve_bt82x -I../common -I../../snippets 
LD = $(CC)
LDFLAGS = 

TARGET = simple_$(shell basename $(CURDIR))

OBJECTS = $(patsubst %.c, %.o, $(shell find . -name "*.c"))
OBJECTS += $(patsubst %.c, %.o, $(shell find ../common -name "*.c"))
OBJECTS += $(patsubst %.c, %.o, $(shell find ../../../source -name "*.c"))
OBJECTS += $(patsubst %.c, %.o, $(shell find ../../snippets -name "touch.c"))
OBJECTS += $(patsubst %.c, %.o, $(shell find ../../../ports/eve_arch_beaglebone -name "EVE_Linux_BBB.c"))
OBJECTS += $(patsubst %.c, %.o, $(shell find ../../../ports/eve_bt82x -name "patch_base.c"))

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(LD) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm $(TARGET) $(OBJECTS)
