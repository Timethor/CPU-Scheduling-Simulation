/*
 * File:   MemoryRegion.c
 * Author: Timethor
 *
 * Created on November 7, 2013, 6:20 AM
 */

#include <stdlib.h>
#include <stdio.h>

#include "MemoryRegion.h"

MemoryRegion* MemoryRegion_init(int processId, int kiloStart, int kiloEnd) {
    MemoryRegion* this = malloc(sizeof (*this));
    this->processId = processId;
    this->kiloStart = kiloStart;
    this->kiloEnd = kiloEnd;
    this->segmentId = -1;
    return this;
}

MemoryRegion* MemoryRegion_initSeg(int processId, int kiloStart, int kiloEnd, int segmentId) {
    MemoryRegion* this = MemoryRegion_init(processId, kiloStart, kiloEnd);
    this->segmentId = segmentId;
    return this;
}

void MemoryRegion_destruct(MemoryRegion* this) {
    free(this);
}

int MR_getSize(MemoryRegion* this) {
    return this->kiloEnd - this->kiloStart;
}

char* MR_toString(MemoryRegion* this, char* buffer) {
    if (this->segmentId == -1)
        sprintf(buffer, "Process %2d", this->processId);
    else
        sprintf(buffer, "Process %2d, Segment %2d", this->processId, this->segmentId);
    return buffer;
}