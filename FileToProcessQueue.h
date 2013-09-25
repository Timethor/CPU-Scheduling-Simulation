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

char* print_line(const char* begin, const char* end, const int line_size);
int read_lines(const char* fname, int line_size, char* (*call_back)(const char*, const char*, const int));

#endif	/* FILETOPROCESSQUEUE_H */

