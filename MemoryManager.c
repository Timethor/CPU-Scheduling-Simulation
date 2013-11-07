/*
 * File:   MemoryManager.c
 * Author: Timethor
 *
 * Created on November 4, 2013, 4:23 PM
 */

#include "MemoryManager.h"

MemoryManager* MemoryManager_init(enum MemoryPolicy policy, int size, int params) {
    MemoryManager* this = malloc(sizeof (*this));
    this->memKiloSize = size;
    this->policy = policy;
    this->policyParams = params;
    return this;
}

void MemoryManager_destruct(MemoryManager * this) {
    free(this);
}