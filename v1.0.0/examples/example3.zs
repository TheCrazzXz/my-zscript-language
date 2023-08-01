//
	Ask user for username and password and age
//

string username = "jack"
string password = "8gre82g"

print "Welcome to login service, you need to be 10 years+ to log in."

printr "Enter your age : "
int age = readint

if age<0
	print "Error : You're not even born yet ??"
else
	if age>=10

		printr "Enter username : "
		string inputname = readstr

		if inputname==username
			printr "Enter password for" username ": "
			string inputpass = readstr
			if inputpass==password
				print "Logged in as : " username " : " password
			else
				print "Error : Wrong password for" username
			endif
		else
			print "Error : User" inputname "doesn't exist"
		endif
	else
		print "Error : Can't use this app at age" age
	endif
endif
print "Ended program"
exit
