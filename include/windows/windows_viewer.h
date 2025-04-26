#ifndef APP_WINDOWS_VIEWER_H
#define APP_WINDOWS_VIEWER_H

#include "windows.h"

Win* Win_viewer(size_t begin_y, size_t begin_x);

void Win_viewer_draw(Win* winptr);

void Handle_input_viewer(struct ViewManager* vm, struct Win** winptr, void* context);

#endif