#define _GNU_SOURCE
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>
#include "ProcessControlBlockList.h"

//#define DEBUG
#include "debug.h"
#include "ProcessQueueList.h"

PCB* PCB_init(int id) {
    PCB* process = malloc(sizeof (*process));
    process->id = id;
    process->arrival_time = -1;
    process->running_time = 0;
    process->waiting_time = 0;
    process->state = PCB_NEW;
    BurstNode_deque_init(&process->schedule, false, false);
    return process;
}

void PCB_destruct(PCB* this){
    free(this);
}

void PCB_toString(PCB* this) {
    printf("Process %2d", this->id);
}

void PCB_SystemWideTick(PCB* this) {
    //    printf("Doing PCB TICK\n");
    switch (this->state) {
        case PCB_WAITING:
            this->waiting_time++;
            break;
        case PCB_RUNNING:
            this->running_time++;
            BurstNode* bn = BurstNode_deque_peekF(&this->schedule);
            bn->duration--;
            if (bn->duration == 0) {
                BurstNode_deque_pollF(&this->schedule);
                this->state = PCB_BURST_FINISHED;
            }
            break;
        default:
            //            printf("==============NO TICK!=================\n");
            break;
    };
//    nanosleep((struct timespec[]){{0, 62500000}}, NULL);
}

void PCB_checkProcessTermination(PCB* this) {
    if (BurstNode_deque_empty(&this->schedule)) {
        this->state = PCB_TERMINATED;
    }
}