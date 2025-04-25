#include <stdlib.h>
#include <string.h>

#include "windows.h"
#include "constants.h"
#include "log_utils.h"
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

/***********************************/
/********DRAWING FUNCTIONS**********/
/***********************************/

void Win_draw(Win* winptr){
    if(winptr->label) log_message("Win_draw: drawing window with label '%s'.", winptr->label);
    else              log_message("Win_draw: drawing window with anonymous label.");

    box(winptr->windowptr, 0, 0);
    if(winptr->label != NULL) mvwprintw(winptr->windowptr, 0, 2, winptr->label);

    log_message("Win_draw: OK.");
}

void Win_menu_draw(Win* winptr){
    MenuData* m = (MenuData*)winptr->userdata;

    box(winptr->windowptr, 0, 0);
    if (winptr->label != NULL) mvwprintw(winptr->windowptr, 0, 2, winptr->label);

    size_t n = m->option_count;

    for (size_t i = 0; i < n-1; ++i) {
        if (i == winptr->highlight) wattron(winptr->windowptr, A_REVERSE);
        mvwprintw(winptr->windowptr, (i + 1) * 2, (winptr->width - strlen(m->options[i])) / 2, m->options[i]);
        wattroff(winptr->windowptr, A_REVERSE);
    }

    if(n-1 == winptr->highlight) wattron(winptr->windowptr, A_REVERSE);
    mvwprintw(winptr->windowptr, winptr->height-3, (APP_SIDE_WIDTH - strlen(m->options[n-1])) / 2, m->options[n-1]);
    wattroff(winptr->windowptr, A_REVERSE);
}

void draw_viewer_background(WINDOW* win, int lines, int cols) {
    for (int y = 0; y < lines; ++y) {
        int pair = (y % 2 == 0) ? COLOR_PAIR(1) : COLOR_PAIR(2);
        wattron(win, pair);
        mvwhline(win, y, 0, ' ', cols);
        wattroff(win, pair);
    }
}

void Win_viewer_draw(Win* winptr){
    werase(winptr->windowptr);
    draw_viewer_background(winptr->windowptr, winptr->height, winptr->width);
    box(winptr->windowptr, 0, 0);
    if(winptr->label != NULL) mvwprintw(winptr->windowptr, 0, 2, winptr->label);
}

/***********************************/
/*****INPUT HANDLING FUNCTIONS******/
/***********************************/

void Handle_input_menu_main(struct ViewManager* vm, struct Win** winptr, void* context){
    MenuData* data = (MenuData*)(*winptr)->userdata;
    if(!data) return;
    InputContext* ctx = (InputContext*)context;

    switch((*winptr)->keypress){
        case KEY_UP:
            if((*winptr)->highlight > 0) --((*winptr)->highlight);
            (*winptr)->dirty = TRUE;
            break;
        case KEY_DOWN:
            if((*winptr)->highlight < data->option_count - 1) ++((*winptr)->highlight);
            (*winptr)->dirty = TRUE;
            break;
        case KEY_RIGHT: {
            // shift focus to viewer window
            ViewManager_focus(vm, WIN_ROLE_VIEWER);
            *winptr = vm->windows[vm->focused];
            (*winptr)->dirty = TRUE;
            break;
        }
        case ENTER:
            switch((*winptr)->highlight){
                // each of these cases should shift the focus in ViewManager* vm
                case 0: {
                    // accounts
                    
                    // populate viewer with data from Accounts table
                    // Win* wViewer = vm->windows[WIN_ROLE_VIEWER];

                    // set new menu
                    ViewManager_push_menu(vm, (*winptr));
                    ViewManager_set(vm, WIN_ROLE_MENU, Win_menu_accounts((*winptr)->begin_y, (*winptr)->begin_x));
                    *winptr = vm->windows[vm->focused];
                    break;
                }
                case 1: {
                    // transactions
                    Win* wViewer = vm->windows[WIN_ROLE_VIEWER];
                    werase(wViewer->windowptr);
                    mvwprintw(wViewer->windowptr, 2, 2, "Now in menu: transactions");
                    wViewer->draw(wViewer);
                    ViewManager_push_menu(vm, (*winptr));
                    ViewManager_set(vm, WIN_ROLE_MENU, Win_menu_transactions((*winptr)->begin_y, (*winptr)->begin_x));
                    *winptr = vm->windows[vm->focused];
                    break;
                }
                case 2: {
                    // currencies
                    Win* wViewer = vm->windows[WIN_ROLE_VIEWER];
                    werase(wViewer->windowptr);
                    mvwprintw(wViewer->windowptr, 2, 2, "Now in menu: currencies");
                    wViewer->draw(wViewer);
                    ViewManager_push_menu(vm, (*winptr));
                    ViewManager_set(vm, WIN_ROLE_MENU, Win_menu_currencies((*winptr)->begin_y, (*winptr)->begin_x));
                    *winptr = vm->windows[vm->focused];
                    break;
                }
                case 3: {
                    // transaction categories
                    Win* wViewer = vm->windows[WIN_ROLE_VIEWER];
                    werase(wViewer->windowptr);
                    mvwprintw(wViewer->windowptr, 2, 2, "Now in menu: transaction categories");
                    wViewer->draw(wViewer);
                    ViewManager_push_menu(vm, (*winptr));
                    ViewManager_set(vm, WIN_ROLE_MENU, Win_menu_transaction_categories((*winptr)->begin_y, (*winptr)->begin_x));
                    *winptr = vm->windows[vm->focused];
                    break;
                }
                case 4: {
                    // options
                    Win* wPopup = Win_popup("Not implemented!", "| [!] Error |", (__yMax-5)/2, (__xMax-10)/2);
                    Win_draw(wPopup);
                    wrefresh(wPopup->windowptr);
                    getch();
                    Win_delete(&wPopup);
                    ViewManager_redraw_all(vm);
                    break;
                }
                case 5:
                    *(ctx->loop) = 0;
                    break;
                default:
                    break;
            }
            break;
    }
}

