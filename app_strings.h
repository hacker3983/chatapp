#ifndef _APPSTRINGS_H
#define _APPSTRINGS_H
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

char* string_dupstr(const char* string);
bool string_concatstr(char** dest_string, const char* src_string);
#endif