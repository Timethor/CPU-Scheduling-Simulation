/*
 * File:   MemoryRegion.h
 * Author: Timethor
 *
 * Created on November 7, 2013, 6:20 AM
 */

#ifndef MEMORYREGION_H
#define	MEMORYREGION_H

typedef struct MemoryRegion {
    int processId;
    int segmentId;
    int kiloStart;
    int kiloEnd;
} MemoryRegion;

//>>	Construct / Destruct
MemoryRegion* MemoryRegion_init(int processId, int kiloStart, int kiloEnd);
MemoryRegion* MemoryRegion_initSeg(int processId, int kiloStart, int kiloEnd, int segmentId);
void MemoryRegion_destruct(MemoryRegion * this);

int MR_getSize(MemoryRegion* this);
char* MR_toString(MemoryRegion* this, char* buffer);

#endif	/* MEMORYREGION_H */

