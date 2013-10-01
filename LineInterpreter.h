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
    int seen_stage_req;
    bool error_thrown;
    PCB_dequeue notYetArrived;
    DD_dequeue proto_devices;
    PQ_dequeue proto_queues;
    void (*call_back)(struct InputState*, const char*, const char*);
} InputState;

void processInputLine(InputState * this, const char* begin, const char* end);
void initInputState(InputState* state);

bool hasNonProcessableLine(InputState * this, char* line);

int hasCpuBurst(char* line);
int hasIOBurst(char* line);
int hasIODevice(char* line);

bool processLineForTimeQuantum(InputState * this, char* line);
bool processLineForNewProcess(InputState * this, char* line);
bool processLineForProcessArrival(InputState * this, char* line);
bool processLineForProcessSchedule(InputState * this, char* line);

int hasSubString(char* line, char* needle);
bool isEmptyLine(char* line);
bool isEndMultiLineComment(char* line);
bool isSingleLineComment(char* line);
bool isStartMultiLineComment(char* line);

#endif	/* LINEINTERPRETER_H */

