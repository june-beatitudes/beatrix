GIT_HASH := $(shell git rev-parse HEAD)
INCLUDES := -I./include -I../extern/openlibm/include -I../extern/umm_malloc/src -I../drivers \
			-I../extern/fatfs/source

cfiles := $(wildcard src/*.c)
hfiles := $(wildcard src/*.h)
ofiles := $(addprefix build/, $(addsuffix .o, $(basename $(notdir $(cfiles)))))

build: $(ofiles)

build/%.o: src/%.c $(hfiles)
	$(CC) $^ -o $@ $(CFLAGS) $(INCLUDES) -DBEA_GIT_HASH="\"$(GIT_HASH)\""
