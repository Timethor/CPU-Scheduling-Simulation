/* 
 * File:   ProcessControlBlock.h
 * Author: Timethor
 *
 * Created on September 21, 2013, 11:18 AM
 */

#ifndef PROCESSCONTROLBLOCK_H
#define	PROCESSCONTROLBLOCK_H

#include <stdio.h>
#include "BurstNodeList.h"
#include "Logger.h"

enum PCBState {
    PCB_NEW,
    PCB_RUNNING,
    PCB_WAITING,
    PCB_BURST_FINISHED,
    PCB_TERMINATED
};

// Structure of PCB (process control block)

typedef struct PCB {
    //>>	The ID of the process
    int id;
    //>>	The arrival time of the process
    int arrival_time;
    //>>	The enum representation of the state of the process
    enum PCBState state;
    //>>	The amount of time this process spends waiting until completion
    int waiting_time;
    //>>	The internal time that this process completed at
    int turnaround_time;
    //>>	A linked list of burstNodes
    BurstNode_deque schedule;
} PCB;

PCB* PCB_init(int id);
void PCB_destruct(PCB* this);

char* PCB_toString(PCB* pcb, char* buffer);
void PCB_SystemWideTick(PCB * this, Logger* logs, bool inDevice);
void PCB_checkProcessTermination(PCB * this);

#endif	/* PROCESSCONTROLBLOCK_H */

