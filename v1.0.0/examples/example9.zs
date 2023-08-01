//
	One parameter functions, they can return
//

defunc square a
	return a*a
endef

int factorial_i = 0
int factorial_ret = 0
int i = 0
defunc factorial n
	factorial_ret = 1
	for i = 1 ; i<=n ; i += 1
		factorial_ret = factorial_ret*i
	endloop
	return factorial_ret
endef

print "10² =" square(10)
print "15! =" factorial(15)
