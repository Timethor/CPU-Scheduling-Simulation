/* 
 * File:   LogEntry.h
 * Author: Nicholas `Timethor` Rich     G00399909
 *
 * Created on October 4, 2013, 4:25 PM
 */

#ifndef LOGENTRY_H
#define	LOGENTRY_H

#include <limits.h>
#include <stdbool.h>


//>>	This is a very rudimentary mockery of the Java.util.logging.level
//>>	LogLevels ... it works, that's all that counts xD
enum LogLevel {
    LogLevel_ALL = INT_MIN,
    LogLevel_FINEST = 300,
    LogLevel_FINER = 400,
    LogLevel_FINE = 500,
    LogLevel_CONFIG = 700,
    LogLevel_INFO = 800,
    LogLevel_SEVERE = 1000,
    LogLevel_WARNING = 900,
    LogLevel_OFF = INT_MAX,
};

typedef struct LogEntry {
    char* entry;
    enum LogLevel level;
} LogEntry;

//>>	Construct / Destruct
LogEntry* LogEntry_init(char* entry, enum LogLevel level);
void LogEntry_destruct(LogEntry * this);

#endif	/* LOGENTRY_H */

