//

	Rectangle Builder example : Uses loops

//

int i = 0
int j = 0

print "Rectangle builder"

printr "Enter size in X of the rectangle to be drawn : "
int sizex = readint*2

printr "Enter size in Y of the rectangle to be drawn : "
int sizey = readint

loop i<sizey
	j = 0
	loop j<sizex
		printr "-"
		j = j+1
	endloop
	print ""
	i = i+1
endloop
