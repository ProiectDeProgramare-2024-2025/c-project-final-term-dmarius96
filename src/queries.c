#include "queries.h"

const char* database_filename = "database.db";

/*
 *
 * WARNING:
 * The table names in 'table_names' MUST ABSOLUTELY correspond with the queries in 'queries_create_tables'! 
 * Also - order matters. Some tables depend on others. Don't change it unless you know what you're doing!
 *
*/

const char* table_names[] = {
    "Currencies",
    "TransactionCategories",
    "Accounts",
    "Transactions"
};

const int NUM_TABLES = sizeof(table_names) / sizeof(table_names[0]);

const char* queries_create_tables[] = {
    /* 0 - Currencies */
    "CREATE TABLE Currencies("
    "currencyID INTEGER PRIMARY KEY,"
    "currencyName TEXT NOT NULL UNIQUE);",

    /* 1 - TransactionCategories */
    "CREATE TABLE TransactionCategories("
    "categoryID INTEGER PRIMARY KEY,"
    "categoryName TEXT NOT NULL UNIQUE);",

    /*********************************/

    /* 2 - Accounts */
    "CREATE TABLE Accounts("
    "accountID INTEGER PRIMARY KEY,"
    "accountName TEXT NOT NULL UNIQUE,"
    "dateCreated DATE,"                     // time?
    "currencyType INTEGER,"
    "accountBallance DECIMAL(32, 2) DEFAULT 0.00,"
    "IBAN TEXT,"
    "FOREIGN KEY (currencyType) REFERENCES Currencies(currencyID));",

    /* 3 - Transactions */
    "CREATE TABLE Transactions("
    "transactionID INTEGER PRIMARY KEY AUTOINCREMENT,"
    "transactionDate DATE,"
    "accountID INTEGER,"
    "transactionCategory INTEGER,"
    "transactionAmount DECIMAL(32,2) NOT NULL,"
    "transactionDescription TEXT,"
    "FOREIGN KEY (accountID) REFERENCES Accounts(accountID) ON DELETE CASCADE,"
    "FOREIGN KEY (transactionCategory) REFERENCES TransactionCategories(categoryID));"
};

// WARNING - if used raw, append ';'
const char* queries_fetch_tables[] = {
    /* 0 - Currencies */
    "SELECT * FROM Currencies",

    /* 1 - TransactionCategories */
    "SELECT * FROM TransactionCategories",

    /* 2 - Accounts */
    "SELECT " 
    "Accounts.accountID, Accounts.accountName, Accounts.dateCreated, Currencies.currencyName, Accounts.accountBallance, Accounts.IBAN "
    "FROM Accounts "
    "JOIN Currencies ON Accounts.currencyType = Currencies.currencyID",

    /* 3 - Transactions */
    "SELECT " 
	"Transactions.transactionID, Transactions.transactionDate, Accounts.accountName, TransactionCategories.categoryName, Transactions.transactionAmount, Transactions.transactionDescription "
    "FROM "
	"Transactions "
    "JOIN "
	"Accounts ON Transactions.accountID = Accounts.accountID "
    "JOIN "
	"TransactionCategories ON Transactions.transactionCategory = TransactionCategories.categoryID"
};