GIT_HASH := $(shell git rev-parse HEAD)

CC := ../tools/LLVM-ET-Arm-18.1.3-Linux-x86_64/bin/clang
LD := ../tools/LLVM-ET-Arm-18.1.3-Linux-x86_64/bin/ld.lld
FMT ?= clang-format
SHELL := bash

MATHLIB := ../extern/openlibm/libopenlibm.a
BUILTINSLIB := ../tools/LLVM-ET-Arm-18.1.3-Linux-x86_64/lib/clang-runtimes/arm-none-eabi/armv7em_hard_fpv4_sp_d16/lib/libclang_rt.builtins.a
SYSROOT := ../tools/LLVM-ET-Arm-18.1.3-Linux-x86_64/lib/clang-runtimes/arm-none-eabi/armv7em_hard_fpv4_sp_d16

DEBUGFLAGS :=
ifdef DEBUG
	DEBUGFLAGS += -g -O0 -DDEBUG
endif
ifdef SEMIHOSTING
	DEBUGFLAGS += -DSEMIHOSTING
endif

CFLAGS += -c -Wall -Werror -g -O0 -I./include -ffreestanding \
	  --target=thumbv7em-none-unknown-eabihf -mfpu=fpv4-sp-d16 \
	  -fno-exceptions -fno-rtti -DBEA_GIT_HASH="\"$(GIT_HASH)\"" \
	  -std=c99 -isystem $(SYSROOT)/include -I../extern/openlibm/include

cfiles := $(wildcard src/*.c)
afiles := $(cfiles:.c=.a)
hfiles := $(wildcard include/*.h)

clean:
	rm -rf build/* src/*.a src/*.c.json

build: $(afiles) main.ld $(hfiles) $(MATHLIB)
	$(LD) $(afiles) $(MATHLIB) $(BUILTINSLIB) -o build/main.elf -T main.ld
	rm compile_commands.json
	echo "[" > compile_commands.json
	cat $(cfiles:.c=.c.json) >> compile_commands.json
	echo "]" >> compile_commands.json

src/%.a: src/%.c
	$(CC) $^ -o $@ $(CFLAGS) $(DEBUGFLAGS) -MJ $^.json

$(MATHLIB):
	cd ../extern/openlibm; \
	make libopenlibm.a ARCH=arm CC=../../tools/LLVM-ET-Arm-18.1.3-Linux-x86_64/bin/clang CFLAGS_add="-fno-builtin -fno-strict-aliasing -std=c99 -Wall -I./include -I./src -I. -I./arm -ffreestanding --target=thumbv7em-none-unknown-eabihf -mfpu=fpv4-sp-d16 -fno-exceptions -fno-rtti -DASSEMBLER -D__BSD_VISIBLE -fgnuc-version=0 -D'openlibm_weak_reference(x,y)=' -D'openlibm_strong_reference(x,y)=' -D__powerpc__ -DOLM_LONG_DOUBLE" LONG_DOUBLE_NOT_DOUBLE=0 -j

format: $(cfiles) $(hfiles)
	clang-format $^ -i
