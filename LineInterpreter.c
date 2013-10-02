#define _GNU_SOURCE
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "LineInterpreter.h"

InputState* InputState_init() {
    InputState* this = malloc(sizeof(*this));
    this->in_comment = false;
    this->stage = FS_TQ;
    this->seen_stage_req = -1;
    this->error_thrown = false;
    DD_dequeue_init(&this->proto_devices, false, false);
    PQ_dequeue_init(&this->proto_queues, false, false);
    PCB_dequeue_init(&this->notYetArrived, false, false);
    this->call_back = IS_processInputLine;
    this->bn = NULL;
    return this;
}

/*
 * Callback function for mmap file read, appropriately sets line_buffer to the value
 * of the memory location passed
 */
void IS_processInputLine(InputState* this, const char* begin, const char* end) {
    //>>	Copy values from memory at beginning -> end into line_buffer
    char line[end - begin + 1];
    int copyend = (end - begin + 1);
    strncpy(line, begin, copyend);
    line[copyend] = 0;

    //>>	Determine what kind of line we have
//    printf("============================================================\n|\tSTAGE ::: %d\n============================================================\n", this->stage);
    if (IS_hasNonProcessableLine(this, line)) {
        return;
    }
    if (this->stage == FS_TQ && IS_processLineForTimeQuantum(this, line)) {
        return;
    }
    if (this->stage == FS_PN_NEW && IS_processLineForNewProcess(this, line)) {
        return;
    }
    if (this->stage == FS_PN_AT && IS_processLineForProcessArrival(this, line)) {
        return;
    }
    if (this->stage == FS_PN_SCHEDULE && IS_processLineForProcessSchedule(this, line)) {
        return;
    }
//    printf("File Format Violates Specification!\nPlease check for errors and try again.\nLine in Violation:\t`%s", line);
    this->error_thrown = true;
}

int IS_hasSubString(char* line, char* needle) {
    char* locate = strcasestr(line, needle);
    if (locate != NULL) {
        return strlen(needle);
    }
    return -1;
}

bool IS_processLineForTimeQuantum(InputState* this, char* line) {
    char needle[] = "time quantum ";
    int found = IS_hasSubString(line, needle);
    if (found == -1)
        return false;
    this->seen_stage_req = true;
//    printf("Time Quantum Found::\n");
    int id = strtol((line + found), NULL, 10);
//    printf("\tID Found: `%d`\n", id);
    int quantum = strtol((line + found + 2), NULL, 10);
//    printf("\tAmount Found: `%d`\n", quantum);
    PQ* RoundRobin1 = PQ_init_RoundRobin(id, quantum);
    PQ_dequeue_pushL(&this->proto_queues, RoundRobin1);
    PQ_printQueue(PQ_dequeue_peekF(&this->proto_queues));
    if (id++ == 2) {
        PQ* FCFS = PQ_init_FCFS(id);
        PQ_dequeue_pushL(&this->proto_queues, FCFS);
        PQ_printQueue(PQ_dequeue_peekF(&this->proto_queues));
    }
    return true;
}

bool IS_processLineForNewProcess(InputState* this, char* line) {
    char needle[] = "process id: ";
    int found = IS_hasSubString(line, needle);
    if (found == -1)
        return false;
    this->seen_stage_req = -1;
    this->stage++;
    int id = strtol((line + found), NULL, 10);
//    printf("Process ID Found:: %d\n", id);
    PCB* process = PCB_init(id);
    PCB_dequeue_pushL(&this->notYetArrived, process);
    return true;
}

bool IS_processLineForProcessArrival(InputState* this, char* line) {
    char needle[] = "arrival time: ";
    int found = IS_hasSubString(line, needle);
    if (found == -1)
        return false;
    this->seen_stage_req = -1;
    this->stage++;
    int time = strtol((line + found), NULL, 10);
//    printf("Arrival Time Found:: %d\n", time);
    PCB_dequeue_peekL(&this->notYetArrived)->arrival_time = time;
    return true;
}

