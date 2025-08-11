# ==== Toolchain e percorsi Spike/PK (come da tue variabili) ====
CC     := riscv64-unknown-elf-gcc
SPIKE  := $(HOME)/RISC-V/spike/bin/spike
PK     := $(HOME)/RISC-V/pk/riscv32-unknown-elf/bin/pk   # meglio rv64 se disponibile

# ==== ISA/ABI: quelli che abbiamo usato ====
MARCH  := rv64gcv_zicsr_zicntr
ABI    := lp64

# ==== Flag minimi, solo ciò che serve ====
CFLAGS := -O2 -march=$(MARCH) -mabi=$(ABI)

# ==== Sorgenti/target ====
TARGET := main.elf
SRCS   := main.c fmatmul.c printf.c util.c dataset.c serial.c

# ==== Regole ====
.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

run: $(TARGET)
	"$(SPIKE)" --isa=$(MARCH) "$(PK)" ./$(TARGET)

clean:
	$(RM) $(TARGET)
