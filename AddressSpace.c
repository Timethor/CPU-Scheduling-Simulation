/*
 * File:   AddressSpace.c
 * Author: Timethor
 *
 * Created on November 3, 2013, 6:04 AM
 */

#include <stdlib.h>
#include <string.h>

#include "AddressSpace.h"

AddressSpace* AddressSpace_init(int size) {
    AddressSpace* this = malloc(sizeof (*this));
    this->size = size;
    return this;
}

void AddressSpace_destruct(AddressSpace* this) {
    free(this);
}