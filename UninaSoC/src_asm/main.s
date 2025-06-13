########
# Main #
########


# Constant size
# NOTE: this must be visible in this file

.eqv    SIZE, 5

	.text
	.align 2
	.global main

# We' re using the "printf_" function defined in the tinyio.a library.
# In order to correctly setup the library we need to specify the UART
# memory-mapped base address in the "tinyIO_init" function.
# So here we're refering to "_peripheral_UART_start" that is a symbol
# (defined in the linker script UninaSoC.ld) which is placed in the specified address
# during linking phase
# (_peripheral_UART_start = 0x0000000000020000;)

    .extern _peripheral_UART_start
    .extern format_string

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

    # init-TinyIO
    la a0, _peripheral_UART_start 
    jal tinyIO_init
	
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
    # printf_("%d\n", dest)
    la  a0, format_string
    lw	a1, 4(sp)
    jal printf_
	
	# Return sequence (return 0)
	
	# Restore return address
	lw	ra, 0(sp)
	# Restore stack pointer
	addi	sp, sp, 8
	
	# return 0
	li	a0, 0
	ret			# Should call ret here...
