#include "windows/windows_menus.h"

/***********************************/
/********DRAWING FUNCTIONS**********/
/***********************************/

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

void Win_menu_destructor(Win** winptr){
    delwin((*winptr)->windowptr);
    if((*winptr)->label) free((*winptr)->label);
    if((*winptr)->userdata) free((*winptr)->userdata);
    free(*winptr);
    *winptr = NULL;
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
                    wPopup->destructor(&wPopup);
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
                    (*winptr)->destructor(winptr);
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
                    (*winptr)->destructor(winptr);
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
                    (*winptr)->destructor(winptr);
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
                    (*winptr)->destructor(winptr);
                    *winptr = vm->windows[vm->focused];
                    break;
                }
                default:
                    break;
            }
            break;
    }
}

/***********************************/
/*******SPAWNING FUNCTIONS**********/
/***********************************/

Win* Win_menu_main(size_t begin_y, size_t begin_x){
    log_message("Win_main_menu: creating main menu.");
    Win* wMainMenu = Win_init(" [+] MAIN MENU ", __yMax-APP_BANNER_LINES-3, APP_SIDE_WIDTH, begin_y, begin_x, WIN_ROLE_MENU);
    wMainMenu->draw = Win_menu_draw;
    wMainMenu->handle_input = Handle_input_menu_main;
    wMainMenu->destructor = Win_menu_destructor;
    wMainMenu->dirty = TRUE;
    
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
    wAccountsMenu->handle_input = Handle_input_menu_accounts;
    wAccountsMenu->destructor = Win_menu_destructor;
    wAccountsMenu->dirty = TRUE;

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
    wTransactionsMenu->handle_input = Handle_input_menu_transactions;
    wTransactionsMenu->destructor = Win_menu_destructor;
    wTransactionsMenu->dirty = TRUE;

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
    wCurrenciesMenu->handle_input = Handle_input_menu_currencies;
    wCurrenciesMenu->destructor = Win_menu_destructor;
    wCurrenciesMenu->dirty = TRUE;

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
    wTransactionCategoriesMenu->handle_input = Handle_input_menu_transaction_categories;
    wTransactionCategoriesMenu->destructor = Win_menu_destructor;
    wTransactionCategoriesMenu->dirty = TRUE;

    keypad(wTransactionCategoriesMenu->windowptr, TRUE);

    MenuData* m = (MenuData*)malloc(sizeof(MenuData));
    m->options = __APP_TRANSACTION_CATEGORIES_MENU_CHOICES;
    m->option_count = APP_TRANSACTION_CATEGORIES_MENU_CHOICES_NO;
    wTransactionCategoriesMenu->userdata = (void*)m;

    return wTransactionCategoriesMenu;
}
