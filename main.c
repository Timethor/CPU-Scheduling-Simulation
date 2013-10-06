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

/**
 * Due to the generic nature in which this programs input parsing was written, 
 * this program can handle unique situations outside the specification given for
 * the assignment. Time Quantum lines can be omitted to get a single FCFS ready 
 * queue. Any number Time Quantum lines can be added to the input file to simul-
 * ate deeper multi-level queues. Additionally, any number of processes and dev-
 * ices can be invoked in the input file to the complexity of your liking. The 
 * possibilities are only limited by the memory of the machine running this pro-
 * gram.
 */
int main(int argc, char** argv) {

    //>>	Malloc and initialize the elements of Settings
    Settings* set = Settings_init(argc, argv);

    //>>	Grab an easy pointer for printing
    void(*Printf) (Logger*, enum LogLevel, const char*, ...) = set->logger->log;

    Printf(set->logger, LogLevel_CONFIG, "=========== INIT  InputState! ===========\n");
    //>>	Malloc and initialize the elements of SimulationState
    //>>	Simulation state holds the majority of the information for parsing
    //>>	the input file. I was done quite generically as stated above.
    SimulationState* ss = SimulationState_init();

    Printf(set->logger, LogLevel_CONFIG, "=========== INIT  FileReader! ===========\n");
    //>>	Malloc and initialize the elements of ... etc, all _init functions 
    //>>	will perform as described.
    FileReader* reader = FileReader_init(ss);

    Printf(set->logger, LogLevel_CONFIG, "\tReading Input File!\n");
    //>>	This will read the lines from the file designated in `set` and save 
    //>>	the info in `ss`
    if (!reader->readLines(reader, set, ss)) {
        return EXIT_FAILURE;
    }
    Printf(set->logger, LogLevel_CONFIG, "\tCompleted Input File Read!\n");

    Printf(set->logger, LogLevel_CONFIG, "============== INIT V-CPU! ==============\n");

    VirtualCPU* cpu = VirtualCPU_init(ss, set);
    Printf(set->logger, LogLevel_CONFIG, "======= V-CPU Ready and Ticking! ========\n");
    //>>	Here is where the majority of the simulation work is done,
    //>>	doClockCycle will be called until the program thinks it is done. Th-
    //>>	en it will run one more time to ensure it was right.
    bool haveWorkToDo = true;
    while (!PCB_deque_empty(&ss->notYetArrived) || haveWorkToDo) {
        haveWorkToDo = cpu->doClockCycle(cpu, &ss->notYetArrived);
        if (!haveWorkToDo) {
            haveWorkToDo = cpu->doClockCycle(cpu, &ss->notYetArrived);
        }
    }
    //>>	Printing the Calculated values  at the end

    //>>	This flag allows me to print the Time = Summary: tag for pretty output
    set->logger->simulationCompleted = true;
    Printf(set->logger, LogLevel_CONFIG, "Simulation Ends\n");
    Printf(set->logger, LogLevel_INFO, "Average Waiting Time    : %5d", cpu->getAvgWaitingTime(cpu));
    Printf(set->logger, LogLevel_INFO, "Average Turnaround Time : %5d\n\n", cpu->getAvgTurnAroundTime(cpu));

    //>>	This function call simultaneously prints the results to console, if
    //>>	the user doesn`t indicate not to, and returns a huge char* for writing
    //>>	to file
    char* toFileText = set->logger->doPrintResults(set->logger);
    if (!set->optfileProvided) {
        fprintf(stderr, "\nREMINDER:: Output file not provided, defaulting to %s\n", set->jobOutputName);
    }
    //>>	Write the huge char* to file and free it!
    FW_write_lines(set, toFileText);
    free(toFileText);

    //>>	These _destruct functions are the opposite of the _init functions...
    //>>	Anything malloc'ed will be free'd, any lists will be emptied and the
    //>>	Associated object type will have it's _destruct function called, and
    //>>	so on until we hit the atom `object`, in most cases this is 
    //>>	the `BurstNode` struct.
    
    SimulationState_destruct(ss);
    FileReader_destruct(reader);
    VirtualCPU_destruct(cpu);
    //>>	What? no Settings_destruct? that's right! VirtualCPU is built with
    //>>	the _init'ed set and contains a pointer to it. VirtualCPU_destruct
    //>>	has a call to Settings_destruct :D

    return EXIT_SUCCESS;
}