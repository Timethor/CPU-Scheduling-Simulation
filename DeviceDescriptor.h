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

void DD_init(DD* queue);

#endif	/* DEVICEDESCRIPTOR_H */