void Handle_input_menu_accounts(struct ViewManager* vm, struct Win** winptr, void* context){
    (void)context;
    MenuData* data = (MenuData*)(*winptr)->userdata;
    if(!data) return;

    switch((*winptr)->keypress){
        case KEY_UP:
            if((*winptr)->highlight > 0) --((*winptr)->highlight);
            (*winptr)->dirty = TRUE;
            break;
        case KEY_DOWN:
            if((*winptr)->highlight < data->option_count - 1) ++((*winptr)->highlight);
            (*winptr)->dirty = TRUE;
            break;
        case KEY_RIGHT: {
            // shift focus to viewer window
            ViewManager_focus(vm, WIN_ROLE_VIEWER);
            // (*winptr)->dirty = TRUE;
            *winptr = vm->windows[vm->focused];
            break;
        }
        case ENTER:
            switch((*winptr)->highlight){
                // create account
                case 0:
                    break;
                // search account
                case 1:
                    break;
                // delete account
                case 2:
                    break;
                case 3: {
                    Win* wPrevMenu = ViewManager_pop_menu(vm);
                    ViewManager_set(vm, WIN_ROLE_MENU, wPrevMenu);
                    Win_delete(winptr);
                    *winptr = vm->windows[vm->focused];
                    break;
                }
                default:
                    break;
            }
            break;
    }
}

void Handle_input_menu_transactions(struct ViewManager* vm, struct Win** winptr, void* context){
    (void)context;
    MenuData* data = (MenuData*)(*winptr)->userdata;
    if(!data) return;

    switch((*winptr)->keypress){
        case KEY_UP:
            if((*winptr)->highlight > 0) --((*winptr)->highlight);
            (*winptr)->dirty = TRUE;
            break;
        case KEY_DOWN:
            if((*winptr)->highlight < data->option_count - 1) ++((*winptr)->highlight);
            (*winptr)->dirty = TRUE;
            break;
        case KEY_RIGHT: {
            // shift focus to viewer window
            ViewManager_focus(vm, WIN_ROLE_VIEWER);
            *winptr = vm->windows[vm->focused];
            break;
        }
        case ENTER:
            switch((*winptr)->highlight){
                case 0:
                    // add transaction
                case 1:
                    // search transaction
                case 2: {
                    // back
                    Win* wPrevMenu = ViewManager_pop_menu(vm);
                    ViewManager_set(vm, WIN_ROLE_MENU, wPrevMenu);
                    Win_delete(winptr);
                    *winptr = vm->windows[vm->focused];
                    break;
                }
                default:
                    break;
            }
            break;
    }
}

