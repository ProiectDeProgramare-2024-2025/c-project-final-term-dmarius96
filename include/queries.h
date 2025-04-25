#ifndef APP_QUERIES_H
#define APP_QUERIES_H

extern const char* database_filename;

extern const char* table_names[];

extern const int NUM_TABLES;

extern const char* queries_create_tables[];

// WARNING - if used raw, append ';'
extern const char* queries_fetch_tables[];

#endif