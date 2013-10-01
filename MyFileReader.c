#include "MyFileReader.h"
#include "FileToProcessQueue.h"

int read_lines(MyFileReader* this, const char* fname, VirtualCPU* cpu) {
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
    VCPUInputState state;
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
        this->call_back(begin, end, &state, cpu);

        if ((begin = ++end) >= buf_end)
            break;
    }

    munmap(buf, fs.st_size);
    close(fd);
    return 1;
}

void initFileReader(MyFileReader* fr, void(*call_back)(const char* begin, const char* end, VCPUInputState* state, VirtualCPU*)) {
    fr->call_back = call_back;
    fr->readLines = read_lines;
}