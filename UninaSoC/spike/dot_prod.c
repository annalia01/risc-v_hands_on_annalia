#include "dot_prod_asm.h"

int dot_prod(int* vec1, int* vec2, int* dest, int size){
	
	// Reset the accumulator
	*dest = 0;

	// Mupltiply and accumulate
	for ( unsigned int i = 0; i < size; i++ ){
		*dest += vec1[i] * vec2[i];
	}

	return 0;
}
