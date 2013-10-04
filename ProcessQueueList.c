#include "ProcessQueueList.h"

DEQUE(PQ);

void PQ_deque_SystemWideTick(PQ_deque* this) {
//    printf("Doing PQ_D TICK\n");
    PQ_dequeI iter;
    PQ_dequeI_init(&iter, this);
    PQ* pq = PQ_dequeI_examine(&iter);
    while (pq != NULL) {
        PQ_systemWideTick(pq);
        pq = PQ_dequeI_next(&iter);
    }
}

void PQ_deque_ProcArrival(PQ_deque* this, PCB* process) {
    if (process->state == PCB_NEW) {
        PCB_toString(process);
        printf(" arrives\n");
        process->state = PCB_WAITING;
    }
    PCB_toString(process);
    printf(" requests CPU\n");
    PQ_enqueueProcess(PQ_deque_peekF(this), process);
}

void PQ_deque_print(PQ_deque* this) {
    PQ_dequeI it;
    PQ_dequeI_init(&it, this);
    while (true) {
        PQ* pq = PQ_dequeI_examine(&it);
        if (pq == NULL)
            break;
        PQ_printQueue(pq);
        if (PQ_dequeI_next(&it) == NULL)
            break;
    }
}
