#ifndef APP_CONSTANTS_H
#define APP_CONSTANTS_H

#include <string.h>

#define ENTER   10
#define ESC     27


#define APP_SIDE_WIDTH              (strlen(__APP_BANNER__[0])+4)
#define APP_BANNER_HEIGHT           9
#define APP_BANNER_LINES            5
#define APP_MAIN_MENU_CHOICES_NO    6


extern const char* __APP_BANNER__[APP_BANNER_LINES];
extern const char* __APP_MAIN_MENU_CHOICES[APP_MAIN_MENU_CHOICES_NO];


#endif