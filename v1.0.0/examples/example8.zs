int i = 0
int j = 0
int k = 0

vector vec = vector
vector curra = vector
vector currb = vector
vector currc = vector

printr "Enter maximum : "
int MAX = readint

for i = 0 ; i<MAX ; i += 1
	print i
	vec[ i ] = vector
	curra = vec[i]
	for j = 0 ; j<MAX ; j += 1
		curra[ j ] = vector
		currb = curra[j]
		print "	" j
		for k = 0 ; k<MAX ; k += 1
			currb[ k ] = i*j*k
			//
			if k==2
				crack
			endif
			//
			print "		" k ":" i "*" k "*" j "=" currb[k]
		endloop
	endloop
endloop

print vec
