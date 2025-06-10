# Project Setup Instructions

Follow these steps to set up and run the project:

1. **Initialize the Repository**
   ```bash
   make init
   ```
   This command clones the UninaSoC submodule.

2. **Install Examples**
   ```bash
   make install
   ```
   This installs the examples into the UninaSoC build system.

3. **Set Environment Variables**
   Depending on your board version, run one of the following commands:
   ```bash
   source UninaSoC/settings.sh embedded nexys_a7_50t
   ```
   or
   ```bash
   source UninaSoC/settings.sh embedded nexys_a7_100t
   ```
   If unspecified, the default board version is `nexys_a7_100t`.

4. **Build the Bitstream**
   ```bash
   make hw
   ```
   This command builds the bitstream for the hardware.

5. **Program the FPGA**
   ```bash
   make program_bitstream 
   ```
   This command runs the bitstream on FPGA.


6. **Run OpenOCD**
   ```bash
   make openocd_run
   ```
   This command starts OpenOCD.

7. **Compile Examples**
   ```bash
   make sw
   ```
   This compiles the software examples.

8. **Run C Dot Product Example**
   ```bash
   make gdb_run
   ```
   This command runs the C dot product example.

9. **Run Assembly Dot Product Example**
   ```bash
   make gdb_run_asm
   ```
   This command runs the assembly dot product example.
