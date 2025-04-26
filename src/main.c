#include <string.h>

#include "interactive.h"
#include "noninteractive.h"
#include "log_utils.h"
#include "database.h"
#include "constants.h"

int main(const int argv, char* argc[]){

    log_init();
    open_database(&__db);
    if(!check_schema(__db)) init_database(__db);

    for(int i = 1; i < argv; i++)
        if(!strcmp(argc[i], "-i")){
            log_message("Running in interactive mode...");
            runtime_interactive();
            close_database(__db);
            log_cleanup();
            return 0;
        }

    log_message("Running in noninteractive mode...");
    runtime_noninteractive(argv, argc);

    close_database(__db);
    log_cleanup();
}