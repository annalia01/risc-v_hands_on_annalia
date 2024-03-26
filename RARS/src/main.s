########
# Main #
########

# Constant size
# NOTE: this must be visible in this file
.eqv    SIZE, 5

	.text
	.align 2
	.global main

main:
	# Remember, we want to call this function:
	# void dot_prod_asm (int* vec1, int* vec2, int* dest, int size);
	
	# Stack handling:
	# 1. Assuming this main has been called by someone, we must save
	# 	our return value on the stack
	# 2. The third argument of dot_prod_asm, i.e. dest, has to be passed
	#	by reference, therefore we need to store it somewhere
	# Hence, we need space for 2 dwords on the stack
	
	addi	sp, sp, -8	# Make space on the stack for two words
				# NOTE: the stack grows towards lesser addresses
	sw	ra, 0(sp)	# At offset 0, store ra
	sw	zero, 4(sp)	# At offset 4, store dest
				# NOTE: this is equivalent to: 
				#	int dest = 0;
	
	# Load parameter registers
	la	a0, vec1	# Load address of vec1
	la	a1, vec2	# Load address of vec1
	li	a3, SIZE	# Load constant size value
	# Load address of dest
	mv	t0, sp		# Copy the stack pointer value
	addi	t0, t0, 4	# Add in the offset of dest
	mv	a2, t0		# Copy this address to a2	
	
	# Call to subroutine
	jal	dot_prod_asm
	
	# Check return value in a0-a1
	# - Not necessay for us
	
	# Print out result (dest)
	lw	a0, 4(sp)
	jal	print_int
	
	# Return sequence (return 0)
	
	# Restore return address
	lw	ra, 0(sp)
	# Restore stack pointer
	addi	sp, sp, 8
	
	# return 0
	li	a0, 0
	ret			# Should call ret here...

	# NOTE: since there is no actual caller, calling ret here woulw jump 
	#	to garbage is in ra
	
