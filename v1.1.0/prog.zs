int i = 0
string str = ""
defunc random_string_seq len min max
	for i = 0 ; i<len ; i += 1
		str[ i ] = char(randint(min,max))
	endloop
	return str
endef

printr "Enter length : "
int length = readint

print random_string_seq(length,65,65+25)
