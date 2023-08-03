auto file = openfile("test.txt","w")
writefile(file,"hello")
closefile(file)

file = openfile("test.txt","r")
print "Read from file the string :" readfile(file,5)
closefile(file)
