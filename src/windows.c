#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "windows.h"
#include "constants.h"

MenuWin* MenuWin_init(
    int nlines, int ncols,
    int yPos, int xPos,
    char* label
){
    MenuWin* wMenu = (MenuWin*)malloc(sizeof(MenuWin));
    wMenu->windowptr = newwin(nlines, ncols, yPos, xPos);
    wMenu->label = label;
    wMenu->xMax = ncols;
    wMenu->yMax = nlines;
    wMenu->keypress = 0;
    wMenu->highlight = 0;
    keypad(wMenu->windowptr, TRUE);
    return wMenu;
}

void MenuWin_draw(MenuWin* wMenu){
    box(wMenu->windowptr, 0, 0);
    mvwprintw(wMenu->windowptr, 0, 2, wMenu->label);
    wrefresh(wMenu->windowptr);
}

void MenuWin_kill(MenuWin* wMenu){
    delwin(wMenu->windowptr);
    free(wMenu);
}

void winredraw(WINDOW* wptr){
    box(wptr, 0, 0);
    wrefresh(wptr);
}

void err_window(
    const char* msg,
    int nlines, int ncols,
    int begin_y, int begin_x
){
    WINDOW* errwin = newwin(nlines, ncols, begin_y, begin_x);
    box(errwin, 0, 0);
    mvwprintw(errwin, 0, 2, "[!] ERROR");
    mvwprintw(errwin, nlines/2, (ncols - strlen(msg))/2, "%s", msg);
    wrefresh(errwin);
    refresh();

    getch();

    delwin(errwin);
    touchwin(stdscr);
    refresh();
}

WINDOW* draw_banner(int yPos, int xPos){
    WINDOW* banner = newwin(APP_BANNER_HEIGHT, APP_SIDE_WIDTH, yPos, xPos);
    for(int i = 0; i < (int)(sizeof(__APP_BANNER__)/sizeof(__APP_BANNER__[0])); i++)
        mvwprintw(banner, i+2, 2, "%s", __APP_BANNER__[i]);
    refresh();

    return banner;
}
