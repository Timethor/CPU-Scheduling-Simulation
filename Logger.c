
#include "Logger.h"

void LogStatement(CPULogger* this, const char* append, ...) {

}

CPULogger* CPUL_init() {
    CPULogger* this = malloc(sizeof (*this));
    this->LogStatement = LogStatement;
    this->lastClockTime = -1;
    this->buffer = NULL;
    this->
    return this;
}