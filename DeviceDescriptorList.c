#include "DeviceDescriptorList.h"

DEQUE(DeviceDescriptor);

bool DD_deque_hasDeviceId(DD_deque* this, int id) {
    if (DD_deque_getDeviceId(this, id) == NULL)
        return false;
    return true;
}

DeviceDescriptor* DD_deque_getDeviceId(DD_deque* this, int id) {
    DD_dequeI iter;
    DD_dequeI_init(&iter, this);
    DeviceDescriptor* dd = DD_dequeI_examine(&iter);
    while (dd != NULL) {
        if (dd->id == id) return dd;
        dd = DD_dequeI_next(&iter);
    }
    return NULL;
}

void DD_deque_ProcArrival(DD_deque* this, PCB* process) {
    if (process->state == PCB_NEW) {
        PCB_toString(process);
        printf(" Arrives\n");
        process->state = PCB_WAITING;
    }
    PCB_toString(process);
    printf(" requests I/O on Device %2d\n", BurstNode_deque_peekF(&process->schedule)->queue_id);
    DD_enqueueProcess(DD_deque_peekF(this), process);
}

void DD_deque_SystemWideTick(DD_deque* this) {
    //    printf("Doing DD_D TICK\n");
    DD_dequeI iter;
    DD_dequeI_init(&iter, this);
    DeviceDescriptor* dd = DD_dequeI_examine(&iter);
    while (dd != NULL) {
        DD_SystemWideTick(dd);
        dd = DD_dequeI_next(&iter);
    }
}

void DD_deque_print(DD_deque* this) {
    DD_dequeI it;
    DD_dequeI_init(&it, this);
    while (true) {
        DeviceDescriptor* dd = DD_dequeI_examine(&it);
        if (dd == NULL)
            break;
        DD_printQueue(dd);
        if (DD_dequeI_next(&it) == NULL)
            break;
    }
}
