#include <stdlib.h>
#include <stdbool.h>
#include "ProcessQueue.h"

ProcessQueue* PQ_init_RoundRobin(int id, int quantum) {
    ProcessQueue* this = PQ_init(id);
    this->quantum = quantum;
    return this;
}

bool PQ_isRoundRobin(ProcessQueue* this) {
    return (this->quantum == -1 ? false : true);
}

ProcessQueue* PQ_init_FCFS(int id) {
    ProcessQueue* this = PQ_init(id);
    this->quantum = -1;
    return this;
}

bool PQ_isFCFS(ProcessQueue* this) {
    return (this->quantum == -1 ? true : false);
}

ProcessQueue* PQ_init(int id) {
    ProcessQueue* this = malloc(sizeof (*this));
    this->id = id;
    this->quantumCheck = 0;
    PCB_deque_init(&this->queue, false, false);
    return this;
}

void ProcessQueue_destruct(ProcessQueue* this){
    free(this);
}

bool PQ_hasWaitingProcess(ProcessQueue* this) {
    PCB* firstInQueue = PCB_deque_peekF(&this->queue);
    if (firstInQueue != NULL) {
        return (firstInQueue->state == PCB_WAITING ? true : false);
    }
    return false;
}

bool PQ_hasRunningProcess(ProcessQueue* this) {
    PCB* firstInQueue = PCB_deque_peekF(&this->queue);
    if (firstInQueue != NULL) {
        return (firstInQueue->state == PCB_RUNNING ? true : false);
    }
    return false;
}

PCB* PQ_hasBurstEndedProcess(ProcessQueue* this) {
    PCB* firstInQueue = PCB_deque_peekF(&this->queue);
    if (firstInQueue != NULL && firstInQueue->state == PCB_BURST_FINISHED) {
        PCB_checkProcessTermination(firstInQueue);
        return PCB_deque_pollF(&this->queue);
    }
    return NULL;
}

void PQ_systemWideTick(ProcessQueue* this) {
    //    printf("Doing PQ TICK - Queue %d\n", this->id);
    PCB_dequeI it;
    PCB_dequeI_init(&it, &this->queue);
    PCB* pcb = PCB_dequeI_examine(&it);
    if (pcb != NULL && pcb->state == PCB_RUNNING) {
        //        printf("QuantumCheck++\n");
        this->quantumCheck++;
    }
    //    printf("PQ TICK Looping\n");
    while (true) {
        pcb = PCB_dequeI_examine(&it);
        if (pcb == NULL) {
            //            printf("Bailing Loop - First Null\n");
            break;
        }
        PCB_SystemWideTick(pcb);
        if (PCB_dequeI_next(&it) == NULL) {
            //            printf("Bailing Loop - End List\n");
            break;
        }
    }
}

void PQ_stopRunningProcess(ProcessQueue* this) {
    PCB* firstInQueue = PCB_deque_peekF(&this->queue);
    PCB_toString(firstInQueue);
    printf(" is preempted\n");
    if (firstInQueue != NULL)
        firstInQueue->state = PCB_WAITING;
}

void PQ_startWaitingProcess(ProcessQueue* this) {
    PCB* firstInQueue = PCB_deque_peekF(&this->queue);
    if (firstInQueue != NULL) {
        printf("Dispatcher moves ");
        PCB_toString(firstInQueue);
        printf(" to Running State\n");
        firstInQueue->state = PCB_RUNNING;
    }
}

void PQ_enqueueProcess(ProcessQueue* this, PCB* process) {
    if (process != NULL)
        PCB_deque_pushL(&this->queue, process);
}

void PQ_printQueue(ProcessQueue* this) {
    if (this == NULL) {
        printf("Cant print null queue\n");
        return;
    }
    PCB_dequeI it;
    PCB_dequeI_init(&it, &this->queue);
    printf("%-11s Q%-2d {%s%-3d} = [", "Ready Queue", this->id, (this->quantum >= 0 ? "RnRo:" : "FcFs:"), this->quantum == -1 ? 0 : this->quantum);
    while (true) {
        PCB* pcb = PCB_dequeI_examine(&it);
        if (pcb == NULL)
            break;
        if (pcb->state != PCB_RUNNING) {
            PCB_toString(pcb);
        }
        if (PCB_dequeI_next(&it) != NULL)
            printf(", ");
        else break;
    }
    printf("]\n");
}

PCB* PQ_getQuantumViolator(ProcessQueue* this) {
    if (PQ_isRoundRobin(this) && this->quantum == this->quantumCheck) {
        this->quantumCheck = 0;
        PCB* violator = PCB_deque_pollF(&this->queue);
//        if (violator->state != PCB_BURST_FINISHED)
            return violator;
    }
    return NULL;
}