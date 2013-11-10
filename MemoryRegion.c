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
    this->partition = -1;
    this->isSeg = false;
    return this;
}

MemoryRegion* MemoryRegion_initPart(int processId, int kiloStart, int kiloEnd, int partId) {
    MemoryRegion* this = MemoryRegion_init(processId, kiloStart, kiloEnd);
    this->partition = partId;
    return this;
}

MemoryRegion* MemoryRegion_initSeg(int processId, int kiloStart, int kiloEnd, int segmentId) {
    MemoryRegion* this = MemoryRegion_initPart(processId, kiloStart, kiloEnd, segmentId);
    this->isSeg = true;
    return this;
}

MemoryRegion* MemoryRegion_initPag(int processId, int kiloStart, int kiloEnd, int pageId) {
    return MemoryRegion_initPart(processId, kiloStart, kiloEnd, pageId);
}

void MR_reset(MemoryRegion* this) {
    this->processId = -1;
    this->partition = -1;
    this->isSeg = false;
}

void MemoryRegion_destruct(MemoryRegion* this) {
    free(this);
}

int MR_getSize(MemoryRegion* this) {
    return this->kiloEnd - this->kiloStart;
}

char* MR_toString(MemoryRegion* this, char* buffer, const char* holeName) {
    if (this->processId == -1) {
        sprintf(buffer, holeName);
    } else {
        if (this->partition == -1)
            sprintf(buffer, "Process %2d", this->processId);
        else
            sprintf(buffer, "Process %2d, %s %2d", this->processId, (this->isSeg ? "Segement" : "Page"), this->partition);
    }
    return buffer;
}