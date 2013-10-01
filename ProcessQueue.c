#include <stdlib.h>
#include "ProcessQueue.h"

DEQUEUE(PQ);

void PQ_init_RoundRobin(PQ* queue, int id, int quantum) {
    queue->id = id;
    queue->state = PQ_EMPTY;
    queue->quantum = quantum;
}

void PQ_init_FCFS(PQ* queue, int id) {
    queue->id = id;
    queue->state = PQ_EMPTY;
    queue->quantum = 0;
}