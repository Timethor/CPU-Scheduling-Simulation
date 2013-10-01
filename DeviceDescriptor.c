#include <stdlib.h>
#include "DeviceDescriptor.h"

DEQUEUE(DD);

bool SearchDeviceIds(DD_dequeue* container, int id) {
    DD_dequeueI iter;
    DD_dequeueI_init(&iter, container);
    while (DD_dequeueI_next(&iter) != NULL) {
        if (DD_dequeueI_examine(&iter)->id == id) return true;
    }
    return false;
}

DD* DD_init(int id) {
    DD* newDevice = malloc(sizeof (*newDevice));
    newDevice->id = id;
    newDevice->state = DD_NEW;
    return newDevice;
}