/* 
 * File:   VirtualCpu.h
 * Author: Timethor
 *
 * Created on September 21, 2013, 11:18 AM
 */

#ifndef VIRTUALCPU_H
#define	VIRTUALCPU_H

// Structure of DD Device Descriptor
typedef struct DD{
    int device_id;
    enum State device_state;
    dequeue_PCB queue;
} DD;

DEQUEUE_PROTOTYPE(DD);

#endif	/* VIRTUALCPU_H */

