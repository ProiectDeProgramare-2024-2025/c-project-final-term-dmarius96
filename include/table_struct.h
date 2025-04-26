#ifndef APP_TABLE_STRUCT_H
#define APP_TABLE_STRUCT_H

#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "log_utils.h"

typedef enum{
    CELL_TYPE_NULL,
    CELL_TYPE_INT,
    CELL_TYPE_DOUBLE,
    CELL_TYPE_TEXT
} CellType;

typedef union{
    int intValue;
    double doubleValue;
    char* textValue;
} CellValue;

typedef struct{
    CellValue* values;
    CellType* types;
    int numColumns;
} Row;

typedef struct TableData {
    Row* rows;
    int numRows;
    int numColumns;
} TableData;

/********************************************************/

void free_table(TableData* table);

TableData* fetch_table_chunk(sqlite3* db, const char* name, const char* query, size_t offset, size_t limit);

TableData* fetch_chunk(sqlite3* db, const char* query);

#endif