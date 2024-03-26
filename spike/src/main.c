#include <stdio.h>	// for printf()
#include <stdlib.h> // for rand()
#include <assert.h> // for assert()

#include "dot_prod_asm.h"

int main(){
	
	int vec1 [SIZE];
	int vec2 [SIZE];
	int dest;
	int dest_asm;

	int return_val = 0;

	// Seed PRNG
	srand(0);

	printf("[INFO] Initilaizing vectors...\n");
	for ( unsigned int i = 0; i < SIZE; i++ ) {
		vec1[i] = i+1; //rand();
		vec2[i] = i+1; //rand();
	}

	printf("[INFO] vec1 = { ");
	for ( unsigned int i = 0; i < SIZE; i++ ) {
		printf("%d ", vec1[i]);
	}
	printf("}\n");

	printf("[INFO] vec2 = { ");
	for ( unsigned int i = 0; i < SIZE; i++ ) {
		printf("%d ", vec2[i]);
	}
	printf("}\n");


	// Reset output
	dest = 0;

	printf("[INFO] Calling dot_prod...\n");
	return_val = dot_prod(vec1, vec2, &dest, SIZE);
	printf("[INFO] Result of dot_prod:\t%d\n", dest);
	
	// Reset output
	dest_asm = 0;

	printf("[INFO] Calling dot_prod_asm...\n");
	return_val = dot_prod_asm(vec1, vec2, &dest_asm, SIZE);
	printf("[INFO] Result of dot_prod_asm:\t%d\n", dest_asm);

	// Compare results
	assert( dest == dest_asm );

	printf("[INFO] Results match!!\n");
	
	return 0;
}
