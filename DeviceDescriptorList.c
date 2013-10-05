#include "DeviceDescriptorList.h"

DEQUE(DeviceDescriptor);

bool DeviceDescriptor_deque_hasDeviceId(DeviceDescriptor_deque* this, int id) {
    if (DeviceDescriptor_deque_getDeviceId(this, id) == NULL)
        return false;
    return true;
}

DeviceDescriptor* DeviceDescriptor_deque_getDeviceId(DeviceDescriptor_deque* this, int id) {
    DeviceDescriptor_dequeI iter;
    DeviceDescriptor_dequeI_init(&iter, this);
    DeviceDescriptor* dd = DeviceDescriptor_dequeI_examine(&iter);
    while (dd != NULL) {
        if (dd->id == id) return dd;
        dd = DeviceDescriptor_dequeI_next(&iter);
    }
    return NULL;
}

void DeviceDescriptor_deque_ProcArrival(DeviceDescriptor_deque* this, PCB* process) {
    if (process->state == PCB_NEW) {
        PCB_toString(process);
        printf(" Arrives\n");
        process->state = PCB_WAITING;
    }
    PCB_toString(process);
    printf(" requests I/O on Device %2d\n", BurstNode_deque_peekF(&process->schedule)->queue_id);
    DD_enqueueProcess(DeviceDescriptor_deque_peekF(this), process);
}

void DeviceDescriptor_deque_SystemWideTick(DeviceDescriptor_deque* this) {
    //    printf("Doing DD_D TICK\n");
    DeviceDescriptor_dequeI iter;
    DeviceDescriptor_dequeI_init(&iter, this);
    DeviceDescriptor* dd = DeviceDescriptor_dequeI_examine(&iter);
    while (dd != NULL) {
        DD_SystemWideTick(dd);
        dd = DeviceDescriptor_dequeI_next(&iter);
    }
}

void DeviceDescriptor_deque_print(DeviceDescriptor_deque* this) {
    DeviceDescriptor_dequeI it;
    DeviceDescriptor_dequeI_init(&it, this);
    while (true) {
        DeviceDescriptor* dd = DeviceDescriptor_dequeI_examine(&it);
        if (dd == NULL)
            break;
        DD_printQueue(dd);
        if (DeviceDescriptor_dequeI_next(&it) == NULL)
            break;
    }
}
