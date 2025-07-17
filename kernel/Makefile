GIT_HASH := $(shell git rev-parse HEAD)

CC := ../tools/ATfE-20.1.0-Linux-x86_64/bin/clang
LD := ../tools/ATfE-20.1.0-Linux-x86_64/bin/ld.lld
FMT ?= clang-format
SHELL := bash

MATHLIB := ../extern/openlibm/libopenlibm.a
BUILTINSLIB := ../tools/ATfE-20.1.0-Linux-x86_64/lib/clang-runtimes/arm-none-eabi/armv7m_hard_fpv4_sp_d16/lib/libclang_rt.builtins.a
SYSROOT := ../tools/ATfE-20.1.0-Linux-x86_64/lib/clang-runtimes/arm-none-eabi/armv7m_hard_fpv4_sp_d16
UMM_MALLOC := ../extern/umm_malloc
umm_malloc_os := $(wildcard $(UMM_MALLOC)/*.o)

DEBUGFLAGS :=
ifdef DEBUG
	DEBUGFLAGS += -g -O0 -DDEBUG
endif
ifdef SEMIHOSTING
	DEBUGFLAGS += -DSEMIHOSTING
endif

CFLAGS += -c -Wall -Werror -g -O0 -I./include -ffreestanding \
	  --target=thumbv7m-unknown-none-eabihf -mfpu=fpv4-sp-d16 \
	  -fno-exceptions -fno-rtti -DBEA_GIT_HASH="\"$(GIT_HASH)\"" \
	  -std=c99 -I../extern/openlibm/include -DDEBUG -DSEMIHOSTING \
	  -I$(UMM_MALLOC)

cfiles := $(wildcard src/*.c)
afiles := $(cfiles:.c=.a)
hfiles := $(wildcard include/*.h)

clean:
	rm -rf build/* src/*.a src/*.c.json

build: $(afiles) main.ld $(hfiles) $(MATHLIB) $(umm_malloc_os)
	$(LD) $(afiles) $(umm_malloc_os) $(MATHLIB) $(BUILTINSLIB) -o build/main.elf -T main.ld
	rm compile_commands.json
	echo "[" > compile_commands.json
	cat $(cfiles:.c=.c.json) >> compile_commands.json
	echo "]" >> compile_commands.json

src/%.a: src/%.c
	$(CC) $^ -o $@ $(CFLAGS) $(DEBUGFLAGS) -MJ $^.json

$(MATHLIB):
	cd ../extern/openlibm; \
	make libopenlibm.a ARCH=arm CC=../../tools/ATfE-20.1.0-Linux-x86_64/bin/clang CFLAGS_add="-fno-strict-aliasing -std=c99 -Wall -I./include -I./src -I. -I./arm -ffreestanding --target=thumbv7m-unknown-none-eabihf -mfpu=fpv4-sp-d16 -fno-exceptions -fno-rtti -DASSEMBLER -D__BSD_VISIBLE -fgnuc-version=0 -D'openlibm_weak_reference(x,y)=' -D'openlibm_strong_reference(x,y)=' -D__powerpc__ -DOLM_LONG_DOUBLE -DNULL=0x0" LONG_DOUBLE_NOT_DOUBLE=0 -j

$(umm_malloc_os):
	make build -f umm_malloc.mk

format: $(cfiles) $(hfiles)
	clang-format $^ -i

COMPILER_URL := https://github.com/arm/arm-toolchain/releases/download/release-20.1.0-ATfE/ATfE-20.1.0-Linux-x86_64.tar.xz

setup:
ifeq (,$(wildcard ../tools/*))
	@echo Downloading tools...
	cd ../tools; \
	wget $(COMPILER_URL); \
	tar xf *
else
	@echo Tooling already set up!
endif
