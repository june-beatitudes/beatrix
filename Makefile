CC := $(realpath tools/ATfE-20.1.0-Linux-x86_64/bin/clang)
LD := $(abspath tools/ATfE-20.1.0-Linux-x86_64/bin/ld.lld)
CFLAGS := -c -Wall -g -O0 -ffreestanding \
	  --target=thumbv7m-unknown-none-eabihf -mfpu=fpv4-sp-d16 \
	  -fno-exceptions -fno-rtti -std=c99 -DDEBUG -DSEMIHOSTING \
	  -DBEA_KERNEL_INTERNAL
COMPILER_URL := https://github.com/arm/arm-toolchain/releases/download/release-20.1.0-ATfE/ATfE-20.1.0-Linux-x86_64.tar.xz
BUILTINSLIB := tools/ATfE-20.1.0-Linux-x86_64/lib/clang-runtimes/arm-none-eabi/armv7m_hard_fpv4_sp_d16/lib/libclang_rt.builtins.a

cfiles := $(wildcard kernel/src/*.c) $(wildcard drivers/*.c) $(wildcard drivers/gpio/*.c) \
	  $(wildcard drivers/spi/*.c) $(wildcard drivers/sd/*.c) $(wildcard drivers/display_direct/*c) \
	  $(wildcard drivers/graphics/*.c)
hfiles := $(wildcard kernel/include/*.h) $(wildcard drivers/*.h) $(wildcard drivers/gpio/*.h) \
	  $(wildcard drivers/spi/*.h) $(wildcard drivers/sd/*.h) $(wildcard drivers/display_direct/*.h) \
	  $(wildcard drivers/graphics/*.h)
mdfiles := $(wildcard *.md) $(wildcard kernel/*.md)

setup:
ifeq (,$(wildcard tools/*))
	@echo Downloading tools...
	cd tools; \
	wget $(COMPILER_URL); \
	tar xf *
else
	@echo Tooling already set up!
endif

docs: $(cfiles) $(hfiles) $(mdfiles) Doxyfile
	rm -rf docs/*
	doxygen Doxyfile
	cd extern/doxybook; \
	uv run doxybook -i ../../docs/xml -o ../../docs/markdown.md --template-lang c --doxygen-extra-args="../../Doxyfile"
	head -n 1 docs/markdown.md > docs/fixed-markdown.md
	tail +3 docs/markdown.md | sed "s/^## File/# File/; s/^#/##/" >> docs/fixed-markdown.md
	pandoc -N -s -t chunkedhtml -o docs/html.zip docs/fixed-markdown.md --shift-heading-level-by=-1 --toc=true
	unzip docs/html.zip -d docs/html
	rm -rf docs/xml docs/markdown.md docs/fixed-markdown.md docs/html.zip

format:
	clang-format -i $(cfiles) $(hfiles)

kernel: setup
	cd kernel && make build CC="$(CC)" CFLAGS="$(CFLAGS) -Wall"

umm_malloc: setup
	cd extern && make build -f umm_malloc.mk CC="$(CC)" CFLAGS="$(CFLAGS) -Wall"

drivers: setup
	cd drivers && make build CC="$(CC)" CFLAGS="$(CFLAGS) -Wall"

openlibm: setup
	cd extern && make build -f openlibm.mk CC="$(CC)" CFLAGS="$(CFLAGS)"

fatfs: setup
	cd extern && make build -f fatfs.mk CC="$(CC)" CFLAGS="$(CFLAGS) -Wall"

ofiles := $(wildcard resources/build/*.o) $(wildcard kernel/build/*.o) extern/openlibm/libopenlibm.a \
	  $(wildcard extern/umm_malloc/src/*.o) $(wildcard drivers/build/*.o) $(wildcard extern/fatfs/source/*.o)

build: drivers kernel umm_malloc openlibm setup fatfs
	$(LD) -o bin/main.elf -Tmain.ld $(ofiles) $(BUILTINSLIB)
