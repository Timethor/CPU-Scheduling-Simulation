/* 
 * File:   FileToProcessQueue.h
 * Author: Timethor
 *
 * Created on September 21, 2013, 6:39 PM
 */

#ifndef FILETOPROCESSQUEUE_H
#define	FILETOPROCESSQUEUE_H

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


// Structure of DD Device Descriptor

typedef struct readLineState {
    int empty_lines;
    bool in_comment;
} readLineState;

#include "ProcessControlBlock.h"
#include "VirtualCpu.h"


void interpret_line(const char* begin, const char* end, readLineState* state, VirtualCPU* cpu);
int read_lines(const char* fname, void (*call_back)(const char*, const char*, readLineState*, VirtualCPU*), VirtualCPU* cpu);

// Functions to process the lines
bool isEmptyLine(char* line);
bool isSingleLineComment(char* line);
bool isStartMultiLineComment(char* line);
bool isEndMultiLineComment(char* line);

#endif	/* FILETOPROCESSQUEUE_H */

