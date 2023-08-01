string rv_string = ""
int i = 0
int len = 0
defunc reverse_string str
	rv_string = ""
	len = len(str)
	for i = 0 ; i<len ; i += 1
		rv_string[ i ] = str[len-i-1]
	endloop
	return rv_string
endef

print "-- super string reverser --"

string reverse = ""
printr "Enter string : "
string str = readstr
reverse = reverse_string(str)
print "[" str "] in reverse is" "[" reverse "]"
 
if reverse==str
	print "They are the same !"
else
	print "They are not the same"
endif
