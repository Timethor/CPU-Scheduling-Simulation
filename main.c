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
   
    printf("INIT SETTINGS!\n");
    Settings* set = setup(argc,argv);
    
    printf("INIT INPUTSTATE!\n");
    InputState is;
    InputState_init(&is);
    
    printf("INIT FILEREADER!\n");
    MyFileReader reader;
    MyFileReader_init(&reader, &is);

    printf("Reading File!\n");
    if (!reader.readLines(&reader, set, &is)){
        return EXIT_FAILURE;
    }
    
    
    printf("==============INIT VCPU!=============\n");
    VirtualCPU cpu;
    VCPU_init(&cpu, &is);
    printf("==============VCPU TICKING!=============\n");
    while (!PCB_dequeue_empty(&is.notYetArrived)){
        VCPU_doClockCycle(&cpu, &is.notYetArrived);
    }
    
    return EXIT_SUCCESS;
}

