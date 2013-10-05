/* 
 * File:   ProcessQueueList.h
 * Author: Timethor
 *
 * Created on October 2, 2013, 3:36 PM
 */

#ifndef PROCESSQUEUELIST_H
#define	PROCESSQUEUELIST_H

#include "Deque.h"
#include "ProcessQueue.h"

DEQUE_PROTOTYPE(ProcessQueue);
void ProcessQueue_deque_SystemWideTick(ProcessQueue_deque * this);
void ProcessQueue_deque_ProcArrival(ProcessQueue_deque * this, PCB* process);
void ProcessQueue_deque_print(ProcessQueue_deque * this);

#endif	/* PROCESSQUEUELIST_H */



