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


#include "VirtualCpu.h"


typedef struct MyFileReader {
    void (*call_back)(const char*, const char*, VCPUInputState*, VirtualCPU*);
    int (*readLines)(struct MyFileReader*, const char*, VirtualCPU*);
} MyFileReader;

void initFileReader(MyFileReader* fr, void(*call_back)(const char* begin, const char* end, VCPUInputState* state, VirtualCPU*));

int read_lines(MyFileReader* this, const char* fname, VirtualCPU* cpu);

#endif	/* FILEREADER_H */

