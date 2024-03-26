# RISC-V Hands-on

## Verified setup
This tree has been verified with:
* Ubuntu 22.04 LTS
* Java JRE 11
* GCC 11.4.0

## Project Structure
This tree is structured as follows:
``` 
├── common
│   └── dot_prod_asm.s      # Common source file
├── RARS                    
│   ├── rars1_6.jar         # RARS JAR
│   ├── README.md           # Documentation
│   └── src                 
│       ├── data.s          # Static data definition
│       ├── main.s          # Assembly Main program calling subroutines
│       └── print_int.s     # Utility function
├── README.md               # This file
└── spike               
    ├── install_spike.sh    # Install script for GCC, Spike and PK
    ├── Makefile            # Makefile for building and running the example on spike
    ├── settings.sh         # Environment setup script
    └── src         
        ├── dot_prod_asm.h  # Header defining the subroutine prototypes
        ├── dot_prod.c      # C implementation
        └── main.c          # C Main program calling subroutines
```

## Running the examples
Move to [RARS](RARS) or [spike](spike) subdirectory to run the related examples.
