#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#include "interactive.h"
#include "constants.h"
#include "windows.h"

#define CHOICES             4

const char* errmsg = "Not implemented yet!";
const char* tempmsg = "Window reserved for data viewing...";

void runtime_interactive(){

    // initialize screen
    // sets up memory and clears the screen
    initscr();

    // doesn't wait for ENTER after each input
    cbreak();

    // disable automatic echoing of characters to the terminal
    noecho();

    // makes the cursor disappear
    curs_set(0);

    // get screen size
    int xMax = 0;
    int yMax = 0;
    getmaxyx(stdscr, yMax, xMax);

    // draw banner
    WINDOW* wBanner = draw_banner(0, 0);
    winredraw(wBanner);

    // create main menu
    MenuWin* wMenu = MenuWin_init(yMax-APP_BANNER_HEIGHT, APP_SIDE_WIDTH, APP_BANNER_HEIGHT, 0, "[+] MAIN MENU");
    MenuWin_draw(wMenu);

    // create viewing window
    WINDOW* wViewer = newwin(yMax, xMax-APP_SIDE_WIDTH, 0, APP_SIDE_WIDTH);
    mvwprintw(wViewer, yMax/2, (xMax/2 - sizeof(tempmsg)/2)/2 + 3, "%s", tempmsg);
    winredraw(wViewer);
    refresh();

    // initialize menu variables
    char* choices[CHOICES] = { " Accounts ", " Transactions ", " Options ", " Exit " };

    int loop = 1;

    while(loop){
        // print menu choices
        for(int i = 0; i < CHOICES-1; i++){
            if(i == wMenu->highlight) wattron(wMenu->windowptr, A_REVERSE);
            mvwprintw(wMenu->windowptr, (i*2)+5, wMenu->xMax/2 - strlen(choices[i])/2, choices[i]);
            wattroff(wMenu->windowptr, A_REVERSE);
        }

        // draw exit button
        if(wMenu->highlight == CHOICES-1) wattron(wMenu->windowptr, A_REVERSE);
        mvwprintw(wMenu->windowptr, wMenu->yMax-3, wMenu->xMax/2 - strlen(choices[CHOICES-1])/2, choices[CHOICES-1]);
        wattroff(wMenu->windowptr, A_REVERSE);

        // get choice from user
        wMenu->keypress = wgetch(wMenu->windowptr);
        
        // enable scrolling in the menu
        switch (wMenu->keypress){
            case KEY_UP:
                if(wMenu->highlight > 0) --(wMenu->highlight);
                break;

            case KEY_DOWN:
                if(wMenu->highlight < CHOICES-1) ++(wMenu->highlight);
                break;
        
            default:
                break;
        }

        if(wMenu->keypress == ENTER){
            switch(wMenu->highlight){
                case 0:
                case 1:
                case 2:
                    err_window(errmsg, 5, sizeof(errmsg)/sizeof(char)+6, yMax/2 - 5, xMax/2 - (sizeof(errmsg)/sizeof(char)+6)/2);
                    MenuWin_draw(wMenu);
                    winredraw(wBanner);
                    winredraw(wViewer);
                    break;

                case CHOICES-1:
                    loop = 0;

                default:
                    break;
            }
        }
    }

    // deletes windows
    delwin(wBanner);
    MenuWin_kill(wMenu);

    // deallocates memory and ends ncurses
    endwin();
    
}