/*
 * File:   MemoryRegion.c
 * Author: Timethor
 *
 * Created on November 7, 2013, 6:20 AM
 */

#include <stdlib.h>

#include "MemoryRegion.h"

MemoryRegion* MemoryRegion_init(int processId, int kiloStart, int kiloEnd) {
    MemoryRegion* this = malloc(sizeof (*this));
    this->processId = processId;
    this->kiloStart = kiloStart;
    this->kiloEnd = kiloEnd;
    return this;
}

void MemoryRegion_destruct(MemoryRegion* this) {
    free(this);
}

int MR_getSize(MemoryRegion* this) {
    return this->kiloEnd - this->kiloStart;
}