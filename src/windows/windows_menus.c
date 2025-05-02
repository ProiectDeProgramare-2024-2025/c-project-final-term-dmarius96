#include "windows/windows_menus.h"
#include "windows/viewer_tab.h"
#include "windows/menu_helpers.h"

#include "database.h"
#include "queries.h"
#include "log_utils.h"

/***********************************/
/********DRAWING FUNCTIONS**********/
/***********************************/

void Win_menu_draw(const Win* winptr){
    if (winptr->label)  log_message("Win_menu: drawing menu with label '%s'.", winptr->label);
    else                log_message("Win_menu: drawing menu with anonymous label.");

    const MenuData* m = winptr->userdata;
    if (m == NULL) {
        log_error("Win_menu: menu data is NULL.");
        return;
    }

    if(box(winptr->windowptr, 0, 0)) log_warning("Win_menu: failed to draw border.");
    if (winptr->label != NULL && mvwprintw(winptr->windowptr, 0, 2, winptr->label)) {
        log_error("Win_menu: label is not NULL; something went wrong while printing the label.");
        return;
    }

    const size_t n = m->option_count;

    for (size_t i = 0; i < n-1; ++i) {
        if (i == winptr->highlight) wattron(winptr->windowptr, A_REVERSE);
        if (mvwprintw(winptr->windowptr, ((int)i + 1) * 2, (int)(winptr->width - strlen(m->options[i])) / 2, m->options[i]))
            log_error("Win_menu: something went wrong while printing the menu data.");
        wattroff(winptr->windowptr, A_REVERSE);
    }

    if(n-1 == winptr->highlight) wattron(winptr->windowptr, A_REVERSE);
    if (mvwprintw(winptr->windowptr, (int)winptr->height-3, (int)(APP_SIDE_WIDTH - strlen(m->options[n-1])) / 2, m->options[n-1]))
        log_error("Win_menu: something went wrong while printing the menu data.");
    wattroff(winptr->windowptr, A_REVERSE);

    log_message("Win_menu: OK.");
}

void Win_menu_destructor(Win** winptr){
    if ((*winptr)->label)   log_message("Win_menu: destroying menu window with label '%s'.", (*winptr)->label);
    else                    log_message("Win_menu: destroying menu window with anonymous label.");

    delwin((*winptr)->windowptr);
    if((*winptr)->label) free((*winptr)->label);
    if((*winptr)->userdata) free((*winptr)->userdata);
    free(*winptr);
    *winptr = NULL;

    log_message("Win_menu: OK.");
}

/***********************************/
/*****INPUT HANDLING FUNCTIONS******/
/***********************************/

