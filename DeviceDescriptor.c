/*
 * File:   DeviceDescriptor.c
 * Author: Nicholas `Timethor` Rich     G00399909
 *
 * Created on September 21, 2013, 5:58 PM
 */

#include <stdlib.h>
#include <string.h>

#include "DeviceDescriptor.h"

DeviceDescriptor* DeviceDescriptor_init(int id) {
    DeviceDescriptor* newDevice = malloc(sizeof (*newDevice));
    newDevice->id = id;
    newDevice->state = DD_IDLE;
    PCB_deque_init(&newDevice->queue, false, false);
    return newDevice;
}

void DeviceDescriptor_destruct(DeviceDescriptor* this) {
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

void DD_enqueueProcess(DeviceDescriptor* this, PCB* process, Logger* logs) {
    if (process != NULL) {
        PCB_deque_pushL(&this->queue, process);
    }
}

void DD_tryActivateDevice(DeviceDescriptor* this, Logger* logs) {
    if (this->state == DD_IDLE && !PCB_deque_empty(&this->queue)) {
        PCB* firstInQueue = PCB_deque_peekF(&this->queue);
        firstInQueue->state = PCB_RUNNING;
        this->state = DD_BUSY;
        char s[16];
        logs->log(logs, LogLevel_INFO, "I/O Device %2d now in use by %s\n", this->id, PCB_toString(firstInQueue, s));
    }
}

void DD_SystemWideTick(DeviceDescriptor* this, Logger* logs) {
    logs->log(logs, LogLevel_FINE, "\tDoing DD TICK - Queue %d\n", this->id);
    PCB_dequeI it;
    PCB_dequeI_init(&it, &this->queue);
    logs->log(logs, LogLevel_FINER, "\t\tDD TICK Looping\n");
    while (true) {
        PCB* pcb = PCB_dequeI_examine(&it);
        if (pcb == NULL) {
            logs->log(logs, LogLevel_FINER, "\t\t\tBailing Loop - First Null\n");
            break;
        }
        PCB_SystemWideTick(pcb, logs, true);
        if (PCB_dequeI_next(&it) == NULL) {
            logs->log(logs, LogLevel_FINER, "\t\t\tBailing Loop - End Queue\n");
            break;
        }
    }
}

void DD_printQueue(DeviceDescriptor* this, Logger* logs, enum LogLevel level) {
    if (this == NULL) {
        logs->log(logs, LogLevel_INFO, "Can't print null queue\n");
        return;
    }
    PCB_dequeI it;
    PCB_dequeI_init(&it, &this->queue);
    char buffer[100];
    char buffer2[100];
    sprintf(buffer, "%-11s %-3d %-10s = [", "I/O Device", this->id, "Queue");
    while (true) {
        PCB* pcb = PCB_dequeI_examine(&it);
        if (pcb == NULL)
            break;
        char s[16];
        if (PCB_dequeI_next(&it) != NULL) {
            sprintf(buffer2, "%s, ", PCB_toString(pcb, s));
            strcat(buffer, buffer2);
        } else {
            sprintf(buffer2, "%s", PCB_toString(pcb, s));
            strcat(buffer, buffer2);
            break;
        }
    }
    strcat(buffer, "]\n");
    logs->log(logs, level, buffer);
}