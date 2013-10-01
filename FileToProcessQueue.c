#include "FileToProcessQueue.h"
#include "VirtualCpu.h"

#include <regex.h>

/*
 * Callback function for mmap file read, approprietely sets line_buffer to the value
 * of the memory location passed
 */
void interpret_line(const char* begin, const char* end, readLineState* state, VirtualCPU* cpu) {
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

int read_lines(const char* fname, void (*call_back)(const char*, const char*, readLineState*, VirtualCPU*), VirtualCPU* cpu) {
    //>>	
    int fd = open(fname, O_RDONLY);
    struct stat fs;
    char *buf, *buf_end;
    char *begin, *end, c;

    if (fd == -1) {
        err(1, "open: %s", fname);
        return 0;
    }

    if (fstat(fd, &fs) == -1) {
        err(1, "stat: %s", fname);
        return 0;
    }

    buf = mmap(0, fs.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (buf == (void*) - 1) {
        err(1, "mmap: %s", fname);
        close(fd);
        return 0;
    }

    buf_end = buf + fs.st_size;

    begin = end = buf;
    //>>	Keep track of what we have seen so far
    readLineState state;
    state.empty_lines = 0;
    state.in_comment = false;

    //>>	Main loop for each line in file
    while (1) {
        if (!(*end == '\r' || *end == '\n')) {
            if (++end < buf_end) continue;
        } else if (1 + end < buf_end) {
            /* see if we got "\r\n" or "\n\r" here */
            c = *(1 + end);
            if ((c == '\r' || c == '\n') && c != *end)
                ++end;
        }
        /* call the call back and check error indication. Announce
           error here, because we didn't tell call_back the file name */
        //memset(r, 32, sizeof (r) - 1);
        call_back(begin, end, &state, cpu);

        if ((begin = ++end) >= buf_end)
            break;
    }

    munmap(buf, fs.st_size);
    close(fd);
    return 1;
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