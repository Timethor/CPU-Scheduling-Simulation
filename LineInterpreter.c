#define _GNU_SOURCE
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "LineInterpreter.h"

SimulationState* SimulationState_init() {
    SimulationState* this = malloc(sizeof (*this));
    this->in_comment = false;
    this->stage = FS_TQ;
    this->seen_stage_req = -1;
    this->error_thrown = false;
    DeviceDescriptor_deque_init(&this->proto_devices, false, false);
    ProcessQueue_deque_init(&this->proto_queues, false, false);
    PCB_deque_init(&this->notYetArrived, false, false);
    this->call_back_read = SS_processInputLine;
    this->bn = NULL;
    return this;
}

void SimulationState_destruct(SimulationState* this){
    DeviceDescriptor_deque_freeElements(&this->proto_devices);
    ProcessQueue_deque_freeElements(&this->proto_queues);
    PCB_deque_freeElements(&this->notYetArrived);
    free(this);
}

/*
 * Callback function for mmap file write, appropriately sets line_buffer to the value
 * of the memory location passed
 */
void SS_processOutputLine(SimulationState* this, const char* begin, const char* end) {
    
}


/*
 * Callback function for mmap file read, appropriately sets line_buffer to the value
 * of the memory location passed
 */
void SS_processInputLine(SimulationState* this, const char* begin, const char* end) {
    //>>	Copy values from memory at beginning -> end into line_buffer
    char line[end - begin + 1];
    int copyend = (end - begin + 1);
    strncpy(line, begin, copyend);
    line[copyend] = 0;

    //>>	Determine what kind of line we have
    //    printf("============================================================\n|\tSTAGE ::: %d\n============================================================\n", this->stage);
    if (SS_hasNonProcessableLine(this, line)) {
        return;
    }
    if (this->stage == FS_TQ && SS_processLineForTimeQuantum(this, line)) {
        return;
    }
    if ((this->stage == FS_PN_NEW || this->stage == FS_TQ) && SS_processLineForNewProcess(this, line)) {
        return;
    }
    if (this->stage == FS_PN_AT && SS_processLineForProcessArrival(this, line)) {
        return;
    }
    if (this->stage == FS_PN_SCHEDULE && SS_processLineForProcessSchedule(this, line)) {
        return;
    }
    //    printf("File Format Violates Specification!\nPlease check for errors and try again.\nLine in Violation:\t`%s", line);
    this->error_thrown = true;
}

int SS_hasSubString(char* line, char* needle) {
    char* locate = strcasestr(line, needle);
    if (locate != NULL) {
        return strlen(needle);
    }
    return -1;
}

bool SS_processLineForTimeQuantum(SimulationState* this, char* line) {
    char needle[] = "time quantum ";
    int found = SS_hasSubString(line, needle);
    if (found == -1)
        return false;
    this->seen_stage_req = true;
    //    printf("Time Quantum Found::\n");
    int id = strtol((line + found), NULL, 10);
    //    printf("\tID Found: `%d`\n", id);
    int quantum = strtol((line + found + 2), NULL, 10);
    //    printf("\tAmount Found: `%d`\n", quantum);
    ProcessQueue* RoundRobin1 = PQ_init_RoundRobin(id, quantum);
    ProcessQueue_deque_pushL(&this->proto_queues, RoundRobin1);
    return true;
}

bool SS_processLineForNewProcess(SimulationState* this, char* line) {
    char needle[] = "process id: ";
    int found = SS_hasSubString(line, needle);
    if (found == -1)
        return false;
    this->seen_stage_req = -1;
    if (this->stage != FS_PN_NEW)
        this->stage = FS_PN_NEW;
    this->stage++;
    int id = strtol((line + found), NULL, 10);
    //    printf("Process ID Found:: %d\n", id);
    PCB* process = PCB_init(id);
    PCB_deque_pushL(&this->notYetArrived, process);
    return true;
}

bool SS_processLineForProcessArrival(SimulationState* this, char* line) {
    char needle[] = "arrival time: ";
    int found = SS_hasSubString(line, needle);
    if (found == -1)
        return false;
    this->seen_stage_req = -1;
    this->stage++;
    int time = strtol((line + found), NULL, 10);
    //    printf("Arrival Time Found:: %d\n", time);
    PCB_deque_peekL(&this->notYetArrived)->arrival_time = time;
    return true;
}

