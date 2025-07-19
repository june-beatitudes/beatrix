cfiles := $(wildcard *.c) $(wildcard gpio/*.c) $(wildcard spi/*.c) $(wildcard sd/*.c) $(wildcard display_direct/*.c)
ofiles := $(addprefix build/, $(addsuffix .o, $(basename $(notdir $(cfiles)))))

INCLUDES := -I. -I../kernel/include

build: $(ofiles)

build/gpio.o: gpio/gpio.c
	$(CC) $^ -o $@ $(CFLAGS) $(INCLUDES)

build/spi.o: spi/spi.c
	$(CC) $^ -o $@ $(CFLAGS) $(INCLUDES)

build/display_direct.o: display_direct/display_direct.c
	$(CC) $^ -o $@ $(CFLAGS) $(INCLUDES)

build/sd.o: sd/sd.c
	$(CC) $^ -o $@ $(CFLAGS) $(INCLUDES)

build/%.o: %.c
	$(CC) $^ -o $@ $(CFLAGS) $(INCLUDES)
