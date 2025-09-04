// SPDX-License-Identifier: Apache-2.0
// Unico GEMM RVV: kernel 16×VL, packing A/B, niente dispatcher multi-kernel.

#include "fmatmul.h"
#include <stdlib.h>
#include <string.h>

#define MIN(a,b) ((a)<(b)?(a):(b))

//------------------------------------------------------------------------------
// Packing B: pannello Kc × Pc contiguo (unit-stride), riusabile su tutto Mc
//------------------------------------------------------------------------------
static inline float* pack_B_panel_kc(const float* B,
                                     unsigned long N,  // = K totale
                                     unsigned long P,  // ldB originale
                                     unsigned long k0, unsigned long Kc,
                                     unsigned long p0, unsigned long Pc)
{
  float* Bp;
#if defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE >= 200112L
  if (posix_memalign((void**)&Bp, 64, Kc*Pc*sizeof(float)) != 0) return NULL;
#else
  Bp = (float*)malloc(Kc*Pc*sizeof(float));
#endif
  if (!Bp) return NULL;

  for (unsigned long kk = 0; kk < Kc; ++kk) {
    const float* src = B + (k0 + kk)*P + p0; // B[k0+kk][p0 .. p0+Pc-1]
    float*       dst = Bp + kk*Pc;           // riga kk contigua in Bp
    memcpy(dst, src, Pc*sizeof(float));
  }
  return Bp;
}

//------------------------------------------------------------------------------
// Packing A: blocco 16 × Kc contiguo, con zero-padding sulle righe mancanti
//------------------------------------------------------------------------------
static inline float* pack_A_panel_16xkc(const float* A,
                                        unsigned long M,  // per calcolo Mr
                                        unsigned long N,  // ldA originale
                                        unsigned long m0, unsigned long Mr, // <=16
                                        unsigned long k0, unsigned long Kc)
{
  float* Ap;
#if defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE >= 200112L
  if (posix_memalign((void**)&Ap, 64, 16*Kc*sizeof(float)) != 0) return NULL;
#else
  Ap = (float*)malloc(16*Kc*sizeof(float));
#endif
  if (!Ap) return NULL;

  // copia le Mr righe reali
  for (unsigned long r = 0; r < Mr; ++r) {
    const float* src = A + (m0 + r)*N + k0; // A[m0+r][k0 .. k0+Kc-1]
    float*       dst = Ap + r*Kc;           // riga r contigua in Ap
    memcpy(dst, src, Kc*sizeof(float));
  }
  // zero-pad le righe finte
  for (unsigned long r = Mr; r < 16; ++r) {
    float* dst = Ap + r*Kc;
    memset(dst, 0, Kc*sizeof(float));
  }
  return Ap;
}

