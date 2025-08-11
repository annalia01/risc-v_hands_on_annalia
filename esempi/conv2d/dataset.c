#include <stdint.h>
#include "runtime.h"

// Dimensions
int64_t M __attribute__((aligned(16))) = 8;
int64_t N __attribute__((aligned(16))) = 8;
int64_t F __attribute__((aligned(16))) = 3;

// Buffers (dummy initial values)
int64_t i[192] __attribute__((aligned(16))) = {0};
int64_t f[27] __attribute__((aligned(16))) = {0};
int64_t o[64] __attribute__((aligned(16))) = {0};
int64_t golden_o[64] __attribute__((aligned(16))) = {0};

// UART dummy register
volatile uint32_t fake_uart = 0;
