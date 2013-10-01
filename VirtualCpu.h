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

void initCPU(VirtualCPU* cpu);
#endif	/* VIRTUALCPU_H */

