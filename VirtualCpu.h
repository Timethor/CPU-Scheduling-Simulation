/* 
 * File:   VirtualCpu.h
 * Author: Timethor
 *
 * Created on September 21, 2013, 11:18 AM
 */

#ifndef VIRTUALCPU_H
#define	VIRTUALCPU_H

#include "Dequeue.h"
#include "DeviceDescriptor.h"
#include "ProcessQueue.h"

// Structure of DD Device Descriptor

enum FormatStage {
    FS_TQ,
    FS_PN_NEW,
    FS_PN_AT,
    FS_PN_SCHEDULE
};

typedef struct InputState {
    bool in_comment;
    enum FormatStage stage;
    int seen_stage_req;
    bool error_thrown;
} InputState;

typedef struct VirtualCPU {
    InputState state;
    int clockTime;
    PCB_dequeue notYetArrived;
    DD_dequeue devices;
    PQ_dequeue queues;
    void (*processInputLine)(struct VirtualCPU*, const char*, const char*);
} VirtualCPU;

void processInputLine(VirtualCPU * this, const char* begin, const char* end);
void initCPU(VirtualCPU* cpu);
void initInputState(InputState* state);

bool hasNonProcessableLine(VirtualCPU * this, char* line);

int hasCpuBurst(char* line);
int hasIOBurst(char* line);
int hasIODevice(char* line);

bool processLineForTimeQuantum(VirtualCPU * this, char* line);
bool processLineForNewProcess(VirtualCPU * this, char* line);
bool processLineForProcessArrival(VirtualCPU * this, char* line);
bool processLineForProcessSchedule(VirtualCPU * this, char* line);

int hasSubString(char* line, char* needle);
bool isEmptyLine(char* line);
bool isEndMultiLineComment(char* line);
bool isSingleLineComment(char* line);
bool isStartMultiLineComment(char* line);

#endif	/* VIRTUALCPU_H */

