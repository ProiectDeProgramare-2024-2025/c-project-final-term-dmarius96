#include "windows/windows.h"
#include "database.h"
#include "queries.h"

/***********************************/
/*******GENERIC Win FUNCTIONS*******/
/***********************************/

Win* Win_init(
    const char* label,
    size_t nlines, size_t ncols,
    size_t begin_y, size_t begin_x,
    WinRole role
){
    if(label != NULL) log_message("Win_init: initializing window with label '%s'.", label);
    else log_message("Win_init: initializing window with anonymous label.");

    Win* winptr = (Win*)malloc(sizeof(Win));
    if(winptr == NULL) log_error("Win_init: failed to allocate memory for window.");

    winptr->windowptr = newwin(nlines, ncols, begin_y, begin_x);
    if(label != NULL) winptr->label = strdup(label);
    else winptr->label = NULL;
    winptr->begin_y = begin_y;
    winptr->begin_x = begin_x;
    winptr->width = ncols;
    winptr->height = nlines;
    winptr->keypress = 0;
    winptr->highlight = 0;
    winptr->role = role;
    winptr->draw = NULL;
    winptr->userdata = NULL;

    log_message("Win_init: OK.");
    return winptr;
}

void Win_delete(Win** winptr){
    if(*winptr == NULL) {
        log_message("Win_delete: no window to delete.");
        return;
    }

    if((*winptr)->label == NULL) {
        log_message("Win_delete: deleting window with anonymous label.");
    }else{
        log_message("Win_delete: deleting window with label '%s'.", (*winptr)->label);
        free((*winptr)->label);
    }

    if(delwin((*winptr)->windowptr)) {
        log_error("Win_delete: failed to deallocate window.");
        return;
    }

    if((*winptr)->userdata) {
        if((*winptr)->role == WIN_ROLE_MENU){
            free((*winptr)->userdata);
            (*winptr)->userdata = NULL;
        }else if((*winptr)->role == WIN_ROLE_VIEWER){
            ViewerData* wd = (ViewerData*)(*winptr)->userdata;
            if(wd->header) free(wd->header);
            if(wd->table_chunk_prev) free_table(wd->table_chunk_prev);
            if(wd->table_chunk_current) free_table(wd->table_chunk_current);
            if(wd->table_chunk_next) free_table(wd->table_chunk_next);
            free((*winptr)->userdata);
            (*winptr)->userdata = NULL;
        }
    }

    free(*winptr);
    *winptr = NULL;

    log_message("Win_delete: OK.");
}

void Win_draw(Win* winptr){
    if(winptr->label) log_message("Win_draw: drawing window with label '%s'.", winptr->label);
    else              log_message("Win_draw: drawing window with anonymous label.");

    box(winptr->windowptr, 0, 0);
    if(winptr->label != NULL) mvwprintw(winptr->windowptr, 0, 2, winptr->label);

    log_message("Win_draw: OK.");
}