/* 
 * File:   VirtualCpu.h
 * Author: Timethor
 *
 * Created on September 21, 2013, 11:18 AM
 */

#ifndef VIRTUALCPU_H
#define	VIRTUALCPU_H

#include <stdbool.h>

#include "DeviceDescriptorList.h"
#include "ProcessQueueList.h"
#include "LineInterpreter.h"
#include "Settings.h"

typedef struct VirtualCPU {
    int clockTime;
    DD_deque devices;
    PQ_deque queues;
    PCB_deque terminated;
    Settings* settings;
} VirtualCPU;

VirtualCPU* VCPU_init(InputState* istate, Settings* settings);
void VCPU_MergeWithInputState(VirtualCPU * this, InputState* istate);
bool VCPU_canEnd(VirtualCPU * this);
bool VCPU_checkDeviceQueuesClear(VirtualCPU * this);
bool VCPU_doClockCycle(VirtualCPU * this, PCB_deque* notYetArrived);

PQ* VCPU_getHighestWaitingProcessQueue(VirtualCPU * this);
PQ* VCPU_getHighestRunningProcessQueue(VirtualCPU * this);

void VCPU_doPreemptProcess(VirtualCPU * this);
void VCPU_doCheckProcessStateChange(VirtualCPU * this);
void VCPU_doProcessArrivingProcesses(VirtualCPU * this, PCB_deque* notYetArrived);
void VCPU_doDispatcherProcessing(VirtualCPU * this);
void VCPU_doSystemWideTick(VirtualCPU * this);

void VCPU_doPrintQueues(VirtualCPU * this);

#endif	/* VIRTUALCPU_H */

