#include <string.h>

#include "include/interactive.h"
#include "include/noninteractive.h"

int main(int argv, char* argc[]){

    for(int i = 1; i < argv; i++)
        if(!strcmp(argc[i], "-i")){
            runtime_interactive();
            return 0;
        }

    runtime_noninteractive(argv, argc);

    return 0;
    
}