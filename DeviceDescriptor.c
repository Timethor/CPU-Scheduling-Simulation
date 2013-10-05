
#include <stdlib.h>

#include "DeviceDescriptor.h"

DeviceDescriptor* DeviceDescriptor_init(int id) {
    DeviceDescriptor* newDevice = malloc(sizeof (*newDevice));
    newDevice->id = id;
    newDevice->state = DD_IDLE;
    PCB_deque_init(&newDevice->queue, false, false);
    return newDevice;
}

void DeviceDescriptor_destruct(DeviceDescriptor* this){
    PCB_deque_freeElements(&this->queue);
    free(this);
}

PCB* DD_hasBurstEndedProcess(DeviceDescriptor* this) {
    PCB* firstInQueue = PCB_deque_peekF(&this->queue);
    if (firstInQueue != NULL && firstInQueue->state == PCB_BURST_FINISHED) {
        this->state = DD_IDLE;
        PCB_checkProcessTermination(firstInQueue);
        return PCB_deque_pollF(&this->queue);
    }
    return NULL;
}

void DD_enqueueProcess(DeviceDescriptor* this, PCB* process) {
    if (process != NULL) {
        PCB_deque_pushL(&this->queue, process);
    }
}

void DD_tryActivateDevice(DeviceDescriptor* this) {
    if (this->state == DD_IDLE && !PCB_deque_empty(&this->queue)) {
        PCB* firstInQueue = PCB_deque_peekF(&this->queue);
        firstInQueue->state = PCB_RUNNING;
        this->state = DD_BUSY;
        printf("I/O Device %2d now in use by ", this->id);
        PCB_toString(firstInQueue);
        printf("\n");
    }
}

void DD_SystemWideTick(DeviceDescriptor* this) {
    //    printf("Doing DD TICK\n");
    PCB_dequeI it;
    PCB_dequeI_init(&it, &this->queue);
    while (true) {
        PCB* pcb = PCB_dequeI_examine(&it);
        if (pcb == NULL)
            break;
        PCB_SystemWideTick(pcb);
        if (PCB_dequeI_next(&it) == NULL)
            break;
    }
}

void DD_printQueue(DeviceDescriptor* this) {
    if (this == NULL) {
        printf("Can't print null queue\n");
        return;
    }
    PCB_dequeI it;
    PCB_dequeI_init(&it, &this->queue);
    printf("%-11s %-3d %-10s = [", "I/O Device", this->id, "Queue");
    while (true) {
        PCB* pcb = PCB_dequeI_examine(&it);
        if (pcb == NULL)
            break;
        PCB_toString(pcb);
        if (PCB_dequeI_next(&it) != NULL)
            printf(", ");
        else break;
    }
    printf("]\n");
}