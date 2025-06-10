CUR_DIR = $(CURDIR)
TARGET_DIR = $(CURDIR)/UninaSoC/sw/SoC/examples/dot_prod
SOC_HW_DIR = $(CURDIR)/UninaSoC/hw/xilinx
SOC_ROOT_DIR = $(CURDIR)/UninaSoC

init:
	## Clone UninaSoC
	cd .. && git submodule update --init --recursive
	rm -r ${CUR_DIR}/UninaSoC/hw/xilinx/ips/common/xlnx_highperformance_crossbar

install: init
	mkdir -p ${TARGET_DIR}
	mkdir ${TARGET_DIR}/ld
	ln -s ${CUR_DIR}/src ${TARGET_DIR}/src
	ln -s ${CUR_DIR}/configs/Makefile ${TARGET_DIR}/Makefile
	ln -s ${CUR_DIR}/configs/user.ld ${TARGET_DIR}/ld/user.ld

clean:
	$(MAKE) -C $(SOC_ROOT_DIR) clean
	rm -r ${TARGET_DIR}

openocd_run:
	pkill -9 hw_server
	$(MAKE) -C $(SOC_HW_DIR) openocd_run

gdb_run:
	$(MAKE) -C $(SOC_HW_DIR) gdb_run ELF_PATH=

hw: init
	sudo true && $(MAKE) -C $(SOC_ROOT_DIR) units
	$(MAKE) -C $(SOC_HW_DIR) ips
	$(MAKE) -C $(SOC_HW_DIR) bitstream

sw: init
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


