#include "FileToProcessQueue.h"

#include <regex.h>

/*
 * 
 */
void print_line(const char* begin, const char* end, char* line_buffer, const int line_size) {
    int copyend = (end - begin + 1) < line_size ? (end - begin + 1) : line_size;
    strncpy(line_buffer, begin, copyend);
    line_buffer[copyend] = NULL;
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
            if (r[0] == ' ' || r[0] == '\r' || r[0] == '\n') {
                printf("====================================Empty line!\n");
            } else if (r[0] == '/' && r[1] == '*') {
                printf("====================================Comment line started\n");
            } else {
                size_t len = strlen(r);
                if (r[len - 1] == '/' && r[len - 2] == '*') {
                    printf("====================================Comment line ended\n");
                }
            }
            printf("Caught String: %s\n", r);
        }

        if ((begin = ++end) >= buf_end)
            break;
    }

    munmap(buf, fs.st_size);
    close(fd);
    return 1;
}
