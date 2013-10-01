#define _GNU_SOURCE
#include <string.h>
#include <stdlib.h>

#include "VirtualCpu.h"
#include "ProcessQueue.h"
#include "DeviceDescriptor.h"
#include "LineInterpreter.h"

void VCPU_init(VirtualCPU* this) {
    this->clockTime = 0;
    DD_dequeue_init(&this->devices, false, true);
    PQ_dequeue_init(&this->queues, false, true);
}

void VCPU_MergeWithInputState(VirtualCPU* this, InputState* state){
    
}