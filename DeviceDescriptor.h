/* 
 * File:   DeviceDescriptor.h
 * Author: Timethor
 *
 * Created on September 21, 2013, 5:58 PM
 */

#ifndef DEVICEDESCRIPTOR_H
#define	DEVICEDESCRIPTOR_H

#include "ProcessControlBlockList.h"

enum DDState {
    DD_IDLE,
    DD_BUSY,
};

// Structure of PCB (process control block)

typedef struct DeviceDescriptor {
    //>>	The ID of the device
    int id;
    //>>	The enum representation of the state of the process
    enum DDState state;
    //>>	Queue of PCB to be processed
    PCB_deque queue;
} DeviceDescriptor;


DeviceDescriptor* DeviceDescriptor_init(int id);
void DeviceDescriptor_destruct(DeviceDescriptor* this);

PCB* DD_hasBurstEndedProcess(DeviceDescriptor * this);

void DD_enqueueProcess(DeviceDescriptor * this, PCB* process, Logger* logs);
void DD_tryActivateDevice(DeviceDescriptor * this, Logger* logs);
void DD_SystemWideTick(DeviceDescriptor * this, Logger* logs);
void DD_printQueue(DeviceDescriptor * this, Logger* logs, enum LogLevel level);

#endif	/* DEVICEDESCRIPTOR_H */

