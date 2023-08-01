vector vec = vector
vector curr = vector
int i = 0
int j = 0

loop i<10
	vec[ i ] = vector
	curr = vec[i]
	j = 0
	loop j<10
		curr[ j ] = i+10-j
		j += 1
	endloop
	i += 1
endloop

print vec
