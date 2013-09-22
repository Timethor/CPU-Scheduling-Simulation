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
typedef struct PQ{
    int queue_id;
    int quantum;
    enum State device_state;
    dequeue_PCB queue;
} PQ;

DEQUEUE_PROTOTYPE(PQ);

#endif	/* PROCESSQUEUE_H */

