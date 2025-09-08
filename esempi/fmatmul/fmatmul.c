// Copyright 2020 ETH Zurich and University of Bologna.
// SPDX-License-Identifier: Apache-2.0
//
// Ottimizzazione 2025: packing dei pannelli di A per eliminare i load a stride N.
// - Si packano blocchi (4|8|16) x N in un buffer contiguo Apack.
// - I micro-kernel leggono da Apack con 4/8/16 load contigui.
// - Si mantiene la logica RVV originale (ping-pong su B, stessi accumulatori).

#include "fmatmul.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#define MIN(a,b) ((a)<(b)?(a):(b))
#define ASSUME_ALIGNED_64(p) ((float*)__builtin_assume_aligned((p), 64))



// --- Dispatcher --------------------------------------------------------------

void fmatmul(float * __restrict c_,
             const float * __restrict a_,
             const float * __restrict b_,
             const unsigned long int M,
             const unsigned long int N,
             const unsigned long int P) {
  float       * __restrict c = ASSUME_ALIGNED_64(c_);
  const float * __restrict a = ASSUME_ALIGNED_64(a_);
  const float * __restrict b = ASSUME_ALIGNED_64(b_);

  if (M <= 4) {
    fmatmul_4x4(c, a, b, M, N, P);
  } else if (M <= 8) {
    fmatmul_8x8(c, a, b, M, N, P);
  } else if (M <= 64) {
    fmatmul_16x16(c, a, b, M, N, P);
  } else if (M <= 128) {
    // VL=64; con 8x8 (LMUL=2) ottieni VL effettiva 128
    fmatmul_8x8(c, a, b, M, N, P);
  } else {
    // VL=64; con 4x4 (LMUL=4) ottieni VL effettiva 256
    fmatmul_4x4_packed(c, a, b, M, N, P);
  }
}

// ============================================================================
// 4x4
// ============================================================================

void fmatmul_4x4(float *c, const float *a, const float *b,
                 const unsigned long int M, const unsigned long int N,
                 const unsigned long int P) {
  // We work on 4 rows of the matrix at once
  const unsigned long int block_size = 4;
  unsigned long int block_size_p;

  // Set the vector configuration
  asm volatile("vsetvli %0, %1, e32, m4, ta, ma" : "=r"(block_size_p) : "r"(P));

  // Slice the matrix into a manageable number of columns p_
  for (unsigned long int p = 0; p < P; p += block_size_p) {
    // Set the vector length
    const unsigned long int p_ = MIN(P - p, block_size_p);

    // Find pointers to the submatrices
    const float *b_ = b + p;
    float *c_ = c + p;

    asm volatile("vsetvli zero, %0, e32, m4, ta, ma" ::"r"(p_));

    // Iterate over the rows
    for (unsigned long int m = 0; m < M; m += block_size) {
      // Find pointer to the submatrices
      const float *a_ = a + m * N;
      float *c__ = c_ + m * P;

      fmatmul_vec_4x4_slice_init();
      fmatmul_vec_4x4(c__, a_, b_, N, P);
    }
  }
}

void fmatmul_vec_4x4_slice_init() {
  asm volatile("vmv.v.i v0,  0");
  asm volatile("vmv.v.i v4,  0");
  asm volatile("vmv.v.i v8,  0");
  asm volatile("vmv.v.i v12, 0");
}


void fmatmul_vec_4x4(float *c, const float *a, const float *b,
                       const unsigned long int N, const unsigned long int P) {
  unsigned long stride_a = N * sizeof(float);

  // Accumulatori per il blocco C (4 colonne)
  

  for (unsigned long k = 0; k < N; k++) {
    // Carica colonna k di A (4 elementi con stride N)
    asm volatile("vlse32.v v24, (%0), %1" :: "r"(a + k), "r"(stride_a));

    // Carica 4 scalari dalla riga k di B
    float b0 = b[k * P + 0];
    float b1 = b[k * P + 1];
    float b2 = b[k * P + 2];
    float b3 = b[k * P + 3];

    // Outer product: colonna(A) × scalari di B
    asm volatile("vfmacc.vf v0, %0, v24" :: "f"(b0));
    asm volatile("vfmacc.vf v4, %0, v24" :: "f"(b1));
    asm volatile("vfmacc.vf v8, %0, v24" :: "f"(b2));
    asm volatile("vfmacc.vf v12, %0, v24" :: "f"(b3));
  }

  // Scrivi il blocco 4×4 di C
  asm volatile("vse32.v v0, (%0)" :: "r"(c + 0*P));
  asm volatile("vse32.v v4, (%0)" :: "r"(c + 1*P));
  asm volatile("vse32.v v8, (%0)" :: "r"(c + 2*P));
  asm volatile("vse32.v v12, (%0)" :: "r"(c + 3*P));
}
// ============================================================================
// 8x8
// ============================================================================

