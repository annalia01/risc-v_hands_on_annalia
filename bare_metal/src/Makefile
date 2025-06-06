XLEN ?= 32
RV_PREFIX ?= riscv${XLEN}-unknown-elf-

CC          = $(RV_PREFIX)gcc
LD          = $(RV_PREFIX)ld
OBJDUMP     = $(RV_PREFIX)objdump
OBJCOPY     = $(RV_PREFIX)objcopy

#########
# Flags #
#########

DFLAG ?= -g
CFLAGS ?= -march=rv${XLEN}imac_zicsr_zifencei -mabi=ilp${XLEN} -O0 $(DFLAG)
LD_SCRIPT = UninaSoC.ld
LDFLAGS = -nostdlib -T$(LD_SCRIPT)
LIB_OBJ_TINYIO = tinyio.a

all: 
	$(CC) $(CFLAGS) $(LDFLAGS) main.s data.s startup.s ../../common/dot_prod_asm.s tinyio.a -o main.elf

clean:
	rm -f *.o
	rm -f main.elf
