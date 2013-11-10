/*
 * File:   Logger.c
 * Author: Nicholas `Timethor` Rich     G00399909
 *
 * Created on October 4, 2013, 4:03 PM
 */

#define _GNU_SOURCE
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h> /* mmap() is defined in this header */
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>

#include "Logger.h"
#include "Settings.h"

bool LOG_canLog(Logger * this, int trace);
void LOG_LogStatement(Logger* this, enum LogLevel level, const char* append, ...);
void LOG_dumpToConsole(Logger* this);
char* LOG_dumpToFile(Logger* this);
char* LOG_doPrintResults(Logger* this);

Logger* Logger_init(char* jobInputName, char* jobOutputName, enum LogLevel level, bool fileOnly) {
    Logger* this = malloc(sizeof (*this));
    this->fileOnly = fileOnly;
    this->dumpToConsole = LOG_dumpToConsole;
    this->dumpToFile = LOG_dumpToFile;
    this->doPrintResults = LOG_doPrintResults;
    this->inputHandle = open(jobInputName, O_RDONLY);
    if (this->inputHandle == -1) {
        perror("In File Error: ");
        exit(EXIT_FAILURE);
    }

    this->outputHandle = open(jobOutputName, O_RDWR | O_CREAT | O_TRUNC, 0111 | 0200 | 0444);
    if (this->outputHandle == -1) {
        perror("Out File Error: ");
        exit(EXIT_FAILURE);
    }

    this->filesize = 0;
    this->simulationCompleted = false;
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
    free(this->buffer);
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
                sprintf(timeBuffer, "Time = %-7s: ", "Init");
            } else if (this->simulationCompleted) {
                sprintf(timeBuffer, "Time = %-7s: ", "Summary");
            } else {
                sprintf(timeBuffer, "Time = %-7d: ", this->currentClock);
            }
        } else {
            sprintf(timeBuffer, " ");
        }
        if (this->clockHasChanged || this->simulationCompleted) {
            sprintf(formatBuffer2, "\n%-16s %s", timeBuffer, formatBuffer);
            this->clockHasChanged = false;
        } else
            sprintf(formatBuffer2, "%-16s %s", timeBuffer, formatBuffer);
//        printf(formatBuffer2);
//        usleep(150);
        this->filesize += strlen(formatBuffer2) + 5;
        LogEntry* le = LogEntry_init(formatBuffer2, level);
        LogEntry_deque_pushL(&this->records, le);
    }
}

bool LOG_canLog(Logger* this, enum LogLevel level) {
    return this->LoggerLevel <= level ? true : false;
}

void LOG_dumpToConsole(Logger* this) {
    LogEntry_dequeI it;
    LogEntry_dequeI_init(&it, &this->records);
    LogEntry* le = LogEntry_dequeI_examine(&it);
    while (le != NULL) {
        if (this->canLog(this, le->level))
            printf(le->entry);
        le = LogEntry_dequeI_next(&it);
    }
}

char* LOG_dumpToFile(Logger* this) {
    char* text = (char*) malloc(this->filesize);
    strcpy(text, "\n");
    LogEntry_dequeI it;
    LogEntry_dequeI_init(&it, &this->records);
    LogEntry* le = LogEntry_dequeI_examine(&it);
    while (le != NULL) {
        if (this->canLog(this, le->level))
            strcat(text, le->entry);
        le = LogEntry_dequeI_next(&it);
    }
    return text;
}

char* LOG_doPrintResults(Logger* this) {
    if (!this->fileOnly) {
        this->dumpToConsole(this);
    }
    return this->dumpToFile(this);
}