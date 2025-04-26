#include "windows/windows.h"
#include "database.h"
#include "queries.h"

/***********************************/
/*******GENERIC Win FUNCTIONS*******/
/***********************************/

Win* Win_init(
    const char* label,
    const size_t nlines, const size_t ncols,
    const size_t begin_y, const size_t begin_x,
    const WinRole role
){
    if(label != NULL) log_message("Win_init: initializing window with label '%s'.", label);
    else log_message("Win_init: initializing window with anonymous label.");

    Win* winptr = malloc(sizeof(Win));
    if(winptr == NULL) {
        log_error("Win_init: failed to allocate memory for window.");
        return NULL;
    }

    winptr->windowptr = newwin((int)nlines, (int)ncols, (int)begin_y, (int)begin_x);
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

void Win_draw(const Win* winptr){
    if(winptr->label) log_message("Win_draw: drawing window with label '%s'.", winptr->label);
    else              log_message("Win_draw: drawing window with anonymous label.");

    box(winptr->windowptr, 0, 0);
    if(winptr->label != NULL) mvwprintw(winptr->windowptr, 0, 2, winptr->label);

    log_message("Win_draw: OK.");
}