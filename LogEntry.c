
#include "LogEntry.h"

DEQUE(LogEntry);

LogEntry* LogEntry_init(char* entry, LogEntry level, bool logtoConsole, bool logtoFile) {
    LogEntry* this = malloc(sizeof (*this));
    this->entry = entry;
    this->level = level;
    this->tToConsole = logtoConsole;
    this->toFile = logtoFile;
    return this;
}

void LogEntry_destruct(LogEntry* this) {
    free(this->entry);
    free(this);
}