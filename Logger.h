/* 
 * File:   Logger.h
 * Author: Timethor
 *
 * Created on October 4, 2013, 4:03 PM
 */

#ifndef LOGGER_H
#define	LOGGER_H

#include "LogEntry.h"

typedef struct CPULogger {
    char* buffer;
    int lastClockTime;
    Log_deque records;
    void (*LogStatement)(struct CPULogger*, const char*);
} CPULogger;


void LogStatement(CPULogger * this, const char* append, ...);
CPULogger* CPUL_init();

#endif	/* LOGGER_H */

