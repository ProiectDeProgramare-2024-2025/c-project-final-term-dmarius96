#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sqlite3.h>

#include "database.h"
#include "log_utils.h"
#include "queries.h"
#include "utilities.h"
#include "table_struct.h"


// TODO: Replace all table name literals with their respective references from queries.c



int open_database(sqlite3** db){
    struct stat st;
    int file_exists = (stat(database_filename, &st) == 0);
    
    if(!file_exists) log_message("open_database: No database file found. Attempting to create one...");

    if(sqlite3_open(database_filename, db) != SQLITE_OK){
        log_error("open_database: Can't open database: %s. Aborting!", sqlite3_errmsg(*db));
        log_cleanup();
        return 1;
    }

    log_message("open_database: Opened database.");
    return 0;
}

int close_database(sqlite3* db){
    if(sqlite3_close(db)) {
        log_error("close_database: Database didn't close smoothly: %s", sqlite3_errmsg(db));
        log_cleanup();
        return 1;
    }
    
    log_message("close_database: Closed database.");
    return 0;
}

int table_exists(sqlite3* db, const char* name){
    log_message("table_exists: Checking if table '%s' exists.", name);

    // is db open?
    if(!db) {
        log_error("table_exists: Database is not open!");
        return 0;
    }

    // construct check query
    char query[] = "SELECT 1 FROM sqlite_master WHERE type='table' AND name=?;";

    sqlite3_stmt* stmt;
    // if query preparation failed, assume table doesn't exist
    if(sqlite3_prepare_v2(db, query, -1, &stmt, 0) != SQLITE_OK) {
        log_error("table_exists: Could not prepare statement: %s", sqlite3_errmsg(db));
        return 0;
    }

    // attempt to bind table name to statement
    if(sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC) != SQLITE_OK){
        log_error("table_exists: Could bind table name to statement: %s", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return 0;
    }

    // if a row is returned, the table surely exists
    int exists = (sqlite3_step(stmt) == SQLITE_ROW);
    sqlite3_finalize(stmt);

    if(exists) log_message("table_exists: Exists!");
    else log_message("table_exists: Does not exist!");

    return exists;
}

int create_table(sqlite3* db, const char* query){
    log_message("create_table: Attempting to create table.");
    // is db open?
    if(!db) {
        log_error("create_table: Database is not open!");
        return 1;
    }

    char* errmsg = NULL;

    if(sqlite3_exec(db, query, 0, 0, &errmsg)){
        log_error("create_table: Failed to create table: %s", errmsg);
        sqlite3_free(errmsg);
        return 1;
    }

    log_message("create_table: OK.");
    return 0;
}

int drop_table(sqlite3* db, const char* name){
    log_message("drop_table: Attempting to drop table '%s'.", name);
    // is db open?
    if(!db) {
        log_error("drop_table: Database is not open!");
        return 1;
    }

    // dynamically allocate string for drop query
    char* drop_query = sqlite3_mprintf("DROP TABLE IF EXISTS %Q;", name);

    char* errmsg = NULL;

    // attempt to execute query
    if(sqlite3_exec(db, drop_query, 0, 0, &errmsg) != SQLITE_OK) {
        log_error("drop_table: Failed to drop table '%s': %s", name, errmsg);
        sqlite3_free(drop_query);
        sqlite3_free(errmsg);
        return 1;
    }

    sqlite3_free(drop_query);
    log_message("drop_table: OK.");

    return 0;
}

