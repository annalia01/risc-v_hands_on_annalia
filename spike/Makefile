RISCV_TAGET := riscv32-unknown-elf
AS := $(RISCV_TAGET)-as
CC := $(RISCV_TAGET)-gcc
OBJDUMP := $(RISCV_TAGET)-objdump
SPIKE := spike
PK := $(RISCV)/pk/$(RISCV_TAGET)/bin/pk
ISA := rv32gc_zifencei_zicsr

TEST_NAME := dot_prod_asm

all: main.spike main.dis

help:
	@echo "Make targets:"
	@echo "    run_spike    Run main.spike on Spike with Proxy Kernel"
	@echo "    main.spike   Build Spike executable"
	@echo "    main.dis     Disassemble executable"
	@echo "    clean        Clean outputs"

$(TEST_NAME).o: ../common/$(TEST_NAME).s
	$(AS) -march=rv32gc -mabi=ilp32 -o $@ $<

main.spike: src/main.c $(TEST_NAME).o src/dot_prod.c 
	$(CC) -o $@ $^

main.dis: main.spike
	$(OBJDUMP) -d main.spike > main.dis

run_spike: main.spike
	$(SPIKE) --isa=$(ISA) $(PK) $<

clean:
	rm -f *.o
	rm -f main.dis
	rm -f main.spike
