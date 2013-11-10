/*
 * File:   MemoryManager.c
 * Author: Timethor
 *
 * Created on November 4, 2013, 4:23 PM
 */

#define _GNU_SOURCE
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>

#include "MemoryManager.h"

MemoryRegion* MMAN__Allocate(MemoryManager* this, MemoryRegion* tobealloced, int id, int size, int partition);
void MMAN_coalesceMemory(MemoryManager * this, MemoryRegion* data);

MemoryManager* MemoryManager_init(enum MemoryPolicy policy, int size, int params) {
    MemoryManager* this = malloc(sizeof (*this));
    this->memKiloSize = size;
    this->policy = policy;
    this->policyParams = params;
    MemoryRegion_deque_init(&this->memory, false, false);
    if (policy == MP_PAG) {
        int index = 0;
        while (index + params <= size) {
            MemoryRegion* new = MemoryRegion_init(-1, index, index + (params - 1));
            MemoryRegion_deque_pushL(&this->memory, new);
            index += params;
        }
        //>>	Do paging setup
    } else if (policy == MP_INF) {
        //>>	Do setup for infinite memory, i.e. NOTHING :D
    } else if (policy == MP_SEG || policy == MP_VSP) {
        //>>	Do setup generic setup
        MemoryRegion* new = MemoryRegion_init(-1, 0, size - 1);
        MemoryRegion_deque_pushL(&this->memory, new);
    }
    return this;
}

void MemoryManager_destruct(MemoryManager * this) {
    MemoryRegion_deque_freeElements(&this->memory);
    free(this);
}

MemoryRegion* MMAN_getVSPAllocation(MemoryManager* this, PCB* process) {
    if (process == NULL) {
        return NULL;
    }
    AddressSpace* as = AddressSpace_deque_peekF(&process->a_space);
    if (as == NULL) {
        return NULL;
    }
    return MMAN_getFitAllocation(this, AddressSpace_deque_peekF(&process->a_space)->size);
}

bool MMAN_checkVSPAllocationPotential(MemoryManager* this, PCB* process) {
    return MMAN_getVSPAllocation(this, process) == NULL ? false : true;
}

MemoryRegion_deque* MMAN_getPAGAllocation(MemoryManager* this, PCB* process) {
    int sizeNeeded = AddressSpace_deque_peekF(&process->a_space)->size;
    return MMAN_getPageFitAllocation(this, sizeNeeded);
}

bool MMAN_checkPAGAllocationPotential(MemoryManager* this, PCB* process) {
    int sizeNeeded = AddressSpace_deque_peekF(&process->a_space)->size;
    return MMAN_getPageFitAllocation(this, sizeNeeded) == NULL ? false : true;
}

MemoryRegion_deque* MMAN_getSEGAllocation(MemoryManager* this, PCB* process) {
    MemoryRegion_deque* segments = malloc(sizeof (MemoryRegion_deque));
    MemoryRegion_deque_init(segments, false, false);
    AddressSpace_dequeI it;
    AddressSpace_dequeI_init(&it, &process->a_space);
    AddressSpace* as = AddressSpace_dequeI_examine(&it);
    bool allAllocated = true;
    while (as != NULL && allAllocated == true) {
        MemoryRegion* seg = MMAN_getFitAllocation(this, as->size);
        if (seg == NULL) {
            allAllocated = false;
            break;
        }
        MemoryRegion* allocedSeg = MMAN__Allocate(this, seg, -2, as->size, -1);
        MemoryRegion_deque_pushL(segments, allocedSeg);
        as = AddressSpace_dequeI_next(&it);
    }
    if (!allAllocated) {
        MMAN_deAllocateProcess(this, -2);
        MemoryRegion_deque_empty(segments);
        free(segments);
        return NULL;
    }
    return segments;
}

bool MMAN_checkSEGAllocationPotential(MemoryManager* this, PCB* process) {
    MemoryRegion_deque* segments = MMAN_getSEGAllocation(this, process);
    if (segments != NULL) {
        MMAN_deAllocateProcess(this, -2);
        MemoryRegion_deque_empty(segments);
        free(segments);
        return true;
    }
    return false;
}

bool MMAN_checkAllocationPotential(MemoryManager* this, PCB* process) {
    if (this->policy == MP_PAG) {
        return MMAN_checkPAGAllocationPotential(this, process);
    }
    if (this->policy == MP_SEG) {
        return MMAN_checkSEGAllocationPotential(this, process);
    }
    if (this->policy == MP_VSP) {
        return MMAN_checkVSPAllocationPotential(this, process);
    }
    if (this->policy == MP_INF) {
        return true;
    }
    return false;
}

