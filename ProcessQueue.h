/*
 * File:   ProcessQueue.h
 * Author: Nicholas `Timethor` Rich     G00399909
 *
 * Created on September 21, 2013, 11:17 AM
 */

#ifndef PROCESSQUEUE_H
#define	PROCESSQUEUE_H

#include "ProcessControlBlockList.h"

// Structure of PQ (Process Queue)

typedef struct ProcessQueue {
    int id;
    int quantum;
    int quantumCheck;
    bool hasMultiProcessor;
    PCB_deque queue;
} ProcessQueue;

ProcessQueue* PQ_init_RoundRobin(int id, int quantum, bool multiProc);
ProcessQueue* PQ_init_FCFS(int id, bool multiProc);
ProcessQueue* PQ_init(int id, bool multiProc);
void ProcessQueue_destruct(ProcessQueue* this);

PCB* PQ_getQuantumViolator(ProcessQueue * this);

bool PQ_isRoundRobin(ProcessQueue * this);
bool PQ_isFCFS(ProcessQueue * this);
bool PQ_hasWaitingProcess(ProcessQueue * this);
bool PQ_hasRunningProcess(ProcessQueue * this);
PCB_deque* PQ_getWaitingProcesses(ProcessQueue * this);
PCB* PQ_getNextWaitingProcess(ProcessQueue * this);
PCB* PQ_getNextRunningProcess(ProcessQueue * this);
PCB* PQ_hasBurstEndedProcess(ProcessQueue * this);

void PQ_systemWideTick(ProcessQueue * this, Logger* logs);
void PQ_stopRunningProcess(ProcessQueue * this, Logger* logs);
void PQ_startWaitingProcess(ProcessQueue * this, Logger* logs);
void PQ_startWaitingProcess_Specific(ProcessQueue* this, PCB* process, Logger* logs);
void PQ_enqueueProcess(ProcessQueue * this, PCB* process, Logger* logs);

void PQ_printQueue(ProcessQueue * this, Logger* logs, enum LogLevel level);



#endif	/* PROCESSQUEUE_H */

