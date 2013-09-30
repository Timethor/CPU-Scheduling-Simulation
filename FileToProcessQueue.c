#include "FileToProcessQueue.h"

#include <regex.h>

/*
 * 
 */
void print_line(const char* begin, const char* end, char* line_buffer, const int line_size) {
    int copyend = (end - begin + 1) < line_size ? (end - begin + 1) : line_size;
    strncpy(line_buffer, begin, copyend);
    line_buffer[copyend] = 0;
}

int read_lines(const char* fname, int line_size, void (*call_back)(const char*, const char*, char*, const int)) {
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
    printf("Looping!\n");
    interpreterState state;
    state.empty_lines = 0;
    state.in_comment = false;
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
        char r[line_size];
        memset(r, 33, sizeof (r) - 1);
        call_back(begin, end, r, line_size);
        if (r == NULL) {
            err(1, "[callback] %s", fname);
            break;
        } else {
            interpretLine(r, &state);
        }

        if ((begin = ++end) >= buf_end)
            break;
    }

    munmap(buf, fs.st_size);
    close(fd);
    return 1;
}

void interpretLine(char* r, interpreterState* state) {
    if (isEmptyLine(r)) {
        state->empty_lines++;
        printf("=====================================Empty line!\n");
    } else {
        state->empty_lines = 0;
        if (isStartMultiLineComment(r)) {
            if (!isEndMultiLineComment(r)) {
                state->in_comment = true;
                printf("======MLSRT==========================Comment line\n");
            } else {
                printf("======MLSLC==========================Comment line\n");
            }
        } else if (isEndMultiLineComment(r)) {
            state->in_comment = false;
            printf("======MLEND==========================Comment line\n");
        } else if (state->in_comment == true) {
            printf("======MLINC==========================Comment line\n");
        } else if (isSingleLineComment(r)) {
            printf("======SINGL==========================Comment line\n");
        } else {
            // This line is useful
            printf("%s\n", r);
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