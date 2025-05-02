#include "windows/menu_helpers.h"

#include "database.h"
#include "queries.h"
#include "windows/viewer_tab.h"
#include "windows/windows.h"

// spawners
Win* Win_menu_account_deletion(const Win* winptr) {
    log_message("Win_menu_accounts: creating deletion menu.");
    Win* wDeletionMenu = Win_init(" [+] ACCOUNTS: DELETE ", winptr->height, winptr->width, winptr->begin_y, winptr->begin_x, WIN_ROLE_MENU);
    if (wDeletionMenu == NULL) {
        log_message("Win_menu_accounts: failed to create deletion menu.");
        return NULL;
    }
    wDeletionMenu->draw = Win_menu_draw;
    wDeletionMenu->handle_input = Handle_input_menu_account_deletion;
    wDeletionMenu->destructor = Win_menu_destructor;
    wDeletionMenu->dirty = TRUE;

    keypad(wDeletionMenu->windowptr, TRUE);

    MenuData* m = malloc(sizeof(MenuData));
    if (m == NULL) {
        log_message("Win_menu_accounts: failed to allocate memory for deletion menu data.");
        wDeletionMenu->destructor(&wDeletionMenu);
        return NULL;
    }
    m->options = __APP_ACCOUNTS_MENU_MOD_CHOICES;
    m->option_count = APP_ACCOUNTS_MENU_MOD_CHOICES_NO;
    wDeletionMenu->userdata = (void*)m;

    log_message("Win_menu_accounts: OK.");
    return wDeletionMenu;
}

Win* Win_menu_account_search(const Win* winptr) {
    log_message("Win_menu_accounts: creating search menu.");
    Win* wSearchMenu = Win_init(" [+] ACCOUNTS: SEARCH ", winptr->height, winptr->width, winptr->begin_y, winptr->begin_x, WIN_ROLE_MENU);
    if (wSearchMenu == NULL) {
        log_message("Win_menu_accounts: failed to create search menu.");
        return NULL;
    }
    wSearchMenu->draw = Win_menu_draw;
    wSearchMenu->handle_input = Handle_input_menu_account_search;
    wSearchMenu->destructor = Win_menu_destructor;
    wSearchMenu->dirty = TRUE;

    keypad(wSearchMenu->windowptr, TRUE);

    MenuData* m = malloc(sizeof(MenuData));
    if (m == NULL) {
        log_message("Win_menu_accounts: failed to allocate memory for search menu data.");
        wSearchMenu->destructor(&wSearchMenu);
        return NULL;
    }
    m->options = __APP_ACCOUNTS_MENU_MOD_CHOICES;
    m->option_count = APP_ACCOUNTS_MENU_MOD_CHOICES_NO;
    wSearchMenu->userdata = (void*)m;

    log_message("Win_menu_accounts: OK.");
    return wSearchMenu;
}

// input handlers
void Handle_input_menu_account_deletion(ViewManager* vm, Win** winptr, const void* context) {
    log_message("Handle_input_menu: now handling input for account deletion.");
    (void)context;

    const MenuData* data = (*winptr)->userdata;
    if(data == NULL) {
        log_error("Handle_input_menu: menu data is NULL.");
        return;
    }

    switch((*winptr)->keypress) {
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
        case ENTER:{
            const size_t size = 32;
            switch((*winptr)->highlight) {
            case 0: {
                log_message("Handle_input_menu: deleting by ID.");
                char* endptr;
                char id[size];
                prompt("ID: ", "| [!] PROMPT |", id, (__yMax-5)/2, (__xMax-size)/2, size, vm);
                if (remove_account_id(__db, strtol(id, &endptr, 10)))
                    log_error("Handle_input_menu: failed to remove account.");
                const Win* wViewer = vm->windows[WIN_ROLE_VIEWER];
                ViewerData* vd = wViewer->userdata;
                ViewerTab_populate_tab(&vd->tabs[TABLE_ACCOUNTS], wViewer->height, table_names[2], queries_fetch_tables[2]);
                vm->windows[WIN_ROLE_VIEWER]->draw(vm->windows[WIN_ROLE_VIEWER]);
                wrefresh(vm->windows[WIN_ROLE_VIEWER]->windowptr);
                break;
            }
            case 1:{
                log_message("Handle_input_menu: deleting by name.");
                char name[size];
                prompt("Name: ", "| [!] PROMPT |", name, (__yMax-5)/2, (__xMax-size)/2, size, vm);
                if (remove_account_name(__db, name))
                    log_error("Handle_input_menu: failed to remove account.");
                const Win* wViewer = vm->windows[WIN_ROLE_VIEWER];
                ViewerData* vd = wViewer->userdata;
                ViewerTab_populate_tab(&vd->tabs[TABLE_ACCOUNTS], wViewer->height, table_names[2], queries_fetch_tables[2]);
                vm->windows[WIN_ROLE_VIEWER]->draw(vm->windows[WIN_ROLE_VIEWER]);
                wrefresh(vm->windows[WIN_ROLE_VIEWER]->windowptr);
                break;
            }
            case 2:{
                log_message("Handle_input_menu: deleting by creation date.");
                char date[size];
                prompt("Creation date: ", "| [!] PROMPT |", date, (__yMax-5)/2, (__xMax-size)/2, size, vm);
                if (remove_account_date(__db, date))
                    log_error("Handle_input_menu: failed to remove account.");
                const Win* wViewer = vm->windows[WIN_ROLE_VIEWER];
                ViewerData* vd = wViewer->userdata;
                ViewerTab_populate_tab(&vd->tabs[TABLE_ACCOUNTS], wViewer->height, table_names[2], queries_fetch_tables[2]);
                vm->windows[WIN_ROLE_VIEWER]->draw(vm->windows[WIN_ROLE_VIEWER]);
                wrefresh(vm->windows[WIN_ROLE_VIEWER]->windowptr);
                break;
            }
            case 3:{
                log_message("Handle_input_menu: deleting by IBAN.");
                char iban[size];
                prompt("IBAN: ", "| [!] PROMPT |", iban, (__yMax-5)/2, (__xMax-size)/2, size, vm);
                if (remove_account_iban(__db, iban))
                    log_error("Handle_input_menu: failed to remove account.");
                const Win* wViewer = vm->windows[WIN_ROLE_VIEWER];
                ViewerData* vd = wViewer->userdata;
                ViewerTab_populate_tab(&vd->tabs[TABLE_ACCOUNTS], wViewer->height, table_names[2], queries_fetch_tables[2]);
                vm->windows[WIN_ROLE_VIEWER]->draw(vm->windows[WIN_ROLE_VIEWER]);
                wrefresh(vm->windows[WIN_ROLE_VIEWER]->windowptr);
                break;
            }
            case 4: {
                log_message("Handle_input_menu: returning to previous menu.");
                Win* wPrevMenu = ViewManager_pop_menu(vm);
                ViewManager_set(vm, WIN_ROLE_MENU, wPrevMenu);
                (*winptr)->destructor(winptr);
                *winptr = vm->windows[vm->focused];
                break;
            }
            default: ;
            }
            break;
        }
        default: ;
    }
    log_message("Handle_input_menu: OK.");
}

