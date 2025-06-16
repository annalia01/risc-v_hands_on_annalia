# Spike 

## Installation
Just source the install script, but also take a look to se what it is doing.
``` console
$ source install_spike.sh
```
> NOTE: The script also clones and builds GCC for RISC-V, which might take a while.

This script will clone the sources in a new `downloads` directory and install them in the user home under `~/RISC-V`.

## Setup the environment
After installation and for each session, you must setup your environment
``` console
$ source settings.sh
```
> NOTE: This script assumes the same installation path as `install_spike.sh`

## Run the example
Run the spike binary:
``` console
$ make run_spike
```

Run the following to see the other make targets:
``` console
$ make help
```

