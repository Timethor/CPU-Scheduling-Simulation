/* 
 * File:   VirtualCpu.h
 * Author: Timethor
 *
 * Created on September 21, 2013, 11:18 AM
 */

#ifndef VIRTUALCPU_H
#define	VIRTUALCPU_H

#include "Dequeue.h"
#include "DeviceDescriptor.h"
#include "ProcessQueue.h"
// Structure of DD Device Descriptor

typedef struct VCPU_FTable {
    void (*processInputLine)(char*);
} VCPU_FTable;

typedef struct VirtualCPU {
    int clockTime;
    DD_dequeue devices;
    PQ_dequeue queues;
    VCPU_FTable* func;

} VirtualCPU;

void processInputLine(char* line);
void initCPU(VirtualCPU* cpu);
VCPU_FTable* initVCPU_FTable();

#endif	/* VIRTUALCPU_H */

