
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#include <stdbool.h>
#include <fcntl.h>

#include "Settings.h"
// Printout of how to use the program properly

void printUsage(int argc, char*argv[]) {
    printf("usage:\n %s [vdf] Input File \n"
            " [-o] outputfile", argv[0]);
    printf("\n \t -v : Run in Verbose Mode\n");
    printf("\t -v : Run in Debug Mode\n");
    printf("\t -f : Run in FileOnly Mode, will not print to console. Works with [-o] \n");
    printf("\tInput File : The file to load jobs from\n");
    printf("\t -o filename : The output file, if desired. Output will be printed to console. \n");
    printf("\n");
}

Settings* setup(int argc, char *argv[]) {

    if (argc < 2) {
        printUsage(argc, argv);
        exit(EXIT_FAILURE);
    }

    int c;
    bool debug = false, trace = false, fileOnly = false;
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
                trace = true;
                break;
            case 'o':
                outputFile = optarg;
                break;
            case '?':
                if (optopt == 'o')
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint(optopt))
                    fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf(stderr,
                        "Unknown option character `\\x%x'.\n",
                        optopt);
            default:
                abort();
        }

    errno = 0;
    printf("FILE TO BE READ FROM:: %s", argv[optind] );
    
    int fp = open(argv[optind], O_RDONLY);

    if (fp == -1) {
        perror(NULL);
        exit(EXIT_FAILURE);
    }
    errno = 0;
    FILE* fpo;
    if (outputFile != NULL) {
        fpo = fopen(outputFile, "w+");

        if (fpo == NULL) {
            perror(NULL);
        }
    }
    Settings* returnpair = malloc(sizeof (*returnpair));
    returnpair->jobInput = fp;
    returnpair->jobInputName = argv[optind];
    returnpair->jobOutput = fpo;
    returnpair->jobOutputName = outputFile;
    returnpair->debug = debug;
    returnpair->trace = trace;
    return returnpair;
}