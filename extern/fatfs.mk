cfiles := $(wildcard fatfs/source/*.c)
ofiles := $(cfiles:.c=.o)

INCLUDES := -Ifatfs/source -I../drivers -I../kernel/include

build: $(ofiles)

fatfs/%.o: fatfs/%.c
	$(CC) $^ -o $@ $(CFLAGS) $(INCLUDES)
