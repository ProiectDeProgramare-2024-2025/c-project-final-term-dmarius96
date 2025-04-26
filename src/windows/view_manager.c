#include "windows/view_manager.h"

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
