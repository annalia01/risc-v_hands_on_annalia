
########
# Data #
########

# Statically define our data
.global vec1
.global vec2
.global format_string

.data

# Allocate a word vector with .word macro
vec1:
	.word 1
	.word 5
	.word 3
	.word 4
	.word 5

# Allocate a word vector with .word macro
vec2:
	.word 1
	.word 2
	.word 3
	.word 4
	.word 5

# Format for printf_ function
format_string:
	.string "%d\r\n"

