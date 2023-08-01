# ZSCRIPT v1.0.0

Zscript is an open source interpreted programming language that supports functionnal paradigm.  
The V1 supports expressions, variables, vectors (lists), conditionnal statements, loops and functions.  

Future versions would possibly include :  
- multi-parameter functions (for now it's only one)
- modules (coding from different files)
- file management (reading, writing from files)
- sockets (communicating through the network)

The language is written in C langugage, a standard functionnal language that's low-level.  
All the source code can be found in src/.  
To compile the program, simply use build.sh or compile.sh bin/zscript from this folder.  
To compile it in another os than linux, you can just use the same gcc command : gcc ...src/... -lm -o bin/zscript.  
To use the interpreter, there are two ways, either you can use it directly without a file and writting code : just launch zscript executable.  
And you can of course use a file where the script lies, example : bin/zscript examples/example4.zs.  

zs is the standard extension for zscript files.  
 
Author : TheCrazzXz / PainHandler
