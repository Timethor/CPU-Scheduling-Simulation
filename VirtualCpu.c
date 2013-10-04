#define _GNU_SOURCE
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "VirtualCpu.h"

VirtualCPU* VCPU_init(InputState* istate, Settings* settings) {
    VirtualCPU* this = malloc(sizeof (*this));
    this->clockTime = 0;
    this->settings = settings;
    this->
    DD_deque_init(&this->devices, false, false);
    PQ_deque_init(&this->queues, false, false);
    printf("STARTING SYNC-MERGE\n");
    VCPU_MergeWithInputState(this, istate);
    return this;
}

void VCPU_MergeWithInputState(VirtualCPU* this, InputState* istate) {
    bool(*LoggerTest) (Settings*, int) = this->settings->canLog;
    int i;
    for (i = DD_deque_length(&istate->proto_devices); i > 0; i--) {
        DD_deque_pushL(&this->devices, DD_deque_pollF(&istate->proto_devices));
    }
    for (i = PQ_deque_length(&istate->proto_queues); i > 0; i--) {
        PQ_deque_pushL(&this->queues, PQ_deque_pollF(&istate->proto_queues));
    }
    LoggerTest(this->settings, 0) && printf("After Print");
    LoggerTest(this->settings, 0) && printf("SIZEOF:: THIS_>PROCESQUEUE:: %d\n\n", PQ_deque_length(&this->queues));
    LoggerTest(this->settings, 0) && printf("QUEUE HEAD ID: %d\n", this->queues.head->data->id);
    PQ_deque_print(&this->queues);
}

bool VCPU_checkDeviceQueuesClear(VirtualCPU* this) {
    DD_dequeI ddI;
    DD_dequeI_init(&ddI, &this->devices);
    DeviceDescriptor* current = DD_dequeI_examine(&ddI);
    bool clear = true;
    while (current != NULL && clear == true) {
        if (!PCB_deque_empty(&current->queue))
            clear = false;
        current = DD_dequeI_next(&ddI);
    }
    return clear;
}

bool VCPU_canEnd(VirtualCPU* this) {
    PQ* running = VCPU_getHighestRunningProcessQueue(this);
    PQ* waiting = VCPU_getHighestWaitingProcessQueue(this);
    if (running != NULL || waiting != NULL) {
        return false;
    }
    return VCPU_checkDeviceQueuesClear(this);
}

bool VCPU_doClockCycle(VirtualCPU* this, PCB_deque* notYetArrived) {
    printf("\n==========================  CPU Time: %-4d  ==========================\n", this->clockTime);
    //>>	Process Preempt Check
    VCPU_doPreemptProcess(this);
    //>>	Check Completion
    VCPU_doCheckProcessStateChange(this);
    //>>	Process Arriving Processes
    VCPU_doProcessArrivingProcesses(this, notYetArrived);
    //>>	Grab a process from Ready Queues
    VCPU_doDispatcherProcessing(this);
    //>>	Do a system wide tick for all nodes in all queues
    VCPU_doSystemWideTick(this);
    //>>	Check if we are done

    return !VCPU_canEnd(this);
}

PQ* VCPU_getHighestWaitingProcessQueue(VirtualCPU* this) {
    PQ_dequeI pqI;
    PQ_dequeI_init(&pqI, &this->queues);
    PQ* current = PQ_dequeI_examine(&pqI);
    while (current != NULL) {
        if (PQ_hasWaitingProcess(current))

            return current;
        current = PQ_dequeI_next(&pqI);
    }
    return NULL;
}

PQ* VCPU_getHighestRunningProcessQueue(VirtualCPU* this) {
    PQ_dequeI pqI;
    PQ_dequeI_init(&pqI, &this->queues);
    PQ* current = PQ_dequeI_examine(&pqI);
    while (current != NULL) {
        if (PQ_hasRunningProcess(current))

            return current;
        current = PQ_dequeI_next(&pqI);
    }
    return NULL;
}

/*
 *  Iterate over process queues
 *       For each Queue
 *          if queue not empty && roundRobin
 *              is queue head in running state?
 *                  yes: is Queue quantum  == quantum check?
 *                      yes: pollF queue, send to next queue
 *                  no: continue
 */
void VCPU_doPreemptProcess(VirtualCPU* this) {
    PQ* running = VCPU_getHighestRunningProcessQueue(this);
    PQ* waiting = VCPU_getHighestWaitingProcessQueue(this);
    if (running != NULL && waiting != NULL && waiting->id < running->id) {
        PQ_stopRunningProcess(running);
    }
    PQ_dequeI pqI;
    PQ_dequeI_init(&pqI, &this->queues);
    running = PQ_dequeI_examine(&pqI);
    while (running != NULL) {
        PCB* movingProcess = PQ_getQuantumViolator(running);
        PQ* prev = running;
        running = PQ_dequeI_next(&pqI);
        if (movingProcess != NULL) {
            PCB_toString(movingProcess);
            printf(" is preempted\n");
            movingProcess->state = PCB_WAITING;
            PCB_toString(movingProcess);
            if (running != NULL) {
                printf(" is demoted to Ready Queue Q%d\n", running->id);
                PQ_enqueueProcess(running, movingProcess);
            } else {
                printf(" is requeued to Ready Queue Q%d\n", running->id);
                PQ_enqueueProcess(prev, movingProcess);
            }
            VCPU_doPrintQueues(this);
            movingProcess = NULL;
        }
    }
}

