/* 
 * File:   FileWriter.h
 * Author: Timethor
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

FileWriter* FileReader_init(SimulationState* istate);
int FW_write_lines(FileWriter* this, Settings* settings, SimulationState* cpu);

#endif	/* FILEWRITER_H */
