/*
 * File:   LineInterpreter.c
 * Author: Nicholas `Timethor` Rich     G00399909
 *
 * Created on October 1, 2013, 2:18 PM
 */

#define _GNU_SOURCE
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "LineInterpreter.h"
#include "Settings.h"

//>>	== See Note in Report about _init and _destruct ==    <<//

SimulationState* SimulationState_init(Settings* settings) {
    SimulationState* this = malloc(sizeof (*this));
    this->in_comment = false;
    this->addFCFSToEnd = !settings->roundRobinOnly;
    this->policy = MP_INF;
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

void SimulationState_destruct(SimulationState* this) {
    free(this->bn);
    DeviceDescriptor_deque_freeElements(&this->proto_devices);
    ProcessQueue_deque_freeElements(&this->proto_queues);
    PCB_deque_freeElements(&this->notYetArrived);
    free(this);
}

//>>	=== Utility Functions, the lot of them. === <<//

/*
 * Callback function for mmap file read, appropriately sets line_buffer to the value
 * of the memory location passed
 */
void SS_processInputLine(SimulationState* this, const char* begin, const char* end, Logger* logger) {
    void(*LogPrintf) (Logger*, enum LogLevel, const char*, ...) = logger->log;
    char line[end - begin + 1];
    int copyend = (end - begin + 1);
    strncpy(line, begin, copyend);
    line[copyend] = 0;

    if (SS_hasNonProcessableLine(this, line)) {
        return;
    }
    //>>	This now also searches for Memory Size and if found changes the enum state so we can do state++
    //>>	For the Memory Management type of input processing
    if (this->stage == FS_TQ && (SS_processLineForTimeQuantum(this, line) || SS_processLineForMemorySize(this, line))) {
        LogPrintf(logger, LogLevel_FINER, "\tSeen TQ or MS\n");
        return;
    }
    //>>	The above if statement must come before this one b/c ...MemorySize sets a different state if it is found;
    if ((this->stage == FS_PN_NEW || this->stage == FS_TQ) && SS_processLineForNewProcess(this, line)) {
        LogPrintf(logger, LogLevel_FINER, "\tSeen New Proc\n");
        return;
    }
    if (this->stage == FS_PN_AT && SS_processLineForProcessArrival(this, line)) {
        LogPrintf(logger, LogLevel_FINER, "\tSeen Proc AT\n");
        return;
    }
    if (this->stage == FS_PN_SCHEDULE && SS_processLineForProcessSchedule(this, line)) {
        LogPrintf(logger, LogLevel_FINER, "\tSeen Proc Sched\n");
        return;
    }
    //>>	New For Project 2, defines processing state => action correlation
    if (this->stage == FS_M_MMP && SS_processLineForMemoryPolicy(this, line)) {
        LogPrintf(logger, LogLevel_FINER, "\tSeen Mem Policy\n");
        return;
    }
    if (this->stage == FS_M_PP && SS_processLineForPolicyParams(this, line)) {
        LogPrintf(logger, LogLevel_FINER, "\tSeen Mem Policy Params\n");
        return;
    }
    if (this->stage == FS_M_PID && SS_processLineForNewProcess(this, line)) {
        LogPrintf(logger, LogLevel_FINER, "\tSeen New MEM MAN Proc\n");
        return;
    }
    if (this->stage == FS_M_AT && SS_processLineForProcessArrival(this, line)) {
        LogPrintf(logger, LogLevel_FINER, "\tSeen MEM MAN Proc AT\n");
        return;
    }
    if (this->stage == FS_M_LIM && SS_processLineForLifetime(this, line)) {
        LogPrintf(logger, LogLevel_FINER, "\tSeen MEM MAN Proc LIM\n");
        return;
    }
    if (this->stage == FS_M_AS && SS_processLineForAddressSpace(this, line)) {
        LogPrintf(logger, LogLevel_FINER, "\tSeen MEM MAN Proc AS\n");
        return;
    }
    this->error_thrown = true;
}

int SS_hasSubString(char* line, char* needle) {
    char* locate = strcasestr(line, needle);
    if (locate != NULL) {
        return strlen(needle);
    }
    return -1;
}

bool SS_processLineForMemorySize(SimulationState * this, char* line) {
    char needle[] = "memory size: ";
    int found = SS_hasSubString(line, needle);
    if (found == -1)
        return false;
    //>>	set the stage
    this->stage = FS_M_MS;
    this->seen_stage_req = -1;
    this->stage++;
    //>>	save our memory size for MemManager later (MOVE THIS PROP TO MEMMAN)
    this->memKiloSize = strtol((line + found), NULL, 10);
    return true;
}

bool SS_processLineForMemoryPolicy(SimulationState * this, char* line) {
    char needle[] = "memory management policy: ";
    int found = SS_hasSubString(line, needle);
    if (found == -1)
        return false;
    //>>	set the stage
    this->seen_stage_req = -1;
    this->stage++;
    //>>	Set the policy type for MemManager later (MOVE THIS PROP TO MEMMAN)
    if (strlen((line + found)) <= 4) {
        SS_determinePolicy(this, (line + found));
    } else {
        return false;
    }
    return true;
}

bool SS_processLineForPolicyParams(SimulationState * this, char* line) {
    char needle[] = "policy parameter: ";
    int found = SS_hasSubString(line, needle);
    if (found == -1)
        return false;
    //>>	set the stage
    this->seen_stage_req = -1;
    this->stage++;
    //>>	save policy params for MemManager later (MOVE THIS PROP TO MEMMAN)
    this->policyParams = strtol((line + found), NULL, 10);
    if ((this->policy == MP_VSP || this->policy == MP_SEG) && (this->policyParams < 0 || this->policyParams > 2)) {
        //>>	Policy param here should only ever be 0,1,2 so error otherwise
        return false;
    }//>>	we dont need a check for PAG Policy
    return true;
}

bool SS_processLineForLifetime(SimulationState * this, char* line) {
    BurstNode_deque* sched = &PCB_deque_peekL(&this->notYetArrived)->schedule;
    char needle[] = "lifetime in memory: ";
    int found = SS_hasSubString(line, needle);
    if (found == -1)
        return false;
    this->seen_stage_req = -1;
    this->stage++;

    int time = strtol((line + found), NULL, 10);
    if (this->bn == NULL) {
        //>>	Here lies the source of a memory leak. Much has been done to vanquish
        //>>	this venal and virulent vermin to no avail. The only verdict is 
        //>>	vengeance; a vendetta, held as a votive, probably in vain, for 
        //>>	the value and veracity of such shall one day vindicate the vigilant
        //>>	and the virtuous from the vexation it provides.
        this->bn = BurstNode_init();
    }
    this->bn->type = BT_MM;
    this->bn->duration = time;
    BurstNode_deque_pushL(sched, this->bn);
    this->bn = NULL;
    return true;

}

bool SS_processLineForAddressSpace(SimulationState * this, char* line) {
    AddressSpace_deque* a_space = &PCB_deque_peekL(&this->notYetArrived)->a_space;
    char needle[] = "address space: ";
    int found = SS_hasSubString(line, needle);
    if (found == -1)
        return false;
    if (this->policy == MP_SEG) {
        //>>	If we see that SEG policy is used, get all peices of address space.
        char** list;
        size_t index, length;
        explode((line + found), " ", &list, &length);
        /* push list to deque */
        for (index = 0; index < length; ++index) {
            AddressSpace* new = AddressSpace_init(strtol(list[index], NULL, 10));
            AddressSpace_deque_pushL(a_space, new);
        }
        /* free list */
        for (index = 0; index < length; ++index)
            free(list[index]);
        free(list);
    } else {
        AddressSpace* new = AddressSpace_init(strtol((line + found), NULL, 10));
        AddressSpace_deque_pushL(a_space, new);
    }
    this->seen_stage_req = true;
    return true;
}

/**
 * Finds out if this line has time quantum in it and processes as needed
 */
bool SS_processLineForTimeQuantum(SimulationState* this, char* line) {
    char needle[] = "time quantum ";
    int found = SS_hasSubString(line, needle);
    if (found == -1)
        return false;
    this->seen_stage_req = true;
    int id = strtol((line + found), NULL, 10);
    int quantum = strtol((line + found + 2), NULL, 10);
    ProcessQueue* RoundRobin1 = PQ_init_RoundRobin(id, quantum);
    ProcessQueue_deque_pushL(&this->proto_queues, RoundRobin1);
    return true;
}

/**
 * Finds out if this line has process id: in it and processes as needed
 */
bool SS_processLineForNewProcess(SimulationState* this, char* line) {
    char needle[] = "process id: ";
    int found = SS_hasSubString(line, needle);
    if (found == -1)
        return false;
    this->seen_stage_req = -1;
    if (this->stage == FS_TQ)
        this->stage = FS_PN_NEW;
    this->stage++;
    int id = strtol((line + found), NULL, 10);
    PCB* process = PCB_init(id);
    PCB_deque_pushL(&this->notYetArrived, process);
    return true;
}

/**
 * Finds out if this line has arrival time: in it and processes as needed
 */
bool SS_processLineForProcessArrival(SimulationState* this, char* line) {
    char needle[] = "arrival time: ";
    int found = SS_hasSubString(line, needle);
    if (found == -1)
        return false;
    this->seen_stage_req = -1;
    this->stage++;
    int time = strtol((line + found), NULL, 10);
    PCB_deque_peekL(&this->notYetArrived)->arrival_time = time;
    return true;
}

//>>	=== Helper functions for SS_determinePolicy ===   <<//

int SS_policyTypeVSP(char* line) {
    char needle[] = "vsp";
    return SS_hasSubString(line, needle);
}

int SS_policyTypePAG(char* line) {
    char needle[] = "pag";
    return SS_hasSubString(line, needle);
}

int SS_policyTypeSEG(char* line) {
    char needle[] = "seg";
    return SS_hasSubString(line, needle);
}

/**
 * Simply sets this->policy according to the input with enum values
 */
void SS_determinePolicy(SimulationState * this, char* line) {
    int found = SS_policyTypeVSP(line);
    if (found != -1) {
        this->policy = MP_VSP;
    }
    found = SS_policyTypePAG(line);
    if (found != -1) {
        this->policy = MP_PAG;
    }
    found = SS_policyTypeSEG(line);
    if (found != -1) {
        this->policy = MP_SEG;
    }
}

/**
 * Finds out if this line has process schedule data in it and processes as needed
 */
bool SS_processLineForProcessSchedule(SimulationState* this, char* line) {
    BurstNode_deque* sched = &PCB_deque_peekL(&this->notYetArrived)->schedule;
    int found = SS_hasCpuBurst(line);
    if (this->bn == NULL) {
        this->seen_stage_req = 0;
        //>>	Here lies the source of a memory leak. Much has been done to vanquish
        //>>	this venal and virulent vermin to no avail. The only verdict is 
        //>>	vengeance; a vendetta, held as a votive, probably in vain, for 
        //>>	the value and veracity of such shall one day vindicate the vigilant
        //>>	and the virtuous from the vexation it provides.
        this->bn = BurstNode_init();
    }
    if (found != -1) {
        int cpuBurst = strtol((line + found), NULL, 10);
        this->seen_stage_req |= (1 << 0);
        this->bn->type = BT_CPU;
        this->bn->duration = cpuBurst;
    }
    found = SS_hasIOBurst(line);
    if (found != -1) {
        int ioBurst = strtol((line + found), NULL, 10);
        this->seen_stage_req |= (1 << 1);
        this->bn->duration = ioBurst;
    }
    found = SS_hasIODevice(line);
    if (found != -1) {
        int ioID = strtol((line + found), NULL, 10);
        this->seen_stage_req |= (1 << 2);
        if (DeviceDescriptor_deque_empty(&this->proto_devices) || !DeviceDescriptor_deque_hasDeviceId(&this->proto_devices, ioID)) {
            DeviceDescriptor_deque_pushL(&this->proto_devices, DeviceDescriptor_init(ioID));
        }
        this->bn->type = BT_IO;
        this->bn->queue_id = ioID;
    }
    if (this->seen_stage_req > 0) {
        if (BN_fullyFormed(this->bn)) {
            this->seen_stage_req = -1;
            BurstNode_deque_pushL(sched, this->bn);
            this->bn = NULL;
        }
        return true;
    }
    return false;
}

//>>	=== Helper functions for SS_processLineForProcessSchedule ===   <<//

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

/**
 * If we find an empty line or any type of C comment process as needed
 */
bool SS_hasNonProcessableLine(SimulationState* this, char* line) {
    if (SS_isEmptyLine(line)) {
        this->in_comment = false;
        //>>	We found an empty line where we are not in the schedule stage
        //>>	and our stage is ready to transition.
        if (this->stage != FS_PN_SCHEDULE && this->seen_stage_req != -1) {
            if (this->stage == FS_TQ) {
                ProcessQueue* FCFS = ProcessQueue_deque_peekL(&this->proto_queues);
                //>>	Add a FCFS queue to the end of the list unless the user 
                //>>	Says not to.
                if (this->addFCFSToEnd) {
                    if (FCFS != NULL) {
                        FCFS = PQ_init_FCFS(FCFS->id + 1);
                    } else {
                        FCFS = PQ_init_FCFS(1);
                    }
                    ProcessQueue_deque_pushL(&this->proto_queues, FCFS);
                }
            }
            if (this->stage == FS_M_AS) {
                this->stage = FS_M_PID;
            } else {
                this->stage++;
            }
            this->seen_stage_req = -1;
        } else if (this->stage == FS_PN_SCHEDULE && this->seen_stage_req == -1) {
            this->stage = FS_PN_NEW;
        }
        return true;
    }
    if (SS_isStartMultiLineComment(line)) {
        if (!SS_isEndMultiLineComment(line)) {
            this->in_comment = true;
        }
        return true;
    } else if (SS_isEndMultiLineComment(line)) {
        this->in_comment = false;
        return true;
    } else if (this->in_comment == true) {
        return true;
    } else if (SS_isSingleLineComment(line)) {
        return true;
    }
    return false;
}

//>>	=== Helper functions for SS_hasNonProcessableLine ===   <<//

/* and correspond to all types of nonProcessable lines, like comments and empties */
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

char* my_strdup(const char *src) {
    char *tmp = malloc(strlen(src) + 1);
    if (tmp)
        strcpy(tmp, src);
    return tmp;
}

void explode(const char *src, const char *tokens, char ***list, size_t *len) {
    if (src == NULL || list == NULL || len == NULL)
        return;

    char *str, *copy, **_list = NULL, **tmp;
    *list = NULL;
    *len = 0;

    copy = my_strdup(src);
    if (copy == NULL)
        return;

    str = strtok(copy, tokens);
    if (str == NULL)
        goto free_and_exit;

    _list = realloc(NULL, sizeof *_list);
    if (_list == NULL)
        goto free_and_exit;

    _list[*len] = my_strdup(str);
    if (_list[*len] == NULL)
        goto free_and_exit;
    (*len)++;


    while ((str = strtok(NULL, tokens))) {
        tmp = realloc(_list, (sizeof *_list) * (*len + 1));
        if (tmp == NULL)
            goto free_and_exit;

        _list = tmp;

        _list[*len] = strdup(str);
        if (_list[*len] == NULL)
            goto free_and_exit;
        (*len)++;
    }


free_and_exit:
    *list = _list;
    free(copy);
}