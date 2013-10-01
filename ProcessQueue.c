#include <stdlib.h>
#include "ProcessQueue.h"

DEQUEUE(PQ);

void PQ_init_RoundRobin(PQ* queue) {
    queue->state = PQ_EMPTY;
}

void PQ_init_FCFS(PQ* queue) {
    queue->state = PQ_EMPTY;
    queue->quantum = 0;
}