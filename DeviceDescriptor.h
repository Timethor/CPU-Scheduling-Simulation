/* 
 * File:   DeviceDescriptor.h
 * Author: Timethor
 *
 * Created on September 21, 2013, 5:58 PM
 */

#ifndef DEVICEDESCRIPTOR_H
#define	DEVICEDESCRIPTOR_H

#include "ProcessControlBlock.h"

// Structure of DD Device Descriptor
typedef struct DD{
    int device_id;
    enum PCBState device_state;
    dequeue_PCB queue;
} DD;

DEQUEUE_PROTOTYPE(DD);

#endif	/* DEVICEDESCRIPTOR_H */

