#include <stdlib.h>
#include "ProcessQueue.h"

DEQUEUE(PQ);

PQ* PQ_init_RoundRobin(int id, int quantum) {
    PQ* this = PQ_init_base(id);
    this->quantum = quantum;
    return this;
}

PQ* PQ_init_FCFS(int id) {
    PQ* this = PQ_init_base(id);
    this->quantum = -1;
    return this;
}

PQ* PQ_init_base(int id) {
    PQ* this = malloc(sizeof (*this));
    this->id = id;
    this->quantumCheck = 0;
    PCB_dequeue_init(&this->queue, false, false);
    return this;
}

void PQ_SystemWideTick(PQ* this) {
    PCB_dequeueI it;
    PCB_dequeueI_init(&it, &this->queue);
    while (true) {
        PCB* pcb = PCB_dequeueI_examine(&it);
        if (pcb == NULL)
            break;
        PCB_SystemWideTick(pcb);
        if (PCB_dequeueI_next(&it) == NULL)
            break;
    }
}

void PQ_dequeue_SystemWideTick(PQ_dequeue* this) {
    PQ_dequeueI iter;
    PQ_dequeueI_init(&iter, this);
    while (PQ_dequeueI_next(&iter) != NULL) {
        PQ_SystemWideTick(PQ_dequeueI_examine(&iter));
    }
}

void PQ_dequeue_ProcArrival(PQ_dequeue* this, PCB* process) {
    PCB_toString(process);
    printf(" arrives\n");
    PCB_toString(process);
    printf(" requests CPU\n");
    PCB_dequeue* pcbd = &(PQ_dequeue_peekF(this)->queue);
    PCB_dequeue_pushL(pcbd, process);
    PQ_dequeue_print(this);
}

void PQ_dequeue_print(PQ_dequeue* this) {
    PQ_dequeueI it;
    PQ_dequeueI_init(&it, this);
    while (true) {
        PQ* pq = PQ_dequeueI_examine(&it);
        if (pq == NULL)
            break;
        PQ_printQueue(pq);
        if (PQ_dequeueI_next(&it) == NULL)
            break;
    }
}

void PQ_printQueue(PQ* this) {
    if (this == NULL) {
        printf("Cant print null queue\n");
        return;
    }
    PCB_dequeueI it;
    PCB_dequeueI_init(&it, &this->queue);
    printf("Ready Queue Q%d {%s%-3d} = [", this->id, (this->quantum >= 0 ? "RnRo:" : "FcFs:"), this->quantum == -1 ? 0 : this->quantum);
    while (true) {
        PCB* pcb = PCB_dequeueI_examine(&it);
        if (pcb == NULL)
            break;
        if (pcb->state != PCB_RUNNING) {
            PCB_toString(pcb);
        }
        if (PCB_dequeueI_next(&it) != NULL)
            printf(", ");
        else break;
    }
    printf("]\n");
}