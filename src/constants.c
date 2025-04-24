#include "constants.h"

size_t __xMax = 0;
size_t __yMax = 0;

const char* __APP_BANNER__[] = {
    "       _____                                        ___       ",
    "      / __(_)___  ____ _____  ________        _____/ (_)      ",
    "     / /_/ / __ \\/ __ `/ __ \\/ ___/ _ \\______/ ___/ / /    ",
    "    / __/ / / / / /_/ / / / / /__/  __/_____/ /__/ / /        ",
    "   /_/ /_/_/ /_/\\__,_/_/ /_/\\___/\\___/      \\___/_/_/     "
};

const char* __APP_MAIN_MENU_CHOICES[] = { 
    " Accounts ",
    " Transactions ",
    " Currencies ",
    " Transaction Categories ",
    " Options ",
    
    " Exit "
};

const char* __APP_ACCOUNTS_MENU_CHOICES[] = {
    " Create account ",
    " Search account ",
    " Delete account ",

    " Back "
};

const char* __APP_TRANSACTIONS_MENU_CHOICES[] = {
    " Add transaction ",
    " Search transaction ",

    " Back "
};

const char* __APP_CURRENCIES_MENU_CHOICES[] = {
    " Add currency ",
    " Remove currency ",

    " Back "
};

const char* __APP_TRANSACTION_CATEGORIES_MENU_CHOICES[] = {
    " Add transaction category ",
    " Remove transaction category ",

    " Back "
};
