/* 
 * File:   Logger.h
 * Author: Timethor
 *
 * Created on October 4, 2013, 4:03 PM
 */

#ifndef LOGGER_H
#define	LOGGER_H

#include "LogEntryList.h"

typedef struct Logger {
    int inputHandle;
    int outputHandle;
    int fileOnly;
    char* buffer;
    int currentClock;
    bool clockHasChanged;
    bool simulationCompleted;
    enum LogLevel LoggerLevel;
    LogEntry_deque records;
    void (*log)(struct Logger*, enum LogLevel, const char*, ...);
    bool (*canLog)(struct Logger*, int);
    void (*dumpToConsole)(struct Logger*);
    char* (*dumpToFile)(struct Logger*);
    char* (*doPrintResults)(struct Logger*);
    int filesize;
} Logger;

//>>	== PUBLIC FUNCTIONS ==

//>>	Construct & Destruct
Logger* Logger_init(char* jobInputName, char* jobOutputName, enum LogLevel level, bool fileOnly);
void Logger_destruct(Logger * this);



#endif	/* LOGGER_H */

