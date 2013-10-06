
#include <stdlib.h>
#include <string.h>

#include "LogEntry.h"

LogEntry* LogEntry_init(char* entry, enum LogLevel level) {
    LogEntry* this = malloc(sizeof (*this));
    char *entryp = (char*)malloc(200);
    strcpy(entryp, entry);
    this->entry = entryp;
    this->level = level;
    return this;
}

void LogEntry_destruct(LogEntry* this) {
    free(this->entry);
    free(this);
}