#include "windows/windows_popup.h"

Win* Win_popup(const char* msg, const char* label, size_t begin_y, size_t begin_x){
    log_message("Win_popup: spawning popup window with label '%s'.", label);
    Win* wPopup = Win_init(label, 5, strlen(msg)+4, begin_y, begin_x, WIN_ROLE_POPUP);
    wPopup->draw = Win_draw;
    wPopup->handle_input = NULL;
    wPopup->dirty = TRUE;
    
    mvwprintw(wPopup->windowptr, 2, 2, msg);

    log_message("Win_popup: OK.");
    return wPopup;
}