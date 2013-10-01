#include <stdlib.h>
#include "ProcessControlBlock.h"

//#define DEBUG
#include "debug.h"

DEQUEUE(PCB);

// Comparing functions for qsort to use
int compare_Job_Arrival(const PCB** job1, const PCB** job2){
	return ((*job1)->arrival_time)-((*job2)->arrival_time);
}