void Handle_input_menu_main(ViewManager* vm, Win** winptr, const void* context){
    log_message("Handle_input_menu: now handling input for main menu.");

    const MenuData* data = (*winptr)->userdata;
    if(data == NULL) {
        log_error("Handle_input_menu: menu data is NULL.");
        return;
    }

    if (context == NULL) {
        log_error("Handle_input_menu: context is NULL.");
        return;
    }
    const InputContext* ctx = context;

    switch((*winptr)->keypress){
        case KEY_UP:
            if((*winptr)->highlight > 0) --(*winptr)->highlight;
            (*winptr)->dirty = TRUE;
            break;
        case KEY_DOWN:
            if((*winptr)->highlight < data->option_count - 1) ++(*winptr)->highlight;
            (*winptr)->dirty = TRUE;
            break;
        case KEY_RIGHT: {
            log_message("Handle_input_menu: shifting focus to viewer window.");
            ViewManager_focus(vm, WIN_ROLE_VIEWER);
            *winptr = vm->windows[vm->focused];
            break;
        }
        case ENTER:{
            Win* wViewer = vm->windows[WIN_ROLE_VIEWER];
            if (wViewer == NULL) {
                log_error("Handle_input_menu: no viewer window found.");
                return;
            }

            ViewerData* vd = wViewer->userdata;
            if (vd == NULL) {
                log_error("Handle_input_menu: viewer data is NULL.");
                return;
            }

            switch((*winptr)->highlight){
                case 0: {
                    log_message("Handle_input_menu: chosen menu - Accounts.");
                    if(vd->tabs[TABLE_ACCOUNTS] == NULL) {
                        log_message("Handle_input_menu: no data for menu Accounts.");
                        ViewerTab_populate_tab(&vd->tabs[TABLE_ACCOUNTS], wViewer->height, table_names[2], queries_fetch_tables[2]);
                    } else {
                        log_message("Handle_input_menu: data for menu Accounts exists.");
                    }
                    vd->on_focus = TABLE_ACCOUNTS;

                    if(vd->tabs[vd->on_focus]->page_current->numRows == 0){
                        log_warning("Handle_input_menu: the Accounts table is empty.");
                        popup("There is no data!", "| [!] Error |", (__yMax-5)/2, (__xMax-10)/2, vm);
                    }
                    wViewer->dirty = TRUE;

                    // set new menu
                    ViewManager_push_menu(vm, *winptr);
                    ViewManager_set(vm, WIN_ROLE_MENU, Win_menu_accounts((*winptr)->begin_y, (*winptr)->begin_x));
                    *winptr = vm->windows[WIN_ROLE_VIEWER];
                    break;
                }
                case 1: {
                    log_message("Handle_input_menu: chosen menu - Transactions.");
                    if(vd->tabs[TABLE_TRANSACTIONS] == NULL) {
                        log_message("Handle_input_menu: no data for menu Transactions.");
                        ViewerTab_populate_tab(&vd->tabs[TABLE_TRANSACTIONS], wViewer->height, table_names[3], queries_fetch_tables[3]);
                    } else {
                        log_message("Handle_input_menu: data for menu Transactions exists.");
                    }
                    vd->on_focus = TABLE_TRANSACTIONS;

                    if(vd->tabs[vd->on_focus]->page_current->numRows == 0){
                        log_warning("Handle_input_menu: the Transactions table is empty.");
                        popup("There is no data!", "| [!] Error |", (__yMax-5)/2, (__xMax-10)/2, vm);
                    }
                    wViewer->dirty = TRUE;

                    // set new menu
                    ViewManager_push_menu(vm, *winptr);
                    ViewManager_set(vm, WIN_ROLE_MENU, Win_menu_transactions((*winptr)->begin_y, (*winptr)->begin_x));
                    *winptr = vm->windows[WIN_ROLE_VIEWER];
                    break;
                }
                case 2: {
                    log_message("Handle_input_menu: chosen menu - Currencies.");
                    if(vd->tabs[TABLE_CURRENCIES] == NULL) {
                        log_message("Handle_input_menu: no data for menu Currencies.");
                        ViewerTab_populate_tab(&vd->tabs[TABLE_CURRENCIES], wViewer->height, table_names[0], queries_fetch_tables[0]);
                    } else {
                        log_message("Handle_input_menu: data for menu Currencies exists.");
                    }
                    vd->on_focus = TABLE_CURRENCIES;

                    if(vd->tabs[vd->on_focus]->page_current->numRows == 0){
                        log_warning("Handle_input_menu: the Currencies table is empty.");
                        popup("There is no data!", "| [!] Error |", (__yMax-5)/2, (__xMax-10)/2, vm);
                    }
                    wViewer->dirty = TRUE;

                    // set new menu
                    ViewManager_push_menu(vm, *winptr);
                    ViewManager_set(vm, WIN_ROLE_MENU, Win_menu_currencies((*winptr)->begin_y, (*winptr)->begin_x));
                    *winptr = vm->windows[WIN_ROLE_VIEWER];
                    break;
                }
                case 3: {
                    log_message("Handle_input_menu: chosen menu - TransactionCategories.");
                    if(vd->tabs[TABLE_TRANSACTION_CATEGORIES] == NULL) {
                        log_message("Handle_input_menu: no data for menu TransactionCategories.");
                        ViewerTab_populate_tab(&vd->tabs[TABLE_TRANSACTION_CATEGORIES], wViewer->height, table_names[1], queries_fetch_tables[1]);
                    } else {
                        log_message("Handle_input_menu: data for menu TransactionCategories exists.");
                    }
                    vd->on_focus = TABLE_TRANSACTION_CATEGORIES;

                    if(vd->tabs[vd->on_focus]->page_current->numRows == 0){
                        log_warning("Handle_input_menu: the TransactionCategories table is empty.");
                        popup("There is no data!", "| [!] Error |", (__yMax-5)/2, (__xMax-10)/2, vm);
                    }
                    wViewer->dirty = TRUE;

                    // set new menu
                    ViewManager_push_menu(vm, *winptr);
                    ViewManager_set(vm, WIN_ROLE_MENU, Win_menu_transaction_categories((*winptr)->begin_y, (*winptr)->begin_x));
                    *winptr = vm->windows[WIN_ROLE_VIEWER];
                    break;
                }
                case 4: {
                    // options
                    log_message("Handle_input_menu: chosen menu - Options.");
                    popup("Not implemented!", "| [!] Error |", (__yMax-5)/2, (__xMax-10)/2, vm);
                    break;
                }
                case 5:
                    log_message("Handle_input_menu: chosen - Exit; breaking listener.");
                    *ctx->loop = 0;
                    break;
                default:
                    break;
            }
            break;
        }
        default:
            break;
    }

    log_message("Handle_input_menu: OK.");
}

