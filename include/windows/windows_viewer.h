#ifndef APP_WINDOWS_VIEWER_H
#define APP_WINDOWS_VIEWER_H

#include "windows.h"

Win* Win_viewer(size_t begin_y, size_t begin_x);

void Win_viewer_destructor(Win** winptr);

void Win_viewer_draw(const Win* winptr);

void Handle_input_viewer(ViewManager* vm, Win** winptr, const void* context);

#endif