void VCPU_doPrintQueues(VirtualCPU* this) {
    PQ_deque_print(&this->queues);
    DD_deque_print(&this->devices);
}

/*
 *  For each processQueue
 *      check for PCB with state == PCB_BURST_FINISHED
 *          do DD_deque_ProcArrival()
 * 
 *  For each deviceQueue
 *      check for PCB with state == PCB_BURST_FINISHED
 *          do PQ_deque_ProcArrival()
 */
void VCPU_doCheckProcessStateChange(VirtualCPU* this) {
    PQ_dequeI pqI;
    PQ_dequeI_init(&pqI, &this->queues);
    PQ* currentPQ = PQ_dequeI_examine(&pqI);
    while (currentPQ != NULL) {
        PCB* requester = PQ_hasBurstEndedProcess(currentPQ);
        if (requester != NULL) {
            printf("CPU Burst of ");
            PCB_toString(requester);
            printf(" completes\n");
            VCPU_doPrintQueues(this);
            if (requester->state != PCB_TERMINATED) {
                requester->state = PCB_WAITING;
                DD_deque_ProcArrival(&this->devices, requester);
                VCPU_doPrintQueues(this);
            } else {
                PCB_toString(requester);
                printf(" completes\n");
                PCB_deque_pushL(&this->terminated, requester);
                VCPU_doPrintQueues(this);
            }
        }
        currentPQ = PQ_dequeI_next(&pqI);
    }

    DD_dequeI ddI;
    DD_dequeI_init(&ddI, &this->devices);
    DeviceDescriptor* currentDD = DD_dequeI_examine(&ddI);
    while (currentDD != NULL) {
        PCB* requester = DD_hasBurstEndedProcess(currentDD);
        if (requester != NULL) {
            printf("I/O Burst of ");
            PCB_toString(requester);
            printf(" completes\n");
            VCPU_doPrintQueues(this);
            if (requester->state != PCB_TERMINATED) {
                currentDD->state = DD_IDLE;
                requester->state = PCB_WAITING;
                PQ_deque_ProcArrival(&this->queues, requester);
                VCPU_doPrintQueues(this);
            } else {
                PCB_toString(requester);
                printf(" completes\n");
                PCB_deque_pushL(&this->terminated, requester);
                VCPU_doPrintQueues(this);
            }
        }
        currentDD = DD_dequeI_next(&ddI);
    }
}

void VCPU_doProcessArrivingProcesses(VirtualCPU* this, PCB_deque* notYetArrived) {

    while (!PCB_deque_empty(notYetArrived) && PCB_deque_peekF(notYetArrived)->arrival_time == this->clockTime) {
        PCB* requester = PCB_deque_pollF(notYetArrived);

        if (BurstNode_deque_peekF(&requester->schedule)->type == BT_CPU) {
            PQ_deque_ProcArrival(&this->queues, requester);
            VCPU_doPrintQueues(this);
            //>>	Preempt any running process in lower queues
            PQ* running = VCPU_getHighestRunningProcessQueue(this);
            PQ* front = PQ_deque_peekF(&this->queues);
            if (front != NULL && running != NULL && front->id < running->id) {
                PQ_stopRunningProcess(running);
            }
        } else if (BurstNode_deque_peekF(&requester->schedule)->type == BT_IO) {

            DD_deque_ProcArrival(&this->devices, requester);
        }
    }
}

/*
 *  Can we find a queue that is running?
 *      yes: no need to dispatch
 *      no: can we find a queue that is waiting?
 *          yes: try to start it
 *          no: oh well
 * 
 *  Iterate over device queues
 *      For each Queue
 *          if queue not empty
 *              is head process running?
 *                  yes: continue
 *                  no: set to running state
 */
void VCPU_doDispatcherProcessing(VirtualCPU * this) {
    //>>	If no queues are running processes, get the highest priority queue 
    //>>	and start its first process
    if (VCPU_getHighestRunningProcessQueue(this) == NULL) {
        PQ_dequeI pqI;
        PQ_dequeI_init(&pqI, &this->queues);
        PQ* waiting = VCPU_getHighestWaitingProcessQueue(this);
        if (waiting != NULL) {
            PQ_startWaitingProcess(waiting);
            VCPU_doPrintQueues(this);
            PCB_toString(PCB_deque_peekF(&waiting->queue));
            printf(" starts running\n");
        }
    }

    //>>	For each Device, if !busy, get it working
    DD_dequeI ddI;
    DD_dequeI_init(&ddI, &this->devices);
    DeviceDescriptor* current = DD_dequeI_examine(&ddI);
    while (current != NULL) {

        DD_tryActivateDevice(current);
        current = DD_dequeI_next(&ddI);
    }
}

void VCPU_doSystemWideTick(VirtualCPU * this) {
    this->clockTime++;
    PQ_deque_SystemWideTick(&this->queues);
    DD_deque_SystemWideTick(&this->devices);
}