char* get_table_schema(sqlite3* db, const char* name){
    // is db open?
    if(!db) {
        log_error("get_table_schema: Database is not open!");
        return NULL;
    }

    // query to get schema for a specific table
    char* query_get_table_schema = sqlite3_mprintf("SELECT sql FROM sqlite_master WHERE type='table' AND name=%Q;", name);
    sqlite3_stmt* stmt;

    // prepare statement
    if(sqlite3_prepare_v2(db, query_get_table_schema, -1, &stmt, 0) != SQLITE_OK) {
        log_error("get_table_schema: Cannot prepare statement: %s", sqlite3_errmsg(db));
        return NULL;
    }

    // step through the statement
    if(sqlite3_step(stmt) != SQLITE_ROW){
        log_error("get_table_schema: Failed to fetch schema for table '%s'.", name);
        sqlite3_finalize(stmt);
        sqlite3_free(query_get_table_schema);
        return NULL;
    }

    // get proper result
    const char* temp = (const char*)sqlite3_column_text(stmt, 0);
    char* result = temp ? strdup(temp) : NULL;

    // cleanup
    sqlite3_finalize(stmt);
    sqlite3_free(query_get_table_schema);

    log_message("get_table_schema: Fetched schema for table '%s'.", name);
    return result;
}

int check_schema(sqlite3* db){
    log_message("check_schema: Checking database schema.");
    // is db open?
    if(!db) {
        log_error("check_schema: Database is not open!");
        return 0;
    }

    for(int i = 0; i < NUM_TABLES; i++){
        // retrieve schema of table 'i'
        char* this_schema = get_table_schema(db, table_names[i]);

        // check if the schema is valid
        if(!this_schema){
            log_message("check_schema: Schema is bad!");
            return 0;
        }

        // compare retrieved schema with right one
        if(strncmp(this_schema, queries_create_tables[i], strlen(queries_create_tables[i])-1)) {
            log_message("check_schema: Schema is bad!");
            return 0;
        }else{
            log_message("check_schema: Schema of table '%s' OK.", table_names[i]);
        }

        free(this_schema);
    }

    log_message("check_schema: Database schema OK.");
    return 1;
}

int init_database(sqlite3* db){
    log_message("init_database: Attempting to (re)initialize the database.");

    // is db open?
    if(!db) {
        log_error("init_database: Database is not open!");
        return 1;
    }

    for(int i = 0; i < NUM_TABLES; i++){
        // checks if table already exists. if it exists, it forcefully drops it.
        if(table_exists(db, table_names[i])) drop_table(db, table_names[i]);
        // create table
        if(create_table(db, queries_create_tables[i])) return 1;
    }

    log_message("init_database: Successfully (re)initialized database!");
    return 0;
}