void Handle_input_menu_currencies(struct ViewManager* vm, struct Win** winptr, void* context){
    (void)context;
    MenuData* data = (MenuData*)(*winptr)->userdata;
    if(!data) return;

    switch((*winptr)->keypress){
        case KEY_UP:
            if((*winptr)->highlight > 0) --((*winptr)->highlight);
            (*winptr)->dirty = TRUE;
            break;
        case KEY_DOWN:
            if((*winptr)->highlight < data->option_count - 1) ++((*winptr)->highlight);
            (*winptr)->dirty = TRUE;
            break;
        case KEY_RIGHT: {
            // shift focus to viewer window
            ViewManager_focus(vm, WIN_ROLE_VIEWER);
            *winptr = vm->windows[vm->focused];
            break;
        }
        case ENTER:
            switch((*winptr)->highlight){
                case 0:
                    // add currency
                case 1:
                    // remove currency
                case 2: {
                    // back
                    Win* wPrevMenu = ViewManager_pop_menu(vm);
                    ViewManager_set(vm, WIN_ROLE_MENU, wPrevMenu);
                    Win_delete(winptr);
                    *winptr = vm->windows[vm->focused];
                    break;
                }
                default:
                    break;
            }
            break;
    }
}

void Handle_input_menu_transaction_categories(struct ViewManager* vm, struct Win** winptr, void* context){
    (void)context;
    MenuData* data = (MenuData*)(*winptr)->userdata;
    if(!data) return;

    switch((*winptr)->keypress){
        case KEY_UP:
            if((*winptr)->highlight > 0) --((*winptr)->highlight);
            (*winptr)->dirty = TRUE;
            break;
        case KEY_DOWN:
            if((*winptr)->highlight < data->option_count - 1) ++((*winptr)->highlight);
            (*winptr)->dirty = TRUE;
            break;
        case KEY_RIGHT: {
            // shift focus to viewer window
            ViewManager_focus(vm, WIN_ROLE_VIEWER);
            *winptr = vm->windows[vm->focused];
            break;
        }
        case ENTER:
            switch((*winptr)->highlight){
                case 0:
                    // add transaction category
                case 1:
                    // remove transaction category
                case 2: {
                    // back
                    Win* wPrevMenu = ViewManager_pop_menu(vm);
                    ViewManager_set(vm, WIN_ROLE_MENU, wPrevMenu);
                    Win_delete(winptr);
                    *winptr = vm->windows[vm->focused];
                    break;
                }
                default:
                    break;
            }
            break;
    }
}

void Handle_input_viewer(struct ViewManager* vm, struct Win** winptr, void* context){
    (void)context;
    switch((*winptr)->keypress){
        case KEY_LEFT:
            // shift focus to last menu
            ViewManager_focus(vm, WIN_ROLE_MENU);
            *winptr = vm->windows[vm->focused];
            (*winptr)->dirty = TRUE;
            break;
        case KEY_DOWN:
            (*winptr)->dirty = TRUE;
            break;
        case KEY_UP:
            (*winptr)->dirty = TRUE;
            break;
        default:
            break;
    }
}

/***********************************/
/*******SPAWNING FUNCTIONS**********/
/***********************************/

Win* Win_banner(size_t begin_y, size_t begin_x){
    log_message("Win_banner: creating banner window.");
    Win* wBanner = Win_init(NULL, APP_BANNER_LINES+3, APP_SIDE_WIDTH, begin_y, begin_x, WIN_ROLE_BANNER);
    wBanner->draw = Win_draw;
    wBanner->handle_input = NULL;
    wBanner->dirty = TRUE;
    for(size_t i = 0; i < APP_BANNER_LINES; ++i){
        mvwprintw(wBanner->windowptr, i+1, 3, "%s", __APP_BANNER__[i]);
    }

    log_message("Win_banner: OK.");
    return wBanner;
}

Win* Win_viewer(size_t begin_y, size_t begin_x){
    log_message("Win_viewer: creating viewer window.");
    Win* wViewer = Win_init(NULL, __yMax, __xMax-APP_SIDE_WIDTH, begin_y, begin_x, WIN_ROLE_VIEWER);
    wViewer->draw = Win_viewer_draw;
    wViewer->handle_input = Handle_input_viewer;
    wViewer->dirty = TRUE;
    
    keypad(wViewer->windowptr, TRUE);

    ViewerData* wd = (ViewerData*)malloc(sizeof(ViewerData));
    wd->header = NULL;
    wd->header_len = 0;
    wd->table_chunk_prev = NULL;
    wd->table_chunk_current = NULL;
    wd->table_chunk_next = NULL;
    wViewer->userdata = (void*)wd;

    log_message("Win_viewer: OK.");
    return wViewer;
}

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

