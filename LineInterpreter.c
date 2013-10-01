#define _GNU_SOURCE
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "LineInterpreter.h"


void initInputState(InputState* state) {
    state->in_comment = false;
    state->stage = FS_TQ;
    state->seen_stage_req = -1;
    state->error_thrown = false;
    DD_dequeue_init(&state->proto_devices, false, true);
    PQ_dequeue_init(&state->proto_queues, false, true);
    PCB_dequeue_init(&state->notYetArrived, false, true);
    state->call_back = processInputLine;
}


/*
 * Callback function for mmap file read, appropriately sets line_buffer to the value
 * of the memory location passed
 */
void processInputLine(InputState* this, const char* begin, const char* end) {
    //>>	Copy values from memory at beginning -> end into line_buffer
    char line[end - begin + 1];
    int copyend = (end - begin + 1);
    strncpy(line, begin, copyend);
    line[copyend] = 0;

    //>>	Determine what kind of line we have
    if (hasNonProcessableLine(this, line)) {
        return;
    }
    if (this->stage == FS_TQ && processLineForTimeQuantum(this, line)) {
        return;
    }
    if (this->stage == FS_PN_NEW && processLineForNewProcess(this, line)) {
        return;
    }
    if (this->stage == FS_PN_AT && processLineForProcessArrival(this, line)) {
        return;
    }
    if (this->stage == FS_PN_SCHEDULE && processLineForProcessSchedule(this, line)) {
        return;
    }
    printf("File Format Violates Specification!\nPlease check for errors and try again.\nLine in Violation:\t`%s", line);
    this->error_thrown = true;
}

int hasSubString(char* line, char* needle) {
    char* locate = strcasestr(line, needle);
    if (locate != NULL) {
        return strlen(needle);
    }
    return -1;
}

bool processLineForTimeQuantum(InputState* this, char* line) {
    char needle[] = "time quantum ";
    int found = hasSubString(line, needle);
    if (found == -1)
        return false;
    this->seen_stage_req = true;
    printf("Time Quantum Found::\n");
    int id = strtol((line + found), NULL, 10);
    printf("\tID Found: `%d`\n", id);
    int quantum = strtol((line + found + 2), NULL, 10);
    printf("\tAmount Found: `%d`\n", quantum);
    PQ RoundRobin1;
    PQ_init_RoundRobin(&RoundRobin1, id, quantum);
    PQ_dequeue_pushL(&this->proto_queues, &RoundRobin1);
    if (id == 2) {
        PQ FCFS1;
        PQ_init_FCFS(&FCFS1, id + 1);
        PQ_dequeue_pushL(&this->proto_queues, &FCFS1);
    }
    return true;
}

bool processLineForNewProcess(InputState* this, char* line) {
    char needle[] = "process id: ";
    int found = hasSubString(line, needle);
    if (found == -1)
        return false;
    this->seen_stage_req = -1;
    this->stage++;
    int id = strtol((line + found), NULL, 10);
    printf("Process ID Found:: %d\n", id);
    PCB* process = malloc(sizeof (*process));
    process->process_id = id;
    PCB_dequeue_pushL(&this->notYetArrived, process);
    return true;
}

bool processLineForProcessArrival(InputState* this, char* line) {
    char needle[] = "arrival time: ";
    int found = hasSubString(line, needle);
    if (found == -1)
        return false;
    this->seen_stage_req = -1;
    this->stage++;
    int time = strtol((line + found), NULL, 10);
    printf("Arrival Time Found:: %d\n", time);
    PCB_dequeue_peekL(&this->notYetArrived)->arrival_time = time;
    return true;
}

