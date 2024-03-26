
########
# Data #
########

# Utility macro to allocate double words from 2 32-bit words
.macro dword(%value_high, %value_low)
	.word %value_low
	.word %value_high
.end_macro

# E.g.
	#dword(0,1)
	#dword(0,2)
	#dword(0,3)
	#dword(0,4)
	#dword(0,5)
	
# Statically define our data
.global vec1
.global vec2

	.data
	
# Allocate a double words vector with dword macro
vec1:
	.word 1
	.word 2
	.word 3
	.word 4
	.word 5

# Allocate a double words vector with basic directives
vec2:	
	.word 1
	.word 2
	.word 3
	.word 4
	.word 5
