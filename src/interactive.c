#include <ncurses.h>

#include "interactive.h"
#include "log_utils.h"
#include "windows/windows.h"
#include "constants.h"

void runtime_interactive(){

    if(initscr() == stdscr) {
        log_message("Initialized screen.");
    }else{
        log_error("Failed to initialize screen. Quitting.");
        return;
    }

    cbreak();
    noecho();
    curs_set(0);
    start_color();

    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, 236);

    refresh();

    getmaxyx(stdscr, __yMax, __xMax);

    // spawn all base windows
    Win* wBanner = Win_banner(0, 0);
    Win* wViewer = Win_viewer(0, APP_SIDE_WIDTH);
    Win* wMainMenu = Win_menu_main(APP_BANNER_HEIGHT-1, 0);
    
    // initialize ViewManager
    ViewManager* vm = ViewManager_init();
    ViewManager_set(vm, WIN_ROLE_BANNER, wBanner);
    ViewManager_set(vm, WIN_ROLE_VIEWER, wViewer);
    ViewManager_set(vm, WIN_ROLE_MENU, wMainMenu);
    ViewManager_focus(vm, WIN_ROLE_MENU);

    // begin listening for user input
    int loop = 1;
    InputContext ctx = {.loop = &loop};
    ViewManager_listen(vm, &ctx);

    // destroy all windows in the manager and the manager itself
    ViewManager_destroy(&vm);

    if(!endwin()) log_message("Stopped screen.");
}