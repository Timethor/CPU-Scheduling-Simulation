#define _GNU_SOURCE
#include <string.h>
#include <stdlib.h>

#include "VirtualCpu.h"
#include "ProcessQueue.h"
#include "DeviceDescriptor.h"

void initCPU(VirtualCPU* this) {
    this->clockTime = 0;
    DD_dequeue_init(&this->devices, false, true);
    PQ_dequeue_init(&this->queues, false, true);
}