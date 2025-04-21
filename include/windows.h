#ifndef APP_WINDOWS_H
#define APP_WINDOWS_H

#include <ncurses.h>

// Struct for menu windows
typedef struct MenuWin{
    WINDOW* windowptr;
    int yMax;
    int xMax;
    char* label;
    int keypress;
    int highlight;
} MenuWin;

MenuWin* MenuWin_init(int nlines, int ncols, int yPos, int xPos, char* label);

void MenuWin_draw(MenuWin* wMenu);

void MenuWin_kill(MenuWin* wMenu);

void winredraw(WINDOW* wptr);

void err_window(const char* msg, int nlines, int ncols, int begin_y, int begin_x);

WINDOW* draw_banner(int yPos, int xPos);

#endif
