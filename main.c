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
#include "Settings.h"

/*
 * 
 */
int main(int argc, char** argv) {
   
    Settings* set = setup(argc,argv);
    
    VirtualCPU cpu;
    initCPU(&cpu);
    
    MyFileReader reader;
    initFileReader(&reader, &cpu);

    printf("Reading File!\n");
    if (!reader.readLines(&reader, set, &cpu)){
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

