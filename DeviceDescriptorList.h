/* 
 * File:   DeviceDescriptorList.h
 * Author: Timethor
 *
 * Created on October 2, 2013, 3:36 PM
 */

#ifndef DEVICEDESCRIPTORLIST_H
#define	DEVICEDESCRIPTORLIST_H

#include <stdlib.h>

#include "Deque.h"
#include "DeviceDescriptor.h"

DEQUE_PROTOTYPE(DeviceDescriptor);

bool DD_deque_hasDeviceId(DD_deque * this, int id);
DeviceDescriptor* DD_deque_getDeviceId(DD_deque * this, int id);

void DD_deque_ProcArrival(DD_deque * this, PCB* process);
void DD_deque_SystemWideTick(DD_deque * this);
void DD_deque_print(DD_deque * this);

#endif	/* DEVICEDESCRIPTORLIST_H */

