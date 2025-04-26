#include "windows/windows_viewer.h"

// spawner
Win* Win_viewer(const size_t begin_y, const size_t begin_x){
    log_message("Win_viewer: creating viewer window.");
    Win* wViewer = Win_init(NULL, __yMax, __xMax-APP_SIDE_WIDTH, begin_y, begin_x, WIN_ROLE_VIEWER);
    wViewer->draw = Win_viewer_draw;
    wViewer->handle_input = Handle_input_viewer;
    wViewer->destructor = Win_viewer_destructor;
    wViewer->dirty = TRUE;
    
    keypad(wViewer->windowptr, TRUE);
    
    wViewer->userdata = (void*)ViewerData_init();

    log_message("Win_viewer: OK.");
    return wViewer;
}

// destructor
void Win_viewer_destructor(Win** winptr){
    delwin((*winptr)->windowptr);
    if((*winptr)->label) free((*winptr)->label);
    if((*winptr)->userdata){
        ViewerData* vd = (*winptr)->userdata;
        ViewerData_destroy(&vd);
        (*winptr)->userdata = NULL;
    }
    free(*winptr);
    *winptr = NULL;
}

// helper function
void draw_viewer_background(WINDOW* win, const size_t lines, const size_t cols) {
    for (size_t y = 0; y < lines; ++y) {
        const int pair = (y % 2 == 0) ? COLOR_PAIR(1) : COLOR_PAIR(2);
        wattron(win, pair);
        mvwhline(win, y, 0, ' ', cols);
        wattroff(win, pair);
    }
}

// drawer
void Win_viewer_draw(const Win* winptr){
    werase(winptr->windowptr);
    draw_viewer_background(winptr->windowptr, winptr->height, winptr->width);
    box(winptr->windowptr, 0, 0);
    if(winptr->label != NULL) mvwprintw(winptr->windowptr, 0, 2, "%s", winptr->label);
    const ViewerData* vd = (ViewerData*)winptr->userdata;
    if(vd->on_focus != MENU_NONE){
        ViewerTab* focused_tab = vd->tabs[vd->on_focus];
        mvwprintw(winptr->windowptr, 1, 1, "%s", focused_tab->page_current->rows[0].values[1].textValue);
    }
}

// input handler
void Handle_input_viewer(ViewManager* vm, Win** winptr, const void* context){
    (void)context;
    switch((*winptr)->keypress){
        case KEY_LEFT:
            // shift focus to last menu
            ViewManager_focus(vm, WIN_ROLE_MENU);
            *winptr = vm->windows[vm->focused];
            (*winptr)->dirty = TRUE;
            break;
        case KEY_DOWN:
            // (*winptr)->dirty = TRUE;
            // break;
        case KEY_UP:
            (*winptr)->dirty = TRUE;
            break;
        default:
            break;
    }
}
