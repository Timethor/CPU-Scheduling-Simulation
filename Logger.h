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
    char* buffer;
    int lastClockTime;
    enum LogLevel LoggerLevel;
    bool debug;
    LogEntry_deque records;
    void (*log)(struct Logger*, const char*, ...);
    bool (*canLog)(struct Logger*, int);
} Logger;

//>>	== PUBLIC FUNCTIONS ==

//>>	Construct & Destruct
Logger* Logger_init(char* jobInputName, char* jobOutputName, bool debug, enum LogLevel level);
void Logger_destruct(Logger * this);

#endif	/* LOGGER_H */

