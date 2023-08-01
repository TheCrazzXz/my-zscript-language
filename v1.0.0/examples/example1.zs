//
	This program is an example program that will ask name and age of a user and then print them back
//

/ prompt username
printr "Enter your username : "
string username = readstr

/ prompt age
printr "Enter your age : "
int age = readint

print "Hello,\"" username "\", you are" age "years old"