void fmatmul_8x8(float *c, const float *a, const float *b,
                 const unsigned long int M, const unsigned long int N,
                 const unsigned long int P) {
  // We work on 4 rows of the matrix at once
  const unsigned long int block_size = 8;
  unsigned long int block_size_p;

  // Set the vector configuration
  asm volatile("vsetvli %0, %1, e32, m2, ta, ma" : "=r"(block_size_p) : "r"(P));

  // Slice the matrix into a manageable number of columns p_
  for (unsigned long int p = 0; p < P; p += block_size_p) {
    // Set the vector length
    const unsigned long int p_ = MIN(P - p, block_size_p);

    // Find pointers to the submatrices
    const float *b_ = b + p;
    float *c_ = c + p;

    asm volatile("vsetvli zero, %0, e32, m2, ta, ma" ::"r"(p_));

    // Iterate over the rows
    for (unsigned long int m = 0; m < M; m += block_size) {
      // Find pointer to the submatrices
      const float *a_ = a + m * N;
      float *c__ = c_ + m * P;

      fmatmul_vec_8x8_slice_init();
      fmatmul_vec_8x8(c__, a_, b_, N, P);
    }
  }
}

void fmatmul_vec_8x8(float *c, const float *a, const float *b,
                     const unsigned long int N, const unsigned long int P) {
  unsigned long stride_a = N * sizeof(float);

  for (unsigned long k = 0; k < N; k++) {
    // Carica colonna k di A (8 elementi distanziati di N)
    asm volatile("vlse32.v v24, (%0), %1" :: "r"(a + k), "r"(stride_a));

    // Carica 8 scalari dalla riga k di B
    float b0 = b[k * P + 0];
    float b1 = b[k * P + 1];
    float b2 = b[k * P + 2];
    float b3 = b[k * P + 3];
    float b4 = b[k * P + 4];
    float b5 = b[k * P + 5];
    float b6 = b[k * P + 6];
    float b7 = b[k * P + 7];

    // Outer product: colonna(A) × scalari di B
    asm volatile("vfmacc.vf v0, %0, v24"  :: "f"(b0));
    asm volatile("vfmacc.vf v2, %0, v24"  :: "f"(b1));
    asm volatile("vfmacc.vf v4, %0, v24"  :: "f"(b2));
    asm volatile("vfmacc.vf v6, %0, v24"  :: "f"(b3));
    asm volatile("vfmacc.vf v8, %0, v24"  :: "f"(b4));
    asm volatile("vfmacc.vf v10, %0, v24" :: "f"(b5));
    asm volatile("vfmacc.vf v12, %0, v24" :: "f"(b6));
    asm volatile("vfmacc.vf v14, %0, v24" :: "f"(b7));
  }

  // Scrivi il blocco 8×8 di C (righe contigue in row-major)
  asm volatile("vse32.v v0,  (%0)" :: "r"(c + 0*P));
  asm volatile("vse32.v v2,  (%0)" :: "r"(c + 1*P));
  asm volatile("vse32.v v4,  (%0)" :: "r"(c + 2*P));
  asm volatile("vse32.v v6,  (%0)" :: "r"(c + 3*P));
  asm volatile("vse32.v v8,  (%0)" :: "r"(c + 4*P));
  asm volatile("vse32.v v10, (%0)" :: "r"(c + 5*P));
  asm volatile("vse32.v v12, (%0)" :: "r"(c + 6*P));
  asm volatile("vse32.v v14, (%0)" :: "r"(c + 7*P));
}

void fmatmul_vec_8x8_slice_init() {
  asm volatile("vmv.v.i v0,  0");
  asm volatile("vmv.v.i v2,  0");
  asm volatile("vmv.v.i v4,  0");
  asm volatile("vmv.v.i v6,  0");
  asm volatile("vmv.v.i v8,  0");
  asm volatile("vmv.v.i v10, 0");
  asm volatile("vmv.v.i v12, 0");
  asm volatile("vmv.v.i v14, 0");
}





// ============================================================================
// 16x16
// ============================================================================

