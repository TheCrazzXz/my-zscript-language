# ZSCRIPT v1.1.0

New features added :
- Functions can now have multiple parameters :
	```
	defunc hello a b c d e
		print a	b c d e
	endef

	hello(1,2,3,4,0-1)
	```

- It's now possible to create a vector with elements directly in it (brace encloser list) :
	```
	vector vec = {"hello this",{"is the new feature"},"in version V",1.1,".0}
	```

- `auto` keyword :
	```
	auto variable = 5
	```

- File management :
	```
	auto file = openfile("hello.txt","w")
	writefile(file,"hey !")
	closefile(file)
	```
This version V1.1.0 is released the 3 of August, 2023
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
