#ifndef MENU_HELPERS_H
#define MENU_HELPERS_H

#include "windows/windows.h"

// spawners
Win* Win_menu_account_deletion(const Win* winptr);

Win* Win_menu_account_search(const Win* winptr);

// input handlers
void Handle_input_menu_account_deletion(ViewManager* vm, Win** winptr, const void* context);

void Handle_input_menu_account_search(ViewManager* vm, Win** winptr, const void* context);

#endif