void fmatmul_vec_16x16_slice_init() {
  asm volatile("vmv.v.i v0,  0");
  asm volatile("vmv.v.i v1,  0");
  asm volatile("vmv.v.i v2,  0");
  asm volatile("vmv.v.i v3,  0");
  asm volatile("vmv.v.i v4,  0");
  asm volatile("vmv.v.i v5,  0");
  asm volatile("vmv.v.i v6,  0");
  asm volatile("vmv.v.i v7,  0");
  asm volatile("vmv.v.i v8,  0");
  asm volatile("vmv.v.i v9,  0");
  asm volatile("vmv.v.i v10, 0");
  asm volatile("vmv.v.i v11, 0");
  asm volatile("vmv.v.i v12, 0");
  asm volatile("vmv.v.i v13, 0");
  asm volatile("vmv.v.i v14, 0");
  asm volatile("vmv.v.i v15, 0");
}
void fmatmul_vec_16x16(float *c, const float *a, const float *b,
                       const unsigned long int N, const unsigned long int P) {
unsigned long int stride_a = N*sizeof(float);
for(int k=0; k<N; k++) {
asm volatile("vlse32.v v24, (%0), %1" :: "r"(a + k), "r"(stride_a));
float b0 = b[k * P + 0];
float b1 = b[k * P + 1];
float b2 = b[k * P + 2];
float b3 = b[k * P + 3];
float b4 = b[k * P + 4];
float b5 = b[k * P + 5];
float b6 = b[k * P + 6];
float b7 = b[k * P + 7];
float b8 = b[k * P + 8];
float b9 = b[k * P + 9];
float b10 = b[k * P + 10];
float b11 = b[k * P + 11];
float b12 = b[k * P + 12];
float b13 = b[k * P + 13];
float b14 = b[k * P + 14];
float b15 = b[k * P + 15];

    asm volatile("vfmacc.vf v0, %0, v24"  :: "f"(b0));
    asm volatile("vfmacc.vf v1, %0, v24"  :: "f"(b1));
    asm volatile("vfmacc.vf v2, %0, v24"  :: "f"(b2));
    asm volatile("vfmacc.vf v3, %0, v24"  :: "f"(b3));
    asm volatile("vfmacc.vf v4, %0, v24"  :: "f"(b4));
    asm volatile("vfmacc.vf v5, %0, v24" :: "f"(b5));
    asm volatile("vfmacc.vf v6, %0, v24" :: "f"(b6));
    asm volatile("vfmacc.vf v7, %0, v24" :: "f"(b7));
    asm volatile("vfmacc.vf v8, %0, v24"  :: "f"(b8));
    asm volatile("vfmacc.vf v9, %0, v24"  :: "f"(b9));
    asm volatile("vfmacc.vf v10, %0, v24"  :: "f"(b10));
    asm volatile("vfmacc.vf v11, %0, v24"  :: "f"(b11));
    asm volatile("vfmacc.vf v12, %0, v24"  :: "f"(b12));
    asm volatile("vfmacc.vf v13, %0, v24" :: "f"(b13));
    asm volatile("vfmacc.vf v14, %0, v24" :: "f"(b14));
    asm volatile("vfmacc.vf v15, %0, v24" :: "f"(b15));
}

  asm volatile("vse32.v v0,  (%0)" :: "r"(c + 0*P));
  asm volatile("vse32.v v1,  (%0)" :: "r"(c + 1*P));
  asm volatile("vse32.v v2,  (%0)" :: "r"(c + 2*P));
  asm volatile("vse32.v v3,  (%0)" :: "r"(c + 3*P));
  asm volatile("vse32.v v4,  (%0)" :: "r"(c + 4*P));
  asm volatile("vse32.v v5, (%0)" :: "r"(c + 5*P));
  asm volatile("vse32.v v6, (%0)" :: "r"(c + 6*P));
  asm volatile("vse32.v v7, (%0)" :: "r"(c + 7*P));
  asm volatile("vse32.v v8,  (%0)" :: "r"(c + 8*P));
  asm volatile("vse32.v v9,  (%0)" :: "r"(c + 9*P));
  asm volatile("vse32.v v10,  (%0)" :: "r"(c + 10*P));
  asm volatile("vse32.v v11,  (%0)" :: "r"(c + 11*P));
  asm volatile("vse32.v v12,  (%0)" :: "r"(c + 12*P));
  asm volatile("vse32.v v13, (%0)" :: "r"(c + 13*P));
  asm volatile("vse32.v v14, (%0)" :: "r"(c + 14*P));
  asm volatile("vse32.v v15, (%0)" :: "r"(c + 15*P));
    
}


void fmatmul_16x16(float *c, const float *a, const float *b,
                   unsigned long int M, unsigned long int N,
                   unsigned long int P) {
  // We work on 4 rows of the matrix at once
  const unsigned long int block_size = 16;
  unsigned long int block_size_p;

  // Set the vector configuration
  asm volatile("vsetvli %0, %1, e32, m1, ta, ma" : "=r"(block_size_p) : "r"(P));

  // Slice the matrix into a manageable number of columns p_
  for (unsigned long int p = 0; p < P; p += block_size_p) {
    // Set the vector length
    const unsigned long int p_ = MIN(P - p, block_size_p);

    // Find pointers to the submatrices
    const float *b_ = b + p;
    float *c_ = c + p;

    asm volatile("vsetvli zero, %0, e32, m1, ta, ma" ::"r"(p_));

    // Iterate over the rows
    for (unsigned long int m = 0; m < M; m += block_size) {
      // Find pointer to the submatrices
      const float *a_ = a + m * N;
      float *c__ = c_ + m * P;

      fmatmul_vec_16x16_slice_init();
      fmatmul_vec_16x16(c__, a_, b_, N, P);
    }
  }
}



