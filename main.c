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

    Settings* set = Settings_init(argc, argv);
    void(*Printf) (Logger*, enum LogLevel, const char*, ...) = set->logger->log;
    Printf(set->logger, LogLevel_CONFIG, "INIT INPUTSTATE!\n");
    SimulationState* ss = SimulationState_init();

    Printf(set->logger, LogLevel_CONFIG, "INIT FILEREADER!\n");
    FileReader* reader = FileReader_init(ss);

    Printf(set->logger, LogLevel_CONFIG, "Reading File!\n");
    if (!reader->readLines(reader, set, ss)) {
        return EXIT_FAILURE;
    }

    Printf(set->logger, LogLevel_CONFIG, "==============INIT VCPU!=============\n");
    VirtualCPU* cpu = VirtualCPU_init(ss, set);
    Printf(set->logger, LogLevel_CONFIG, "\n==============VCPU TICKING!=============\n========================================\n========================================\n\n");
    bool haveWorkToDo = true;
    while (!PCB_deque_empty(&ss->notYetArrived) || haveWorkToDo) {
        haveWorkToDo = cpu->doClockCycle(cpu, &ss->notYetArrived);
        if (!haveWorkToDo) {
            haveWorkToDo = cpu->doClockCycle(cpu, &ss->notYetArrived);
        }
    }
    Printf(set->logger, LogLevel_CONFIG, "Simulation Ends\n");
    
    //>>	Wait time, turnover time cooker here
    
    Settings_destruct(set);
    SimulationState_destruct(ss);
//    FileReader_destruct(reader);
    
    VirtualCPU_destruct(cpu);

    return EXIT_SUCCESS;
}