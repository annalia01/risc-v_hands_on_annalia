# ==== Config di toolchain / architettura ====
PREFIX ?= riscv64-unknown-elf
CC     := $(PREFIX)-gcc
OBJDUMP:= $(PREFIX)-objdump
OBJCOPY:= $(PREFIX)-objcopy

# ISA usata sia a compile-time sia a run-time (Spike)
MARCH ?= rv64gcv_zicsr_zicntr
ABI   ?= lp64d

# ==== Sorgenti / output ====
PROJECT := main
SRCS    := main.c fmatmul.c printf.c util.c dataset.c serial.c
OBJS    := $(SRCS:.c=.o)
DEPS    := $(OBJS:.o=.d)

# ==== Flag ====
CFLAGS  ?= -O3 -Wall -Wextra -Wno-unused-parameter -ffast-math -funroll-loops \
           -march=$(MARCH) -mabi=$(ABI) -I. -MMD -MP
LDFLAGS ?=
LDLIBS  ?=

# ==== Spike / PK ====
SPIKE ?= spike
PK    ?= pk

# ==== Regole principali ====
.PHONY: all run dump clean

all: $(PROJECT).elf

$(PROJECT).elf: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

# Compilazione oggetti
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Esecuzione con Spike + PK
run: $(PROJECT).elf
	$(SPIKE) --isa=$(MARCH) $(PK) ./$(PROJECT).elf

# Disassemblato utile per debug
dump: $(PROJECT).elf
	$(OBJDUMP) -d -S $< > $(PROJECT).dump
	@echo "Creato: $(PROJECT).dump"

clean:
	$(RM) $(OBJS) $(DEPS) $(PROJECT).elf $(PROJECT).dump

# Includi le dipendenze generate da -MMD
-include $(DEPS)