Win* Win_menu_main(size_t begin_y, size_t begin_x){
    log_message("Win_main_menu: creating main menu.");
    Win* wMainMenu = Win_init(" [+] MAIN MENU ", __yMax-APP_BANNER_LINES-3, APP_SIDE_WIDTH, begin_y, begin_x, WIN_ROLE_MENU);
    wMainMenu->draw = Win_menu_draw;
    wMainMenu->dirty = TRUE;
    wMainMenu->handle_input = Handle_input_menu_main;
    
    keypad(wMainMenu->windowptr, TRUE);

    MenuData* m = (MenuData*)malloc(sizeof(MenuData));
    m->options = __APP_MAIN_MENU_CHOICES;
    m->option_count = APP_MAIN_MENU_CHOICES_NO;
    wMainMenu->userdata = (void*)m;

    log_message("Win_main_menu: OK.");
    return wMainMenu;
}

Win* Win_menu_accounts(size_t begin_y, size_t begin_x){
    Win* wAccountsMenu = Win_init(" [+] ACCOUNTS ", __yMax-APP_BANNER_LINES-3, APP_SIDE_WIDTH, begin_y, begin_x, WIN_ROLE_MENU);
    wAccountsMenu->draw = Win_menu_draw;
    wAccountsMenu->dirty = TRUE;
    wAccountsMenu->handle_input = Handle_input_menu_accounts;

    keypad(wAccountsMenu->windowptr, TRUE);

    MenuData* m = (MenuData*)malloc(sizeof(MenuData));
    m->options = __APP_ACCOUNTS_MENU_CHOICES;
    m->option_count = APP_ACCOUNTS_MENU_CHOICES_NO;
    wAccountsMenu->userdata = (void*)m;

    return wAccountsMenu;
}

Win* Win_menu_transactions(size_t begin_y, size_t begin_x){
    Win* wTransactionsMenu = Win_init(" [+] TRANSACTIONS ", __yMax-APP_BANNER_LINES-3, APP_SIDE_WIDTH, begin_y, begin_x, WIN_ROLE_MENU);
    wTransactionsMenu->draw = Win_menu_draw;
    wTransactionsMenu->dirty = TRUE;
    wTransactionsMenu->handle_input = Handle_input_menu_transactions;

    keypad(wTransactionsMenu->windowptr, TRUE);

    MenuData* m = (MenuData*)malloc(sizeof(MenuData));
    m->options = __APP_TRANSACTIONS_MENU_CHOICES;
    m->option_count = APP_TRANSACTIONS_MENU_CHOICES_NO;
    wTransactionsMenu->userdata = (void*)m;

    return wTransactionsMenu;
}

Win* Win_menu_currencies(size_t begin_y, size_t begin_x){
    Win* wCurrenciesMenu = Win_init(" [+] CURRENCIES ", __yMax-APP_BANNER_LINES-3, APP_SIDE_WIDTH, begin_y, begin_x, WIN_ROLE_MENU);
    wCurrenciesMenu->draw = Win_menu_draw;
    wCurrenciesMenu->dirty = TRUE;
    wCurrenciesMenu->handle_input = Handle_input_menu_currencies;

    keypad(wCurrenciesMenu->windowptr, TRUE);

    MenuData* m = (MenuData*)malloc(sizeof(MenuData));
    m->options = __APP_CURRENCIES_MENU_CHOICES;
    m->option_count = APP_CURRENCIES_MENU_CHOICES_NO;
    wCurrenciesMenu->userdata = (void*)m;

    return wCurrenciesMenu;
}

Win* Win_menu_transaction_categories(size_t begin_y, size_t begin_x){
    Win* wTransactionCategoriesMenu = Win_init(" [+] TRANSACTION CATEGORIES ", __yMax-APP_BANNER_LINES-3, APP_SIDE_WIDTH, begin_y, begin_x, WIN_ROLE_MENU);
    wTransactionCategoriesMenu->draw = Win_menu_draw;
    wTransactionCategoriesMenu->dirty = TRUE;
    wTransactionCategoriesMenu->handle_input = Handle_input_menu_transaction_categories;

    keypad(wTransactionCategoriesMenu->windowptr, TRUE);

    MenuData* m = (MenuData*)malloc(sizeof(MenuData));
    m->options = __APP_TRANSACTION_CATEGORIES_MENU_CHOICES;
    m->option_count = APP_TRANSACTION_CATEGORIES_MENU_CHOICES_NO;
    wTransactionCategoriesMenu->userdata = (void*)m;

    return wTransactionCategoriesMenu;
}

