/*
 * File:   MemoryManager.c
 * Author: Timethor
 *
 * Created on November 4, 2013, 4:23 PM
 */

#include "MemoryManager.h"

MemoryManager* MemoryManager_init(MemoryPolicy policy, int size, int params);
void MemoryManager_destruct(MemoryManager * this);