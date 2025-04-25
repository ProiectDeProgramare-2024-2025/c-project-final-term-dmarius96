#ifndef APP_TABLE_STRUCT_H
#define APP_TABLE_STRUCT_H

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

#endif