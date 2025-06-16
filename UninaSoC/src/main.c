// substitute stdio with custom printf library
#include "tinyIO.h"

// #include <stdio.h>

// can't use rand or assert anymore (we're compiling targeting bare metal without std libraries)
// #include <stdlib.h> // for rand()
// #include <assert.h> // for assert()

#include "dot_prod_asm.h"
#include <stdint.h> // for uint32_t

// UART base address defined in UninaSoC.ld
// note that TinyIO define all the UART parameters as volatile
// in order to avoid compiler optimizations on load/store operations
// so it shouldn't be necessary to define "_peripheral_UART_start" as volatile
// WARNING:
// note the subtle difference:
// - in assembly "_peripheral_UART_start" is interpreted as an address, so 
// la a0, _peripheral_UART_start effectively becames
// la a0, 0x0000000000020000
// - in C "_peripheral_UART_start" is interpreted as the variable that is placed
// at the address 0x0000000000020000
// so to get "0x0000000000020000" you must type &_peripheral_UART_start
extern volatile uint32_t _peripheral_UART_start;

int main(){
    
    // initialize tinyIO for printf usage
    tinyIO_init((uint32_t) &_peripheral_UART_start);
	
	int vec1 [SIZE];
	int vec2 [SIZE];
	int dest;
	int dest_asm;

	int return_val = 0;

	// Seed PRNG
	// srand(0);

	printf("[INFO] Initilaizing vectors...\r\n");
	for ( unsigned int i = 0; i < SIZE; i++ ) {
		vec1[i] = i+1; //rand();
		vec2[i] = i+1; //rand();
	}

	printf("[INFO] vec1 = { ");
	for ( unsigned int i = 0; i < SIZE; i++ ) {
		printf("%d ", vec1[i]);
	}
	printf("}\r\n");

	printf("[INFO] vec2 = { ");
	for ( unsigned int i = 0; i < SIZE; i++ ) {
		printf("%d ", vec2[i]);
	}
	printf("}\r\n");


	// Reset output
	dest = 0;

	printf("[INFO] Calling dot_prod...\r\n");
	return_val = dot_prod(vec1, vec2, &dest, SIZE);
	printf("[INFO] Result of dot_prod:\t%d\r\n", dest);
	
	// Reset output
	dest_asm = 0;

	printf("[INFO] Calling dot_prod_asm...\r\n");
	return_val = dot_prod_asm(vec1, vec2, &dest_asm, SIZE);
	printf("[INFO] Result of dot_prod_asm:\t%d\r\n", dest_asm);

	// Compare results
	// assert( dest == dest_asm );

    while(1);
	//return 0;
}
