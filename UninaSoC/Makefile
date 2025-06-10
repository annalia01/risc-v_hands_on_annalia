CUR_DIR = $(CURDIR)
PREV_DIR = $(shell dirname $(CUR_DIR))
TARGET_DIR = $(CURDIR)/UninaSoC/sw/SoC/examples/dot_prod
TARGET_DIR_ASM = $(CURDIR)/UninaSoC/sw/SoC/examples/dot_prod_asm
SOC_HW_DIR = $(CURDIR)/UninaSoC/hw/xilinx
SOC_ROOT_DIR = $(CURDIR)/UninaSoC

init:
	## Clone UninaSoC
	cd .. && git submodule update --init --recursive
	rm -r ${CUR_DIR}/UninaSoC/hw/xilinx/ips/common/xlnx_highperformance_crossbar

install:
	mkdir -p ${TARGET_DIR} ${TARGET_DIR}/ld ${TARGET_DIR_ASM} ${TARGET_DIR_ASM}/ld
	ln -s ${CUR_DIR}/src ${TARGET_DIR}
	ln -s ${CUR_DIR}/configs/Makefile ${TARGET_DIR}/Makefile
	ln -s ${CUR_DIR}/configs/user.ld ${TARGET_DIR}/ld/user.ld
	ln -s ${CUR_DIR}/src_asm ${TARGET_DIR_ASM}/src
	ln -s ${CUR_DIR}/configs/Makefile ${TARGET_DIR_ASM}/Makefile
	ln -s ${CUR_DIR}/configs/user.ld ${TARGET_DIR_ASM}/ld/user.ld
	ln -s ${PREV_DIR}/common/dot_prod_asm.s ${TARGET_DIR}/src/dot_prod_asm.s
	ln -s ${PREV_DIR}/common/dot_prod_asm.s ${TARGET_DIR_ASM}/src/dot_prod_asm.s
	cd $(SOC_ROOT_DIR) && git apply ../configs/asm_compile.patch

clean:
	-$(MAKE) -C $(SOC_ROOT_DIR) clean
	-rm -r ${TARGET_DIR}
	-rm -r ${TARGET_DIR_ASM}
	-rm src/dot_prod_asm.s
	-rm src_asm/dot_prod_asm.s

openocd_run:
	pkill -9 hw_server
	$(MAKE) -C $(SOC_HW_DIR) openocd_run

gdb_run:
	$(MAKE) -C $(SOC_HW_DIR) gdb_run ELF_PATH=$(TARGET_DIR)/bin/dot_prod.elf

gdb_run_asm:
	$(MAKE) -C $(SOC_HW_DIR) gdb_run ELF_PATH=$(TARGET_DIR_ASM)/bin/dot_prod_asm.elf

hw: 
	$(MAKE) -C $(SOC_ROOT_DIR) config
	sudo true && $(MAKE) -C $(SOC_ROOT_DIR) units
	$(MAKE) -C $(SOC_HW_DIR) ips
	$(MAKE) -C $(SOC_HW_DIR) bitstream

sw:
	## Recompile tinyIO (gdb can't see tinyIO symbols using precompiled .a)
	$(MAKE) -C $(SOC_ROOT_DIR)/sw/SoC/lib/tinyio clean
	$(MAKE) -C $(SOC_ROOT_DIR)/sw/SoC/lib/tinyio
	$(MAKE) -C $(SOC_ROOT_DIR) sw

program_bitstream:
	$(MAKE) -C $(SOC_HW_DIR) start_hw_server
	$(MAKE) -C $(SOC_HW_DIR) program_bitstream 

.PHONY:
	init
	install
	clean
	openocd_run
	gdb_run


