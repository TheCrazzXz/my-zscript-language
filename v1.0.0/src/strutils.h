#ifndef __STR_UTILS_H__
#define __STR_UTILS_H__

#include <stdlib.h>

//Returns the index of the first occurence of char c in char* string. If not found -1 is returned.
ssize_t chridx(char* string, char c);
ssize_t chridxn(char* string, char c, size_t n);
/* counts the number of occurence of a character in a string */
size_t chrocc(char* str, char c);
/* duplicates a string (malloc) until it finds the end character */
char* strdupc(char* str, char end, int enablequoting);
/* finds the index of a string in an array of strings (if not found, -1 is returned) */
ssize_t strarrstr(char** str_str, size_t len, size_t start_idx, char* find);
#endif