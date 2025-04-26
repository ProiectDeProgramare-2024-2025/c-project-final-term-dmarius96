#ifndef APP_WINDOWS_BANNER_H
#define APP_WINDOWS_BANNER_H

#include "windows.h"

Win* Win_banner(size_t begin_y, size_t begin_x);

void Win_banner_destructor(Win** winptr);

#endif