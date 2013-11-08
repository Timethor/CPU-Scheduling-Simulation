/*
 * File:   MemoryManager.c
 * Author: Timethor
 *
 * Created on November 4, 2013, 4:23 PM
 */

#define _GNU_SOURCE
#include <stdlib.h>
#include <limits.h>

#include "MemoryManager.h"

MemoryManager* MemoryManager_init(enum MemoryPolicy policy, int size, int params) {
    MemoryManager* this = malloc(sizeof (*this));
    this->memKiloSize = size;
    this->policy = policy;
    this->policyParams = params;
    this->freeSpace = 0;
    MemoryRegion_deque_init(&this->memory, false, false);
    if (policy == MP_PAG) {
        int index = 0;
        while (index + params < size - 1) {
            MemoryRegion* new = MemoryRegion_init(-1, index, index + (params - 1));
            MemoryRegion_deque_pushL(&this->memory, new);
            index += params;
            //>>	Free space shall be calculated based on page size * number of pages
            this->freeSpace += params;
        }
        //>>	Do paging setup
    } else if (policy == MP_INF) {
        //>>	Do setup for infinite memory, i.e. NOTHING :D
    } else if (policy == MP_SEG || policy == MP_VSP) {
        //>>	Do setup generic setup
        this->freeSpace = size;
        MemoryRegion* new = MemoryRegion_init(-1, 0, size - 1);
        MemoryRegion_deque_pushL(&this->memory, new);
    }
    return this;
}

void MemoryManager_destruct(MemoryManager * this) {
    free(this);
}

bool MMAN_checkVSPAllocationPotential(MemoryManager* this, PCB* process) {
    int sizeNeeded = AddressSpace_deque_peekF(&process->a_space)->size;
    if (this->policyParams == MFM_BEST && MMAN_getBestFitAllocation(this, sizeNeeded) != NULL)
        return true;
    if (this->policyParams == MFM_WORST && MMAN_getWorstFitAllocation(this, sizeNeeded) != NULL)
        return true;
    if (this->policyParams == MFM_FIRST && MMAN_getFirstFitAllocation(this, sizeNeeded) != NULL)
        return true;
    return false;
}

bool MMAN_checkPAGAllocationPotential(MemoryManager* this, PCB* process) {
    return MMAN_checkVSPAllocationPotential(this, process);
}

bool MMAN_checkSEGAllocationPotential(MemoryManager* this, PCB* process) {
    MemoryRegion_deque segments;
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
            seg->processId = -2;
            MemoryRegion_deque_pushL(&segments, seg);
            as = AddressSpace_dequeI_next(&it);
        }
    }
    if (this->policyParams == MFM_WORST) {
        while (as != NULL && allAllocated == true) {
            MemoryRegion* seg = MMAN_getWorstFitAllocation(this, as->size);
            if (seg == NULL) {
                allAllocated = false;
                break;
            }
            seg->processId = -2;
            MemoryRegion_deque_pushL(&segments, seg);
            as = AddressSpace_dequeI_next(&it);
        }
    }
    if (this->policyParams == MFM_FIRST) {
        while (as != NULL && allAllocated == true) {
            MemoryRegion* seg = MMAN_getFirstFitAllocation(this, as->size);
            if (seg == NULL) {
                allAllocated = false;
                break;
            }
            seg->processId = -2;
            MemoryRegion_deque_pushL(&segments, seg);
            as = AddressSpace_dequeI_next(&it);
        }
    }
    MemoryRegion* r = MemoryRegion_deque_pollF(&segments);
    while (r != NULL) {
        //>>	reset and delink prospective nodes
        r->processId = -1;
        r = MemoryRegion_deque_pollF(&segments);
    }
    return allAllocated;
}

