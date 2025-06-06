
########
# Data #
########

# Statically define our data
.global vec1
.global vec2
.global CR_string

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

# for print
CR_string:
	.string "%d\n"

