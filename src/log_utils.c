#include "log_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

char* __app_log_filename = "finance-cli.log";

static FILE* __app_logfile = NULL;

char* getTimestamp() {
    time_t rawtime;
    struct tm *timeinfo;
    char* timestamp = malloc(22 * sizeof(char));

    if (timestamp == NULL) return NULL;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(timestamp, 22, "[%Y-%m-%d %H:%M:%S]", timeinfo);
    return timestamp;
}

void log_init(){
    if(__app_logfile != NULL) return;

    __app_logfile = fopen(__app_log_filename, "a"); // Open log file in append mode
    if(__app_logfile == NULL){
        fprintf(stderr, "ERROR: Failed to open log file: %s\nExiting...\n", __app_log_filename);
        fflush(__app_logfile);
        exit(EXIT_FAILURE);
    }

    char* t = getTimestamp();
    fprintf(__app_logfile, "%s SESSION START\n", t);
    free(t);
    fflush(__app_logfile);
}

void log_message(const char* format, ...){
    if(__app_logfile == NULL){
        fprintf(stderr, "ERROR: Failed to open log file: %s\nExiting...\n", __app_log_filename);
        fflush(__app_logfile);
        exit(EXIT_FAILURE);
    }

    va_list args;
    va_start(args, format);

    char* t = getTimestamp();
    fprintf(__app_logfile, "%s INFO: ", t);
    vfprintf(__app_logfile, format, args);
    fprintf(__app_logfile, "\n");
    free(t);

    va_end(args);
    fflush(__app_logfile);
}

void log_warning(const char* format, ...){
    if(__app_logfile == NULL){
        fprintf(stderr, "ERROR: Failed to open log file: %s\nExiting...\n", __app_log_filename);
        fflush(__app_logfile);
        exit(EXIT_FAILURE);
    }
    
    va_list args;
    va_start(args, format);

    char* t = getTimestamp();
    fprintf(__app_logfile, "%s WARN: ", t);
    vfprintf(__app_logfile, format, args);
    fprintf(__app_logfile, "\n");
    free(t);

    va_end(args);
    fflush(__app_logfile);
}

void log_error(const char* format, ...){
    if(__app_logfile == NULL){
        fprintf(stderr, "ERROR: Failed to open log file: %s\nExiting...\n", __app_log_filename);
        fflush(__app_logfile);
        exit(EXIT_FAILURE);
    }
    
    va_list args;
    va_start(args, format);

    char* t = getTimestamp();
    fprintf(__app_logfile, "%s ERROR: ", t);
    vfprintf(__app_logfile, format, args);
    fprintf(__app_logfile, "\n");
    free(t);

    va_end(args);
    fflush(__app_logfile);
}

void log_cleanup(){
    if(__app_logfile == NULL) return;
    
    char* t = getTimestamp();
    fprintf(__app_logfile, "%s SESSION END\n", t);
    free(t);
    for(int i = 0; i < 50; ++i) fputc('-', __app_logfile);
    fputc('\n', __app_logfile);
    fflush(__app_logfile);
    fclose(__app_logfile);
    __app_logfile = NULL;
}