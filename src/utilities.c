#include <stdlib.h>
#include <string.h>

#include "utilities.h"
#include "log_utils.h"

int stoi(const char* str){
    int result = 0;
    for(const char *p = str; *p != '\0'; p++){
        if(*p >= '0' && *p <= '9') result = result*10 + *p-'0';
    }
    return result;
}

char* to_upper(const char* str){
    char* result = malloc((strlen(str)+1)*sizeof(char));
    
    size_t i;

    for(i = 0; str[i] != '\0'; i++){
        if(str[i] >= 'a' && str[i] <= 'z') result[i] = str[i]-'a'+'A';
        else result[i] = str[i];
    }

    // make sure to add the null terminator
    result[i] = str[i];

    return result;
}

int check_date(const char* str){
    log_message("check_date: Checking if provided date is alright.");

    if(*(str+4) != '-' && *(str+7) != '-') {
        log_message("check_date: Date is bad!");
        return 0;
    }

    // check year
    for(size_t i = 0; i < 4; i++){
        if(*(str+i) < '0' || *(str+i) > '9') {
            log_message("check_date: Date is bad!");
            return 0;
        }
    }

    // check month
    char* temp = malloc(3*sizeof(char));
    temp[0] = *(str+5);
    temp[1] = *(str+6);
    temp[2] = '\0';
    const int mo = stoi(temp);
    free(temp);
    if(mo < 1 || mo > 12) {
        log_message("check_date: Date is bad!");
        return 0;
    }
    
    // check day
    temp = (char*)malloc(3*sizeof(char));
    temp[0] = *(str+8);
    temp[1] = *(str+9);
    temp[2] = '\0';
    const int day = stoi(temp);
    free(temp);
    if(day < 1 || day > 31) {
        log_message("check_date: Date is bad!");
        return 0;
    }

    log_message("check_date: Date OK.");
    return 1;
}