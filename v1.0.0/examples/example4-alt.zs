//
	same as example4, just to show that the crack keyword works
//

int i = 0
int j = 0
int k = 0
print "Rectangle builder"

printr "Enter size in X of the rectangle to be drawn : "
int sizex = readint*2

printr "Enter size in Y of the rectangle to be drawn : "
int sizey = readint

loop i<sizey
	j = 0

	if i==3
		print "stopping"
		crack
	endif

	loop j<sizex
		k = 0
		loop k<10
			printr "-"
			k = k+1
		endloop
		j = j+1
	endloop
	print ""
	i = i+1
endloop

