#include "windows/windows_banner.h"

Win* Win_banner(const size_t begin_y, const size_t begin_x){
    log_message("Win_banner: creating banner window.");
    Win* wBanner = Win_init(NULL, APP_BANNER_LINES+3, APP_SIDE_WIDTH, begin_y, begin_x, WIN_ROLE_BANNER);
    if (wBanner == NULL) {
        log_error("Win_banner: failed to create banner window.");
        return NULL;
    }

    wBanner->draw = Win_draw;
    wBanner->handle_input = NULL;
    wBanner->destructor = Win_banner_destructor;
    wBanner->dirty = TRUE;
    for(size_t i = 0; i < APP_BANNER_LINES; ++i){
        if (mvwprintw(wBanner->windowptr, (int)i+1, 3, "%s", __APP_BANNER__[i]))
            log_warning("Win_banner: something went wrong while printing the banner.");
    }

    log_message("Win_banner: OK.");
    return wBanner;
}

void Win_banner_destructor(Win** winptr){
    log_message("Win_banner: destroying banner window.");
    delwin((*winptr)->windowptr);
    if((*winptr)->label) free((*winptr)->label);
    free(*winptr);
    *winptr = NULL;
    log_message("Win_banner: OK.");
}