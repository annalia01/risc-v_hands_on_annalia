CUR_DIR = $(CURDIR)
PREV_DIR = $(shell dirname $(CUR_DIR))
TARGET_DIR = $(CURDIR)/UninaSoC/sw/SoC/examples/dot_prod
TARGET_DIR_ASM = $(CURDIR)/UninaSoC/sw/SoC/examples/dot_prod_asm
SOC_HW_DIR = $(CURDIR)/UninaSoC/hw/xilinx
SOC_ROOT_DIR = $(CURDIR)/UninaSoC

init:
	## Clone UninaSoC
	cd .. && git submodule update --init --recursive
	## Temporary patch for HBUS in embedded profile  
	rm -r ${CUR_DIR}/UninaSoC/hw/xilinx/ips/common/xlnx_highperformance_crossbar

install:
	## Install all the src files inside UninaSoC
	mkdir -p ${TARGET_DIR} ${TARGET_DIR}/ld ${TARGET_DIR_ASM} ${TARGET_DIR_ASM}/ld
	ln -s ${CUR_DIR}/src ${TARGET_DIR}
	ln -s ${CUR_DIR}/configs/Makefile ${TARGET_DIR}/Makefile
	ln -s ${CUR_DIR}/configs/user.ld ${TARGET_DIR}/ld/user.ld
	ln -s ${CUR_DIR}/src_asm ${TARGET_DIR_ASM}/src
	ln -s ${CUR_DIR}/configs/Makefile ${TARGET_DIR_ASM}/Makefile
	ln -s ${CUR_DIR}/configs/user.ld ${TARGET_DIR_ASM}/ld/user.ld
	ln -s ${PREV_DIR}/common/dot_prod_asm.s ${TARGET_DIR}/src/dot_prod_asm.s
	ln -s ${PREV_DIR}/common/dot_prod_asm.s ${TARGET_DIR_ASM}/src/dot_prod_asm.s
	## Patch to enable compiling for .s files
	cd $(SOC_ROOT_DIR) && git apply ../configs/asm_compile.patch

clean:
	## Run clean of UninaSoC (need to redo make hw after this)
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

help:
	@echo ""
	@echo "Project Setup Help"
	@echo "------------------"
	@echo "make init               	 - Initialize the repository and submodules"
	@echo "make install            	 - Set up symbolic links and apply patches"
	@echo "make hw                 	 - Build hardware bitstream"
	@echo "make program_bitstream  	 - Load bitstream onto the FPGA"
	@echo "make openocd_run        	 - Start OpenOCD server"
	@echo "make sw                 	 - Compile software examples"
	@echo "make gdb_run            	 - Run dot product (C version) with gdb"
	@echo "make gdb_run_asm        	 - Run dot product (Assembly version) with gdb"
	@echo "make clean              	 - Clean all generated files"
	@echo "source UninaSoC/settings.sh      - Configure environment (choose FPGA board)"
	@echo ""
	@echo "Board Options for 'settings.sh':"
	@echo "  embedded nexys_a7_50t     - For Nexys A7-50T board"
	@echo "  embedded nexys_a7_100t    - (Default) For Nexys A7-100T board"
	@echo ""

.PHONY:
	init
	install
	clean
	openocd_run
	gdb_run
	gdb_run_asm
	sw
	hw
	program_bitstream
	help