bool SS_processLineForProcessSchedule(SimulationState* this, char* line) {
    //    printf("ENTER PROCESS SCHEDULE\n");
    BurstNode_deque* sched = &PCB_deque_peekL(&this->notYetArrived)->schedule;
    int found = SS_hasCpuBurst(line);
    if (this->bn == NULL) {
        this->seen_stage_req = 0;
        //        printf("REINIT BURSTNODE\n");
        this->bn = BurstNode_init();
    }
    BurstNode* peek = this->bn;
    //    printf("State Found BEFORE: `%d:%d:%d`\n", peek->queue_id, peek->duration, peek->type);
    if (found != -1) {
        int cpuBurst = strtol((line + found), NULL, 10);
        //        printf("CPU Burst Found: `%d`\n", cpuBurst);
        this->seen_stage_req |= (1 << 0);
        peek->type = BT_CPU;
        peek->duration = cpuBurst;
    }
    found = SS_hasIOBurst(line);
    if (found != -1) {
        int ioBurst = strtol((line + found), NULL, 10);
        //        printf("IO Burst Found: `%d`\n", ioBurst);
        this->seen_stage_req |= (1 << 1);
        peek->duration = ioBurst;
    }
    found = SS_hasIODevice(line);
    if (found != -1) {
        int ioID = strtol((line + found), NULL, 10);
        //        printf("IO Device ID Found: `%d`\n", ioID);
        this->seen_stage_req |= (1 << 2);
        if (DeviceDescriptor_deque_empty(&this->proto_devices) || !DeviceDescriptor_deque_hasDeviceId(&this->proto_devices, ioID)) {
            DeviceDescriptor_deque_pushL(&this->proto_devices, DeviceDescriptor_init(ioID));
        }
        peek->type = BT_IO;
        peek->queue_id = ioID;
    }
    //    printf("STATE Found AFTER: `%d:%d:%d`\n", peek->queue_id, peek->duration, peek->type);
    if (this->seen_stage_req > 0) {
        //        printf("REQUIREMENT MET\n");
        if (BN_fullyFormed(peek)) {
            //            printf("BURSTNODE FULLY FORMED\n");
            this->seen_stage_req = -1;
            BurstNode_deque_pushL(sched, peek);
            //            printf("\tLENGTH: %d\n", BurstNode_deque_length(sched));
            this->bn = NULL;
        }
        return true;
    }
    return false;
}

int SS_hasCpuBurst(char* line) {
    char needle[] = "cpu burst: ";
    return SS_hasSubString(line, needle);
}

int SS_hasIOBurst(char* line) {
    char needle[] = "i/o burst: ";
    return SS_hasSubString(line, needle);
}

int SS_hasIODevice(char* line) {
    char needle[] = "i/o device id: ";
    return SS_hasSubString(line, needle);
}

bool SS_hasNonProcessableLine(SimulationState* this, char* line) {
    if (SS_isEmptyLine(line)) {
        this->in_comment = false;
        //printf("=====================================Empty line!\n");
        if (this->stage != FS_PN_SCHEDULE && this->seen_stage_req != -1) {
            if (this->stage == FS_TQ) {
                ProcessQueue* FCFS = ProcessQueue_deque_peekL(&this->proto_queues);
                if (FCFS != NULL) {
                    FCFS = PQ_init_FCFS(FCFS->id + 1);
                } else {
                    FCFS = PQ_init_FCFS(1);
                }
                ProcessQueue_deque_pushL(&this->proto_queues, FCFS);
            }
            this->stage++;
            this->seen_stage_req = -1;
        } else if (this->stage == FS_PN_SCHEDULE && this->seen_stage_req == -1) {
            this->stage = FS_PN_NEW;
        }
        return true;
    }
    if (SS_isStartMultiLineComment(line)) {
        if (!SS_isEndMultiLineComment(line)) {
            this->in_comment = true;
            //printf("======MLSRT==========================Comment line\n");
        } else {
            //printf("======MLSLC==========================Comment line\n");
        }
        return true;
    } else if (SS_isEndMultiLineComment(line)) {
        this->in_comment = false;
        //printf("======MLEND==========================Comment line\n");
        return true;
    } else if (this->in_comment == true) {
        //printf("======MLINC==========================Comment line\n");
        return true;
    } else if (SS_isSingleLineComment(line)) {
        //printf("======SINGL==========================Comment line\n");
        return true;
    }
    return false;
}

bool SS_isEmptyLine(char* line) {
    return (line[0] == ' ' || line[0] == '\r' || line[0] == '\n');
}

bool SS_isEndMultiLineComment(char* line) {
    int len = strlen(line);
    int x = 0;
    while (x <= 5) {
        if (x >= len - 1) {
            break;
        }
        if (line[len - x++] == '/' && line[len - x] == '*') {
            return true;
        }
    }
    return false;
}

bool SS_isSingleLineComment(char* line) {
    if (strlen(line) <= 1) {
        return false;
    }
    if (line[0] == '/' && line[1] == '/') {
        return true;
    }
    return false;
}

bool SS_isStartMultiLineComment(char* line) {
    if (strlen(line) <= 1) {
        return false;
    }
    if (line[0] == '/' && line[1] == '*') {
        return true;
    }
    return false;
}

void printInputState(SimulationState* this) {
    PCB_dequeI it;
    PCB_dequeI_init(&it, &this->notYetArrived);

}
