/* 
 * File:   ProcessControlBlock.h
 * Author: Timethor
 *
 * Created on September 21, 2013, 11:18 AM
 */

#ifndef PROCESSCONTROLBLOCK_H
#define	PROCESSCONTROLBLOCK_H

#include <stdio.h>
#include "Dequeue.h"
#include "BurstNode.h"

enum PCBState {
    PCB_NEW,
    PCB_READY,
    PCB_RUNNING,
    PCB_WAITING,
    PCB_TERMINATED
};

// Structure of PCB (process control block)
typedef struct PCB{
    //>>	The ID of the process
    int process_id;
    //>>	The arrival time of the process
    int arrival_time;
    //>>	The enum representation of the state of the process
    enum PCBState state;
    //>>	The amount of time this process spends waiting until completion
    int waiting_time;
    //>>	The internal time that this process completed at
    int completion_time;
    //>>	A linked list of 
    BurstNode* schedule;
} PCB;

DEQUEUE_PROTOTYPE(PCB);
int compare_PCB_Arrival(const PCB ** job1, const PCB** job2);

#endif	/* PROCESSCONTROLBLOCK_H */

