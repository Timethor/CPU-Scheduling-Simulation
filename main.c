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

#include "Settings.h"
#include "LineInterpreter.h"
#include "FileReader.h"
#include "VirtualCpu.h"

/*
 * 
 */
int main(int argc, char** argv) {

    printf("INIT SETTINGS!\n");
    Settings* set = setup(argc, argv);

    printf("INIT INPUTSTATE!\n");
    InputState* is = InputState_init();

    printf("INIT FILEREADER!\n");
    FileReader* reader = FileReader_init(is);

    printf("Reading File!\n");
    if (!reader->readLines(reader, set, is)) {
        return EXIT_FAILURE;
    }

    printf("==============INIT VCPU!=============\n");
    VirtualCPU* cpu = VCPU_init(is, set);
    printf("\n==============VCPU TICKING!=============\n========================================\n========================================\n\n");
    bool haveWorkToDo = true;
    while (!PCB_deque_empty(&is->notYetArrived) || haveWorkToDo) {
        haveWorkToDo = VCPU_doClockCycle(cpu, &is->notYetArrived);
        if (!haveWorkToDo) {
            haveWorkToDo = VCPU_doClockCycle(cpu, &is->notYetArrived);
        }
    }
    printf("Simulation Ends\n");

    return EXIT_SUCCESS;
}