void Handle_input_menu_account_search(ViewManager* vm, Win** winptr, const void* context) {
    log_message("Handle_input_menu: now handling input for account searching.");
    (void)context;

    const MenuData* data = (*winptr)->userdata;
    if(data == NULL) {
        log_error("Handle_input_menu: menu data is NULL.");
        return;
    }

    switch ((*winptr)->keypress) {
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
        case ENTER: {
            const size_t size = 32;
            switch ((*winptr)->highlight) {
                case 0: {
                    log_message("Handle_input_menu: searching by ID.");
                    char* endptr;
                    char id[size];
                    prompt("ID: ", "| [!] PROMPT |", id, (__yMax-5)/2, (__xMax-size)/2, size, vm);
                    if (remove_account_id(__db, strtol(id, &endptr, 10)))
                        log_error("Handle_input_menu: failed to find account.");
                    const Win* wViewer = vm->windows[WIN_ROLE_VIEWER];
                    ViewerData* vd = wViewer->userdata;
                    ViewerTab_populate_tab(&vd->tabs[TABLE_ACCOUNTS], wViewer->height, table_names[2], queries_fetch_tables[2]);
                    vm->windows[WIN_ROLE_VIEWER]->draw(vm->windows[WIN_ROLE_VIEWER]);
                    wrefresh(vm->windows[WIN_ROLE_VIEWER]->windowptr);
                    break;
                    break;
                }
                case 1: {
                    log_message("Handle_input_menu: searching by name.");
                    break;
                }
                case 2: {
                    log_message("Handle_input_menu: searching by date.");
                    break;
                }
                case 3:{
                    log_message("Handle_input_menu: searching by IBAN.");
                    char iban[size];
                    prompt("IBAN: ", "| [!] PROMPT |", iban, (__yMax-5)/2, (__xMax-size)/2, size, vm);
                    if (remove_account_iban(__db, iban))
                        log_error("Handle_input_menu: failed to find account.");
                    const Win* wViewer = vm->windows[WIN_ROLE_VIEWER];
                    ViewerData* vd = wViewer->userdata;
                    ViewerTab_populate_tab(&vd->tabs[TABLE_ACCOUNTS], wViewer->height, table_names[2], queries_fetch_tables[2]);
                    vm->windows[WIN_ROLE_VIEWER]->draw(vm->windows[WIN_ROLE_VIEWER]);
                    wrefresh(vm->windows[WIN_ROLE_VIEWER]->windowptr);
                    break;
                }
                case 4: {
                    log_message("Handle_input_menu: returning to previous menu.");
                    Win* wPrevMenu = ViewManager_pop_menu(vm);
                    ViewManager_set(vm, WIN_ROLE_MENU, wPrevMenu);
                    (*winptr)->destructor(winptr);
                    *winptr = vm->windows[vm->focused];
                    break;
                }
                default: ;
            }
            break;
        }
        default: ;
    }
}

