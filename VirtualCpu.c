#include "stdlib.h"

#include "VirtualCpu.h"
#include "ProcessQueue.h"
#include "DeviceDescriptor.h"


void initCPU(VirtualCPU* cpu){
    cpu->clockTime = 0;
    DD_dequeue_init(&cpu->devices);
    PQ_dequeue_init(&cpu->queues);
    
    PQ RoundRobin1;
    PQ_init_RoundRobin(&RoundRobin1);

//    PQ_dequeue_pushF(&cpu->queues, &RoundRobin1);
}