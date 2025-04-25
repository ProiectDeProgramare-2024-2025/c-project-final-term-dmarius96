#ifndef APP_DATABASE_H
#define APP_DATABASE_H

#include "table_struct.h"

// Opens the database.
int open_database(sqlite3** db);

// Closes the database.
int close_database(sqlite3* db);

// Checks for the existence of a table. Returns 1 for true, 0 for false.
int table_exists(sqlite3* db, const char* name);

// Create table with given query.
int create_table(sqlite3* db, const char* query);

// Drops table of given name, if it finds it.
int drop_table(sqlite3* db, const char* name);

// Returns true if database schema is right.
int check_schema(sqlite3* db);

// Retrieve the schema of a table.
char* get_table_schema(sqlite3* db, const char* name);

// Initializes database with schema. Overwrites previous schema if it exists.
// WARNING: this function doesn't account for tables outside specified in queries.h.
// TODO: make it so no data loss occurs, if possible.
int init_database(sqlite3* db);

// Returns 1 if table has foreign keys.
int table_has_foreign_keys(sqlite3* db, const char* table_name);


/********************************************************/

// Inserts an unique currency into table Currencies.
int insert_currency(sqlite3* db, const char* raw_currency);

// Removes an existing currency from the table Currencies.
// WARNING: deletion of a currency may break the Accounts table.
int remove_currency(sqlite3* db, const char* raw_currency);

// Checks if a currency type exists.
int currency_exists(sqlite3* db, const char* raw_currency);

// Prints the Currencies table to stdout.
void fetch_currencies(sqlite3* db);

/********************************************************/

// Inserts an unique transaction category into table TransactionCategories.
int insert_transaction_category(sqlite3* db, const char* name);

// Removes an existing transaction category from the table TransactionCategories.
int remove_transaction_category(sqlite3* db, const char* name);

// Checks if a specifed transaction category exists.
int transaction_category_exists(sqlite3* db, const char* name);

// Prints the TransactionCategories table to stdout.
void fetch_transaction_categories(sqlite3* db);

/********************************************************/

// Inserts an account into table Accounts.
// Date format: YYYY-MM-DD
int insert_account(
    sqlite3* db,
    const char* name,
    const char* creation_date,
    const char* raw_currency,
    float ballance,
    const char* IBAN
);

// Removes an account from table Accounts based on ID.
int remove_account_id(sqlite3* db, unsigned int id);

// Removes an account from table Accounts based on name.
int remove_account_name(sqlite3* db, const char* name);

// Removes an account from table Accounts based on creation date.
// Date format: YYYY-MM-DD
int remove_account_date(sqlite3* db, const char* creation_date);

// Removes an account from table Accounts based on IBAN.
// IBAN has to be a valid string (aka not NULL).
// If you want to remove all accounts that don't have IBANs, enter "N/A" for the IBAN.
int remove_account_iban(sqlite3* db, const char* IBAN);

// Checks if a specified account exists.
int account_exists(sqlite3* db, const char* name);

// Prints table Accounts to stdout.
void fetch_accounts(sqlite3* db);

/********************************************************/

// Inserts a transaction into table Transactions
// Date format: YYYY-MM-DD
// Account with specified name must exist.
// Transaction category must exist.
// Amount is negative to specify payment, or positive to specify account fueling.
// Description is optional.
// Function updates account specified.
int insert_transaction(
    sqlite3* db,
    const char* transaction_date,
    const char* account_name,
    const char* transaction_category,
    float amount,
    const char* description
);

// Prints table Transactions to stdout.
void fetch_transactions(sqlite3* db);

/********************************************************/

void free_table(TableData* table);

TableData* fetch_table_chunk(sqlite3* db, const char* name, const char* query, size_t offset, size_t limit);

TableData* fetch_chunk(sqlite3* db, const char* query);

#endif