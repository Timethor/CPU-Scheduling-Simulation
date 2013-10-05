#include "ProcessQueueList.h"

DEQUE(ProcessQueue);

void ProcessQueue_deque_SystemWideTick(ProcessQueue_deque* this) {
//    printf("Doing PQ_D TICK\n");
    ProcessQueue_dequeI iter;
    ProcessQueue_dequeI_init(&iter, this);
    ProcessQueue* pq = ProcessQueue_dequeI_examine(&iter);
    while (pq != NULL) {
        PQ_systemWideTick(pq);
        pq = ProcessQueue_dequeI_next(&iter);
    }
}

void ProcessQueue_deque_ProcArrival(ProcessQueue_deque* this, PCB* process) {
    if (process->state == PCB_NEW) {
        PCB_toString(process);
        printf(" arrives\n");
        process->state = PCB_WAITING;
    }
    PCB_toString(process);
    printf(" requests CPU\n");
    PQ_enqueueProcess(ProcessQueue_deque_peekF(this), process);
}

void ProcessQueue_deque_print(ProcessQueue_deque* this) {
    ProcessQueue_dequeI it;
    ProcessQueue_dequeI_init(&it, this);
    while (true) {
        ProcessQueue* pq = ProcessQueue_dequeI_examine(&it);
        if (pq == NULL)
            break;
        PQ_printQueue(pq);
        if (ProcessQueue_dequeI_next(&it) == NULL)
            break;
    }
}
