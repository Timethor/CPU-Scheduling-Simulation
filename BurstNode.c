#include <stdlib.h>
#include "BurstNode.h"

DEQUEUE(BurstNode);

const int BURST_NULL = -1;

bool BurstNode_fullyFormed(BurstNode* bn) {
    return (bn->type != BURST_NULL) ?
                (bn->type == BT_CPU ?
                    (bn->duration != BURST_NULL ? true : false) :
                    ((bn->duration != BURST_NULL && bn->queue_id != BURST_NULL) ? true : false) 
                ) : false;
}

BurstNode* initBurstNode() {
    BurstNode* newBurst = malloc(sizeof(*newBurst));
    newBurst->duration = BURST_NULL;
    newBurst->queue_id = BURST_NULL;
    newBurst->type = BURST_NULL;
    return newBurst;
}