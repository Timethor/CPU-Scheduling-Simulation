/* 
 * File:   VirtualCpu.h
 * Author: Nicholas `Timethor` Rich     G00399909
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
    DeviceDescriptor_deque devices;
    ProcessQueue_deque queues;
    PCB_deque terminated;
    Settings* settings;
    bool (*doClockCycle)(struct VirtualCPU *, PCB_deque*);
    int (*getAvgTurnAroundTime)(struct VirtualCPU*);
    int (*getAvgWaitingTime)(struct VirtualCPU*);
} VirtualCPU;

//>>	== PUBLIC FUNCTION PROTO ==    <<//

//>>	Construct & Destruct
VirtualCPU* VirtualCPU_init(SimulationState* istate, Settings* settings);
void VirtualCPU_destruct(VirtualCPU * this);

#endif	/* VIRTUALCPU_H */

