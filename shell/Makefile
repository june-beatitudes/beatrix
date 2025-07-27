INCLUDES := -I./include -I../extern/fatfs/src -I../drivers -I../kernel/include
cfiles := $(wildcard src/*.c)
hfiles := $(wildcard include/*.h)
ofiles := $(addprefix build/, $(addsuffix .o, $(basename $(notdir $(cfiles)))))

build: $(ofiles)

build/%.o: src/%.c
	$(CC) $^ -o $@ $(INCLUDES) $(SHELL_CFLAGS)