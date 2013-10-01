#include <stdlib.h>
#include "BurstNode.h"

DEQUEUE(BurstNode);

bool BurstNode_fullyFormed(BurstNode* bn) {
    return (bn->type != BT_NONE) ?
                (bn->type == BT_CPU ?
                    (bn->duration != BT_NONE ? true : false) :
                    ((bn->duration != BT_NONE && bn->queue_id != BT_NONE) ? true : false) 
                ) : false;
}

BurstNode* initBurstNode() {
    BurstNode* newBurst = malloc(sizeof(*newBurst));
    newBurst->duration = BT_NONE;
    newBurst->queue_id = BT_NONE;
    newBurst->type = BT_NONE;
    return newBurst;
}