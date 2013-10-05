#define _GNU_SOURCE
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include "ProcessControlBlockList.h"

//#define DEBUG
#include "debug.h"
#include "ProcessQueueList.h"

PCB* PCB_init(int id) {
    PCB* process = malloc(sizeof (*process));
    process->id = id;
    process->arrival_time = -1;
    process->turnaround_time = 0;
    process->waiting_time = 0;
    process->state = PCB_NEW;
    BurstNode_deque_init(&process->schedule, false, false);
    return process;
}

void PCB_destruct(PCB* this) {
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
    //    nanosleep((struct timespec[]){{0, 62500000}}, NULL);
}

void PCB_checkProcessTermination(PCB* this) {
    if (BurstNode_deque_empty(&this->schedule)) {
        this->state = PCB_TERMINATED;
    }
}