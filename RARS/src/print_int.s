# Prototype: void print_int(int val);
# Parameters: 
#	- int val
# Return value:
# 	- None
# Parameters registers:
# a0 = val

	.data
CR_string:
	.string "\n"
	
	.text
	.align 2
	.global print_int

print_int:
	# NOTE: a0, a7 are caller-saved, no need to save and restore them
	
	# Print content of a0 as int
	li a7, 1
	ecall 
	
	# Print \n
    li a7, 4
    la a0, CR_string
    ecall
    
    # Return to caller
    li	a0, 0
	ret