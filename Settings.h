/* 
 * File:   Settings.h
 * Author: Timethor
 *
 * Created on October 1, 2013, 8:24 AM
 */

#ifndef SETTINGS_H
#define	SETTINGS_H

#include <stdlib.h>

// Structure for the settings for the operation of the Program
typedef struct Settings {
    int jobInput;
    char* jobInputName;
    FILE* jobOutput;
    char* jobOutputName;

    bool debug;
    bool trace;

} Settings;



Settings* setup(int argc, char *argv[]);

#endif	/* SETTINGS_H */

