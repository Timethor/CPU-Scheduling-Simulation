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
#include "LineInterpreter.h"
#include "Settings.h"

/*
 * 
 */
int main(int argc, char** argv) {
   
    Settings* set = setup(argc,argv);
    
    InputState is;
    InputState_init(&is);
    
    MyFileReader reader;
    MyFileReader_init(&reader, &is);

    printf("Reading File!\n");
    if (!reader.readLines(&reader, set, &is)){
        return EXIT_FAILURE;
    }
    
    VirtualCPU cpu;
    VCPU_init(&cpu);
    
    return EXIT_SUCCESS;
}

