/* 
 * File:   BurstNode.h
 * Author: Timethor
 *
 * Created on September 21, 2013, 11:18 AM
 */

#ifndef BURSTNODE_H
#define	BURSTNODE_H

#include <stdio.h>
#include <stdbool.h>
#include "Dequeue.h"

enum burstType {
   BT_CPU,
   BT_IO,
};

// Structure of PCB (process control block)
typedef struct BurstNode{
    //>>	The type of Burst this Node is representing
    enum burstType type;
    //>>	The duration of the burst
    int duration;
    //>>	The ID of the queue to start on for CPU type, device_id for IO type
    //>>	0: pick best option
    int queue_id;
} BurstNode;

DEQUEUE_PROTOTYPE(BurstNode);

#endif	/* BURSTNODE_H */

