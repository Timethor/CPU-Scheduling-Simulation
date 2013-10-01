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

typedef struct MyFileReader {
    void (*call_back)(InputState*, const char*, const char*);
    int (*readLines)(struct MyFileReader*, Settings*, InputState*);
} MyFileReader;

void initFileReader(MyFileReader* fr, InputState* cpu);
int read_lines(MyFileReader* this, Settings* settings, InputState* cpu);

#endif	/* FILEREADER_H */

