#ifndef APP_WINDOWS_POPUP_H
#define APP_WINDOWS_POPUP_H

#include "windows.h"

Win* Win_popup(const char* msg, const char* label, size_t begin_y, size_t begin_x);

void Win_popup_destructor(Win** winptr);

void popup(const char* message, const char* label, size_t begin_y, size_t begin_x, const ViewManager* vm);

#endif