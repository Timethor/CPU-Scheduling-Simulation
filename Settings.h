/* 
 * File:   Settings.h
 * Author: Timethor
 *
 * Created on October 1, 2013, 8:24 AM
 */

#ifndef SETTINGS_H
#define	SETTINGS_H

#include <stdlib.h>
#include <stdbool.h>

#include "Logger.h"

// Structure for the settings for the operation of the Program
typedef struct Settings {
    int jobInput;
    char* jobInputName;
    FILE* jobOutput;
    char* jobOutputName;
    bool debug;
    int trace;
    bool (*canLog)(struct Settings*, int);
    CPULogger* log;
} Settings;

Settings* setup(int argc, char *argv[]);
bool canLog(Settings * this, int trace);

#endif	/* SETTINGS_H */

