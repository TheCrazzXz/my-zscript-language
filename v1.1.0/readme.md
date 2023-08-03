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
