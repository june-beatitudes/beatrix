cfiles := $(wildcard ../extern/umm_malloc/src/*.c)
cfiles := $(cfiles:.c=.o)

CC := ../tools/ATfE-20.1.0-Linux-x86_64/bin/clang

CFLAGS += -c -Wall -Werror -g -O0 -I../extern/um_malloc/src \
	  -I../extern/compat-stubs -ffreestanding -std=c99 \
	  --target=thumbv7m-unknown-none-eabihf -mfpu=fpv4-sp-d16 \
	  -fno-exceptions -fno-rtti

build: $(ofiles)

../extern/umm_malloc/%.o: ../extern/umm_malloc/%.c
	$(CC) $^ -o $@ $(CFLAGS)
