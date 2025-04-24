#include <string.h>

#include "include/interactive.h"
#include "include/noninteractive.h"
#include "include/log_utils.h"

int main(int argv, char* argc[]){

    log_init();

    for(int i = 1; i < argv; i++)
        if(!strcmp(argc[i], "-i")){
            log_message("Running in interactive mode...");
            runtime_interactive();
            log_cleanup();
            return 0;
        }

    log_message("Running in noninteractive mode...");
    runtime_noninteractive(argv, argc);
    log_cleanup();

    return 0;
}