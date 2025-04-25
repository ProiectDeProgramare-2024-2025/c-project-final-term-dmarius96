#ifndef APP_UTILS_H
#define APP_UTILS_H

// Returns the uppercase version of the string.
// The programmer is responsible for freeing the string after use!
char* to_upper(const char* str);

// Returns 1 if the date is in the corresponding format: YYYY-MM-DD
int check_date(const char* str);

#endif