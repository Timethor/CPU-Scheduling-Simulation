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
#include <err.h>

#include "ProcessControlBlock.h"

void print_line(const char* begin, const char* end, char* line_buffer, const int line_size);
int read_lines(const char* fname, int line_size, void (*call_back)(const char*, const char*, char*, const int));

#endif	/* FILETOPROCESSQUEUE_H */

