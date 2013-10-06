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

void DeviceDescriptor_deque_ProcArrival(DeviceDescriptor_deque* this, PCB* process, Logger* logs) {
    char s[16];
    if (process->state == PCB_NEW) {
        logs->log(logs, LogLevel_INFO, "%s Arrives\n", PCB_toString(process, s));
        process->state = PCB_WAITING;
    }
    int id = BurstNode_deque_peekF(&process->schedule)->queue_id;
    logs->log(logs, LogLevel_INFO, "%s requests I/O on Device %2d\n", PCB_toString(process, s), id);
    DeviceDescriptor* device = DeviceDescriptor_deque_getDeviceId(this, id);
    DD_enqueueProcess(device, process, logs);
}

void DeviceDescriptor_deque_SystemWideTick(DeviceDescriptor_deque* this, Logger* logs) {
    logs->log(logs, LogLevel_FINER, "Doing DD_D TICK\n");
    DeviceDescriptor_dequeI iter;
    DeviceDescriptor_dequeI_init(&iter, this);
    DeviceDescriptor* dd = DeviceDescriptor_dequeI_examine(&iter);
    while (dd != NULL) {
        DD_SystemWideTick(dd, logs);
        dd = DeviceDescriptor_dequeI_next(&iter);
    }
}

void DeviceDescriptor_deque_print(DeviceDescriptor_deque* this, Logger* logs, enum LogLevel level) {
    DeviceDescriptor_dequeI it;
    DeviceDescriptor_dequeI_init(&it, this);
    while (true) {
        DeviceDescriptor* dd = DeviceDescriptor_dequeI_examine(&it);
        if (dd == NULL)
            break;
        DD_printQueue(dd, logs, level);
        if (DeviceDescriptor_dequeI_next(&it) == NULL)
            break;
    }
}
