/*
 * File:   AddressSpace.c
 * Author: Timethor
 *
 * Created on November 3, 2013, 6:04 AM
 */

#include <stdlib.h>
#include <string.h>

#include "AddressSpace.h"

AddressSpace* AddressSpace_init(int param, enum LogLevel level) {
    AddressSpace* this = malloc(sizeof (*this));
    this->param = param;
    return this;
}

void AddressSpace_destruct(AddressSpace* this) {
    free(this);
}