#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "ProcessQueue.h"
#include "Logger.h"

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

void ProcessQueue_destruct(ProcessQueue* this) {
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

void PQ_systemWideTick(ProcessQueue* this, Logger* logs) {
    logs->log(logs, LogLevel_FINE, "\tDoing PQ TICK - Queue %d\n", this->id);
    PCB_dequeI it;
    PCB_dequeI_init(&it, &this->queue);
    PCB* pcb = PCB_dequeI_examine(&it);
    if (pcb != NULL && pcb->state == PCB_RUNNING) {
        logs->log(logs, LogLevel_FINER, "\t\tQuantumCheck++\n");
        this->quantumCheck++;
    }
    logs->log(logs, LogLevel_FINER, "\t\tPQ TICK Looping\n");
    while (true) {
        pcb = PCB_dequeI_examine(&it);
        if (pcb == NULL) {
            logs->log(logs, LogLevel_FINER, "\t\t\tBailing Loop - First Null\n");
            break;
        }
        PCB_SystemWideTick(pcb, logs, false);
        if (PCB_dequeI_next(&it) == NULL) {
            logs->log(logs, LogLevel_FINER, "\t\t\tBailing Loop - End Queue\n");
            break;
        }
    }
}

void PQ_stopRunningProcess(ProcessQueue* this, Logger* logs) {
    PCB* firstInQueue = PCB_deque_peekF(&this->queue);
    if (firstInQueue != NULL && firstInQueue->state == PCB_RUNNING) {
        char s[16];
        logs->log(logs, LogLevel_INFO, "%s is preemptedB\n", PCB_toString(firstInQueue, s));
        firstInQueue->state = PCB_WAITING;
    }
}

void PQ_startWaitingProcess(ProcessQueue* this, Logger* logs) {
    PCB* firstInQueue = PCB_deque_peekF(&this->queue);
    if (firstInQueue != NULL && firstInQueue->state == PCB_WAITING) {
        char s[16];
        logs->log(logs, LogLevel_INFO, "Dispatcher moves %s to Running State\n", PCB_toString(firstInQueue, s));
        firstInQueue->state = PCB_RUNNING;
    }
}

void PQ_enqueueProcess(ProcessQueue* this, PCB* process, Logger* logs) {
    if (process != NULL) {
        PCB_deque_pushL(&this->queue, process);
    }
}

void PQ_printQueue(ProcessQueue* this, Logger* logs, enum LogLevel level) {
    if (this == NULL) {
        logs->log(logs, LogLevel_WARNING, "Cant print null queue\n");
        return;
    }
    PCB_dequeI it;
    PCB_dequeI_init(&it, &this->queue);
    char buffer[100];
    char buffer2[100];
    sprintf(buffer, "%-11s Q%-2d {%s%-3d} = [", "Ready Queue", this->id, (this->quantum >= 0 ? "RnRo:" : "FcFs:"), this->quantum == -1 ? 0 : this->quantum);
    while (true) {
        PCB* pcb = PCB_dequeI_examine(&it);
        if (pcb == NULL)
            break;
        if (pcb->state != PCB_RUNNING) {
            char s[16];
            if (PCB_dequeI_next(&it) != NULL) {
                sprintf(buffer2, "%s, ", PCB_toString(pcb, s));
                strcat(buffer, buffer2);
            } else {
                sprintf(buffer2, "%s", PCB_toString(pcb, s));
                strcat(buffer, buffer2);
                break;
            }
        } else {
            if (PCB_dequeI_next(&it) == NULL)
                break;
        }
    }
    strcat(buffer, "]\n");
    logs->log(logs, level, buffer);
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