/* === This section is messy and could be refactored to less code but it works well for now === */

int MMAN_getNumPagesNeeded(MemoryManager* this, int sizeNeeded) {
    //>>	Ceiling function in <math.h> wasn't cooperating, i guess this will work :(
    int neededPages = 0;
    while (sizeNeeded > 0) {
        neededPages++;
        sizeNeeded -= this->policyParams;
    }
    return neededPages;
}

MemoryRegion* MMAN_getFitAllocation(MemoryManager* this, int sizeNeeded) {
    MemoryRegion_dequeI it;
    MemoryRegion_dequeI_init(&it, &this->memory);
    MemoryRegion* mr = MemoryRegion_dequeI_examine(&it);
    MemoryRegion* fit = NULL;
    int fitSize = (this->policyParams == MFM_BEST ? INT_MAX : 0);
    while (mr != NULL) {
        int mrsize = mr->kiloEnd - mr->kiloStart + 1;
        if (mr->processId == -1 && mrsize >= sizeNeeded) {
            //>>	we found a big enough hole
            if (this->policyParams == MFM_FIRST) {
                return mr;
            }
            if ((this->policyParams == MFM_WORST && fitSize < mrsize) || (this->policyParams == MFM_BEST && fitSize > mrsize)) {
                fit = mr;
                fitSize = mrsize;
            }
        }
        mr = MemoryRegion_dequeI_next(&it);
    }
    return fit;

}

MemoryRegion_deque* MMAN_getPageFitAllocation(MemoryManager* this, int sizeNeeded) {
    MemoryRegion_dequeI it;
    MemoryRegion_dequeI_init(&it, &this->memory);
    MemoryRegion* mr = MemoryRegion_dequeI_examine(&it);

    MemoryRegion_deque* pages = malloc(sizeof (*pages));
    MemoryRegion_deque_init(pages, false, false);

    int neededPages = MMAN_getNumPagesNeeded(this, sizeNeeded);
    int currentPages = 0;
    while (mr != NULL) {
        if (mr->processId == -1) {
            //>>	Empty hole
            MemoryRegion_deque_pushL(pages, mr);
            currentPages++;
        }
        //>>	Stop when we found enough
        if (currentPages == neededPages) {
            return pages;
        }
        mr = MemoryRegion_dequeI_next(&it);
    }
    //>>	getting here means we didn't find enough, lets cleanup
    MemoryRegion_deque_empty(pages);
    return NULL;
}

/* === END MESSINESS === */

void MMAN_printMemoryMap(MemoryManager* this, Logger* logger) {
    MemoryRegion_dequeI it;
    MemoryRegion_dequeI_init(&it, &this->memory);
    MemoryRegion* mr = MemoryRegion_dequeI_examine(&it);
    logger->log(logger, LogLevel_INFO, "Memory Map:\n");
    if (this->policy == MP_PAG && mr != NULL) {
        int start = 0;
        int finish = 0;
        bool prevWasHole = false;
        char s[30];
        while (mr != NULL) {
            if (mr->processId == -1) {
                if (!prevWasHole) {
                    start = mr->kiloStart;
                    MR_toString(mr, s, "Free Frame(s)");
                }
                finish = mr->kiloEnd;
                prevWasHole = true;
                mr = MemoryRegion_dequeI_next(&it);
                continue;
            } else if (prevWasHole) { //>>	Process is not a hole, so print the total hole size
                logger->log(logger, LogLevel_INFO, "\t%7d-%-7d: %s\n", start, finish, s);
                prevWasHole = false;
            }
            logger->log(logger, LogLevel_INFO, "\t%7d-%-7d: %s\n", mr->kiloStart, mr->kiloEnd, MR_toString(mr, s, "Free Frame(s)"));
            mr = MemoryRegion_dequeI_next(&it);
        }
        if (prevWasHole) {
            logger->log(logger, LogLevel_INFO, "\t%7d-%-7d: %s\n", start, finish, s);
            prevWasHole = false;
        }
    } else {
        while (mr != NULL) {
            char s[30];
            logger->log(logger, LogLevel_INFO, "\t%7d-%-7d: %s\n", mr->kiloStart, mr->kiloEnd, MR_toString(mr, s, "Hole"));
            mr = MemoryRegion_dequeI_next(&it);
        }
    }
}

