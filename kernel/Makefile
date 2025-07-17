GIT_HASH := $(shell git rev-parse HEAD)
INCLUDES := -I./include -I../extern/openlibm/include -I../extern/umm_malloc/src

cfiles := $(wildcard src/*.c)
hfiles := $(wildcard src/*.h)
ofiles := $(cfiles:.c=.o)

build: $(ofiles)

src/%.o: src/%.c $(hfiles)
	$(CC) $^ -o $@ $(CFLAGS) $(INCLUDES) -DBEA_GIT_HASH="\"$(GIT_HASH)\""
