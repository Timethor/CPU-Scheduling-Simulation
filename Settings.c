
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>

#include "Settings.h"
// Printout of how to use the program properly

void printUsage(int argc, char*argv[]) {
    printf("usage:\n %s [df] Input_File\n"
            " [-v] number [-o] outputfile", argv[0]);
    printf("\n \t -d : Run in Debug Mode\n");
    printf("\t -f : Run in FileOnly Mode, will not print to console. Works with [-o] \n");
    printf("\tInput_File : The file to load jobs from\n");
    printf("\t -v number: Run in Verbose Mode from 1-4, number may be omitted, 1 will be assumed\n \n");
    printf("\t -o filename : The output file, if desired. Output will be printed to console. \n");
    printf("\n");
}

Settings* Settings_init(int argc, char *argv[]) {

    if (argc < 2) {
        printUsage(argc, argv);
        exit(EXIT_FAILURE);
    }

    int c;
    bool debug = false, trace = 0, fileOnly = false;
    char* outputFile = NULL;
    while ((c = getopt(argc, argv, "ovdf:")) != -1)
        switch (c) {
            case 'f':
                fileOnly = true;
                break;
            case 'd':
                debug = true;
                break;
            case 'v':
                if (optarg != NULL && optarg != "") {
                    int value = strtol(optarg, NULL, 10);
                    if (value > 4 || value < 1) {
                        fprintf(stderr, "Option -%c requires a range of 1-4.\n", optopt);
                        break;
                    }
                    trace = value;
                } else {
                    trace = 1;
                }
                break;
            case 'o':
                outputFile = optarg;
                break;
            case '?':
                if (optopt == 'o')
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                else if (optopt == 'v')
                    fprintf(stderr, "Option -%c requires an argument in the range 1-4.\n", optopt);
                else if (isprint(optopt))
                    fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf(stderr,
                        "Unknown option character `\\x%x'.\n",
                        optopt);
            default:
                abort();
        }

    Settings* this = malloc(sizeof (*this));
    this->jobInputName = argv[optind];
    this->jobOutputName = outputFile;
    this->logger = Logger_init(this->jobInputName, this->jobOutputName, debug, trace);
    return this;
}

void Settings_destruct(Settings* this){
    Logger_destruct(this->logger);
    free(this);
}