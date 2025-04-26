#include "log_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>

char* app_log_filename = "finance-cli.log";

static FILE* app_logfile = NULL;

char* getTimestamp() {
    time_t rawtime;
    char* timestamp = malloc(22 * sizeof(char));

    if (timestamp == NULL) return NULL;
    time(&rawtime);
    const struct tm *timeinfo = localtime(&rawtime);

    strftime(timestamp, 22, "[%Y-%m-%d %H:%M:%S]", timeinfo);
    return timestamp;
}

void log_init(){
    if(app_logfile != NULL) return;

    app_logfile = fopen(app_log_filename, "a"); // Open log file in append mode
    if(app_logfile == NULL){
        fprintf(stderr, "ERROR: Failed to open log file: %s\nExiting...\n", app_log_filename);
        fflush(app_logfile);
        exit(EXIT_FAILURE);
    }

    char* t = getTimestamp();
    fprintf(app_logfile, "%s SESSION START\n", t);
    free(t);
    fflush(app_logfile);
}

void log_message(const char* format, ...){
    if(app_logfile == NULL){
        fprintf(stderr, "ERROR: Failed to open log file: %s\nExiting...\n", app_log_filename);
        fflush(app_logfile);
        exit(EXIT_FAILURE);
    }

    va_list args;
    va_start(args, format);

    char* t = getTimestamp();
    fprintf(app_logfile, "%s INFO: ", t);
    vfprintf(app_logfile, format, args);
    fprintf(app_logfile, "\n");
    free(t);

    va_end(args);
    fflush(app_logfile);
}

void log_warning(const char* format, ...){
    if(app_logfile == NULL){
        fprintf(stderr, "ERROR: Failed to open log file: %s\nExiting...\n", app_log_filename);
        fflush(app_logfile);
        exit(EXIT_FAILURE);
    }
    
    va_list args;
    va_start(args, format);

    char* t = getTimestamp();
    fprintf(app_logfile, "%s WARN: ", t);
    vfprintf(app_logfile, format, args);
    fprintf(app_logfile, "\n");
    free(t);

    va_end(args);
    fflush(app_logfile);
}

void log_error(const char* format, ...){
    if(app_logfile == NULL){
        fprintf(stderr, "ERROR: Failed to open log file: %s\nExiting...\n", app_log_filename);
        fflush(app_logfile);
        exit(EXIT_FAILURE);
    }
    
    va_list args;
    va_start(args, format);

    char* t = getTimestamp();
    fprintf(app_logfile, "%s ERROR: ", t);
    vfprintf(app_logfile, format, args);
    fprintf(app_logfile, "\n");
    free(t);

    va_end(args);
    fflush(app_logfile);
}

void log_cleanup(){
    if(app_logfile == NULL) return;
    
    char* t = getTimestamp();
    fprintf(app_logfile, "%s SESSION END\n", t);
    free(t);
    for(int i = 0; i < 50; ++i) fputc('-', app_logfile);
    fputc('\n', app_logfile);
    fflush(app_logfile);
    fclose(app_logfile);
    app_logfile = NULL;
}