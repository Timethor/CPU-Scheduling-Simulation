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
    int sizeNeeded = AddressSpace_deque_peekF(&process->a_space)->size;
    MemoryRegion* fit = NULL;
    if (this->policyParams == MFM_BEST)
        fit = MMAN_getBestFitAllocation(this, sizeNeeded);
    else if (this->policyParams == MFM_WORST)
        fit = MMAN_getWorstFitAllocation(this, sizeNeeded);
    else if (this->policyParams == MFM_FIRST)
        fit = MMAN_getFirstFitAllocation(this, sizeNeeded);
    return fit;
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
    AddressSpace_dequeI it;
    AddressSpace_dequeI_init(&it, &process->a_space);
    AddressSpace* as = AddressSpace_dequeI_examine(&it);
    bool allAllocated = true;
    if (this->policyParams == MFM_BEST) {
        while (as != NULL && allAllocated == true) {
            MemoryRegion* seg = MMAN_getBestFitAllocation(this, as->size);
            if (seg == NULL) {
                allAllocated = false;
                break;
            }
            MemoryRegion_deque_pushL(segments, seg);
            as = AddressSpace_dequeI_next(&it);
        }
    } else if (this->policyParams == MFM_WORST) {
        while (as != NULL && allAllocated == true) {
            MemoryRegion* seg = MMAN_getWorstFitAllocation(this, as->size);
            if (seg == NULL) {
                allAllocated = false;
                break;
            }
            MemoryRegion_deque_pushL(segments, seg);
            as = AddressSpace_dequeI_next(&it);
        }
    } else if (this->policyParams == MFM_FIRST) {
        while (as != NULL && allAllocated == true) {
            MemoryRegion* seg = MMAN_getFirstFitAllocation(this, as->size);
            if (seg == NULL) {
                allAllocated = false;
                break;
            }
            MemoryRegion_deque_pushL(segments, seg);
            as = AddressSpace_dequeI_next(&it);
        }  
    }
    if (!allAllocated) {
        MemoryRegion_deque_empty(segments);
        free(segments);
        return NULL;
    }
    return segments;
}

