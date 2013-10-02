#include <stdlib.h>
#include "DeviceDescriptor.h"
#include "ProcessControlBlock.h"

DEQUEUE(DD);

bool SearchDeviceIds(DD_dequeue* container, int id) {
    DD_dequeueI iter;
    DD_dequeueI_init(&iter, container);
    while (DD_dequeueI_next(&iter) != NULL) {
        if (DD_dequeueI_examine(&iter)->id == id) return true;
    }
    return false;
}

DD* DD_init(int id) {
    DD* newDevice = malloc(sizeof (*newDevice));
    newDevice->id = id;
    newDevice->state = DD_NEW;
    PCB_dequeue_init(&newDevice->queue, false, true);
    return newDevice;
}

void DD_SystemWideTick(DD* this) {
    PCB_dequeueI it;
    PCB_dequeueI_init(&it, &this->queue);
    switch (this->state) {
        case DD_RUNNING:
            while (true) {
                PCB* pcb = PCB_dequeueI_examine(&it);
                if (pcb == NULL)
                    break;
                PCB_SystemWideTick(pcb);
                if (PCB_dequeueI_next(&it) == NULL)
                    break;
            }
            break;
        default:
            break;
    }
}

void DD_dequeue_SystemWideTick(DD_dequeue* this) {
    DD_dequeueI iter;
    DD_dequeueI_init(&iter, this);
    while (DD_dequeueI_next(&iter) != NULL) {
        DD_SystemWideTick(DD_dequeueI_examine(&iter));
    }
}

void DD_printQueue(DD* this) {
    if (this == NULL) {
        printf("Cant print null queue\n");
        return;
    }
    PCB_dequeueI it;
    PCB_dequeueI_init(&it, &this->queue);
    printf("I/O Device %d Queue = [", this->id);
    while (true) {
        PCB* pcb = PCB_dequeueI_examine(&it);
        if (pcb == NULL)
            break;
        PCB_toString(pcb);
        if (PCB_dequeueI_next(&it) != NULL)
            printf(", ");
        else break;
    }
    printf("]\n");
}

void DD_dequeue_print(DD_dequeue* this) {
    DD_dequeueI it;
    DD_dequeueI_init(&it, this);
    while (true) {
        DD* dd = DD_dequeueI_examine(&it);
        if (dd == NULL)
            break;
        DD_printQueue(dd);
        if (DD_dequeueI_next(&it) == NULL)
            break;
    }
}