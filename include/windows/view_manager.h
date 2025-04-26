#ifndef APP_VIEW_MANAGER_H
#define APP_VIEW_MANAGER_H

#include "windows.h"

/*******************************************************/

ViewManager* ViewManager_init();

void ViewManager_set(ViewManager* vm, WinRole role, Win* winptr);

void ViewManager_focus(ViewManager* vm, WinRole role);

void ViewManager_redraw_all(const ViewManager* vm);

void ViewManager_destroy(ViewManager** vm);

void ViewManager_listen(ViewManager* vm, InputContext* ctx);

void ViewManager_push_menu(ViewManager* vm, Win* menu);

Win* ViewManager_pop_menu(ViewManager* vm);

#endif