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

typedef struct PQ {
    int id;
    int quantum;
    int quantumCheck;
    PCB_deque queue;
} PQ;

PQ* PQ_init_RoundRobin(int id, int quantum);
PQ* PQ_init_FCFS(int id);
PQ* PQ_init_base(int id);

PCB* getQuantumViolator(PQ * this);

bool PQ_isRoundRobin(PQ * this);
bool PQ_isFCFS(PQ * this);
bool PQ_hasWaitingProcess(PQ * this);
bool PQ_hasRunningProcess(PQ * this);
PCB* PQ_hasBurstEndedProcess(PQ * this);

void PQ_systemWideTick(PQ * this);
void PQ_stopRunningProcess(PQ * this);
void PQ_startWaitingProcess(PQ * this);
void PQ_enqueueProcess(PQ * this, PCB* process);

void PQ_printQueue(PQ * this);



#endif	/* PROCESSQUEUE_H */

