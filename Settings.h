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
typedef struct Settings{
    char* jobInputName;
    char* jobOutputName;
    bool optfileProvided;
    Logger* logger;
} Settings;

Settings* Settings_init(int argc, char *argv[]);
void Settings_destruct(Settings* this);


#endif	/* SETTINGS_H */

