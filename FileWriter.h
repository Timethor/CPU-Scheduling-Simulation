/* 
 * File:   FileWriter.h
 * Author: Nicholas `Timethor` Rich     G00399909
 *
 * Created on October 4, 2013, 7:11 PM
 */

#ifndef FILEWRITER_H
#define	FILEWRITER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>
#include <err.h>

#include "Settings.h"
#include "LineInterpreter.h"

typedef struct FileWriter {
    void (*call_back)(SimulationState*, const char*, const char*);
    int (*writeLines)(struct FileWriter*, Settings*, SimulationState*);
} FileWriter;

FileWriter* FileWriter_init(SimulationState* istate);
void FW_write_lines(Settings* settings, char* textToWrite);

#endif	/* FILEWRITER_H */

