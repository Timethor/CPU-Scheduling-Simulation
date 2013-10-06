#include "FileWriter.h"

int FW_write_lines(Settings* settings, char* textToWrite) {
    size_t bytesWritten = 0;
    int my_offset = 0;
    int fd1;
    int PageSize;
    void *address;
    fd1 = open(settings->jobOutputName,
            (O_CREAT | O_TRUNC | O_RDWR),
            (S_IRWXU | S_IRWXG | S_IRWXO));
    if (fd1 < 0)
        perror("open() error");
    else {

        bytesWritten = write(fd1, textToWrite, strlen(textToWrite));
        if (bytesWritten != strlen(textToWrite)) {
            perror("write() error");
            int closeRC = close(fd1);
            return -1;
        }
        //
        //        PageSize = (int) sysconf(_SC_PAGESIZE);
        //        if (PageSize < 0) {
        //            perror("sysconf() error");
        //        } else {
        //
        //            off_t lastoffset = lseek(fd1, PageSize - 1, SEEK_SET);
        //            if (lastoffset < 0) {
        //                perror("lseek() error");
        //            } else {
        //                bytesWritten = write(fd1, " ", 1); /* grow file 1 to 1 page. */
        //
        //                int len;
        //
        //                my_offset = 0;
        //                len = settings->logger->filesize;
        //                address = mmap(NULL,
        //                        len,
        //                        PROT_READ,
        //                        MAP_SHARED,
        //                        fd1,
        //                        my_offset);
        //                if (address != MAP_FAILED) {
        //                    /* print data from file 1 */
        //                    printf("\n%s", address);
        //                } else {
        //                    perror("munmap() error=");
        //                }
        //                /*
        //                 *  Unmap two pages.
        //                 */
        //                if (munmap(address, 2 * PageSize) < 0) {
        //                    perror("munmap() error");
        //                } else;
        //
        //            }
        //        }

        close(fd1);
        //        unlink("tmp/mmaptest1");
    }
    /*
     *  Unmap two pages.
     */
    if (munmap(address, strlen(textToWrite)) < 0) {
        perror("munmap() error");
    } else;
}

FileWriter* FileWriter_init(SimulationState* istate) {
    FileWriter* fw = malloc(sizeof (*fw));
    //    fw->call_back = istate->call_back_write;
    //    fw->writeLines = FW_write_lines;
    return fw;
}
