#ifndef APP_WINDOWS_MENUS_H
#define APP_WINDOWS_MENUS_H

#include "windows.h"

typedef struct {
    const char** options;
    size_t option_count;
} MenuData;

/***********************************/
/********DRAWING FUNCTIONS**********/
/***********************************/

void Win_menu_draw(const Win* winptr);

void Win_menu_destructor(Win** winptr);

/***********************************/
/*****INPUT HANDLING FUNCTIONS******/
/***********************************/

void Handle_input_menu_main(ViewManager* vm, Win** winptr, const void* context);

void Handle_input_menu_accounts(ViewManager* vm, Win** winptr, const void* context);

void Handle_input_menu_transactions(ViewManager* vm, Win** winptr, const void* context);

void Handle_input_menu_currencies(ViewManager* vm, Win** winptr, const void* context);

void Handle_input_menu_transaction_categories(ViewManager* vm, Win** winptr, const void* context);

/***********************************/
/*******SPAWNING FUNCTIONS**********/
/***********************************/

Win* Win_menu_main(size_t begin_y, size_t begin_x);

Win* Win_menu_accounts(size_t begin_y, size_t begin_x);

Win* Win_menu_transactions(size_t begin_y, size_t begin_x);

Win* Win_menu_currencies(size_t begin_y, size_t begin_x);

Win* Win_menu_transaction_categories(size_t begin_y, size_t begin_x);

// Win* Win_options(size_t begin_y, size_t begin_x);

#endif