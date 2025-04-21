#include <stdio.h>
#include "flags.h"

void display_help(){

    printf("\n");
    printf("       _____                                        ___       \n");
    printf("      / __(_)___  ____ _____  ________        _____/ (_)      \n");
    printf("     / /_/ / __ \\/ __ `/ __ \\/ ___/ _ \\______/ ___/ / /    \n"); 
    printf("    / __/ / / / / /_/ / / / / /__/  __/_____/ /__/ / /        \n");  
    printf("   /_/ /_/_/ /_/\\__,_/_/ /_/\\___/\\___/      \\___/_/_/     \n\n");

    printf("|---------------------------------------------------------|\n");
    printf("| finance-cli: a command-line interface tool for managing |\n");
    printf("| your finances. Use the \"-i\" flag for interactive mode.  |\n");
    printf("| Otherwise, run in non-interactive mode.                 |\n");
    printf("|---------------------------------------------------------|\n");
    printf("\nNon-interactive mode options:\n");
    printf("> help - display this menu.\n");
    printf("> accounts - see and manage your accounts.\n");
    printf("> transactions - see and manage your transactions.\n");
    printf("> options - see and manage app options.\n");
}

void display_accounts(){
    printf("Option \"accounts\" not implemented!\n");
}

void display_transactions(){
    printf("Option \"transactions\" not implemented!\n");
}

void display_options(){
    printf("Option \"options\" not implemented!\n");
}