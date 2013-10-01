#define _GNU_SOURCE
#include <string.h>
#include <stdlib.h>

#include "VirtualCpu.h"
#include "ProcessQueue.h"
#include "DeviceDescriptor.h"
#include "LineInterpreter.h"

void VCPU_init(VirtualCPU* this, InputState* istate) {
    this->clockTime = 0;
    DD_dequeue_init(&this->devices, false, true);
    PQ_dequeue_init(&this->queues, false, true);
    printf("STARTING SYNC-MERGE\n");
    VCPU_MergeWithInputState(this, istate);
}

void VCPU_MergeWithInputState(VirtualCPU* this, InputState* istate) {
    int i;
    printf ("QUEUE HAS %d ELEMENTS, DD_PROTO_DEVICES\n", DD_dequeue_length(&istate->proto_devices));
    for (i = DD_dequeue_length(&istate->proto_devices); i > 0; i--) {
        DD* temp = DD_dequeue_pollF(&istate->proto_devices);
        DD_dequeue_pushL(&this->devices, temp);
        if (DD_dequeue_empty(&istate->proto_devices)){
            break;
        }
    }
    printf ("QUEUE HAS %d ELEMENTS, DD_PROTO_DEVICES\n\n", DD_dequeue_length(&istate->proto_devices));
    
    
    printf ("QUEUE HAS %d ELEMENTS, PQ_PROTO_QUEUES\n", PQ_dequeue_length(&istate->proto_queues));
    for (i = PQ_dequeue_length(&istate->proto_queues); i > 0; i--) {
        PQ* temp = PQ_dequeue_pollF(&istate->proto_queues);
        PQ_dequeue_pushL(&this->queues, temp);
        if (PQ_dequeue_empty(&istate->proto_queues)){
            break;
        }
    }
    printf ("QUEUE HAS %d ELEMENTS, PQ_PROTO_QUEUES\n\n", PQ_dequeue_length(&istate->proto_queues));
}

void VCPU_doClockCycle(VirtualCPU* this, PCB_dequeue* notYetArrived) {
    free(PCB_dequeue_pollF(notYetArrived));
}