bool IS_processLineForProcessSchedule(InputState* this, char* line) {
//    printf("ENTER PROCESS SCHEDULE\n");
    BurstNode_dequeue* sched = &PCB_dequeue_peekL(&this->notYetArrived)->schedule;
    sched->trace = true;
    int found = IS_hasCpuBurst(line);
    if (this->bn == NULL) {
        this->seen_stage_req = 0;
//        printf("REINIT BURSTNODE\n");
        this->bn = initBurstNode();
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
    found = IS_hasIOBurst(line);
    if (found != -1) {
        int ioBurst = strtol((line + found), NULL, 10);
//        printf("IO Burst Found: `%d`\n", ioBurst);
        this->seen_stage_req |= (1 << 1);
        peek->duration = ioBurst;
    }
    found = IS_hasIODevice(line);
    if (found != -1) {
        int ioID = strtol((line + found), NULL, 10);
//        printf("IO Device ID Found: `%d`\n", ioID);
        this->seen_stage_req |= (1 << 2);
        if (DD_dequeue_empty(&this->proto_devices) || !SearchDeviceIds(&this->proto_devices, ioID)) {
            DD_dequeue_pushL(&this->proto_devices, DD_init(ioID));
        }
        peek->type = BT_IO;
        peek->queue_id = ioID;
    }
//    printf("STATE Found AFTER: `%d:%d:%d`\n", peek->queue_id, peek->duration, peek->type);
    if (this->seen_stage_req > 0) {
//        printf("REQUIREMENT MET\n");
        if (BurstNode_fullyFormed(peek)) {
//            printf("BURSTNODE FULLY FORMED\n");
            this->seen_stage_req = -1;
            BurstNode_dequeue_pushL(sched, peek);
//            printf("\tLENGTH: %d\n", BurstNode_dequeue_length(sched));
            this->bn = NULL;
        }
        return true;
    }
    return false;
}

int IS_hasCpuBurst(char* line) {
    char needle[] = "cpu burst: ";
    return IS_hasSubString(line, needle);
}

int IS_hasIOBurst(char* line) {
    char needle[] = "i/o burst: ";
    return IS_hasSubString(line, needle);
}

int IS_hasIODevice(char* line) {
    char needle[] = "i/o device id: ";
    return IS_hasSubString(line, needle);
}

bool IS_hasNonProcessableLine(InputState* this, char* line) {
    if (IS_isEmptyLine(line)) {
        this->in_comment = false;
        //printf("=====================================Empty line!\n");
        if (this->stage != FS_PN_SCHEDULE && this->seen_stage_req != -1) {
            this->stage++;
            this->seen_stage_req = -1;
        } else if (this->stage == FS_PN_SCHEDULE && this->seen_stage_req == -1) {
            this->stage = FS_PN_NEW;
        }
        return true;
    }
    if (IS_isStartMultiLineComment(line)) {
        if (!IS_isEndMultiLineComment(line)) {
            this->in_comment = true;
            //printf("======MLSRT==========================Comment line\n");
        } else {
            //printf("======MLSLC==========================Comment line\n");
        }
        return true;
    } else if (IS_isEndMultiLineComment(line)) {
        this->in_comment = false;
        //printf("======MLEND==========================Comment line\n");
        return true;
    } else if (this->in_comment == true) {
        //printf("======MLINC==========================Comment line\n");
        return true;
    } else if (IS_isSingleLineComment(line)) {
        //printf("======SINGL==========================Comment line\n");
        return true;
    }
    return false;
}

bool IS_isEmptyLine(char* line) {
    return (line[0] == ' ' || line[0] == '\r' || line[0] == '\n');
}

bool IS_isEndMultiLineComment(char* line) {
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

bool IS_isSingleLineComment(char* line) {
    if (strlen(line) <= 1) {
        return false;
    }
    if (line[0] == '/' && line[1] == '/') {
        return true;
    }
    return false;
}

bool IS_isStartMultiLineComment(char* line) {
    if (strlen(line) <= 1) {
        return false;
    }
    if (line[0] == '/' && line[1] == '*') {
        return true;
    }
    return false;
}

void printInputState(InputState* this) {
    PCB_dequeueI it;
    PCB_dequeueI_init(&it, &this->notYetArrived);

}
