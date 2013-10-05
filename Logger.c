
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>

#include "Logger.h"
#include "Settings.h"

bool LOG_canLog(Logger * this, int trace);
void LOG_LogStatement(Logger* this, enum LogLevel level, const char* append, ...);

Logger* Logger_init(char* jobInputName, char* jobOutputName, enum LogLevel level) {
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
    this->currentClock = -1;
    this->clockHasChanged = true;
    switch (level) {
        case 0:
            this->LoggerLevel = LogLevel_OFF;
            break;
        case 1:
            this->LoggerLevel = LogLevel_SEVERE;
            break;
        case 2:
            this->LoggerLevel = LogLevel_WARNING;
            break;
        case 3:
            this->LoggerLevel = LogLevel_INFO;
            break;
        case 4:
            this->LoggerLevel = LogLevel_CONFIG;
            break;
        case 5:
            this->LoggerLevel = LogLevel_FINE;
            break;
        case 6:
            this->LoggerLevel = LogLevel_FINER;
            break;
        case 7:
            this->LoggerLevel = LogLevel_FINEST;
            break;
        case 8:
            this->LoggerLevel = LogLevel_ALL;
            break;
        default:
            this->LoggerLevel = LogLevel_INFO;
            break;
    }
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

void LOG_LogStatement(Logger* this, enum LogLevel level, const char* format, ...) {
    if (this->canLog(this, level)) {
        int currentLen = 0;
        if (this->buffer != NULL) {
            currentLen = strlen(this->buffer);
        }
        char formatBuffer[100];
        va_list args;
        va_start(args, format);
        vsprintf(formatBuffer, format, args);
        va_end(args);
        char formatBuffer2[150];
        char timeBuffer[20];
        if (this->clockHasChanged) {
            if (this->currentClock == -1) {
                sprintf(timeBuffer, "Time = %-4s: ", "Init");
            } else {
                sprintf(timeBuffer, "Time = %-4d: ", this->currentClock);
            }
        } else {
            sprintf(timeBuffer, " ");
        }
        if (this->clockHasChanged) {
            sprintf(formatBuffer2, "\n%-13s %s", timeBuffer, formatBuffer);
            this->clockHasChanged = false;
        } else
            sprintf(formatBuffer2, "%-13s %s", timeBuffer, formatBuffer);
        printf(formatBuffer2);
    }
}

bool LOG_canLog(Logger* this, enum LogLevel level) {
    return this->LoggerLevel <= level ? true : false;
}