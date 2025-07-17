cfiles := $(wildcard umm_malloc/src/*.c)
cfiles := $(cfiles:.c=.o)

INCLUDES := -Ium_malloc/src -Icompat-stubs

build: $(ofiles)

umm_malloc/%.o: umm_malloc/%.c
	$(CC) $^ -o $@ $(CFLAGS) $(INCLUDES)
