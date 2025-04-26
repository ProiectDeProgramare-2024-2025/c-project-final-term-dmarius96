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

void Win_menu_draw(Win* winptr);

void Win_menu_destructor(Win** winptr);

/***********************************/
/*****INPUT HANDLING FUNCTIONS******/
/***********************************/

void Handle_input_menu_main(struct ViewManager* vm, struct Win** winptr, void* context);

void Handle_input_menu_accounts(struct ViewManager* vm, struct Win** winptr, void* context);

void Handle_input_menu_transactions(struct ViewManager* vm, struct Win** winptr, void* context);

void Handle_input_menu_currencies(struct ViewManager* vm, struct Win** winptr, void* context);

void Handle_input_menu_transaction_categories(struct ViewManager* vm, struct Win** winptr, void* context);

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