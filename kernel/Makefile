GIT_HASH := $(shell git rev-parse HEAD)

CC := ../tools/arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi/bin/arm-none-eabi-gcc
LD := ../tools/arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi/bin/arm-none-eabi-ld
FMT ?= clang-format
SHELL := bash

DEBUGFLAGS :=
ifdef DEBUG
	DEBUGFLAGS += -g -O0 -DDEBUG
endif
ifdef SEMIHOSTING
	DEBUGFLAGS += -DSEMIHOSTING
endif

CFLAGS += -c -Wall -Werror -g -O0 -I./include -ffreestanding -nostartfiles -fno-math-errno -nostdlib -mfloat-abi=hard -march=armv7e-m+fp -DBEA_GIT_HASH="\"$(GIT_HASH)\""
LIBM += ../tools/arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi/arm-none-eabi/lib/thumb/v7e-m+fp/hard/libm.a
LIBGCC += ../tools/arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi/lib/gcc/arm-none-eabi/14.2.1/thumb/v7e-m+fp/hard/libgcc.a

cfiles := $(wildcard src/*.c)
cfiles := $(filter-out src/__errno.c, $(cfiles))
ofiles := $(cfiles:.c=.o)
hfiles := $(wildcard include/*.h)

clean:
	rm -rf build/* src/*.o

build: $(ofiles) main.ld $(hfiles) src/__errno.o
	$(LD) $(LIBGCC) $(ofiles) $(LIBM) src/__errno.o -o build/main.elf -T main.ld

src/%.o: src/%.c
	$(CC) $^ -o $@ $(CFLAGS) $(DEBUGFLAGS)

format: $(cfiles) $(hfiles)
	clang-format $^ -i

docs: $(cfiles) $(hfiles) Doxyfile
	rm -rf docs/*
	doxygen
