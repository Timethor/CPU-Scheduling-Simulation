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

enum MemoryFitMode {
    MFM_FIRST = 0,
    MFM_BEST,
    MFM_WORST
};

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

bool MMAN_checkAllocationPotential(MemoryManager * this, PCB* process);
MemoryRegion* MMAN_getFitAllocation(MemoryManager * this, int sizeNeeded);
MemoryRegion_deque* MMAN_getPageFitAllocation(MemoryManager * this, int sizeNeeded);

void MMAN_printMemoryMap(MemoryManager * this, Logger* logger);
bool MMAN_allocateProcess(MemoryManager * this, PCB* process, Logger* logger);
void MMAN_deAllocateProcess(MemoryManager * this, int id);

#endif	/* MEMORYMANAGER_H */

