//
	Simple calculator
//
float number = 0.0
float i = 0.0
int main_loop = 1
float result = 1.0
float othernumber = 0.0
string operation = ""
print "Welcome to calculator"

loop main_loop
	printr "Enter number : "
	number = readfloat
	loop 1
		print "Number :" number
		printr "Enter other number : "
		othernumber = readfloat

		printr "Select operation [+, -, /, *, ** (power), r (reset), q (exit)] : "
		operation = readstr

		if operation=="+"
			number = number+othernumber
		endif

		if operation=="-"
			number = number-othernumber
		endif

		if operation=="/"
			number = number/othernumber
		endif

		if operation=="*"
			number = number*othernumber
		endif
		
		if operation=="**"
			result = 1.0
			loop i<othernumber
				result = result*number
				i = i+1
			endloop
			number = result
			result = 1.0
		endif

		if operation=="r"
			crack
		endif

		if operation=="q"
			main_loop = 0
			crack
		endif
	endloop
endloop