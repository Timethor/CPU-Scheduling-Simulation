/* 
 * File:   ProcessQueue.h
 * Author: Timethor
 *
 * Created on September 21, 2013, 11:17 AM
 */

#ifndef PROCESSQUEUE_H
#define	PROCESSQUEUE_H

#include "ProcessControlBlock.h"

// Structure of PQ (Process Queue)

typedef struct PQ {
    int id;
    int quantum;
    int quantumCheck;
    PCB_dequeue queue;
} PQ;

DEQUEUE_PROTOTYPE(PQ);

PQ* PQ_init_RoundRobin(int id, int quantum);
PQ* PQ_init_FCFS(int id);
PQ* PQ_init_base(int id);

void PQ_SystemWideTick(PQ* this);
void PQ_printQueue(PQ* this);

void PQ_dequeue_SystemWideTick(PQ_dequeue* this);
void PQ_dequeue_ProcArrival(PQ_dequeue* this, PCB* process);
void PQ_dequeue_print(PQ_dequeue* this);

#endif	/* PROCESSQUEUE_H */

