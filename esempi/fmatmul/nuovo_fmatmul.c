//innanzitutto usiamo un solo kernel invece di 3 kernel diversi e usiamo restrict perché il compilatore potrebbe pensare che le matrici di input e output potrebbero 
//sovrapporsi e quindi operare in maniera sequenziale
// A=MxN, B=NxP
void fmatmul(float * restrict c,
             const float * restrict a,
             const float * restrict b,
             unsigned long M, unsigned long N, unsigned long P)
{
    const unsigned long Mc = 16;      // definiamo le righe che consideriamo in un solo colpo che è sempre 16
    unsigned long Pc_max;             // ci serve per calcolare VL 

    //per scegliere VL si fa il minimo tra VLMAX e P. VLMAX in  questo caso è 128/32x1, quindi 4 colonne alla volta, anche se P fosse più grande, per come è stata impostata la vsetvli
    asm volatile("vsetvli %0, %1, e32, m1, ta, ma" : "=r"(Pc_max) : "r"(P));

    for (unsigned long p = 0; p < P; p += Pc_max) { //ci spostiamo ogni volta di 4 colonne, quindi prima lavoriamo sulle prime 4
        unsigned long Pc = (P - p < Pc_max) ? (P - p) : Pc_max; //serve per fare un controllo, ovvero se sono rimaste meno di VL(che sarebbe Pc_max) colonne da
      //caricare, allora considera solo quelle che sono rimaste, quindi P-p, dove p è il puntatore  alla colonna dove ci troviamo, altrimenti premdi Pc_max
        // Una sola vsetvli per pannello (gestisce anche il tail)
        asm volatile("vsetvli zero, %0, e32, m1, ta, ma" :: "r"(Pc)); //ora si usa Pc per definire su quanti valori dobbiamo lavorare contemporaneamente

        const float *b_panel = b + p; //definiamo il puntatore alla matrice b 
        float *c_panel = c + p; //definiamo il puntatore alla matrice c

        for (unsigned long m = 0; m < M; m += Mc) { //mi sposto di 16 righe alla volta
            const float *a_blk = a + m * N; //vuol dire che ogni volta mi sposto di 16 righe sotto 
            float *c_blk = c_panel + m * P;

            fkm16xVL_slice_init();     // azzera gli accumulatori v0..v15
            fkm16xVL(c_blk, a_blk, b_panel, N, P); // kernel 16×VL
        }
    }
}
