/*
 * File:   ProcessControlBlock.c
 * Author: Nicholas `Timethor` Rich     G00399909
 *
 * Created on September 21, 2013, 11:18 AM
 */

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include "ProcessControlBlockList.h"

PCB* PCB_init(int id) {
    PCB* this = malloc(sizeof (*this));
    this->id = id;
    this->arrival_time = -1;
    this->turnaround_time = 0;
    this->waiting_time = 0;
    this->state = PCB_NEW;
    AddressSpace_deque_init(&this->a_space, false, false);
    BurstNode_deque_init(&this->schedule, false, false);
    return this;
}

void PCB_destruct(PCB* this) {
    AddressSpace_deque_freeElements(&this->a_space);
    BurstNode_deque_freeElements(&this->schedule);
    free(this);
}

char* PCB_toString(PCB* this, char* buffer) {
    sprintf(buffer, "Process %2d", this->id);
    return buffer;
}

void PCB_SystemWideTick(PCB* this, Logger* logs, bool inDevice) {
    logs->log(logs, LogLevel_FINE, "\t\t\tDoing PCB TICK\n");
    switch (this->state) {
        case PCB_WAITING:
            this->turnaround_time++;
            if (!inDevice) {
                this->waiting_time++;
            }
            break;
        case PCB_RUNNING:
            this->turnaround_time++;
            BurstNode* bn = BurstNode_deque_peekF(&this->schedule);
            bn->duration--;
            if (bn->duration == 0) {
                BurstNode_deque_pollF(&this->schedule);
                this->state = PCB_BURST_FINISHED;
            }
            break;
        default:
            logs->log(logs, LogLevel_WARNING, "==============NO TICK!=================\n");
            break;
    };
}

void PCB_checkProcessTermination(PCB* this) {
    if (BurstNode_deque_empty(&this->schedule)) {
        this->state = PCB_TERMINATED;
    }
}