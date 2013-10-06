#include "ProcessQueueList.h"

DEQUE(ProcessQueue);

void ProcessQueue_deque_SystemWideTick(ProcessQueue_deque* this, Logger* logs) {
    logs->log(logs, LogLevel_FINER, "Doing PQ_D TICK\n");
    ProcessQueue_dequeI iter;
    ProcessQueue_dequeI_init(&iter, this);
    ProcessQueue* pq = ProcessQueue_dequeI_examine(&iter);
    while (pq != NULL) {
        PQ_systemWideTick(pq, logs);
        pq = ProcessQueue_dequeI_next(&iter);
    }
}

void ProcessQueue_deque_ProcArrival(ProcessQueue_deque* this, PCB* process, Logger* logs) {
    char s[16];
    if (process->state == PCB_NEW) {
        logs->log(logs, LogLevel_INFO, "%s arrives\n", PCB_toString(process, s));
        process->state = PCB_WAITING;
    }
    logs->log(logs, LogLevel_INFO, "%s requests CPU\n", PCB_toString(process, s));
    ProcessQueue* queue = ProcessQueue_deque_peekF(this);
    PQ_enqueueProcess(queue, process, logs);
}

void ProcessQueue_deque_print(ProcessQueue_deque* this, Logger* logs, enum LogLevel level) {
    ProcessQueue_dequeI it;
    ProcessQueue_dequeI_init(&it, this);
    while (true) {
        ProcessQueue* pq = ProcessQueue_dequeI_examine(&it);
        if (pq == NULL)
            break;
        PQ_printQueue(pq, logs, level);
        if (ProcessQueue_dequeI_next(&it) == NULL)
            break;
    }
}