void Handle_input_menu_accounts(ViewManager* vm, Win** winptr, const void* context){
    log_message("Handle_input_menu: now handling input for accounts menu.");
    (void)context;

    const MenuData* data = (*winptr)->userdata;
    if(data == NULL) {
        log_error("Handle_input_menu: menu data is NULL.");
        return;
    }

    switch((*winptr)->keypress){
        case KEY_UP:
            if((*winptr)->highlight > 0) --(*winptr)->highlight;
            (*winptr)->dirty = TRUE;
            break;
        case KEY_DOWN:
            if((*winptr)->highlight < data->option_count - 1) ++(*winptr)->highlight;
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
                case 0: {
                    log_message("Handle_input_menu: creating account.");
                    const size_t size = 32;
                    char* endptr;
                    char acc_name[size];
                    char cdate[size];
                    char currency[size];
                    char balance[size];
                    char iban[size];
                    prompt("Name: ", "| [!] PROMPT |", acc_name, (__yMax-5)/2, (__xMax-size)/2, size, vm);
                    prompt("Creation date: ", "| [!] PROMPT |", cdate, (__yMax-5)/2, (__xMax-size)/2, size, vm);
                    prompt("Currency type: ", "| [!] PROMPT |", currency, (__yMax-5)/2, (__xMax-size)/2, size, vm);
                    prompt("Initial balance: ", "| [!] PROMPT |", balance, (__yMax-5)/2, (__xMax-size)/2, size, vm);
                    prompt("IBAN: ", "| [!] PROMPT |", iban, (__yMax-5)/2, (__xMax-size)/2, size, vm);

                    if (insert_account(__db, acc_name, cdate, currency, strtof(balance, &endptr), iban))
                        log_error("Handle_input_menu: failed to create account.");
                    const Win* wViewer = vm->windows[WIN_ROLE_VIEWER];
                    ViewerData* vd = wViewer->userdata;
                    ViewerTab_populate_tab(&vd->tabs[TABLE_ACCOUNTS], wViewer->height, table_names[2], queries_fetch_tables[2]);
                    vm->windows[WIN_ROLE_VIEWER]->draw(vm->windows[WIN_ROLE_VIEWER]);
                    wrefresh(vm->windows[WIN_ROLE_VIEWER]->windowptr);
                    break;
                }
                // search account
                case 1:
                    log_message("Handle_input_menu: searching account.");
                    ViewManager_push_menu(vm, *winptr);
                    ViewManager_set(vm, WIN_ROLE_MENU, Win_menu_account_search(*winptr));
                    break;
                // delete account
                case 2:
                    log_message("Handle_input_menu: deleting account.");
                    ViewManager_push_menu(vm, *winptr);
                    ViewManager_set(vm, WIN_ROLE_MENU, Win_menu_account_deletion(*winptr));
                    break;
                case 3: {
                    log_message("Handle_input_menu: returning to previous menu.");
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
        default: ;
    }

    log_message("Handle_input_menu: OK.");
}

void Handle_input_menu_transactions(ViewManager* vm, Win** winptr, const void* context){
    (void)context;
    MenuData* data = (*winptr)->userdata;
    if(!data) return;

    switch((*winptr)->keypress){
        case KEY_UP:
            if((*winptr)->highlight > 0) --(*winptr)->highlight;
            (*winptr)->dirty = TRUE;
            break;
        case KEY_DOWN:
            if((*winptr)->highlight < data->option_count - 1) ++(*winptr)->highlight;
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
        default: ;
    }
}

void Handle_input_menu_currencies(ViewManager* vm, Win** winptr, const void* context){
    (void)context;
    MenuData* data = (*winptr)->userdata;
    if(!data) return;

    switch((*winptr)->keypress){
        case KEY_UP:
            if((*winptr)->highlight > 0) --(*winptr)->highlight;
            (*winptr)->dirty = TRUE;
            break;
        case KEY_DOWN:
            if((*winptr)->highlight < data->option_count - 1) ++(*winptr)->highlight;
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
        default: ;
    }
}

void Handle_input_menu_transaction_categories(ViewManager* vm, Win** winptr, const void* context){
    (void)context;
    MenuData* data = (*winptr)->userdata;
    if(!data) return;

    switch((*winptr)->keypress){
        case KEY_UP:
            if((*winptr)->highlight > 0) --(*winptr)->highlight;
            (*winptr)->dirty = TRUE;
            break;
        case KEY_DOWN:
            if((*winptr)->highlight < data->option_count - 1) ++(*winptr)->highlight;
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
        default: ;
    }
}

/***********************************/
/*******SPAWNING FUNCTIONS**********/
/***********************************/

Win* Win_menu_main(const size_t begin_y, const size_t begin_x){
    log_message("Win_main_menu: creating main menu.");
    Win* wMainMenu = Win_init(" [+] MAIN MENU ", __yMax-APP_BANNER_LINES-3, APP_SIDE_WIDTH, begin_y, begin_x, WIN_ROLE_MENU);
    if (wMainMenu == NULL) {
        log_error("Win_main_menu: failed to create main menu.");
        return NULL;
    }
    wMainMenu->draw = Win_menu_draw;
    wMainMenu->handle_input = Handle_input_menu_main;
    wMainMenu->destructor = Win_menu_destructor;
    wMainMenu->dirty = TRUE;
    
    keypad(wMainMenu->windowptr, TRUE);

    MenuData* m = malloc(sizeof(MenuData));
    if (m == NULL) {
        log_error("Win_main_menu: failed to allocate memory for menu data.");
        wMainMenu->destructor(&wMainMenu);
        return NULL;
    }
    m->options = __APP_MAIN_MENU_CHOICES;
    m->option_count = APP_MAIN_MENU_CHOICES_NO;
    wMainMenu->userdata = (void*)m;

    log_message("Win_main_menu: OK.");
    return wMainMenu;
}

Win* Win_menu_accounts(const size_t begin_y, const size_t begin_x){
    log_message("Win_menu_accounts: creating accounts menu.");
    Win* wAccountsMenu = Win_init(" [+] ACCOUNTS ", __yMax-APP_BANNER_LINES-3, APP_SIDE_WIDTH, begin_y, begin_x, WIN_ROLE_MENU);
    if (wAccountsMenu == NULL) {
        log_error("Win_menu_accounts: failed to create accounts menu.");
        return NULL;
    }
    wAccountsMenu->draw = Win_menu_draw;
    wAccountsMenu->handle_input = Handle_input_menu_accounts;
    wAccountsMenu->destructor = Win_menu_destructor;
    wAccountsMenu->dirty = TRUE;

    keypad(wAccountsMenu->windowptr, TRUE);

    MenuData* m = malloc(sizeof(MenuData));
    if (m == NULL) {
        log_error("Win_menu_accounts: failed to allocate memory for menu data.");
        wAccountsMenu->destructor(&wAccountsMenu);
        return NULL;
    }
    m->options = __APP_ACCOUNTS_MENU_CHOICES;
    m->option_count = APP_ACCOUNTS_MENU_CHOICES_NO;
    wAccountsMenu->userdata = (void*)m;

    log_message("Win_menu_accounts: OK.");
    return wAccountsMenu;
}

Win* Win_menu_transactions(const size_t begin_y, const size_t begin_x){
    log_message("Win_menu_transactions: creating transactions menu.");
    Win* wTransactionsMenu = Win_init(" [+] TRANSACTIONS ", __yMax-APP_BANNER_LINES-3, APP_SIDE_WIDTH, begin_y, begin_x, WIN_ROLE_MENU);
    if (wTransactionsMenu == NULL) {
        log_error("Win_menu_transactions: failed to create transactions menu.");
        return NULL;
    }
    wTransactionsMenu->draw = Win_menu_draw;
    wTransactionsMenu->handle_input = Handle_input_menu_transactions;
    wTransactionsMenu->destructor = Win_menu_destructor;
    wTransactionsMenu->dirty = TRUE;

    keypad(wTransactionsMenu->windowptr, TRUE);

    MenuData* m = malloc(sizeof(MenuData));
    if (m == NULL) {
        log_error("Win_menu_transactions: failed to allocate memory for menu data.");
        wTransactionsMenu->destructor(&wTransactionsMenu);
        return NULL;
    }
    m->options = __APP_TRANSACTIONS_MENU_CHOICES;
    m->option_count = APP_TRANSACTIONS_MENU_CHOICES_NO;
    wTransactionsMenu->userdata = (void*)m;

    log_message("Win_menu_transactions: OK.");
    return wTransactionsMenu;
}

Win* Win_menu_currencies(const size_t begin_y, const size_t begin_x){
    log_message("Win_menu_currencies: creating currencies menu.");
    Win* wCurrenciesMenu = Win_init(" [+] CURRENCIES ", __yMax-APP_BANNER_LINES-3, APP_SIDE_WIDTH, begin_y, begin_x, WIN_ROLE_MENU);
    if (wCurrenciesMenu == NULL) {
        log_error("Win_menu_currencies: failed to create currencies menu.");
        return NULL;
    }
    wCurrenciesMenu->draw = Win_menu_draw;
    wCurrenciesMenu->handle_input = Handle_input_menu_currencies;
    wCurrenciesMenu->destructor = Win_menu_destructor;
    wCurrenciesMenu->dirty = TRUE;

    keypad(wCurrenciesMenu->windowptr, TRUE);

    MenuData* m = malloc(sizeof(MenuData));
    if (m == NULL) {
        log_error("Win_menu_currencies: failed to allocate memory for menu data.");
        wCurrenciesMenu->destructor(&wCurrenciesMenu);
        return NULL;
    }
    m->options = __APP_CURRENCIES_MENU_CHOICES;
    m->option_count = APP_CURRENCIES_MENU_CHOICES_NO;
    wCurrenciesMenu->userdata = (void*)m;

    log_message("Win_menu_currencies: OK.");
    return wCurrenciesMenu;
}

Win* Win_menu_transaction_categories(const size_t begin_y, const size_t begin_x){
    log_message("Win_menu_transaction_categories: creating transaction categories menu.");
    Win* wTransactionCategoriesMenu = Win_init(" [+] TRANSACTION CATEGORIES ", __yMax-APP_BANNER_LINES-3, APP_SIDE_WIDTH, begin_y, begin_x, WIN_ROLE_MENU);
    if (wTransactionCategoriesMenu == NULL) {
        log_error("Win_menu_transaction_category: failed to create transaction categories menu.");
        return NULL;
    }
    wTransactionCategoriesMenu->draw = Win_menu_draw;
    wTransactionCategoriesMenu->handle_input = Handle_input_menu_transaction_categories;
    wTransactionCategoriesMenu->destructor = Win_menu_destructor;
    wTransactionCategoriesMenu->dirty = TRUE;

    keypad(wTransactionCategoriesMenu->windowptr, TRUE);

    MenuData* m = malloc(sizeof(MenuData));
    if (m == NULL) {
        log_error("Win_menu_transaction_category: failed to allocate memory for menu data.");
        wTransactionCategoriesMenu->destructor(&wTransactionCategoriesMenu);
        return NULL;
    }
    m->options = __APP_TRANSACTION_CATEGORIES_MENU_CHOICES;
    m->option_count = APP_TRANSACTION_CATEGORIES_MENU_CHOICES_NO;
    wTransactionCategoriesMenu->userdata = (void*)m;

    log_message("Win_menu_transaction_category: OK.");
    return wTransactionCategoriesMenu;
}
