#ifndef APP_LOG_UTILS_H
#define APP_LOG_UTILS_H

/*
 *      Not properly opening the log file may lead to memory leaks.
 *      If a function does not detect the log file open, it will simply exit,
 *      without deallocating memory. Always make sure the log file is open.
 */

// This is not a const, because it should be changeable from the app settings.
extern char* log_filename;

// Initiates log file
void log_init();

// Sends a regular message with tag INFO
void log_message(const char* format, ...);

// Sends a warning message with tag WARN
void log_warning(const char* format, ...);

// Sends an error message with tag ERROR
void log_error(const char* format, ...);

// Closes the log file
void log_cleanup();

#endif