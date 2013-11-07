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
    int kiloStart;
    int kiloEnd;
} MemoryRegion;

//>>	Construct / Destruct
MemoryRegion* MemoryRegion_init(int processId, int kiloStart, int kiloEnd);
void MemoryRegion_destruct(MemoryRegion * this);

int MR_getSize(MemoryRegion* this);

#endif	/* MEMORYREGION_H */

