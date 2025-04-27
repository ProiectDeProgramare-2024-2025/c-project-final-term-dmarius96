#include <stdio.h>
#include <string.h>

#include "noninteractive.h"
#include "flags.h"

void runtime_noninteractive(const int argv, char* argc[]){

    if(argv == 1){
        display_help();
        return;
    }

    if(!strcmp(argc[1], "help"))                display_help();
    else if(!strcmp(argc[1], "accounts"))       display_accounts();
    else if(!strcmp(argc[1], "transactions"))   display_transactions();
    else if(!strcmp(argc[1], "options"))        display_options();
    else                                        printf("Unknown option! To see all available commands, use \"help\"!\n");

}