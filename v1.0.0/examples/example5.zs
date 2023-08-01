int guess = 0
int value = 0
int tries = 10
int wins = 0
int won = 0
loop 1
	tries = 10
	
	print "Number guess game, won" wins "games"
	value = randint

	loop tries>0
		printr "Guess the number (number of tries left :" tries ") : "
		guess = readint
		if guess<0
			print "Error"
			reit
		endif
	
		if guess>value
			print "too high"
		endif
		if guess<value
			print "too low"
		endif
		if guess==value
			print "Bingo !"
			wins = wins+1
			won = 1
			crack
		endif
		tries = tries-1
	endloop
	if won==0
		print "You lose !"
	else
		won = 0
	endif
	print "Try again ? [Y/N]"
	if readstr=="Y"
		print "Relaunching game"
	else
		crack
	endif
	
endloop
print "end"
