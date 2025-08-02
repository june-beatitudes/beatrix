GIT_HASH := $(shell git rev-parse HEAD)
INCLUDES := -I./include -I../extern/openlibm/include -I../extern/umm_malloc/src -I../drivers \
			-I../extern/fatfs/source -I../shell/include

cfiles := $(wildcard src/*.c)
hfiles := $(wildcard include/*.h)
ofiles := $(addprefix build/, $(addsuffix .o, $(basename $(notdir $(cfiles))))) \
          build/scheduler_direct.o build/syscall.o

build: $(ofiles)

build/%.o: src/%.c
	$(CC) $^ -o $@ $(CFLAGS) $(INCLUDES) -DBEA_GIT_HASH="\"$(GIT_HASH)\""

build/scheduler_direct.o: src/scheduler_direct.S
	$(CC) $^ -o $@ $(CFLAGS) $(INCLUDES)

build/syscall.o: src/syscall.S
	$(CC) $^ -o $@ $(CFLAGS) $(INCLUDES)