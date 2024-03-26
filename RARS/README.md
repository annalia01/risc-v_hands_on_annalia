# Run dot_prod_asm example on RARS

Download RARS from GitHub releases
``` console
$ wget https://github.com/TheThirdOne/rars/releases/download/v1.6/rars1_6.jar
```

Install Java SDK or JRE, then start RARS GUI with:
``` console
$ java -jar rars1_6.jar 
```

Necessary RARS GUI settings for this example:
* Uncheck Settings > 64 bit
* Check Settings > Initialize Program Counter to global ‘main’ if defined
* Check Settings > Assemble all files currently open

Open and explore the following project files:
```
src/main.s
src/print_int.s
src/data.s
../common/dot_prod_asm.s
```

Assemble and run from GUI.

## Reference
For more, refere to [RARS wiki](https://github.com/TheThirdOne/rars/wiki/).