/***********************************/
/*******ViewManager FUNCTIONS*******/
/***********************************/

ViewManager* ViewManager_init(){
    log_message("ViewManager: initializing view manager.");
    
    ViewManager* vm = (ViewManager*)malloc(sizeof(ViewManager));
    if(vm == NULL){
        log_error("ViewManager: failed to allocate memory for view manager.");
        return NULL;
    }

    for(size_t i = 0; i < ROLE_COUNT; ++i) vm->windows[i] = NULL;
    vm->focused = WIN_ROLE_VIEWER;
    vm->menu_stack_top = 0;

    log_message("ViewManager: OK.");
    return vm;
}

void ViewManager_set(ViewManager* vm, WinRole role, Win* winptr){
    const char* r;
    switch (role){
    case WIN_ROLE_BANNER:
        r = "WIN_ROLE_BANNER";
        break;
    case WIN_ROLE_MENU:
        r = "WIN_ROLE_MENU";
        break;
    case WIN_ROLE_POPUP:
        r = "WIN_ROLE_POPUP";
        break;
    case WIN_ROLE_VIEWER:
        r = "WIN_ROLE_VIEWER";
        break;
    default:
        log_error("ViewManager: unknown window role!");
        return;
    }

    vm->windows[role] = winptr;
    if(winptr->draw){
        winptr->draw(winptr);
        wrefresh(winptr->windowptr);
    }
    log_message("ViewManager: set window with label '%s' in role '%s'.", winptr->label, r);
}

void ViewManager_focus(ViewManager* vm, WinRole role){
    const char* r;
    switch (role){
    case WIN_ROLE_BANNER:
        r = "WIN_ROLE_BANNER";
        break;
    case WIN_ROLE_MENU:
        r = "WIN_ROLE_MENU";
        break;
    case WIN_ROLE_POPUP:
        r = "WIN_ROLE_POPUP";
        break;
    case WIN_ROLE_VIEWER:
        r = "WIN_ROLE_VIEWER";
        break;
    default:
        log_error("ViewManager: unknown window role!");
        return;
    }

    vm->focused = role;
    // logic here
    Win_draw(vm->windows[role]);
    wrefresh(vm->windows[vm->focused]->windowptr);

    log_message("ViewManager: set focus on window with role '%s'.", r);
}

void ViewManager_redraw_all(ViewManager* vm){
    log_message("ViewManager: redrawing all windows.");
    for(size_t i = 0; i < ROLE_COUNT; ++i) {
        if(vm->windows[i] != NULL) {
            vm->windows[i]->draw(vm->windows[i]);
            wrefresh(vm->windows[i]->windowptr);
        }
    }
    log_message("ViewManager: OK.");
}

void ViewManager_destroy(ViewManager** vm){
    log_message("ViewManager: destroying view manager.");
    for(size_t i = 0; i < ROLE_COUNT; ++i) Win_delete(&(*vm)->windows[i]);
    free(*vm);
    *vm = NULL;
    log_message("ViewManager: OK.");
}

void ViewManager_listen(ViewManager* vm, InputContext* ctx){
    Win* focused_window = NULL;
    while(*(ctx->loop)){
        focused_window = vm->windows[vm->focused];
        focused_window->keypress = wgetch(focused_window->windowptr);
        if(focused_window && focused_window->handle_input){
            focused_window->handle_input(vm, &focused_window, ctx);

            if(focused_window->dirty){
                if(focused_window->draw){
                    werase(focused_window->windowptr);
                    focused_window->draw(focused_window);
                    wrefresh(focused_window->windowptr);
                }
                focused_window->dirty = FALSE;
            }
        }
    }
}

void ViewManager_push_menu(ViewManager* vm, Win* menu){
    if(vm->menu_stack_top < MENU_STACK_MAX) vm->menu_stack[vm->menu_stack_top++] = menu;
}

Win* ViewManager_pop_menu(ViewManager* vm){
    if(vm->menu_stack_top > 0) return vm->menu_stack[--vm->menu_stack_top];
    return NULL;
}
