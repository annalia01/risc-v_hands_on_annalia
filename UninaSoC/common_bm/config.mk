XLEN ?= 32
RV_PREFIX ?= riscv${XLEN}-unknown-elf-

CC          = $(RV_PREFIX)gcc
LD          = $(RV_PREFIX)ld
OBJDUMP     = $(RV_PREFIX)objdump
OBJCOPY     = $(RV_PREFIX)objcopy

#########
# Flags #
#########

CURRENT_MAKE = $(lastword $(MAKEFILE_LIST))

# Extract directory path (or '.' if no directory part)
CURRENT_DIR = $(dir $(CURRENT_MAKE))

DFLAG ?= -g
CFLAGS ?= -march=rv${XLEN}imac_zicsr_zifencei -mabi=ilp${XLEN} -O0 $(DFLAG)
LD_SCRIPT = ${CURRENT_DIR}/UninaSoC.ld
LDFLAGS = -nostdlib -T$(LD_SCRIPT)
LIB_OBJ_TINYIO = ${CURRENT_DIR}/TinyIO/lib/tinyio.a
BM_STARTUP = ${CURRENT_DIR}/startup.s
