#ifndef APP_WINDOWS_H
#define APP_WINDOWS_H

#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include "log_utils.h"
#include "constants.h"
#include "table_struct.h"

typedef enum {
    WIN_ROLE_BANNER,
    WIN_ROLE_VIEWER,
    WIN_ROLE_MENU,
    WIN_ROLE_POPUP
} WinRole;

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
    void (*destructor)(struct Win**);
    void* userdata;
    bool dirty;
} Win;

typedef struct {
    int* loop;
} InputContext;

typedef struct ViewManager{
    Win* windows[ROLE_COUNT];
    WinRole focused;
    Win* menu_stack[MENU_STACK_MAX];
    size_t menu_stack_top;
} ViewManager;

/*******************************************************/

Win* Win_init(
    const char* label,
    size_t nlines, size_t ncols,
    size_t begin_y, size_t begin_x,
    WinRole role
);

void Win_draw(Win* winptr);

/*******************************************************/

#include "view_manager.h"
#include "windows_banner.h"
#include "windows_popup.h"
#include "windows_viewer.h"
#include "windows_menus.h"

#endif