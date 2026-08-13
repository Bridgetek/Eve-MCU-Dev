PLATFORM?=RASPBERRYPI

CC = gcc
CFLAGS = -c -Wall -g -Os -DPLATFORM_RASPBERRYPI -DUSE_LINUX_SPI_DEV -I. -I../../../include -I../../snippets/ -I../../../source/extensions -I../common
LD = $(CC)
LDFLAGS = 

TARGET = simple_$(shell basename $(CURDIR))

OBJECTS = $(patsubst %.c, %.o, $(shell find . -name "*.c"))
OBJECTS += $(patsubst %.c, %.o, $(shell find ../common -name "*.c"))
OBJECTS += $(patsubst %.c, %.o, $(shell find ../../../source -name "*.c"))
OBJECTS += $(patsubst %.c, %.o, $(shell find ../../snippets/touch.c))
OBJECTS += $(patsubst %.c, %.o, $(shell find ../../../ports/eve_arch_rpi -name "*.c"))
OBJECTS += $(patsubst %.c, %.o, $(shell find ../../../source/extensions -name "*.c"))

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(LD) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm $(TARGET) $(OBJECTS)