bool MMAN_checkSEGAllocationPotential(MemoryManager* this, PCB* process) {
    MemoryRegion_deque* segments = MMAN_getSEGAllocation(this, process);
    if (segments != NULL) {
        MemoryRegion * r = MemoryRegion_deque_pollF(segments);
        while (r != NULL) {
            //>>	reset and delink prospective nodes
            MR_reset(r);
            r = MemoryRegion_deque_pollF(segments);
        }
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

MemoryRegion* MMAN_getBestFitAllocation(MemoryManager* this, int sizeNeeded) {
    MemoryRegion_dequeI it;
    MemoryRegion_dequeI_init(&it, &this->memory);
    MemoryRegion* mr = MemoryRegion_dequeI_examine(&it);
    MemoryRegion* best = NULL;
    int bestSize = INT_MAX;
    if (this->policy == MP_VSP || this->policy == MP_SEG) {
        while (mr != NULL) {
            if (mr->processId == -1 && (mr->kiloEnd - mr->kiloStart + 1 >= sizeNeeded)) {
                //>>	we found a big enough hole
                if (bestSize > (mr->kiloEnd - mr->kiloStart + 1)) {
                    best = mr;
                    bestSize = (mr->kiloEnd - mr->kiloStart + 1);
                }
            }
            mr = MemoryRegion_dequeI_next(&it);
        }
    }
    return best;
}

MemoryRegion* MMAN_getFirstFitAllocation(MemoryManager* this, int sizeNeeded) {
    MemoryRegion_dequeI it;
    MemoryRegion_dequeI_init(&it, &this->memory);
    MemoryRegion* mr = MemoryRegion_dequeI_examine(&it);
    MemoryRegion* fit = NULL;
    if (this->policy == MP_VSP || this->policy == MP_SEG) {
        while (mr != NULL) {
            if (mr->processId == -1 && (mr->kiloEnd - mr->kiloStart + 1 >= sizeNeeded)) {
                //>>	we found a big enough hole
                return mr;
            }
            mr = MemoryRegion_dequeI_next(&it);
        }
    }
    return fit;

}

MemoryRegion* MMAN_getWorstFitAllocation(MemoryManager* this, int sizeNeeded) {
    MemoryRegion_dequeI it;
    MemoryRegion_dequeI_init(&it, &this->memory);
    MemoryRegion* mr = MemoryRegion_dequeI_examine(&it);
    MemoryRegion* worst = NULL;
    int worstSize = 0;
    if (this->policy == MP_VSP || this->policy == MP_SEG) {
        while (mr != NULL) {
            if (mr->processId == -1 && (mr->kiloEnd - mr->kiloStart + 1 >= sizeNeeded)) {
                //>>	we found a big enough hole
                if (worstSize < (mr->kiloEnd - mr->kiloStart + 1)) {
                    worst = mr;
                    worstSize = (mr->kiloEnd - mr->kiloStart + 1);
                }
            }
            mr = MemoryRegion_dequeI_next(&it);
        }
    }
    return worst;
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
    while (mr != NULL) {
        char s[30];
        logger->log(logger, LogLevel_INFO, "\t%7d-%-7d: %s\n", mr->kiloStart, mr->kiloEnd, MR_toString(mr, s));
        mr = MemoryRegion_dequeI_next(&it);
    }
}

bool MMAN_allocateProcess(MemoryManager* this, PCB* process, Logger* logger) {
    char s[16];
    logger->log(logger, LogLevel_INFO, "MM moves %s to memory\n", PCB_toString(process, s));
    if (this->policy == MP_VSP) {
        MemoryRegion* tobealloced = MMAN_getVSPAllocation(this, process);
        //>>	Create a new Region with at the start address of allocatable space
        //>>	and an ending address the approp. size away
        //        this->freeSpace -= AddressSpace_deque_peekF(&process->a_space)->size;
        MemoryRegion* new = MemoryRegion_init(process->id, tobealloced->kiloStart, tobealloced->kiloStart + AddressSpace_deque_pollF(&process->a_space)->size - 1);
        //>>	allocatable space gets shortened to start right after new ends
        tobealloced->kiloStart = new->kiloEnd + 1;
        //>>	get the CAR of the data
        MemoryRegion_dequeN* car = MemoryRegion_deque_getCar(&this->memory, tobealloced);
        //>>	push new directly before CAR
        MemoryRegion_deque_pushB(&this->memory, car, new);
        if (MemoryRegion_deque_peekL(&this->memory)->kiloEnd >= this->memKiloSize || MemoryRegion_deque_peekL(&this->memory)->kiloStart >= this->memKiloSize) {
            MemoryRegion_deque_pollL(&this->memory);
        }
        return true;
    } else if (this->policy == MP_SEG) {
        MemoryRegion_deque* tobealloced = MMAN_getSEGAllocation(this, process);
        MemoryRegion* procAlloc = MemoryRegion_deque_pollF(tobealloced);
        int segmentId = 1;
        while (procAlloc != NULL) {
            //>>	Create a new Region with at the start address of allocatable space
            //>>	and an ending address the approp. size away
            //            this->freeSpace -= AddressSpace_deque_peekF(&process->a_space)->size;
            MemoryRegion* new = MemoryRegion_initSeg(process->id, procAlloc->kiloStart, procAlloc->kiloStart + AddressSpace_deque_pollF(&process->a_space)->size - 1, segmentId++);
            //>>	allocatable space gets shortened to start right after new ends
            procAlloc->kiloStart = new->kiloEnd + 1;
            //>>	get the CAR of the data
            MemoryRegion_dequeN* car = MemoryRegion_deque_getCar(&this->memory, procAlloc);
            //>>	push new directly before CAR
            MemoryRegion_deque_pushB(&this->memory, car, new);
            procAlloc = MemoryRegion_deque_pollF(tobealloced);
        }
        if (MemoryRegion_deque_peekL(&this->memory)->kiloEnd >= this->memKiloSize || MemoryRegion_deque_peekL(&this->memory)->kiloStart >= this->memKiloSize) {
            MemoryRegion_deque_pollL(&this->memory);
        }
        MemoryRegion_deque_freeElements(tobealloced);
        free(tobealloced);
        return true;
    } else if (this->policy == MP_PAG) {
        MemoryRegion_deque* pages = MMAN_getPAGAllocation(this, process);
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
        return false;
    }
    return false;

}

void MMAN_deAllocateProcess(MemoryManager* this, PCB* process, Logger* logger) {
    MemoryRegion_dequeI it;
    MemoryRegion_dequeI_init(&it, &this->memory);
    MemoryRegion* mr = MemoryRegion_dequeI_examine(&it);
    if (this->policy == MP_PAG) {
        while (mr != NULL) {
            if (mr->processId == process->id) {
                MR_reset(mr);
            }
            mr = MemoryRegion_dequeI_next(&it);
        }
    } else if (this->policy == MP_SEG || this->policy == MP_VSP) {
        while (mr != NULL) {
            if (mr->processId == process->id) {
                MR_reset(mr);
                MMAN_coaleseMemory(this, mr, logger);
            }
            mr = MemoryRegion_dequeI_next(&it);
        }
    }
}

void MMAN_coaleseMemory(MemoryManager* this, MemoryRegion* data, Logger* logger) {
    MemoryRegion_dequeN* car = MemoryRegion_deque_getCar(&this->memory, data);
    if (car->prev != NULL && car->prev->data->processId == -1) {
        MemoryRegion* previous = car->prev->data;
        data->kiloStart = previous->kiloStart;
        MemoryRegion_deque_pollN(&this->memory, previous);
    }
    if (car->next != NULL && car->next->data->processId == -1) {
        MemoryRegion* next = car->next->data;
        data->kiloEnd = next->kiloEnd;
        MemoryRegion_deque_pollN(&this->memory, next);
    }

}