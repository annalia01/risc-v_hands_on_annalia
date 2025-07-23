#include <stdint.h>
#include "runtime.h"
#define NR_LANES 8
// Definizione delle dimensioni delle matrici
uint64_t M = 128;
uint64_t N = 128;
uint64_t P = 128;
volatile int fake_uart=0;
// Matrici allocate come array piatti (row-major)
double a[128 * 128] __attribute__((aligned(32 * NR_LANES), section(".l2")));
double b[128 * 128] __attribute__((aligned(32 * NR_LANES), section(".l2")));
double c[128 * 128] __attribute__((aligned(32 * NR_LANES), section(".l2")));
double g[128 * 128] __attribute__((aligned(32 * NR_LANES), section(".l2")));

// Funzione opzionale da richiamare nel main per inizializzare i dati
void init_dataset() {
  for (uint64_t i = 0; i < M; ++i) {
    for (uint64_t j = 0; j < N; ++j) {
      a[i * N + j] = (double)(i + j);
    }
  }

  for (uint64_t i = 0; i < N; ++i) {
    for (uint64_t j = 0; j < P; ++j) {
      b[i * P + j] = (double)(i == j);  // matrice identità
    }
  }

  // c inizialmente a zero
  for (uint64_t i = 0; i < M * P; ++i) {
    c[i] = 0.0;
    g[i] = 0.0;
  }

  // g = a × b (matrici piane)
  for (uint64_t i = 0; i < M; ++i) {
    for (uint64_t j = 0; j < P; ++j) {
      for (uint64_t k = 0; k < N; ++k) {
        g[i * P + j] += a[i * N + k] * b[k * P + j];
      }
    }
  }
}
