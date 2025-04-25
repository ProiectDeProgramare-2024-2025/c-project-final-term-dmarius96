#ifndef APP_WINDOWS_H
#define APP_WINDOWS_H

#include <ncurses.h>
#include "constants.h"

typedef enum {
    WIN_ROLE_BANNER,
    WIN_ROLE_VIEWER,
    WIN_ROLE_MENU,
    WIN_ROLE_POPUP
} WinRole;

typedef struct {
    const char** options;
    size_t option_count;
} MenuData;

typedef struct {
    char* text;
    size_t text_len;
} ViewerData;

typedef struct {
    int* loop;
} InputContext;

struct ViewManager;

typedef struct Win{
    WINDOW* windowptr;
    WinRole role;
    char* label;
    size_t begin_y;
    size_t begin_x;
    size_t width;
    size_t height;
    size_t keypress;
    size_t highlight;
    void (*draw)(struct Win*);
    void (*handle_input)(struct ViewManager*, struct Win**, void* context);
    void* userdata;
    bool dirty;
} Win;

typedef struct ViewManager{
    Win* windows[ROLE_COUNT];
    WinRole focused;
    Win* menu_stack[MENU_STACK_MAX];
    size_t menu_stack_top;
} ViewManager;

/***********************************/
/*******GENERIC Win FUNCTIONS*******/
/***********************************/

Win* Win_init(
    const char* label,
    size_t nlines, size_t ncols,
    size_t begin_y, size_t begin_x,
    WinRole role
);

void Win_delete(Win** winptr);

/***********************************/
/********DRAWING FUNCTIONS**********/
/***********************************/

void Win_draw(Win* winptr);

void Win_menu_draw(Win* winptr);

void Win_viewer_draw(Win* winptr);

/***********************************/
/*****INPUT HANDLING FUNCTIONS******/
/***********************************/

void Handle_input_menu_main(struct ViewManager* vm, struct Win** winptr, void* context);

void Handle_input_menu_accounts(struct ViewManager* vm, struct Win** winptr, void* context);

void Handle_input_menu_transactions(struct ViewManager* vm, struct Win** winptr, void* context);

void Handle_input_menu_currencies(struct ViewManager* vm, struct Win** winptr, void* context);

void Handle_input_menu_transaction_categories(struct ViewManager* vm, struct Win** winptr, void* context);

void Handle_input_viewer(struct ViewManager* vm, struct Win** winptr, void* context);

/***********************************/
/*******SPAWNING FUNCTIONS**********/
/***********************************/

Win* Win_banner(size_t begin_y, size_t begin_x);

Win* Win_viewer(size_t begin_y, size_t begin_x);

Win* Win_popup(const char* msg, const char* label, size_t begin_y, size_t begin_x);

Win* Win_menu_main(size_t begin_y, size_t begin_x);

Win* Win_menu_accounts(size_t begin_y, size_t begin_x);

Win* Win_menu_transactions(size_t begin_y, size_t begin_x);

Win* Win_menu_currencies(size_t begin_y, size_t begin_x);

Win* Win_menu_transaction_categories(size_t begin_y, size_t begin_x);

// Win* Win_options(size_t begin_y, size_t begin_x);

/***********************************/
/*******ViewManager FUNCTIONS*******/
/***********************************/

ViewManager* ViewManager_init();

void ViewManager_set(ViewManager* vm, WinRole role, Win* winptr);

void ViewManager_focus(ViewManager* vm, WinRole role);

void ViewManager_redraw_all(ViewManager* vm);

void ViewManager_destroy(ViewManager** vm);

void ViewManager_listen(ViewManager* vm, InputContext* ctx);

void ViewManager_push_menu(ViewManager* vm, Win* menu);

Win* ViewManager_pop_menu(ViewManager* vm);

#endif