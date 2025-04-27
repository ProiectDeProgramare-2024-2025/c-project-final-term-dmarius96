#include "windows/windows_popup.h"

Win* Win_popup(const char* msg, const char* label, const size_t begin_y, const size_t begin_x){
    log_message("Win_popup: spawning popup window with label '%s'.", label);
    Win* wPopup = Win_init(label, 5, strlen(msg)+4, begin_y, begin_x, WIN_ROLE_POPUP);
    wPopup->draw = Win_draw;
    wPopup->handle_input = NULL;
    wPopup->destructor = Win_popup_destructor;
    wPopup->dirty = TRUE;
    
    if (mvwprintw(wPopup->windowptr, 2, 2, "%s", msg))
        log_error("Win_popup: something went wrong while printing the message.");
    else
        log_message("Win_popup: OK.");

    return wPopup;
}

void Win_popup_destructor(Win** winptr){
    delwin((*winptr)->windowptr);
    if((*winptr)->label) free((*winptr)->label);
    free(*winptr);
    *winptr = NULL;
}

void popup(const char* message, const char* label, const size_t begin_y, const size_t begin_x, const ViewManager* vm) {
    Win* wPopup = Win_popup(message, label, begin_y, begin_x);
    wPopup->draw(wPopup);
    if (wrefresh(wPopup->windowptr)) log_error("Handle_input_menu: failed to refresh popup window.");
    getch();
    wPopup->destructor(&wPopup);
    ViewManager_redraw_all(vm);
}