//------------------------------------------------------------------------------
// Micro-kernel 16×VL su Kc: accumula in v0..v15, legge A packata (ldA=Kc),
// B packata (ldB=Pc), C originale (ldC=P).
// NOTE: nessun vsetvli qui dentro; usa VL impostata dal chiamante.
//------------------------------------------------------------------------------
static inline void fkm_16xVL_slice_init(void) {
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

// Ap: 16×Kc contiguo (ldAp = Kc)
// Bp: Kc×Pc  contiguo (ldBp = Pc)
static inline void fkm_16xVL(float* C,       unsigned long ldC, // = P
                             const float* Ap,unsigned long ldAp,// = Kc
                             const float* Bp,unsigned long ldBp,// = Pc
                             unsigned long Kc,
                             unsigned long Mr)                  // righe reali (<=16)
{
  // scalari A delle 16 righe (pipeline semplice)
  float t0,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15;

  // Prefetch prima "riga" vettoriale di B
  const float* bp = Bp;
  asm volatile("vle32.v v16, (%0);" ::"r"(bp));

  // puntatori riga di A packata
  const float* a0  = Ap + 0*ldAp;
  const float* a1  = Ap + 1*ldAp;
  const float* a2  = Ap + 2*ldAp;
  const float* a3  = Ap + 3*ldAp;
  const float* a4  = Ap + 4*ldAp;
  const float* a5  = Ap + 5*ldAp;
  const float* a6  = Ap + 6*ldAp;
  const float* a7  = Ap + 7*ldAp;
  const float* a8  = Ap + 8*ldAp;
  const float* a9  = Ap + 9*ldAp;
  const float* a10 = Ap +10*ldAp;
  const float* a11 = Ap +11*ldAp;
  const float* a12 = Ap +12*ldAp;
  const float* a13 = Ap +13*ldAp;
  const float* a14 = Ap +14*ldAp;
  const float* a15 = Ap +15*ldAp;

  // carica primo elemento di ciascuna riga (k=0)
  t0=*a0++; t1=*a1++; t2=*a2++; t3=*a3++; t4=*a4++; t5=*a5++; t6=*a6++; t7=*a7++;
  t8=*a8++; t9=*a9++; t10=*a10++; t11=*a11++; t12=*a12++; t13=*a13++; t14=*a14++; t15=*a15++;

  unsigned long k = 1;            // abbiamo già usato k=0 con v16
  const float* bnext = bp + ldBp; // riga successiva in Bp

  while (k != Kc) {
    // FMA con v16
    asm volatile("vfmacc.vf v0,  %0, v16" ::"f"(t0));
    asm volatile("vfmacc.vf v1,  %0, v16" ::"f"(t1));
    asm volatile("vfmacc.vf v2,  %0, v16" ::"f"(t2));
    asm volatile("vfmacc.vf v3,  %0, v16" ::"f"(t3));
    asm volatile("vfmacc.vf v4,  %0, v16" ::"f"(t4));
    asm volatile("vfmacc.vf v5,  %0, v16" ::"f"(t5));
    asm volatile("vfmacc.vf v6,  %0, v16" ::"f"(t6));
    asm volatile("vfmacc.vf v7,  %0, v16" ::"f"(t7));
    asm volatile("vfmacc.vf v8,  %0, v16" ::"f"(t8));
    asm volatile("vfmacc.vf v9,  %0, v16" ::"f"(t9));
    asm volatile("vfmacc.vf v10, %0, v16" ::"f"(t10));
    asm volatile("vfmacc.vf v11, %0, v16" ::"f"(t11));
    asm volatile("vfmacc.vf v12, %0, v16" ::"f"(t12));
    asm volatile("vfmacc.vf v13, %0, v16" ::"f"(t13));
    asm volatile("vfmacc.vf v14, %0, v16" ::"f"(t14));
    asm volatile("vfmacc.vf v15, %0, v16" ::"f"(t15));

    // pre-carica la prossima riga vettoriale di B in v17
    asm volatile("vle32.v v17, (%0);" ::"r"(bnext));

    // carica i prossimi scalari di A (k)
    t0=*a0++; t1=*a1++; t2=*a2++; t3=*a3++; t4=*a4++; t5=*a5++; t6=*a6++; t7=*a7++;
    t8=*a8++; t9=*a9++; t10=*a10++; t11=*a11++; t12=*a12++; t13=*a13++; t14=*a14++; t15=*a15++;

    ++k;
    if (k == Kc) { // epilogo: usa v17 e termina
      asm volatile("vfmacc.vf v0,  %0, v17" ::"f"(t0));
      asm volatile("vfmacc.vf v1,  %0, v17" ::"f"(t1));
      asm volatile("vfmacc.vf v2,  %0, v17" ::"f"(t2));
      asm volatile("vfmacc.vf v3,  %0, v17" ::"f"(t3));
      asm volatile("vfmacc.vf v4,  %0, v17" ::"f"(t4));
      asm volatile("vfmacc.vf v5,  %0, v17" ::"f"(t5));
      asm volatile("vfmacc.vf v6,  %0, v17" ::"f"(t6));
      asm volatile("vfmacc.vf v7,  %0, v17" ::"f"(t7));
      asm volatile("vfmacc.vf v8,  %0, v17" ::"f"(t8));
      asm volatile("vfmacc.vf v9,  %0, v17" ::"f"(t9));
      asm volatile("vfmacc.vf v10, %0, v17" ::"f"(t10));
      asm volatile("vfmacc.vf v11, %0, v17" ::"f"(t11));
      asm volatile("vfmacc.vf v12, %0, v17" ::"f"(t12));
      asm volatile("vfmacc.vf v13, %0, v17" ::"f"(t13));
      asm volatile("vfmacc.vf v14, %0, v17" ::"f"(t14));
      asm volatile("vfmacc.vf v15, %0, v17" ::"f"(t15));
      break;
    }

    // swap: v17 diventa corrente (v16) per la prossima iterazione
    asm volatile("vmv.v.v v16, v17");
    bp    = bnext;
    bnext = bp + ldBp;
  }

  // store delle 16 righe — SOLO quelle reali (Mr)
  float* cptr = C;
  if (Mr > 0) { asm volatile("vse32.v v0,  (%0)" ::"r"(cptr)); } cptr += ldC;
  if (Mr > 1) { asm volatile("vse32.v v1,  (%0)" ::"r"(cptr)); } cptr += ldC;
  if (Mr > 2) { asm volatile("vse32.v v2,  (%0)" ::"r"(cptr)); } cptr += ldC;
  if (Mr > 3) { asm volatile("vse32.v v3,  (%0)" ::"r"(cptr)); } cptr += ldC;
  if (Mr > 4) { asm volatile("vse32.v v4,  (%0)" ::"r"(cptr)); } cptr += ldC;
  if (Mr > 5) { asm volatile("vse32.v v5,  (%0)" ::"r"(cptr)); } cptr += ldC;
  if (Mr > 6) { asm volatile("vse32.v v6,  (%0)" ::"r"(cptr)); } cptr += ldC;
  if (Mr > 7) { asm volatile("vse32.v v7,  (%0)" ::"r"(cptr)); } cptr += ldC;
  if (Mr > 8) { asm volatile("vse32.v v8,  (%0)" ::"r"(cptr)); } cptr += ldC;
  if (Mr > 9) { asm volatile("vse32.v v9,  (%0)" ::"r"(cptr)); } cptr += ldC;
  if (Mr >10) { asm volatile("vse32.v v10, (%0)" ::"r"(cptr)); } cptr += ldC;
  if (Mr >11) { asm volatile("vse32.v v11, (%0)" ::"r"(cptr)); } cptr += ldC;
  if (Mr >12) { asm volatile("vse32.v v12, (%0)" ::"r"(cptr)); } cptr += ldC;
  if (Mr >13) { asm volatile("vse32.v v13, (%0)" ::"r"(cptr)); } cptr += ldC;
  if (Mr >14) { asm volatile("vse32.v v14, (%0)" ::"r"(cptr)); } cptr += ldC;
  if (Mr >15) { asm volatile("vse32.v v15, (%0)" ::"r"(cptr)); }
}

//------------------------------------------------------------------------------
// fmatmul: unico ingresso. Loop a blocchi su P (Pc) e su K (Kc).
// - vsetvli: una volta per pannello di colonne (Pc)
// - packing B su Kc × Pc
// - packing A su 16 × Kc con padding
//------------------------------------------------------------------------------
void fmatmul(float *c, const float *a, const float *b,
             unsigned long M, unsigned long N, unsigned long P)
{
  // Scelgo Pc in base a VL (LMUL=1). Una sola vsetvli “max” per sapere VL.
  unsigned long Pc_max;
  asm volatile("vsetvli %0, %1, e32, m1, ta, ma" : "=r"(Pc_max) : "r"(P));

  // Parametri di blocking “ragionevoli” (poi puoi afinarli)
  const unsigned long Mc = 16;                  // kernel fisso su 16 righe
  const unsigned long Kc = 128;                 // spessore su K (N) per cache
  // buffer packati (ri-allocati ai pannelli reali se tail)
  // (li allochiamo pannello per pannello così non servono dimensioni max globali)

  for (unsigned long p0 = 0; p0 < P; p0 += Pc_max) {
    unsigned long Pc = MIN(P - p0, Pc_max);
    // imposta la VL esatta del pannello (gestisce anche il tail)
    asm volatile("vsetvli zero, %0, e32, m1, ta, ma" ::"r"(Pc));

    for (unsigned long k0 = 0; k0 < N; k0 += Kc) {
      unsigned long Kc_ = MIN(N - k0, Kc);

      // Pack B: pannello Kc_ × Pc
      float* Bp = pack_B_panel_kc(b, N, P, k0, Kc_, p0, Pc);
      if (!Bp) return;

      for (unsigned long m0 = 0; m0 < M; m0 += Mc) {
        unsigned long Mr = MIN(M - m0, Mc);

        // Pack A: blocco 16 × Kc_ (con zero-padding se Mr<16)
        float* Ap = pack_A_panel_16xkc(a, M, N, m0, Mr, k0, Kc_);
        if (!Ap) { free(Bp); return; }

        // Accumula su C[m0..m0+Mr-1, p0..p0+Pc-1]
        fkm_16xVL_slice_init();
        fkm_16xVL(/*C*/  c + m0*P + p0, P,
                  /*Ap*/ Ap, Kc_,
                  /*Bp*/ Bp, Pc,
                  /*Kc*/ Kc_,
                  /*Mr*/ Mr);

        free(Ap);
      }
      free(Bp);
    }
  }
}
