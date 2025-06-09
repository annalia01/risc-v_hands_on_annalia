include ../common_bm/config.mk

all: 
	$(BUILD_TINYIO)
	$(CC) $(CFLAGS) $(LDFLAGS) dot_prod.c main.c ../../common/dot_prod_asm.s $(BM_STARTUP) $(LIB_OBJ_TINYIO) -o main.elf

clean:
	$(CLEAN_TINYIO)
	rm -f main.elf

.PHONY: all, clean
