/* 
 * File:   DeviceDescriptor.h
 * Author: Timethor
 *
 * Created on September 21, 2013, 5:58 PM
 */

#ifndef DEVICEDESCRIPTOR_H
#define	DEVICEDESCRIPTOR_H

#include "ProcessControlBlock.h"
#include "Dequeue.h"

enum DDState {
    DD_NEW,
    DD_READY,
    DD_RUNNING,
};

// Structure of PCB (process control block)
typedef struct DD{
    //>>	The ID of the device
    int id;
    //>>	The enum representation of the state of the process
    enum DDState state;
    //>>	Queue of PCB to be processed
    PCB_dequeue queue;
} DD;

DEQUEUE_PROTOTYPE(DD);

bool SearchDeviceIds(DD_dequeue* container, int id);

DD* DD_init(int id);

void DD_SystemWideTick(DD* this);
void DD_printQueue(DD* this);

void DD_dequeue_SystemWideTick(DD_dequeue* this);
void DD_dequeue_print(DD_dequeue* this);

#endif	/* DEVICEDESCRIPTOR_H */

