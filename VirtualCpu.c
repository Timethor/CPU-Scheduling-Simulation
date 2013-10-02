#define _GNU_SOURCE
#include <string.h>
#include <stdlib.h>

#include "VirtualCpu.h"
#include "ProcessQueue.h"
#include "DeviceDescriptor.h"
#include "LineInterpreter.h"

VirtualCPU* VCPU_init(InputState* istate) {
    VirtualCPU* this = malloc(sizeof (*this));
    this->clockTime = 0;
    DD_dequeue_init(&this->devices, false, true);
    PQ_dequeue_init(&this->queues, false, true);
    printf("STARTING SYNC-MERGE\n");
    VCPU_MergeWithInputState(this, istate);
    return this;
}

void VCPU_MergeWithInputState(VirtualCPU* this, InputState* istate) {
    int i;
    for (i = DD_dequeue_length(&istate->proto_devices); i > 0; i--) {
        DD_dequeue_pushL(&this->devices, DD_dequeue_pollF(&istate->proto_devices));
        if (DD_dequeue_empty(&istate->proto_devices)) {
            break;
        }
    }
    for (i = PQ_dequeue_length(&istate->proto_queues); i > 0; i--) {
        PQ_dequeue_pushL(&this->queues, PQ_dequeue_pollF(&istate->proto_queues));
        if (PQ_dequeue_empty(&istate->proto_queues)) {
            break;
        }
    }
    printf("After Print");
    printf("SIZEOF:: THIS_>PROCESQUEUE:: %d\n\n", PQ_dequeue_length(&this->queues));
    printf("QUEUE HEAD ID: %d\n", this->queues.head->data->id);
    PQ_dequeue_print(&this->queues);
}

void VCPU_SystemWideTick(VirtualCPU* this) {
    this->clockTime++;
    PQ_dequeue_SystemWideTick(&this->queues);
    DD_dequeue_SystemWideTick(&this->devices);
}

void VCPU_doClockCycle(VirtualCPU* this, PCB_dequeue* notYetArrived) {

    //>>	Process Preempt Check

    //>>	Check Completion
    //>>	Process Arriving Processes
    VCPU_ProcessArrivingProcesses(this, notYetArrived);
    //>>	Grab a process from Ready Queues
    VCPU_SystemWideTick(this);
}

void VCPU_ProcessArrivingProcesses(VirtualCPU* this, PCB_dequeue* notYetArrived) {
    while (!PCB_dequeue_empty(notYetArrived) && PCB_dequeue_peekF(notYetArrived)->arrival_time == this->clockTime) {
        PCB* requester = PCB_dequeue_pollF(notYetArrived);
        PQ_dequeue_ProcArrival(&this->queues, requester);
    }
}