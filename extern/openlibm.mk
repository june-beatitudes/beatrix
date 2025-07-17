INCLUDES = -I./include -I./src -I./arm

build:
	cd openlibm; make libopenlibm.a ARCH=arm CC=$(CC) CFLAGS_add="$(CFLAGS) \
		-DASSEMBLER -D__BSD_VISIBLE -fgnuc-version=0 $(INCLUDES)\
		-D'openlibm_weak_reference(x,y)=' -D'openlibm_strong_reference(x,y)=' \
		-D__powerpc__ -DOLM_LONG_DOUBLE -DNULL=0x0" LONG_DOUBLE_NOT_DOUBLE=0 -j
