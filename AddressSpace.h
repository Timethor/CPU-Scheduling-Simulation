/*
 * File:   AddressSpace.h
 * Author: Timethor
 *
 * Created on November 3, 2013, 6:04 AM
 */

#ifndef ADDRESSSPACE_H
#define	ADDRESSSPACE_H

typedef struct AddressSpace {
    int param;
} AddressSpace;

//>>	Construct / Destruct
AddressSpace* AddressSpace_init(int param, enum LogLevel level);
void AddressSpace_destruct(AddressSpace * this);

#endif	/* ADDRESSSPACE_H */