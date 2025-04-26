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

typedef struct {
    const char** options;
    size_t option_count;
} MenuData;

typedef struct {
    char* header;
    size_t header_len;
    TableData* table_chunk_prev;
    TableData* table_chunk_current;
    TableData* table_chunk_next;
} ViewerData;

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

/*******************************************************/

Win* Win_init(
    const char* label,
    size_t nlines, size_t ncols,
    size_t begin_y, size_t begin_x,
    WinRole role
);

void Win_delete(Win** winptr);

void Win_draw(Win* winptr);

/*******************************************************/

#include "view_manager.h"
#include "windows_banner.h"
#include "windows_popup.h"
#include "windows_viewer.h"
#include "windows_menus.h"

#endif