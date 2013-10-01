/* 
 * File:   LineInterpreter.h
 * Author: Timethor
 *
 * Created on October 1, 2013, 2:18 PM
 */

#ifndef LINEINTERPRETER_H
#define	LINEINTERPRETER_H

#include "ProcessControlBlock.h"
#include "ProcessQueue.h"
#include "DeviceDescriptor.h"

enum FormatStage {
    FS_TQ,
    FS_PN_NEW,
    FS_PN_AT,
    FS_PN_SCHEDULE
};

typedef struct InputState {
    bool in_comment;
    enum FormatStage stage;
    BurstNode* bn;
    int seen_stage_req;
    bool error_thrown;
    PCB_dequeue notYetArrived;
    DD_dequeue proto_devices;
    PQ_dequeue proto_queues;
    void (*call_back)(struct InputState*, const char*, const char*);
} InputState;

void IS_processInputLine(InputState * this, const char* begin, const char* end);
void InputState_init(InputState* state);

bool IS_hasNonProcessableLine(InputState * this, char* line);

int IS_hasCpuBurst(char* line);
int IS_hasIOBurst(char* line);
int IS_hasIODevice(char* line);

bool IS_processLineForTimeQuantum(InputState * this, char* line);
bool IS_processLineForNewProcess(InputState * this, char* line);
bool IS_processLineForProcessArrival(InputState * this, char* line);
bool IS_processLineForProcessSchedule(InputState * this, char* line);

int IS_hasSubString(char* line, char* needle);
bool IS_isEmptyLine(char* line);
bool IS_isEndMultiLineComment(char* line);
bool IS_isSingleLineComment(char* line);
bool IS_isStartMultiLineComment(char* line);

#endif	/* LINEINTERPRETER_H */

