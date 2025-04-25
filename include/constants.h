#ifndef APP_CONSTANTS_H
#define APP_CONSTANTS_H

#include <string.h>
#include <sqlite3.h>

extern sqlite3* __db;

extern size_t __xMax;
extern size_t __yMax;

#define ENTER   10
#define ESC     27

#define ROLE_COUNT  4
#define MENU_STACK_MAX  10

#define APP_SIDE_WIDTH              (strlen(__APP_BANNER__[0])+4)
#define APP_BANNER_HEIGHT           9
#define APP_BANNER_LINES            5
extern const char* __APP_BANNER__[APP_BANNER_LINES];


#define APP_MAIN_MENU_CHOICES_NO    6
extern const char* __APP_MAIN_MENU_CHOICES[APP_MAIN_MENU_CHOICES_NO];

#define APP_ACCOUNTS_MENU_CHOICES_NO    4
extern const char* __APP_ACCOUNTS_MENU_CHOICES[APP_ACCOUNTS_MENU_CHOICES_NO];

#define APP_TRANSACTIONS_MENU_CHOICES_NO    3
extern const char* __APP_TRANSACTIONS_MENU_CHOICES[APP_TRANSACTIONS_MENU_CHOICES_NO];

#define APP_CURRENCIES_MENU_CHOICES_NO  3
extern const char* __APP_CURRENCIES_MENU_CHOICES[APP_CURRENCIES_MENU_CHOICES_NO];

#define APP_TRANSACTION_CATEGORIES_MENU_CHOICES_NO  3
extern const char* __APP_TRANSACTION_CATEGORIES_MENU_CHOICES[APP_TRANSACTION_CATEGORIES_MENU_CHOICES_NO];

#endif