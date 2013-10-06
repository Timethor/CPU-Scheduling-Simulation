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
#include "FileWriter.h"

/*
 * 
 */
int main(int argc, char** argv) {

    Settings* set = Settings_init(argc, argv);
    void(*Printf) (Logger*, enum LogLevel, const char*, ...) = set->logger->log;
    Printf(set->logger, LogLevel_CONFIG, "=========== INIT  InputState! ===========\n");
    SimulationState* ss = SimulationState_init();

    Printf(set->logger, LogLevel_CONFIG, "=========== INIT  FileReader! ===========\n");
    FileReader* reader = FileReader_init(ss);

    Printf(set->logger, LogLevel_CONFIG, "\tReading Input File!\n");
    if (!reader->readLines(reader, set, ss)) {
        return EXIT_FAILURE;
    }
    Printf(set->logger, LogLevel_CONFIG, "\tCompleted Input File Read!\n");

    Printf(set->logger, LogLevel_CONFIG, "============== INIT V-CPU! ==============\n");
    VirtualCPU* cpu = VirtualCPU_init(ss, set);
    Printf(set->logger, LogLevel_CONFIG, "======= V-CPU Ready and Ticking! ========\n");
    bool haveWorkToDo = true;
    while (!PCB_deque_empty(&ss->notYetArrived) || haveWorkToDo) {
        haveWorkToDo = cpu->doClockCycle(cpu, &ss->notYetArrived);
        if (!haveWorkToDo) {
            haveWorkToDo = cpu->doClockCycle(cpu, &ss->notYetArrived);
        }
    }
    set->logger->simulationCompleted = true;
    Printf(set->logger, LogLevel_CONFIG, "Simulation Ends\n");
    Printf(set->logger, LogLevel_INFO, "Average Waiting Time    : %5d", cpu->getAvgWaitingTime(cpu));
    Printf(set->logger, LogLevel_INFO, "Average Turnaround Time : %5d\n\n", cpu->getAvgTurnAroundTime(cpu));

    char* toFileText = set->logger->doPrintResults(set->logger);
    if (!set->optfileProvided) {
        fprintf(stderr, "\nREMINDER:: Output file not provided, defaulting to %s\n", set->jobOutputName);
    }
    FW_write_lines(set, toFileText);
    free(toFileText);

        SimulationState_destruct(ss);
        FileReader_destruct(reader);
        VirtualCPU_destruct(cpu);

    return EXIT_SUCCESS;
}