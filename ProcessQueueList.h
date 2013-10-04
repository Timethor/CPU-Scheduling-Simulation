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

DEQUE_PROTOTYPE(PQ);
void PQ_deque_SystemWideTick(PQ_deque * this);
void PQ_deque_ProcArrival(PQ_deque * this, PCB* process);
void PQ_deque_print(PQ_deque * this);

#endif	/* PROCESSQUEUELIST_H */