int table_has_foreign_keys(sqlite3* db, const char* table_name){
    log_message("table_has_foreign_keys: Checking if table '%s' has foreign keys.", table_name);
    
    // is db open?
    if(!db) {
        log_error("table_has_foreign_keys: Database is not open!");
        return 0;
    }

    int result = 0;

    // special query:
    // returns 1 if table has FKs
    // returns 0 if it doesn't
    char* query = sqlite3_mprintf("SELECT CASE WHEN COUNT(*) > 0 THEN 1 ELSE 0 END FROM pragma_foreign_key_list(%Q);", table_name);

    sqlite3_stmt* stmt;
    if(sqlite3_prepare_v2(db, query, -1, &stmt, 0) != SQLITE_OK){
        log_error("table_has_foreign_keys: Could not prepare statement: %s", sqlite3_errmsg(db));
        sqlite3_free(query);
        return 0;
    }

    if(sqlite3_step(stmt) == SQLITE_ROW){
        result = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    sqlite3_free(query);

    if(result) log_message("table_has_foreign_keys: Table '%s' has foreign keys!", table_name);
    else log_message("table_has_foreign_keys: Table '%s' doesn't have foreign keys!", table_name);

    return result;
}

/********************************************************/

int currency_exists(sqlite3* db, const char* raw_currency){
    log_message("currency_exists: Checking if currency '%s' exists.", raw_currency);
    // is db open?
    if(!db) {
        log_error("currency_exists: Database is not open!");
        return 0;
    }

    // ensure proper formatting of currency
    char* currency = to_upper(raw_currency);

    // create query
    char* query = sqlite3_mprintf("SELECT CASE WHEN COUNT(*) > 0 THEN 1 ELSE 0 END FROM Currencies WHERE currencyName = %Q;", currency);

    sqlite3_stmt* stmt;
    if(sqlite3_prepare_v2(db, query, -1, &stmt, 0) != SQLITE_OK){
        log_error("currency_exists: Could not prepare statement: %s", sqlite3_errmsg(db));
        sqlite3_free(query);
        free(currency);
        return 0;
    }

    if(sqlite3_step(stmt) != SQLITE_ROW){
        log_message("currency_exists: Does not exist!");
        sqlite3_finalize(stmt);
        sqlite3_free(query);
        free(currency);
        return 0;
    }

    int result = sqlite3_column_int(stmt, 0);

    if(result) log_message("currency_exists: Exists!");
    else log_message("currency_exists: Does not exist!");

    sqlite3_finalize(stmt);
    sqlite3_free(query);
    free(currency);
    return result;
}

int insert_currency(sqlite3* db, const char* raw_currency){
    log_message("insert_currency: Inserting currency '%s'.", raw_currency);
    // is db open?
    if(!db) {
        log_error("insert_currency: Database is not open!");
        return 1;
    }

    if(currency_exists(db, raw_currency)) return 0;

    // ensure correct formatting of currency name
    char* currency = to_upper(raw_currency);

    // create query for insertion
    char* query = sqlite3_mprintf("INSERT OR IGNORE INTO Currencies VALUES (NULL, %Q);", currency);

    char* errmsg = NULL;
    if(sqlite3_exec(db, query, 0, 0, &errmsg) != SQLITE_OK){
        log_error("insert_currency: Failed to insert currency '%s': %s", currency, errmsg);
        sqlite3_free(query);
        free(currency);
        sqlite3_free(errmsg);
        return 1;    
    }

    log_message("insert_currency: Inserted currency '%s'!", currency);
    sqlite3_free(query);
    free(currency);
    return 0;
}

int remove_currency(sqlite3* db, const char* raw_currency){
    log_message("remove_currency: Removing currency '%s'.", raw_currency);

    // is db open?
    if(!db) {
        log_error("remove_currency: Database is not open!");
        return 1;
    }

    if(!currency_exists(db, raw_currency)) return 0;

    // ensure correct formatting of currency name
    char* currency = to_upper(raw_currency);

    // create query for deletion
    char* query = sqlite3_mprintf("DELETE FROM Currencies WHERE currencyName = %Q;", currency);

    char* errmsg = NULL;
    if(sqlite3_exec(db, query, 0, 0, &errmsg) != SQLITE_OK){
        log_error("remove_currency: Failed to remove currency: %s", errmsg);
        sqlite3_free(query);
        free(currency);
        sqlite3_free(errmsg);
        return 1;    
    }

    log_message("remove_currency: Removed currency '%s'!", currency);

    sqlite3_free(query);
    free(currency);
    return 0;
}

void fetch_currencies(sqlite3* db){
    log_message("fetch_currencies: Fetching currencies from database.");

    // is db open?
    if(!db) {
        log_error("fetch_currencies: Database is not open!");
        return;
    }

    // create and run query
    sqlite3_stmt* stmt;
    const char* query = "SELECT * FROM Currencies;";
    if(sqlite3_prepare_v2(db, query, -1, &stmt, 0) != SQLITE_OK) {
        log_error("fetch_currencies: Couldn't prepare statement: %s", sqlite3_errmsg(db));
        return;
    }

    printf("ID | Currency\n---|---------\n");

    while(sqlite3_step(stmt) == SQLITE_ROW){
        int id = sqlite3_column_int(stmt, 0);
        const char* curr = (const char*)sqlite3_column_text(stmt, 1);

        printf(" %d | %s\n", id, curr);
    }
    printf("\n");

    sqlite3_finalize(stmt);
    log_message("fetch_currencies: OK.");
}

/********************************************************/

int transaction_category_exists(sqlite3* db, const char* name){
    log_message("transaction_category_exists: Checking if transaction category '%s' exists.", name);

    // is db open?
    if(!db) {
        log_error("transaction_category_exists: Database is not open!");
        return 0;
    }

    // create query
    char* query = sqlite3_mprintf("SELECT CASE WHEN COUNT(*) > 0 THEN 1 ELSE 0 END FROM TransactionCategories WHERE categoryName = %Q;", name);

    sqlite3_stmt* stmt;
    if(sqlite3_prepare_v2(db, query, -1, &stmt, 0) != SQLITE_OK){
        log_error("transaction_category_exists: Could not prepare statement: %s", sqlite3_errmsg(db));
        sqlite3_free(query);
        return 0;
    }

    if(sqlite3_step(stmt) != SQLITE_ROW){
        log_message("transaction_category_exists: Does not exist!");
        sqlite3_finalize(stmt);
        sqlite3_free(query);
        return 0;
    }

    int result = sqlite3_column_int(stmt, 0);

    sqlite3_free(query);
    sqlite3_finalize(stmt);
    if(result) log_message("transaction_category_exists: Exists!");
    else log_message("transaction_category_exists: Does not exist!");
    return result;
}

int insert_transaction_category(sqlite3* db, const char* name){
    log_message("insert_transaction_category: Inserting transaction category '%s'.", name);

    // is db open?
    if(!db) {
        log_error("insert_transaction_category: Database is not open!");
        return 1;
    }

    if(transaction_category_exists(db, name)) return 0;

    // create query for insertion
    char* query = sqlite3_mprintf("INSERT OR IGNORE INTO TransactionCategories VALUES (NULL, %Q);", name);

    char* errmsg = NULL;
    if(sqlite3_exec(db, query, 0, 0, &errmsg) != SQLITE_OK){
        log_error("insert_transaction_category: Could not execute query: %s", errmsg);
        sqlite3_free(errmsg);
        sqlite3_free(query);
        return 1;    
    }

    sqlite3_free(query);
    log_message("insert_transaction_category: OK.");
    return 0;
}

int remove_transaction_category(sqlite3* db, const char* name){
    log_message("remove_transaction_category: Removing transaction category '%s'.", name);

    // is db open?
    if(!db) {
        log_error("remove_transaction_category: Database is not open!");
        return 1;
    }

    if(!transaction_category_exists(db, name)) return 0;

    // create query for deletion
    char* query = sqlite3_mprintf("DELETE FROM TransactionCategories WHERE categoryName = %Q;", name);

    char* errmsg = NULL;
    if(sqlite3_exec(db, query, 0, 0, &errmsg) != SQLITE_OK){
        log_error("remove_transaction_category: Failed to execute query: %s", errmsg);
        sqlite3_free(errmsg);
        sqlite3_free(query);
        return 1;    
    }

    sqlite3_free(query);
    log_message("remove_transaction_category: OK.");
    return 0;
}

void fetch_transaction_categories(sqlite3* db){
    log_message("fetch_transaction_categories: Fetching transaction categories.");

    // is db open?
    if(!db) {
        log_error("fetch_transaction_categories: Database is not open!");
        return;
    }

    // create and run query
    sqlite3_stmt* stmt;
    const char* query = "SELECT * FROM TransactionCategories;";
    if(sqlite3_prepare_v2(db, query, -1, &stmt, 0) != SQLITE_OK) {
        log_error("fetch_transaction_categories: Failed to prepare statement: %s", sqlite3_errmsg(db));
        return;
    }

    printf("ID | Category\n---|---------\n");

    while(sqlite3_step(stmt) == SQLITE_ROW){
        int id = sqlite3_column_int(stmt, 0);
        const char* categ = (const char*)sqlite3_column_text(stmt, 1);

        printf(" %d | %s\n", id, categ);
    }
    printf("\n");

    sqlite3_finalize(stmt);
    log_message("fetch_transaction_categories: OK.");
}

/********************************************************/

int account_exists(sqlite3* db, const char* name){
    log_message("account_exists: Checking if account '%s' exists.", name);

    // is db open?
    if(!db) {
        log_error("account_exists: Database is not open!");
        return 0;
    }

    // create query
    char* query = sqlite3_mprintf("SELECT CASE WHEN COUNT(*) > 0 THEN 1 ELSE 0 END FROM Accounts WHERE accountName = %Q;", name);

    sqlite3_stmt* stmt;
    if(sqlite3_prepare_v2(db, query, -1, &stmt, 0) != SQLITE_OK){
        log_error("account_exists: Failed to prepare statement: %s", sqlite3_errmsg(db));
        sqlite3_free(query);
        return 0;
    }

    if(sqlite3_step(stmt) != SQLITE_ROW){
        log_message("account_exists: Does not exist!");
        sqlite3_finalize(stmt);
        sqlite3_free(query);
        return 0;
    }

    int result = sqlite3_column_int(stmt, 0);

    sqlite3_free(query);
    sqlite3_finalize(stmt);

    if(result) log_message("account_exists: Exists!");
    else log_message("account_exists: Does not exist!");
    return result;
}

int insert_account(
    sqlite3* db,
    const char* name,
    const char* creation_date,
    const char* raw_currency,
    float ballance,
    const char* IBAN
){
    log_message("insert_account: Inserting account '%s'.", name);

    // is db open?
    if(!db) {
        log_error("insert_account: Database is not open!");
        return 1;
    }

    if(account_exists(db, name)) return 0;

    // check if currency type actually exists
    if(!currency_exists(db, raw_currency)) return 1;

    // ensure date is okay
    // if date is bad, it defaults to current date
    const char* date_proper = NULL;
    if(!creation_date) {
        log_message("insert_account: No creation date provided; defaulting to current date.");
        date_proper = "DATE('now')";
    }else if(!check_date(creation_date)) {
        log_message("insert_account: Defaulting to current date.");
        date_proper = "DATE('now')";
    }else{
        date_proper = creation_date;
    }

    // ensure correct currency format
    char* currency = to_upper(raw_currency);

    // ensure proper IBAN
    const char* IBAN_proper = NULL;
    if(!IBAN){
        log_message("insert_account: No IBAN provided; defaulting to 'N/A'.");
        IBAN_proper = "N/A";
    }else{
        IBAN_proper = IBAN;
    }

    // create query for insertion
    char* query = sqlite3_mprintf(
        "INSERT INTO Accounts VALUES ("
        "NULL,"     // ID
        "%Q,"       // accountName
        "%s,"       // dateCreated
        "(SELECT 1 FROM Currencies WHERE currencyName = %Q),"   // currencyType
        "%.2f,"     // accountBallance
        "%Q);",     // IBAN
        name, date_proper, currency, ballance, IBAN_proper
    );

    char* errmsg = NULL;
    if(sqlite3_exec(db, query, 0, 0, &errmsg) != SQLITE_OK){
        log_error("insert_account: Failed to execute query: %s", errmsg);
        sqlite3_free(query);
        sqlite3_free(errmsg);
        free(currency);
        return 1;
    }

    sqlite3_free(query);
    free(currency);
    log_message("insert_account: OK.");
    return 0;
}

int remove_account_id(sqlite3* db, unsigned int id){
    log_message("remove_account_id: Removing account with ID '%u'.", id);
    // is db open?
    if(!db) {
        log_error("remove_account_id: Database is not open!");
        return 1;
    }

    // create query for deletion
    char* query = sqlite3_mprintf("DELETE FROM Accounts WHERE accountID = %d;", id);

    char* errmsg = NULL;
    if(sqlite3_exec(db, query, 0, 0, &errmsg) != SQLITE_OK){
        log_error("remove_account_id: Failed to remove account: %s", errmsg);
        sqlite3_free(query);
        sqlite3_free(errmsg);
        return 1;
    }

    sqlite3_free(query);
    log_message("remove_account_id: OK.");
    return 0;
}

int remove_account_name(sqlite3* db, const char* name){
    log_message("remove_account_name: Removing account '%s'.", name);
    // is db open?
    if(!db) {
        log_error("remove_account_name: Database is not open!");
        return 1;
    }

    // create query for deletion
    char* query = sqlite3_mprintf("DELETE FROM Accounts WHERE accountName = %Q;", name);

    char* errmsg = NULL;
    if(sqlite3_exec(db, query, 0, 0, &errmsg) != SQLITE_OK){
        log_error("remove_account_name: Failed to execute query: %s", errmsg);
        sqlite3_free(errmsg);
        sqlite3_free(query);
        return 1;
    }

    sqlite3_free(query);
    log_message("remove_account_name: OK.");
    return 0;
}

int remove_account_date(sqlite3* db, const char* creation_date){
    log_message("remove_account_date: Removing accounts with creation date '%s'.", creation_date);
    // is db open?
    if(!db) {
        log_error("remove_account_date: Database is not open!");
        return 1;
    }

    // ensure date is valid
    if(!check_date(creation_date)) return 1;

    // create query for deletion
    char* query = sqlite3_mprintf("DELETE FROM Accounts WHERE dateCreated = %Q;", creation_date);

    char* errmsg = NULL;
    if(sqlite3_exec(db, query, 0, 0, &errmsg) != SQLITE_OK){
        log_error("remove_account_date: Could not execute query: %s", errmsg);
        sqlite3_free(errmsg);
        sqlite3_free(query);
        return 1;
    }

    sqlite3_free(query);
    log_message("remove_account_date: OK.");
    return 0;
}

int remove_account_iban(sqlite3* db, const char* IBAN){
    // IBAN has to be valid (aka not NULL)
    if(!IBAN) {
        log_message("remove_account_iban: Bad IBAN; cannot remove account(s)!");
        return 1;
    }

    log_message("remove_account_iban: Removing account(s) with IBAN '%s'.", IBAN);
    // is db open?
    if(!db) {
        log_error("remove_account_iban: Database is not open!");
        return 1;
    }

    // create query for deletion
    char* query = sqlite3_mprintf("DELETE FROM Accounts WHERE IBAN = %Q;", IBAN);

    char* errmsg = NULL;
    if(sqlite3_exec(db, query, 0, 0, &errmsg) != SQLITE_OK){
        log_error("remove_account_iban: Could not execute query: %s", errmsg);
        sqlite3_free(errmsg);
        sqlite3_free(query);
        return 1;
    }

    sqlite3_free(query);
    log_message("remove_account_iban: OK.");
    return 0;
}

void fetch_accounts(sqlite3* db){
    log_message("fetch_accounts: Fetching accounts.");
    // is db open?
    if(!db) {
        log_error("fetch_accounts: Database is not open!");
        return;
    }

    // create and run query
    sqlite3_stmt* stmt;
    const char* query = 
    "SELECT " 
    "Accounts.accountID, Accounts.accountName, Accounts.dateCreated, Currencies.currencyName, Accounts.accountBallance, Accounts.IBAN "
    "FROM Accounts "
    "JOIN Currencies ON Accounts.currencyType = Currencies.currencyID;";

    if(sqlite3_prepare_v2(db, query, -1, &stmt, 0) != SQLITE_OK) {
        log_error("fetch_accounts: Could not prepare statement: %s", sqlite3_errmsg(db));
        return;
    }

    printf(" ID |        Name        |    Date    | Currency | Balance |           IBAN           |\n");
    printf("----|--------------------|------------|----------|---------|--------------------------|\n");

    while(sqlite3_step(stmt) == SQLITE_ROW){
        int id = sqlite3_column_int(stmt, 0);
        const char* name = (const char*)sqlite3_column_text(stmt, 1);
        const char* date = (const char*)sqlite3_column_text(stmt, 2);
        const char* curr = (const char*)sqlite3_column_text(stmt, 3);
        float balance = sqlite3_column_double(stmt, 4);
        const char* IBAN = (const char*)sqlite3_column_text(stmt, 5);

        printf(" %-2d | %-18s | %-10s | %-8s | %-7.2f | %-24s |\n", id, name, date, curr, balance, IBAN);
    }
    printf("\n");

    sqlite3_finalize(stmt);
    log_message("fetch_accounts: OK.");
}

/********************************************************/

int insert_transaction(
    sqlite3* db,
    const char* transaction_date,
    const char* account_name,
    const char* transaction_category,
    float amount,
    const char* description
){
    log_message("insert_transaction: Inserting new transaction.");
    if(!db) {
        log_error("insert_transaction: Database is not open!");
        return 1;
    }

    if(!account_exists(db, account_name)) return 1;
    if(!transaction_category_exists(db, transaction_category)) return 1;

    const char* date_proper = NULL;
    char date_buffer[32];

    if(!transaction_date) {
        log_message("insert_transaction: No creation date provided; defaulting to current date.");
        date_proper = "DATE('now')";
    } else if(!check_date(transaction_date)) {
        log_message("insert_transaction: Defaulting to current date.");
        date_proper = "DATE('now')";
    } else {
        snprintf(date_buffer, sizeof(date_buffer), "'%s'", transaction_date);
        date_proper = date_buffer;
    }

    const char* description_proper = ( !description ) ? "No description." : description;

    char* query = sqlite3_mprintf(
        "BEGIN TRANSACTION;"
        "INSERT INTO Transactions (transactionDate, accountID, transactionCategory, transactionAmount, transactionDescription) VALUES ("
        "%s,"
        "(SELECT accountID FROM Accounts WHERE accountName = %Q),"
        "(SELECT categoryID FROM TransactionCategories WHERE categoryName = %Q),"
        "%.2f,"
        "%Q);"
        "UPDATE Accounts SET accountBallance = accountBallance + %.2f WHERE accountName = %Q;"
        "COMMIT;",
        date_proper, account_name, transaction_category, amount, description_proper, amount, account_name
    );

    char* errmsg = NULL;
    if(sqlite3_exec(db, query, 0, 0, &errmsg) != SQLITE_OK){
        log_error("insert_transaction: Failed to execute query: %s", errmsg);
        sqlite3_free(errmsg);
        sqlite3_free(query);
        sqlite3_exec(db, "ROLLBACK;", 0, 0, NULL);
        return 1;
    }

    sqlite3_free(query);
    if(!strcmp(date_proper, "DATE('now')")){
        log_message("insert_transaction: Inserted new transaction on account '%s' of amount '%.2f' today.", account_name, amount);
    } else {
        log_message("insert_transaction: Inserted new transaction on account '%s' of amount '%.2f' on %s.", account_name, amount, date_proper);
    }
    return 0;
}

void fetch_transactions(sqlite3* db){
    log_message("fetch_transactions: Fetching transactions.");
    // is db open?
    if(!db) {
        log_error("fetch_transactions: Database is not open!");
        return;
    }

    // create and run query
    sqlite3_stmt* stmt;
    const char* query = 
    "SELECT " 
	"Transactions.transactionID, Transactions.transactionDate, Accounts.accountName, TransactionCategories.categoryName, Transactions.transactionAmount, Transactions.transactionDescription "
    "FROM "
	"Transactions "
    "JOIN "
	"Accounts ON Transactions.accountID = Accounts.accountID "
    "JOIN "
	"TransactionCategories ON Transactions.transactionCategory = TransactionCategories.categoryID;";

    if(sqlite3_prepare_v2(db, query, -1, &stmt, 0) != SQLITE_OK) {
        log_error("fetch_transactions: Could not prepare statement: %s", sqlite3_errmsg(db));
        return;
    }

    printf(" ID |    Date    |    Account Name    | Category |  Amount  |        Description       |\n");
    printf("----|------------|--------------------|----------|----------|--------------------------|\n");

    while(sqlite3_step(stmt) == SQLITE_ROW){
        int id = sqlite3_column_int(stmt, 0);
        const char* date = (const char*)sqlite3_column_text(stmt, 1);
        const char* name = (const char*)sqlite3_column_text(stmt, 2);
        const char* categ = (const char*)sqlite3_column_text(stmt, 3);
        float amt = sqlite3_column_double(stmt, 4);
        const char* desc = (const char*)sqlite3_column_text(stmt, 5);

        printf(" %-2d | %-10s | %-18s | %-8s | %-8.2f | %-24s |\n", id, date, name, categ, amt, desc);
    }
    printf("\n");

    sqlite3_finalize(stmt);
    log_message("fetch_transactions: OK.");
}

/********************************************************/