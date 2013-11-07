/*
 * File:   LineInterpreter.h
 * Author: Nicholas `Timethor` Rich     G00399909
 *
 * Created on October 1, 2013, 2:18 PM
 */

#ifndef LINEINTERPRETER_H
#define	LINEINTERPRETER_H

#include "ProcessControlBlockList.h"
#include "ProcessQueueList.h"
#include "DeviceDescriptorList.h"
#include "AddressSpaceList.h"

enum FormatStage {
    FS_TQ,
    FS_PN_NEW,
    FS_PN_AT,
    FS_PN_SCHEDULE,
    FS_M_MS,
    FS_M_MMP,
    FS_M_PP,
    FS_M_PID,
    FS_M_AT,
    FS_M_LIM,
    FS_M_AS,
};


enum MemoryPolicy {
    MP_INF,
    MP_VSP,
    MP_PAG,
    MP_SEG
};

typedef struct SimulationState {
    bool in_comment;
    bool error_thrown;
    bool addFCFSToEnd;
    enum FormatStage stage;
    enum MemoryPolicy policy;   //>>	(MOVE THIS PROP TO MEMMAN)
    int memKiloSize;            //>>	(MOVE THIS PROP TO MEMMAN)
    int policyParams;           //>>	(MOVE THIS PROP TO MEMMAN)
    int seen_stage_req;
    BurstNode* bn;
    PCB_deque notYetArrived;
    DeviceDescriptor_deque proto_devices;
    ProcessQueue_deque proto_queues;
    void (*call_back_read)(struct SimulationState*, const char*, const char*, Logger* logger);
    void (*call_back_write)(struct SimulationState*, const char*, const char*);
} SimulationState;

void SS_processInputLine(SimulationState * this, const char* begin, const char* end, Logger* logger);
void SS_processOutputLine(SimulationState * this, const char* begin, const char* end);

SimulationState* SimulationState_init();
void SimulationState_destruct(SimulationState* this);

int SS_hasCpuBurst(char* line);
int SS_hasIOBurst(char* line);
int SS_hasIODevice(char* line);

bool SS_hasNonProcessableLine(SimulationState * this, char* line, Logger* logger);

//>>	New for Project 2, initialization info
bool SS_processLineForMemorySize(SimulationState * this, char* line, Logger* logger);
bool SS_processLineForMemoryPolicy(SimulationState * this, char* line, Logger* logger);
void SS_determinePolicy(SimulationState * this, char* line, Logger* logger);
bool SS_processLineForPolicyParams(SimulationState * this, char* line, Logger* logger);

//>>	New for Project 2, Process info
bool SS_processLineForLifetime(SimulationState * this, char* line, Logger* logger);
bool SS_processLineForAddressSpace(SimulationState * this, char* line, Logger* logger);

//>>	Old from Project 1, but will use for Project 2
bool SS_processLineForNewProcess(SimulationState * this, char* line, Logger* logger);
bool SS_processLineForProcessArrival(SimulationState * this, char* line, Logger* logger);

bool SS_processLineForTimeQuantum(SimulationState * this, char* line, Logger* logger);
bool SS_processLineForProcessSchedule(SimulationState * this, char* line, Logger* logger);

int SS_hasSubString(char* line, char* needle);
bool SS_isEmptyLine(char* line);
bool SS_isEndMultiLineComment(char* line);
bool SS_isSingleLineComment(char* line);
bool SS_isStartMultiLineComment(char* line);

char* my_strdup(const char *src);
void explode(const char *src, const char *tokens, char ***list, size_t *len);

#endif	/* LINEINTERPRETER_H */