bool processLineForProcessSchedule(InputState* this, char* line) {
    BurstNode_dequeue* sched = &PCB_dequeue_peekL(&this->notYetArrived)->schedule;
    sched->trace = true;
    int found = hasCpuBurst(line);
    if (BurstNode_dequeue_empty(sched) || BurstNode_fullyFormed(BurstNode_dequeue_peekL(sched))) {
        this->seen_stage_req = 0;
        BurstNode* newBurst = initBurstNode();
        BurstNode_dequeue_pushL(sched, newBurst);
    }
    if (found != -1) {
        int cpuBurst = strtol((line + found), NULL, 10);
        printf("CPU Burst Found: `%d`\n", cpuBurst);
        this->seen_stage_req |= (1 << 0);
        BurstNode_dequeue_peekL(sched)->type = BT_CPU;
        BurstNode_dequeue_peekL(sched)->duration = cpuBurst;
    }
    found = hasIOBurst(line);
    if (found != -1) {
        int ioBurst = strtol((line + found), NULL, 10);
        printf("IO Burst Found: `%d`\n", ioBurst);
        this->seen_stage_req |= (1 << 1);
        BurstNode_dequeue_peekL(sched)->type = BT_IO;
        BurstNode_dequeue_peekL(sched)->duration = ioBurst;
    }
    found = hasIODevice(line);
    if (found != -1) {
        int ioID = strtol((line + found), NULL, 10);
        printf("IO Device ID Found: `%d`\n", ioID);
        this->seen_stage_req |= (1 << 2);
        if (DD_dequeue_empty(&this->proto_devices) || !SearchDeviceIds(&this->proto_devices, ioID)) {
            DD_dequeue_pushL(&this->proto_devices, DD_init(ioID));
        }
        BurstNode_dequeue_peekL(sched)->type = BT_IO;
        BurstNode_dequeue_peekL(sched)->queue_id = ioID;
    }
    if (this->seen_stage_req > 0) {
        if (BurstNode_fullyFormed(BurstNode_dequeue_peekL(sched))) {
            this->seen_stage_req = -1;
        }
        return true;
    }
    return false;
}

int hasCpuBurst(char* line) {
    char needle[] = "cpu burst: ";
    return hasSubString(line, needle);
}

int hasIOBurst(char* line) {
    char needle[] = "i/o burst: ";
    return hasSubString(line, needle);
}

int hasIODevice(char* line) {
    char needle[] = "i/o device id: ";
    return hasSubString(line, needle);
}

bool hasNonProcessableLine(InputState* this, char* line) {
    if (isEmptyLine(line)) {
        this->in_comment = false;
        printf("=====================================Empty line!\n");
        if (this->stage != FS_PN_SCHEDULE && this->seen_stage_req != -1) {
            this->stage++;
            this->seen_stage_req = -1;
        } else if (this->stage == FS_PN_SCHEDULE && this->seen_stage_req == -1) {
            this->stage = FS_PN_NEW;
        }
        return true;
    }
    if (isStartMultiLineComment(line)) {
        if (!isEndMultiLineComment(line)) {
            this->in_comment = true;
            printf("======MLSRT==========================Comment line\n");
        } else {
            printf("======MLSLC==========================Comment line\n");
        }
        return true;
    } else if (isEndMultiLineComment(line)) {
        this->in_comment = false;
        printf("======MLEND==========================Comment line\n");
        return true;
    } else if (this->in_comment == true) {
        printf("======MLINC==========================Comment line\n");
        return true;
    } else if (isSingleLineComment(line)) {
        printf("======SINGL==========================Comment line\n");
        return true;
    }
    return false;
}

bool isEmptyLine(char* line) {
    return (line[0] == ' ' || line[0] == '\r' || line[0] == '\n');
}

bool isEndMultiLineComment(char* line) {
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

bool isSingleLineComment(char* line) {
    if (strlen(line) <= 1) {
        return false;
    }
    if (line[0] == '/' && line[1] == '/') {
        return true;
    }
    return false;
}

bool isStartMultiLineComment(char* line) {
    if (strlen(line) <= 1) {
        return false;
    }
    if (line[0] == '/' && line[1] == '*') {
        return true;
    }
    return false;
}
