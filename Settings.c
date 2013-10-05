
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>

#include "Settings.h"
// Printout of how to use the program properly

void printUsage(int argc, char*argv[]) {
    printf("usage:\n %s [fvV] Input_File\n"
            " [-o] outputfile", argv[0]);
    printf("\t -f : Run in FileOnly Mode, will not print to console. Works with [-o] \n");
    printf("\t -v : Run in Verbose Mode, Run with `| more` if not using -f\n");
    printf("\t -V : Run in Super Verbose Mode, Run with `| more` if not using -f\n");
    printf("\tInput_File : The file to load jobs from\n");
    printf("\t -o filename : The output file, if desired. Output will be printed to console. \n");
    printf("\n");
}

Settings* Settings_init(int argc, char *argv[]) {

    if (argc < 2) {
        printUsage(argc, argv);
        exit(EXIT_FAILURE);
    }

    int c, v = 3;
    bool fileOnly = false;
    char* outputFile = NULL;
    while ((c = getopt(argc, argv, "ovVf:")) != -1)
        switch (c) {
            case 'f':
                fileOnly = true;
                break;
            case 'v':
                v = 5;
                break;
            case 'V':
                v = 8;
                break;
            case 'o':
                outputFile = optarg;
                break;
            case '?':
                if (optopt == 'o')
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                else if (optopt == 'v')
                    fprintf(stderr, "Option -%c requires an argument in the range 0-8.\n", optopt);
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
    this->logger = Logger_init(this->jobInputName, this->jobOutputName, v);
    return this;
}

void Settings_destruct(Settings* this) {
    Logger_destruct(this->logger);
    free(this);
}