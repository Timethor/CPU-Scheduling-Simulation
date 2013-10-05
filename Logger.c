
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>

#include "Logger.h"
#include "Settings.h"

bool LOG_canLog(Logger * this, int trace);
void LOG_LogStatement(Logger* this, const char* append, ...);

Logger* Logger_init(char* jobInputName, char* jobOutputName, bool debug, enum LogLevel level) {
    Logger* this = malloc(sizeof (*this));

    this->inputHandle = open(jobInputName, O_RDONLY);
    if (this->inputHandle == -1) {
        perror("Error: ");
        exit(EXIT_FAILURE);
    }

    this->outputHandle = -1;
    if (jobOutputName != NULL) {
        this->outputHandle = open(jobOutputName, O_RDWR | O_CREAT | O_TRUNC, NULL);
        if (this->outputHandle == -1) {
            perror("Error: ");
            exit(EXIT_FAILURE);
        }
    }

    this->log = LOG_LogStatement;
    this->canLog = LOG_canLog;
    this->lastClockTime = -1;
    this->debug = debug;
    this->LoggerLevel = level;
    LogEntry_deque_init(&this->records, false, false);
    this->buffer = NULL;
    return this;
}

void Logger_destruct(Logger* this) {
    close(this->inputHandle);
    if (this->outputHandle != -1) {
        close(this->outputHandle);
    }
    LogEntry_deque_freeElements(&this->records);
    free(this);
}

void LOG_LogStatement(Logger* this, const char* format, ...) {
    int currentLen = strlen(this->buffer);
    char formatBuffer[1000];
    va_list args;
    va_start(args, format);
    sprintf(formatBuffer, format, args);
    printf(formatBuffer);
}

bool LOG_canLog(Logger* this, enum LogLevel level) {
    return (this->debug) ? (this->LoggerLevel >= level ? true : false) : false;
}