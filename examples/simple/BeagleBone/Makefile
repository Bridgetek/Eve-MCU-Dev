PLATFORM?=BEAGLEBONE

CC = gcc
CFLAGS = -c -Wall -g -Os -DPLATFORM_BEAGLEBONE -DUSE_LINUX_SPI_DEV -I. -I../../../include -I../../../ports/eve_bt82x -I../common
LD = $(CC)
LDFLAGS = 

TARGET = simple_$(shell basename $(CURDIR))

OBJECTS = $(patsubst %.c, %.o, $(shell find . -name "*.c"))
OBJECTS += $(patsubst %.c, %.o, $(shell find ../common -name "*.c"))
OBJECTS += $(patsubst %.c, %.o, $(shell find ../../../source -name "*.c"))
OBJECTS += $(patsubst %.c, %.o, $(shell find ../../../ports/eve_arch_beaglebone -name "*.c"))
OBJECTS += $(patsubst %.c, %.o, $(shell find ../../../ports/eve_bt82x -name "*.c"))

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(LD) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm $(TARGET) $(OBJECTS)
