CC := ../tools/arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi/bin/arm-none-eabi-gcc
LD := ../tools/arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi/bin/arm-none-eabi-ld
FMT ?= clang-format
SHELL := bash

CFLAGS += -c -Wall -Werror -g -O0 -I./include -nostdlib -nostartfiles -march=armv7e-m

cfiles := $(wildcard src/*.c)
ofiles := $(cfiles:.c=.o)
hfiles := $(wildcard include/*.h)

clean:
	rm -rf build/* src/*.o

build: $(ofiles)
	$(LD) $^ -o build/main.elf -T main.ld

$(ofiles): $(cfiles)
	$(CC) $^ -o $@ $(CFLAGS)

format: $(cfiles) $(hfiles)
	clang-format $^ -i