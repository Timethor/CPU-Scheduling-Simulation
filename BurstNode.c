/*
 * File:   BurstNode.c
 * Author: Nicholas `Timethor` Rich     G00399909
 *
 * Created on September 21, 2013, 11:18 AM
 */

#include <stdlib.h>

#include "BurstNode.h"

BurstNode* BurstNode_init() {
    BurstNode* newBurst = malloc(sizeof (*newBurst));
    newBurst->duration = BT_NONE;
    newBurst->queue_id = BT_NONE;
    newBurst->type = BT_NONE;
    return newBurst;
}

void BurstNode_destruct(BurstNode* this) {
    free(this);
}

bool BN_fullyFormed(BurstNode* bn) {
    return (bn->type == BT_NONE ? false :
            (bn->duration == BT_NONE ? false :
            ((bn->type == BT_IO && bn->queue_id == BT_NONE) ? false : true)));
}