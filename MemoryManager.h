/*
 * File:   MemoryManager.h
 * Author: Timethor
 *
 * Created on November 7, 2013, 4:23 PM
 */

#ifndef MEMORYMANAGER_H
#define	MEMORYMANAGER_H

#include <stdbool.h>

#include "DeviceDescriptorList.h"
#include "ProcessQueueList.h"
#include "LineInterpreter.h"
#include "Settings.h"
#include "MemoryRegionList.h"

typedef struct MemoryManager {
    enum MemoryPolicy policy;
    int memKiloSize;
    int policyParams;
    MemoryRegion_deque memory;
} MemoryManager;

//>>	== PUBLIC FUNCTION PROTO ==    <<//

//>>	Construct & Destruct
MemoryManager* MemoryManager_init(enum MemoryPolicy policy, int size, int params);
void MemoryManager_destruct(MemoryManager * this);

#endif	/* MEMORYMANAGER_H */

