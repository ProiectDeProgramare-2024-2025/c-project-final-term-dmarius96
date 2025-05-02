#include "windows/windows_viewer.h"
#include "windows/viewer_tab.h"

// spawner
Win* Win_viewer(const size_t begin_y, const size_t begin_x){
    log_message("Win_viewer: creating viewer window.");
    Win* wViewer = Win_init(NULL, __yMax, __xMax-APP_SIDE_WIDTH, begin_y, begin_x, WIN_ROLE_VIEWER);
    if (wViewer == NULL) {
        log_message("Win_viewer: failed to create viewer window.");
        return NULL;
    }
    wViewer->draw = Win_viewer_draw;
    wViewer->handle_input = Handle_input_viewer;
    wViewer->destructor = Win_viewer_destructor;
    wViewer->dirty = TRUE;
    
    keypad(wViewer->windowptr, TRUE);

    ViewerData* vd = ViewerData_init();
    if (vd == NULL) {
        log_message("Win_viewer: failed to allocate memory for viewer data.");
        wViewer->destructor(&wViewer);
        return NULL;
    }
    wViewer->userdata = vd;

    log_message("Win_viewer: OK.");
    return wViewer;
}

// destructor
void Win_viewer_destructor(Win** winptr){
    log_message("Win_viewer: destroying viewer window.");
    delwin((*winptr)->windowptr);
    if((*winptr)->label) free((*winptr)->label);
    if((*winptr)->userdata){
        ViewerData* vd = (*winptr)->userdata;
        ViewerData_destroy(&vd);
        (*winptr)->userdata = NULL;
    }
    free(*winptr);
    *winptr = NULL;
    log_message("Win_viewer: OK.");
}

// helper function
void draw_viewer_background(const Win* winptr) {
    const ViewerData* vd = winptr->userdata;
    for (size_t y = 0; y < winptr->height; ++y) {
        const int pair = y % 2 == 0 ? COLOR_PAIR(1) : COLOR_PAIR(2);
        if(vd->tabs[vd->on_focus] != NULL && y == vd->tabs[vd->on_focus]->highlight) wattron(winptr->windowptr, A_REVERSE);
        wattron(winptr->windowptr, pair);
        mvwhline(winptr->windowptr, y, 0, ' ', winptr->width);
        wattroff(winptr->windowptr, pair);
        wattroff(winptr->windowptr, A_REVERSE);
    }
}

// drawer
void Win_viewer_draw(const Win* winptr){
    log_message("Win_viewer: drawing viewer window.");

    const ViewerData* vd = (ViewerData*)winptr->userdata;
    if (vd == NULL) {
        log_error("Win_viewer: viewer data is NULL.");
        return;
    }

    if(vd->on_focus == NONE) {
        log_error("Win_viewer: viewer does not have a focus.");
        return;
    }

    werase(winptr->windowptr);
    draw_viewer_background(winptr);
    box(winptr->windowptr, 0, 0);
    if(winptr->label != NULL && mvwprintw(winptr->windowptr, 0, 2, "%s", winptr->label))
        log_warning("Win_viewer: something went wrong while printing the label.");

    const ViewerTab* focused_tab = vd->tabs[vd->on_focus];
    if (focused_tab == NULL) {
        log_warning("Win_viewer: focused tab is NULL.");
        return;
    }

    int pair = 0;
    for(size_t y = 0; y < focused_tab->page_current->numRows; ++y){
        pair = y % 2 ? COLOR_PAIR(1) : COLOR_PAIR(2);
        size_t txt_offset = 2;
        if(y+1 == vd->tabs[vd->on_focus]->highlight) wattron(winptr->windowptr, A_REVERSE);
        wattron(winptr->windowptr, pair);
        for(size_t x = 0; x < focused_tab->page_current->numColumns; ++x){
            char buf[64];
            switch(focused_tab->page_current->rows[y].types[x]){
                case CELL_TYPE_INT:
                    snprintf(buf, 63, "%d ", focused_tab->page_current->rows[y].values[x].intValue);
                    break;
                case CELL_TYPE_DOUBLE:
                    snprintf(buf, 63, "%.2f ", focused_tab->page_current->rows[y].values[x].doubleValue);
                    break;
                case CELL_TYPE_TEXT:
                    snprintf(buf, 63, "%s ", focused_tab->page_current->rows[y].values[x].textValue);
                    break;
                default:
                    break;
            }
            if (mvwprintw(winptr->windowptr, (int)y+1, (int)txt_offset, buf))
                log_warning("Win_viewer: something went wrong while printing line %lu.", y);
            txt_offset += strlen(buf)+3;
        }
        wattroff(winptr->windowptr, pair);
        wattroff(winptr->windowptr, A_REVERSE);
    }
    log_message("Win_viewer: OK.");
}

// input handler
void Handle_input_viewer(ViewManager* vm, Win** winptr, const void* context){
    log_message("Handle_input_viewer: now handling input for viewer window.");
    (void)context;

    const ViewerData* vd = (*winptr)->userdata;
    if (vd == NULL) {
        log_error("Handle_input_viewer: viewer data is NULL.");
        return;
    }
    if (vd->on_focus == NONE) {
        log_error("Handle_input_viewer: viewer does not have a focus.");
        return;
    }
    const size_t ht = vd->tabs[vd->on_focus]->page_current->numRows;

    switch((*winptr)->keypress){
        case KEY_LEFT:
            log_message("Handle_input_viewer: shifting focus to menu window.");
            ViewManager_focus(vm, WIN_ROLE_MENU);
            *winptr = vm->windows[vm->focused];
            break;
        case KEY_DOWN:
            if(vd->tabs[vd->on_focus]->highlight < ht) ++vd->tabs[vd->on_focus]->highlight;
            (*winptr)->dirty = TRUE;
            break;
        case KEY_UP:
            if(vd->tabs[vd->on_focus]->highlight > 1) --vd->tabs[vd->on_focus]->highlight;
            (*winptr)->dirty = TRUE;
            break;
        default:
            break;
    }

    log_message("Handle_input_viewer: OK.");
}
