/* 
 * File:   FileReader.h
 * Author: Timethor
 *
 * Created on September 30, 2013, 11:15 PM
 */

#ifndef FILEREADER_H
#define	FILEREADER_H

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

typedef struct FileReader {
    void (*call_back)(SimulationState*, const char*, const char*);
    int (*readLines)(struct FileReader*, Settings*, SimulationState*);
} FileReader;

FileReader* FileReader_init(SimulationState* istate);
int FR_read_lines(FileReader* this, Settings* settings, SimulationState* cpu);

#endif	/* FILEREADER_H */

