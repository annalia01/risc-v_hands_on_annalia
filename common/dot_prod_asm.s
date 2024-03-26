###############
# Subroutines #
###############

# Prototype: 
# 	void dot_prod_asm( int * vec1, int * vec2, int* dest, int size);
#
# Parameters:
# 	- vec1 and vec2: pointers of two arrays which do not overlap in memory
# 	- dest: pointer to the buffer which will holw the result
# 	- size: the length, in elements, of the two arrays
#
# Return value: 
#	- None
# 
# Equivalent C syntax:
#	*dest = 0;
#	for(unsigned int i = 0; i < size; i++){
#		*dest += vec1[i] * vec2[i];
#	}
#
# Parameters registers:
# a0 = vec1
# a1 = vec2 
# a2 = dest
# a3 = size
#

  	.text
	.align 2
	.global	dot_prod_asm

dot_prod_asm:

	# Move caller context to stack
	# - Not necessary
	# NOTE: here we are being smart by not using any calle-saved register, 
	# 	so we don't need to push them on the stack
	# NOTE: moreover, we don't need to jump to any other function, so we don't 
	#	need to save our return address
	
	# Init
	li			t0, 0				# Reset counter t0
	li			t4, 0				# Reset accumlator t4

loop:
	# Load values
	lw			t1, (a0)			# Load elment from vec1
	lw 			t2, (a1)			# Load elment from vec2
	
	# Multiply
	mul			t3, t2, t1			# t3 = t2 * t1
	# Accumulate
	add			t4, t4, t3			# t4 = t4 + t3

	# Bump pointers of sizeof(int)
	addi 			a0, a0, 4			# a0 = a0 + 4	
	addi 			a1, a1, 4			# a1 = a1 + 4	

	# Increment counter
	addi			t0, t0, 1			# t0 = t0 + imm

	# Branch back if (counter != size)
	bne			t0, a3, loop

	# Store back
	sw			t4, 0(a2)			# Store the accumulated result in dest
	
	# Return sequence
	
	# Restore stack
	# - Not necessary for the same reasons as before
	
	# Load return value
	li			a0, 0				# Load return register
	ret							# Return to caller

	
	















