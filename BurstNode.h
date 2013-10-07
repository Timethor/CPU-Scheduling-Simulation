/* 
 * File:   BurstNode.h
 * Author: Nicholas `Timethor` Rich     G00399909
 *
 * Created on September 21, 2013, 11:18 AM
 */

#ifndef BURSTNODE_H
#define	BURSTNODE_H

#include <stdio.h>
#include <stdbool.h>

enum burstType {
    BT_NONE = -1,
    BT_CPU,
    BT_IO,
};

// Structure of PCB (process control block)

typedef struct BurstNode {
    //>>	The type of Burst this Node is representing
    enum burstType type;
    //>>	The duration of the burst
    int duration;
    //>>	The ID of the queue to start on for CPU type, device_id for IO type
    //>>	0: pick best option
    int queue_id;
} BurstNode;

//>>	Construct / Destruct
BurstNode* BurstNode_init();
void BurstNode_destruct(BurstNode* this);

//>>	Checks
bool BN_fullyFormed(BurstNode* bn);


#endif	/* BURSTNODE_H */

