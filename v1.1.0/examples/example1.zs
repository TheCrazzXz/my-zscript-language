//
	New feature added in V1.1.0 : Encloser lists and functions with multiple parameters
//

string newstr = ""
int lena = 0
int lenb = 0
int i = 0
defunc concat_strings stra strb
	newstr = stra
	lena = len(stra)
	lenb = len(strb)
	for i = 0 ; i<lenb ; i += 1
		newstr[ i+lena ] = strb[i]
	endloop
	return newstr
endef
concat_strings("bonjour","aurevoir")
