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
#include "LineInterpreter.h"

typedef struct VirtualCPU {
    int clockTime;
    DD_dequeue devices;
    PQ_dequeue queues;
} VirtualCPU;

void VCPU_init(VirtualCPU* cpu, InputState* istate);
void VCPU_MergeWithInputState(VirtualCPU* this, InputState* istate);

void VCPU_doClockCycle(VirtualCPU* this, PCB_dequeue* notYetArrived);

#endif	/* VIRTUALCPU_H */

