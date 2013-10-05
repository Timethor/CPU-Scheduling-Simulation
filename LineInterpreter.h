/* 
 * File:   LineInterpreter.h
 * Author: Timethor
 *
 * Created on October 1, 2013, 2:18 PM
 */

#ifndef LINEINTERPRETER_H
#define	LINEINTERPRETER_H

#include "ProcessControlBlockList.h"
#include "ProcessQueueList.h"
#include "DeviceDescriptorList.h"

enum FormatStage {
    FS_TQ,
    FS_PN_NEW,
    FS_PN_AT,
    FS_PN_SCHEDULE
};

typedef struct SimulationState {
    bool in_comment;
    enum FormatStage stage;
    BurstNode* bn;
    int seen_stage_req;
    bool error_thrown;
    PCB_deque notYetArrived;
    DeviceDescriptor_deque proto_devices;
    ProcessQueue_deque proto_queues;
    void (*call_back_read)(struct SimulationState*, const char*, const char*);
    void (*call_back_write)(struct SimulationState*, const char*, const char*);
} SimulationState;

void SS_processInputLine(SimulationState * this, const char* begin, const char* end);
void SS_processOutputLine(SimulationState * this, const char* begin, const char* end);

SimulationState* SimulationState_init();
void SimulationState_destruct(SimulationState* this);

int SS_hasCpuBurst(char* line);
int SS_hasIOBurst(char* line);
int SS_hasIODevice(char* line);

bool SS_hasNonProcessableLine(SimulationState * this, char* line);

bool SS_processLineForTimeQuantum(SimulationState * this, char* line);
bool SS_processLineForNewProcess(SimulationState * this, char* line);
bool SS_processLineForProcessArrival(SimulationState * this, char* line);
bool SS_processLineForProcessSchedule(SimulationState * this, char* line);

int SS_hasSubString(char* line, char* needle);
bool SS_isEmptyLine(char* line);
bool SS_isEndMultiLineComment(char* line);
bool SS_isSingleLineComment(char* line);
bool SS_isStartMultiLineComment(char* line);

#endif	/* LINEINTERPRETER_H */

