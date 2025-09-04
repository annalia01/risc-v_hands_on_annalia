// Copyright 2020 ETH Zurich and University of Bologna.
// SPDX-License-Identifier: Apache-2.0
//
// Header semplificato per l’implementazione unica di GEMM RVV:
// - Espone solo fmatmul() (API pubblica).
// - Rimuove i vecchi prototipi 4x4/8x8/16x16 e relativi micro-kernel.
// - Protegge event_trigger dietro VCD_DUMP per evitare errori di link.

#ifndef FMATMUL_H
#define FMATMUL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Matrici in row-major:
// C[M x P] = A[M x N] * B[N x P]
void fmatmul(float *c,
             const float *a,
             const float *b,
             unsigned long m,   // M: righe di A e C
             unsigned long n,   // N: colonne di A / righe di B
             unsigned long p);  // P: colonne di B e C

#define DELTA 0.000001

// Esportato solo se si abilita il dump VCD in compilazione.
#ifdef VCD_DUMP
extern int64_t event_trigger;
#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // FMATMUL_H
