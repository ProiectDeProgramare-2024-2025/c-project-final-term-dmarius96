#ifndef APP_CONSTANTS_H
#define APP_CONSTANTS_H

#include <string.h>

#define ENTER   10
#define ESC     27

#define APP_SIDE_WIDTH      (strlen(__APP_BANNER__[0])+4)
#define APP_BANNER_HEIGHT   9
#define APP_BANNER_LINES    5

extern char* __APP_BANNER__[APP_BANNER_LINES];

#endif