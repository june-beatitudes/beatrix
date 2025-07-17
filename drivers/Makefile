cfiles := $(wildcard *.c) $(wildcard gpio/*.c)
ofiles := $(addprefix build/, $(addsuffix .o, $(basename $(notdir $(cfiles)))))

INCLUDES := -I. -I../kernel/include

build: $(ofiles)

build/gpio.o: gpio/gpio.c
	$(CC) $^ -o $@ $(CFLAGS) $(INCLUDES)

build/%.o: %.c
	$(CC) $^ -o $@ $(CFLAGS) $(INCLUDES)
