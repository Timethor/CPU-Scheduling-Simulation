/* 
 * File:   ProcessQueue.h
 * Author: Timethor
 *
 * Created on September 21, 2013, 11:17 AM
 */

#ifndef PROCESSQUEUE_H
#define	PROCESSQUEUE_H

#include "ProcessControlBlock.h"

enum PQState {
    PQ_EMPTY,
};

// Structure of PQ (Process Queue)

typedef struct PQ {
    int id;
    int quantum;
    enum PQState state;
    PCB_dequeue queue;
} PQ;

DEQUEUE_PROTOTYPE(PQ);

void PQ_init_RoundRobin(PQ* queue, int id, int quantum);
void PQ_init_FCFS(PQ* queue, int id);

#endif	/* PROCESSQUEUE_H */

