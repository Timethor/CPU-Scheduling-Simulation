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

bool DeviceDescriptor_deque_hasDeviceId(DeviceDescriptor_deque * this, int id);
DeviceDescriptor* DeviceDescriptor_deque_getDeviceId(DeviceDescriptor_deque * this, int id);

void DeviceDescriptor_deque_ProcArrival(DeviceDescriptor_deque * this, PCB* process, Logger* logs);
void DeviceDescriptor_deque_SystemWideTick(DeviceDescriptor_deque * this, Logger* logs);
void DeviceDescriptor_deque_print(DeviceDescriptor_deque * this, Logger* logs, enum LogLevel level);

#endif	/* DEVICEDESCRIPTORLIST_H */