bool MMAN_allocateProcess(MemoryManager* this, PCB* process, Logger* logger) {
    char s[16];
    logger->log(logger, LogLevel_INFO, "MM moves %s to memory\n", PCB_toString(process, s));
    if (this->policy == MP_VSP) {
        MemoryRegion* tobealloced = MMAN_getVSPAllocation(this, process);
        MMAN__Allocate(this, tobealloced, process->id, AddressSpace_deque_pollF(&process->a_space)->size, -1);
        return true;
    } else if (this->policy == MP_SEG) {
        MemoryRegion_deque* tobealloced = MMAN_getSEGAllocation(this, process);
        if (tobealloced != NULL) {
            MemoryRegion* procAlloc = MemoryRegion_deque_pollF(tobealloced);
            int segmentId = 1;
            while (procAlloc != NULL) {
                procAlloc->isSeg = true;
                procAlloc->partition = segmentId++;
                procAlloc->processId = process->id;
                procAlloc = MemoryRegion_deque_pollF(tobealloced);
            }
            free(tobealloced);
            return true;
        }
        return false;
    } else if (this->policy == MP_PAG) {
        //>>	get the proper locations to allocate
        MemoryRegion_deque* pages = MMAN_getPAGAllocation(this, process);
        //>>	provided we were able to find space, do allocation and return true
        if (pages != NULL) {
            MemoryRegion_dequeI it;
            MemoryRegion_dequeI_init(&it, pages);
            MemoryRegion* page = MemoryRegion_dequeI_examine(&it);
            int currentPage = 1;
            while (page != NULL) {
                page->isSeg = false;
                page->partition = currentPage++;
                page->processId = process->id;
                page = MemoryRegion_dequeI_next(&it);
            }
            return true;
        }
        //>>	Else return false
        return false;
    }
    //>>	Else return false
    return false;

}

MemoryRegion* MMAN__Allocate(MemoryManager* this, MemoryRegion* tobealloced, int id, int size, int partition) {
    //>>	Create a new Region with at the start address of allocatable space
    //>>	and an ending address the approp. size away
    if (tobealloced->kiloEnd == tobealloced->kiloStart + size - 1) {
        //>>	If the node we are using as allocation space is exactly 
        //>>	how big we need, then just assign it to the process
        tobealloced->processId = id;
        if (this->policy == MP_SEG) {
            tobealloced->isSeg = true;
            tobealloced->partition = partition;
        }
        return tobealloced;
    } //>>	Otherwise
    //>>	Create a new Region with at the start address of allocatable space
    //>>	and an ending address the approp. size away
    MemoryRegion * new;
    if (this->policy == MP_SEG) {
        new = MemoryRegion_initSeg(id, tobealloced->kiloStart, tobealloced->kiloStart + size - 1, partition);
    } else {
        new = MemoryRegion_init(id, tobealloced->kiloStart, tobealloced->kiloStart + size - 1);
    }
    //>>	allocatable space gets shortened to start right after new ends
    tobealloced->kiloStart = new->kiloEnd + 1;
    //>>	get the CAR of the data
    MemoryRegion_dequeN* car = MemoryRegion_deque_getCar(&this->memory, tobealloced);
    //>>	push new directly before CAR
    MemoryRegion_deque_pushB(&this->memory, car, new);
    return new;
}

void MMAN_deAllocateProcess(MemoryManager* this, int id) {
    MemoryRegion_dequeI it;
    MemoryRegion_dequeI_init(&it, &this->memory);
    MemoryRegion* mr = MemoryRegion_dequeI_examine(&it);
    while (mr != NULL) {
        if (mr->processId == id) {
            //>>	for paged memory, we simply reset the nodes matching the processID
            MR_reset(mr);
            //>>	for the other two we also want to coalesce memory
            if (this->policy == MP_SEG || this->policy == MP_VSP) {
                MMAN_coalesceMemory(this, mr);
            }
        }
        mr = MemoryRegion_dequeI_next(&it);
    }

}

void MMAN_coalesceMemory(MemoryManager* this, MemoryRegion* data) {
    //>>	get the node with next and prev pointers from the deque
    MemoryRegion_dequeN* car = MemoryRegion_deque_getCar(&this->memory, data);
    //>>	if previous is also a hole, combine it
    if (car->prev != NULL && car->prev->data->processId == -1) {
        MemoryRegion* previous = car->prev->data;
        data->kiloStart = previous->kiloStart;
        MemoryRegion_deque_pollN(&this->memory, previous);
    }
    //>>	if next is also a hole, combine it
    if (car->next != NULL && car->next->data->processId == -1) {
        MemoryRegion* next = car->next->data;
        data->kiloEnd = next->kiloEnd;
        MemoryRegion_deque_pollN(&this->memory, next);
    }
    //>>	That was Easy! We are done.
}