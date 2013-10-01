/* 
 * File:   main.c
 * Author: Timethor
 *
 * Created on September 20, 2013, 8:45 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "main.h"
#include "Dequeue.h"
#include "VirtualCpu.h"
#include "MyFileReader.h"

/*
 * 
 */
int main(int argc, char** argv) {
   
    VirtualCPU cpu;
    initCPU(&cpu);
    MyFileReader reader;
    initFileReader(&reader, &cpu);
    

    char filename[30];
    if (argc != 2) {
        printf("Put Usage Detail here\n");
        return 1;
    }
    strcpy(filename, argv[1]);

    printf("Reading File!\n");
    return reader.readLines(&reader, filename, &cpu) ? 0 : 1;

}

