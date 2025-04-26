#include "windows/windows_viewer.h"

// spawner
Win* Win_viewer(size_t begin_y, size_t begin_x){
    log_message("Win_viewer: creating viewer window.");
    Win* wViewer = Win_init(NULL, __yMax, __xMax-APP_SIDE_WIDTH, begin_y, begin_x, WIN_ROLE_VIEWER);
    wViewer->draw = Win_viewer_draw;
    wViewer->handle_input = Handle_input_viewer;
    wViewer->destructor = Win_viewer_destructor;
    wViewer->dirty = TRUE;
    
    keypad(wViewer->windowptr, TRUE);

    ViewerData* wd = (ViewerData*)malloc(sizeof(ViewerData));
    wd->header = NULL;
    wd->header_len = 0;
    wd->table_chunk_prev = NULL;
    wd->table_chunk_current = NULL;
    wd->table_chunk_next = NULL;
    wViewer->userdata = (void*)wd;

    log_message("Win_viewer: OK.");
    return wViewer;
}

// destructor
void Win_viewer_destructor(Win** winptr){
    delwin((*winptr)->windowptr);
    if((*winptr)->label) free((*winptr)->label);
    if((*winptr)->userdata){
        ViewerData* wd = (ViewerData*)(*winptr)->userdata;
        if(wd->header) free(wd->header);
        if(wd->table_chunk_prev) free_table(wd->table_chunk_prev);
        if(wd->table_chunk_current) free_table(wd->table_chunk_current);
        if(wd->table_chunk_next) free_table(wd->table_chunk_next);
        free(wd);
        (*winptr)->userdata = NULL;
    }
    free(*winptr);
    *winptr = NULL;
}

// helper function
void draw_viewer_background(WINDOW* win, int lines, int cols) {
    for (int y = 0; y < lines; ++y) {
        int pair = (y % 2 == 0) ? COLOR_PAIR(1) : COLOR_PAIR(2);
        wattron(win, pair);
        mvwhline(win, y, 0, ' ', cols);
        wattroff(win, pair);
    }
}

// drawer
void Win_viewer_draw(Win* winptr){
    werase(winptr->windowptr);
    draw_viewer_background(winptr->windowptr, winptr->height, winptr->width);
    box(winptr->windowptr, 0, 0);
    if(winptr->label != NULL) mvwprintw(winptr->windowptr, 0, 2, winptr->label);
}

// input handler
void Handle_input_viewer(struct ViewManager* vm, struct Win** winptr, void* context){
    (void)context;
    switch((*winptr)->keypress){
        case KEY_LEFT:
            // shift focus to last menu
            ViewManager_focus(vm, WIN_ROLE_MENU);
            *winptr = vm->windows[vm->focused];
            (*winptr)->dirty = TRUE;
            break;
        case KEY_DOWN:
            (*winptr)->dirty = TRUE;
            break;
        case KEY_UP:
            (*winptr)->dirty = TRUE;
            break;
        default:
            break;
    }
}
