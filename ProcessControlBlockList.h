/*
 * File:   ProcessControlBlockList.h
 * Author: Nicholas `Timethor` Rich     G00399909
 *
 * Created on October 2, 2013, 3:36 PM
 */

#ifndef PROCESSCONTROLBLOCKLIST_H
#define	PROCESSCONTROLBLOCKLIST_H

#include <stdlib.h>

#include "Deque.h"
#include "ProcessControlBlock.h"

DEQUE_PROTOTYPE(PCB);

void PCB_deque_makeEmpty(PCB_deque* this);

#endif	/* PROCESSCONTROLBLOCKLIST_H */

