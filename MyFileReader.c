#include "MyFileReader.h"
#include "Settings.h"

int FR_read_lines(MyFileReader* this, Settings* settings, InputState* istate) {
    //>>	
    struct stat fs;
    char *buf, *buf_end;
    char *begin, *end, c;
    int fd = settings->jobInput;

    if (fstat(fd, &fs) == -1) {
        err(1, "stat: %s", settings->jobInputName);
        return false;
    }

    buf = mmap(0, fs.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (buf == (void*) - 1) {
        err(1, "mmap: %s", settings->jobInputName);
        close(fd);
        return false;
    }

    buf_end = buf + fs.st_size;

    begin = end = buf;
    //>>	Main loop for each line in file
    while (true) {
        if (istate->error_thrown) {
            break;
        }
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
        this->call_back(istate, begin, end);

        if ((begin = ++end) >= buf_end)
            break;
    }

    munmap(buf, fs.st_size);
    close(fd);
    return true;
}

void MyFileReader_init(MyFileReader* fr, InputState* istate) {
    fr->call_back = istate->call_back;
    fr->readLines = FR_read_lines;
}