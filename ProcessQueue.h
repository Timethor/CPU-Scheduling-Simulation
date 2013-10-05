/* 
 * File:   ProcessQueue.h
 * Author: Timethor
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
    PCB_deque queue;
} ProcessQueue;

ProcessQueue* PQ_init_RoundRobin(int id, int quantum);
ProcessQueue* PQ_init_FCFS(int id);
ProcessQueue* PQ_init(int id);
void ProcessQueue_destruct(ProcessQueue* this);

PCB* PQ_getQuantumViolator(ProcessQueue * this);

bool PQ_isRoundRobin(ProcessQueue * this);
bool PQ_isFCFS(ProcessQueue * this);
bool PQ_hasWaitingProcess(ProcessQueue * this);
bool PQ_hasRunningProcess(ProcessQueue * this);
PCB* PQ_hasBurstEndedProcess(ProcessQueue * this);

void PQ_systemWideTick(ProcessQueue * this);
void PQ_stopRunningProcess(ProcessQueue * this);
void PQ_startWaitingProcess(ProcessQueue * this);
void PQ_enqueueProcess(ProcessQueue * this, PCB* process);

void PQ_printQueue(ProcessQueue * this);



#endif	/* PROCESSQUEUE_H */

