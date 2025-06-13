
#include "app_strings.h"
#include <stdio.h>

char* string_dupstr(const char* string) {
    if(!string) {
        return NULL;
    }
    size_t string_len = strlen(string);
    char* new_str = malloc(string_len+1);
    if(!new_str) {
        return NULL;
    }
    strcpy(new_str, string);
    new_str[string_len] = '\0';
    return new_str;
}

bool string_concatstr(char** dest_string, const char* src_string) {
    if(!src_string) {
        return false;
    }
    
    size_t dest_stringlen = (*dest_string) ? strlen((*dest_string)) :
            0;
    size_t src_stringlen = strlen(src_string);
    size_t new_strlen = dest_stringlen + src_stringlen;
    char* new_str = realloc((*dest_string), new_strlen + 1);
    if(!new_str) {
        return false;
    }
    strcpy(new_str+dest_stringlen, src_string);
    new_str[new_strlen] = '\0';
    *dest_string = new_str;
    return true;
}