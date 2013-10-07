/* 
 * File:   FileWriter.c
 * Author: Nicholas `Timethor` Rich     G00399909
 *
 * Created on October 4, 2013, 7:11 PM
 */

#include "FileWriter.h"

void FW_write_lines(Settings* settings, char* textToWrite) {
    size_t bytesWritten = 0;
    int fd1;
    fd1 = open(settings->jobOutputName,
            (O_CREAT | O_TRUNC | O_RDWR),
            (S_IRWXU | S_IRWXG | S_IRWXO));
    if (fd1 < 0)
        perror("open() error");
    else {

        bytesWritten = write(fd1, textToWrite, strlen(textToWrite));
        if (bytesWritten != strlen(textToWrite)) {
            perror("write() error");
        }
        close(fd1);
    }
}

FileWriter* FileWriter_init(SimulationState* istate) {
    FileWriter* fw = malloc(sizeof (*fw));
    //    fw->call_back = istate->call_back_write;
    //    fw->writeLines = FW_write_lines;
    return fw;
}