bool MMAN_checkAllocationPotential(MemoryManager* this, PCB* process) {
    AddressSpace_dequeI it;
    AddressSpace_dequeI_init(&it, &process->a_space);
    AddressSpace* as = AddressSpace_dequeI_examine(&it);
    int totalAddressSize = 0;
    while (as != NULL) {
        totalAddressSize += as->size;
        as = AddressSpace_dequeI_next(&it);
    }
    if (this->freeSpace < totalAddressSize) {
        return false;
    }
    //>>	END basic Check
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
    } else if (this->policy == MP_PAG) {
        //>>	Ceiling function in <math.h> wasnt cooperating, i guess this will work :(
        int neededPages = 0;
        int currentPages = sizeNeeded; //>>	used temporarily to calculate pages needed
        while (currentPages > 0) {
            neededPages++;
            currentPages -= this->policyParams;
        }
        currentPages = 0; //>>	Reset to real initial value
        MemoryRegion* current = NULL;
        while (mr != NULL) {
            if (mr->processId == -1) {
                //>>	Empty hole
                if (currentPages == 0) {
                    //>>	Starting to find big enough hole
                    current = mr;
                }
                currentPages++;
            } else if (currentPages != 0) {
                //>>	Exiting an empty hole
                if (currentPages >= neededPages) {
                    if (currentPages < bestSize) {
                        bestSize = currentPages;
                        best = current;
                    }
                } else {
                    current = NULL;
                    currentPages = 0;
                }
            }
            if (currentPages == neededPages) {
                return current;
            }
            mr = MemoryRegion_dequeI_next(&it);
        }
        //>>	Make sure we get holes at the end of the space
        if (currentPages >= neededPages) {
            if (currentPages < bestSize) {
                bestSize = currentPages;
                best = current;
            }
        } else {
            current = NULL;
            currentPages = 0;
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
    } else if (this->policy == MP_PAG) {
        //>>	Ceiling function in <math.h> wasnt cooperating, i guess this will work :(
        int neededPages = 0;
        int currentPages = sizeNeeded; //>>	used temporarily to calculate pages needed
        while (currentPages > 0) {
            neededPages++;
            currentPages -= this->policyParams;
        }
        currentPages = 0; //>>	Reset to real initial value
        MemoryRegion* current = NULL;
        while (mr != NULL) {
            if (mr->processId == -1) {
                //>>	Empty hole
                if (currentPages == 0) {
                    //>>	Starting to find big enough hole
                    current = mr;
                }
                currentPages++;
            } else if (currentPages != 0) {
                //>>	Exiting an empty hole
                if (currentPages >= neededPages) {
                    return current;
                } else {
                    current = NULL;
                    currentPages = 0;
                }
            }
            mr = MemoryRegion_dequeI_next(&it);
        }
        //>>	Make sure we get holes at the end of the space
        if (currentPages >= neededPages) {
            return fit;
        } else {
            current = NULL;
            currentPages = 0;
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
    } else if (this->policy == MP_PAG) {
        //>>	Ceiling function in <math.h> wasnt cooperating, i guess this will work :(
        int neededPages = 0;
        int currentPages = sizeNeeded; //>>	used temporarily to calculate pages needed
        while (currentPages > 0) {
            neededPages++;
            currentPages -= this->policyParams;
        }
        currentPages = 0; //>>	Reset to real initial value
        MemoryRegion* current = NULL;
        while (mr != NULL) {
            if (mr->processId == -1) {
                //>>	Empty hole
                if (currentPages == 0) {
                    //>>	Starting to find big enough hole
                    current = mr;
                }
                currentPages++;
            } else if (currentPages != 0) {
                //>>	Exiting an empty hole
                if (currentPages >= neededPages) {
                    if (currentPages > worstSize) {
                        worstSize = currentPages;
                        worst = current;
                    }
                } else {
                    current = NULL;
                    currentPages = 0;
                }
            }
            if (currentPages == neededPages) {
                return current;
            }
            mr = MemoryRegion_dequeI_next(&it);
        }
        //>>	Make sure we get holes at the end of the space
        if (currentPages >= neededPages) {
            if (currentPages > worstSize) {
                worstSize = currentPages;
                worst = current;
            }
        } else {
            current = NULL;
            currentPages = 0;
        }
    }
    return worst;
}

/* === END MESSINESS === */

void MMAN_printMemoryMap(MemoryManager* this, Logger* logger) {
    MemoryRegion_dequeI it;
    MemoryRegion_dequeI_init(&it, &this->memory);
    MemoryRegion* mr = MemoryRegion_dequeI_examine(&it);
    if (this->policy == MP_PAG) {
        while (mr != NULL) {
            mr = MemoryRegion_dequeI_next(&it);
        }
    }
}

bool MMAN_allocateProcess(MemoryManager* this, PCB* process) {
    return false;
}

void MMAN_deAllocateProcess(MemoryManager* this, PCB* process) {
    MemoryRegion_dequeI it;
    MemoryRegion_dequeI_init(&it, &this->memory);
    MemoryRegion* mr = MemoryRegion_dequeI_examine(&it);
    if (this->policy == MP_PAG) {
        while (mr != NULL) {
            if (mr->processId == process->id) {
                mr->processId = -1;
            }
            mr = MemoryRegion_dequeI_next(&it);
        }
    }
}