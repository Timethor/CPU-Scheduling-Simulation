#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include "ProcessControlBlock.h"

//#define DEBUG
#include "debug.h"

DEQUEUE(PCB);

PCB* PCB_init(int id) {
    PCB* process = malloc(sizeof (*process));
    process->id = id;
    process->arrival_time = -1;
    process->running_time = 0;
    process->waiting_time = 0;
    process->state = PCB_NEW;
    BurstNode_dequeue_init(&process->schedule, false, false);
    return process;
}

void PCB_toString(PCB* this) {
    printf("Process %2d", this->id);
}

void PCB_SystemWideTick(PCB* this) {
    switch (this->state) {
        case PCB_WAITING:
            this->waiting_time++;
            printf ("Incr Wait time");
            break;
        case PCB_RUNNING:
            this->running_time++;
            printf ("Incr run time");
            BurstNode* bn = BurstNode_dequeue_peekF(&this->schedule);
            bn->duration--;
            if (bn->duration == 0){
                printf("BURST FINISHED");
                this->state = PCB_BURST_FINISHED;
            }
            break;
        default:
            break;
    };
    usleep(275);
}