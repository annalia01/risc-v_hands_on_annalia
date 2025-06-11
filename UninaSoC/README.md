# Project Setup Instructions

Follow the steps below to set up and run the project:

### 1. Initialize the Repository

Clone the `UninaSoC` submodule:

```bash
make init
```

### 2. Install Examples

Install the software examples into the build system:

```bash
make install
```

### 3. Set Environment Variables

Choose the appropriate command based on your FPGA board version:

```bash
source UninaSoC/settings.sh embedded nexys_a7_50t
```

or

```bash
source UninaSoC/settings.sh embedded nexys_a7_100t
```

> **Note:** If unspecified, `nexys_a7_100t` is used by default.

### 4. Build Hardware Bitstream

Generate the FPGA bitstream:

```bash
make hw
```

### 5. Program the FPGA

Load the bitstream onto the board:

```bash
make program_bitstream
```

### 6. Start OpenOCD to debug the examples

Launch OpenOCD:

```bash
make openocd_run
```

### 7. Compile Software Examples

Build the C and assembly software examples:

```bash
make sw
```

### 8. Run Dot Product Examples using gdb

* **C version:**

  ```bash
  make gdb_run
  ```
* **Assembly version:**

  ```bash
  make gdb_run_asm
  ```
