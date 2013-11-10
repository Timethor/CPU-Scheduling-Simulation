/*
 * File:   MemoryRegion.h
 * Author: Timethor
 *
 * Created on November 7, 2013, 6:20 AM
 */

#ifndef MEMORYREGION_H
#define	MEMORYREGION_H

#include <stdbool.h>

typedef struct MemoryRegion {
    int processId;
    int partition;
    bool isSeg;
    int kiloStart;
    int kiloEnd;
} MemoryRegion;

//>>	Construct / Destruct
MemoryRegion* MemoryRegion_init(int processId, int kiloStart, int kiloEnd);
MemoryRegion* MemoryRegion_initSeg(int processId, int kiloStart, int kiloEnd, int segmentId);
MemoryRegion* MemoryRegion_initPag(int processId, int kiloStart, int kiloEnd, int pageId);
void MemoryRegion_destruct(MemoryRegion * this);

int MR_getSize(MemoryRegion* this);
char* MR_toString(MemoryRegion* this, char* buffer, const char* holeName);
void MR_reset(MemoryRegion* this);

#endif	/* MEMORYREGION_H */

