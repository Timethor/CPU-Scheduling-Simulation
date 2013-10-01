#include "FileToProcessQueue.h"
#include "VirtualCpu.h"

#include <regex.h>

/*
 * Callback function for mmap file read, approprietely sets line_buffer to the value
 * of the memory location passed
 */
void interpret_line(const char* begin, const char* end, VCPUInputState* state, VirtualCPU* cpu) {
    //>>	Copy values from memory at beginning -> end into line_buffer
    char line_buffer[end - begin + 1];
    int copyend = (end - begin + 1);
    strncpy(line_buffer, begin, copyend);
    line_buffer[copyend] = 0;

    //>>	Determine what kind of line we have
    if (isEmptyLine(line_buffer)) {
        state->empty_lines++;
        printf("=====================================Empty line!\n");
    } else {
        state->empty_lines = 0;
        if (isStartMultiLineComment(line_buffer)) {
            if (!isEndMultiLineComment(line_buffer)) {
                state->in_comment = true;
                printf("======MLSRT==========================Comment line\n");
            } else {
                printf("======MLSLC==========================Comment line\n");
            }
        } else if (isEndMultiLineComment(line_buffer)) {
            state->in_comment = false;
            printf("======MLEND==========================Comment line\n");
        } else if (state->in_comment == true) {
            printf("======MLINC==========================Comment line\n");
        } else if (isSingleLineComment(line_buffer)) {
            printf("======SINGL==========================Comment line\n");
        } else {
            // This line is useful, pass to VPCU
            cpu->func->processInputLine(line_buffer);
        }
    }
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