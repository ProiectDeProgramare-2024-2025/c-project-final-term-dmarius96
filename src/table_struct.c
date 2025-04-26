#include "table_struct.h"

void free_table(TableData* table){
    log_message("free_table: Dealocating table.");

    if(table == NULL){
        log_message("free_table: Nothing to dealocate.");
        return;
    }

    if(table->rows == NULL){
        log_message("free_table: No rows to dealocate.");
        free(table);
        return;
    }

    for(int i = 0; i < table->numRows; ++i){
        if(table->rows[i].values != NULL){
            for(int j = 0; j < table->rows[i].numColumns; ++j){
                if(table->rows[i].types[j] == CELL_TYPE_TEXT) free(table->rows[i].values[j].textValue);
            }
            free(table->rows[i].values);
            free(table->rows[i].types);
        }else{
            log_warning("free_table: Blank row.");
        }
    }

    free(table->rows);
    free(table);
    log_message("free_table: OK.");
}

TableData* fetch_table_chunk(sqlite3* db, const char* name, const char* query, size_t offset, size_t limit){
    log_message("fetch_table_chunk: Fetching chunk %lu-%lu of table '%s'.", offset, offset+limit, name);
    
    char* chunk_query = sqlite3_mprintf("%s LIMIT %lu OFFSET %lu;", query, limit, offset);
    if(!chunk_query){
        log_error("fetch_table_chunk: Failed to create query.");
        return NULL;
    }

    TableData* chunk = fetch_chunk(db, chunk_query);
    sqlite3_free(chunk_query);
    log_message("fetch_table_chunk: OK - chunk %lu-%lu.", offset, offset+(chunk->numRows));

    return chunk;
}

TableData* fetch_chunk(sqlite3* db, const char* query){
    log_message("fetch_table: Fetching chunk.");

    if(!db){
        log_error("fetch_table: Database is not open!");
        return NULL;
    }

    sqlite3_stmt* stmt;
    if(sqlite3_prepare_v2(db, query, -1, &stmt, 0) != SQLITE_OK){
        log_error("fetch_table: Failed to prepare statement: %s", sqlite3_errmsg(db));
        return NULL;
    }

    TableData* table = (TableData*)malloc(sizeof(TableData));
    if(table == NULL){
        log_error("fetch_table: Memory allocation failed.");
        sqlite3_finalize(stmt);
        return NULL;
    }

    table->rows = NULL;
    table->numRows = 0;
    table->numColumns = sqlite3_column_count(stmt);

    int capacity = 0;

    while(sqlite3_step(stmt) == SQLITE_ROW){
        if(table->numRows >= capacity){
            capacity += 10;
            table->rows = realloc(table->rows, capacity * sizeof(Row));
            if(table->rows == NULL){
                log_error("fetch_table: Table reallocation failed.");
                sqlite3_finalize(stmt);
                free_table(table);
                return NULL;
            }
        }

        Row* row = &table->rows[table->numRows];
        row->values = (CellValue*)malloc(table->numColumns * sizeof(CellValue));
        row->types = (CellType*)malloc(table->numColumns * sizeof(CellType));
        row->numColumns = table->numColumns;

        if(row->values == NULL || row->types == NULL){
            log_error("fetch_table: Failed to allocate memory for table row.");
            sqlite3_finalize(stmt);
            free_table(table);
            return NULL;
        }

        for(int i = 0; i < table->numColumns; ++i){
            switch(sqlite3_column_type(stmt, i)){
                case SQLITE_INTEGER:
                    row->values[i].intValue = sqlite3_column_int(stmt, i);
                    row->types[i] = CELL_TYPE_INT;
                    break;
                case SQLITE_FLOAT:
                    row->values[i].doubleValue = sqlite3_column_double(stmt, i);
                    row->types[i] = CELL_TYPE_DOUBLE;
                    break;
                case SQLITE_TEXT:
                    row->values[i].textValue = strdup((const char*)sqlite3_column_text(stmt, i));
                    row->types[i] = CELL_TYPE_TEXT;
                    break;
                case SQLITE_NULL:
                    row->types[i] = CELL_TYPE_NULL;
                    break;
                default:
                    log_error("fetch_table: Unknown cell type.");
                    break;
            }
        }
        ++(table->numRows);
    }

    sqlite3_finalize(stmt);
    log_message("fetch_table: OK.");
    return table;
}