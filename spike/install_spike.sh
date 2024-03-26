#!/bin/bash

# Download sources directory
export DOWNLOAD_DIR=$(pwd)/downloads
# Installation diretory
export INSTALL_DIR=$HOME/RISC-V

# Create directories
mkdir -p $DOWNLOAD_DIR
mkdir -p $INSTALL_DIR

# NOTE: Prequisites are specified for Debian/Ubuntu

# Spike
cd $DOWNLOAD_DIR
sudo apt install -y device-tree-compiler
git clone https://github.com/riscv-software-src/riscv-isa-sim --depth 1
cd riscv-isa-sim
mkdir build
cd build
../configure --prefix=$INSTALL_DIR/spike
make
make install

# GNU toolchain
sudo apt-get install -y autoconf automake autotools-dev curl python3 python3-pip \
    libmpc-dev libmpfr-dev libgmp-dev gawk build-essential bison flex texinfo gperf \
    libtool patchutils bc zlib1g-dev libexpat-dev ninja-build git cmake libglib2.0-dev
cd $DOWNLOAD_DIR
git clone https://github.com/riscv/riscv-gnu-toolchain.git --depth 1
cd riscv-gnu-toolchain
git submodule update --init --recursive --depth 1 binutils gcc \
                        glibc dejagnu newlib gdb
mkdir build
cd build
# NOTE: this is going to take a 2-3 hours....
../configure --prefix=$INSTALL_DIR/gnu-toolchain --enable-multilib
make -j $(nproc)
make install

# Proxy Kernel
cd $DOWNLOAD_DIR
git clone https://github.com/riscv/riscv-pk.git --depth 1
cd riscv-pk
mkdir build
cd build
# Add newly installed GCC binaries for RISC-V to our path
export PATH=$INSTALL_DIR/gnu-toolchain/bin:$PATH 
# zifencei_zicsr are necessary for GCC>=12
../configure --prefix=$INSTALL_DIR/pk \
    --host=riscv64-unknown-elf \
    --with-arch=rv64gc_zifencei_zicsr 
make
make install