
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>

#include "Settings.h"
// Printout of how to use the program properly

void printUsage(int argc, char*argv[]) {
    printf("usage:\n %s [-fr] Input_File [Output_File] [-v number]\n", argv[0]);
    printf("\t -f : Run in FileOnly Mode, will not print to console.\n");
    printf("\t -r : Round Robin Only, a FCFS queue will not be added as the end queue.\n");
    printf("\tInput_File : The file to load scheduler simulation data from\n");
    printf("\tOutput_File : [Optional] The file to save results. Defaults to Input_File.result\n");
    printf("\t -v number: Run in Verbose Mode, 1 to 8, Default: 3, Default -v (no arguments): 4\n");
    printf("\t\t If you are not using the -f option, it is highly advisable to run this with `| more`\n");
    printf("\t\t so you can read the output. \n");
    printf("\t\t LogLevel Display Tiers:: \n");
    printf("\t\t 1: Only SEVERE logs are displayed. No actual simulation output\n");
    printf("\t\t 2: WARNING logs are displayed in addition to above. No actual simulation output\n");
    printf("\t\t 3: INFO logs are displayed in addition to above. This is the standard and includes\n");
    printf("\t\t\t an appropriate ammount of simulation output\n");
    printf("\t\t 4: Adds CONFIG logs. This includes useful initialization and destruction info.\n");
    printf("\t\t 5-7: Adds FINE-FINER-FINEST logs. THIS IS TRACE INFORMATION. Can get lengthy.\n");
    printf("\t\t 8: ALL Logs regardless of level will be displayed.\n");
    printf("\n");
}

Settings* Settings_init(int argc, char *argv[]) {

    if (argc < 2) {
        printUsage(argc, argv);
        exit(EXIT_FAILURE);
    }

    int c, v = 3;
    bool fileOnly = false, RROnly = false;
    while ((c = getopt(argc, argv, "frv:")) != -1)
        switch (c) {
            case 'f':
                fileOnly = true;
                break;
            case 'r':
                RROnly = true;
                break;
            case 'v':
                v = strtol(optarg, NULL, 10);
                if (v > 8 || v < 1) {
                    fprintf(stderr, "Option for v out of bounds, 1-8.\n\n");
                    printUsage(argc, argv);
                    exit(EXIT_FAILURE);
                } else if (v == 0) {
                    fprintf(stderr, "Option -%c defaulting to value 4.\n", optopt);
                    v = 4;
                }
                break;
            case '?':
                if (optopt == 'v') {
                    fprintf(stderr, "Option -%c defaulting to value 4.\n", optopt);
                    v = 4;
                }
                if (isprint(optopt))
                    fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf(stderr,
                        "Unknown option character `\\x%x'.\n",
                        optopt);
            default:
                abort();
        }

    if (v >= 5) {
        int i;
        for (i = 0; i < sizeof (argv); i++) {
            fprintf(stderr, "Argv[%d]: %s\n", i, argv[i]);
        }
        fprintf(stderr, "\nArgv[optind]: %s\n", argv[optind]);
        fprintf(stderr, "optind: %d\n", optind);
    }

    Settings* this = malloc(sizeof (*this));
    if (argv[optind] == NULL) {
        fprintf(stderr, "Input file needs to be provided.\n");
        exit(EXIT_FAILURE);
    }
    this->jobInputName = argv[optind];
    if (argv[optind + 1] == NULL) {
        this->jobOutputName = (char*) malloc(100);
        strcpy(this->jobOutputName, this->jobInputName);
        strcat(this->jobOutputName, ".result");
        this->optfileProvided = false;
        fprintf(stderr, "Output file not provided, defaulting to %s\n", this->jobOutputName);
    } else {
        this->optfileProvided = true;
        this->jobOutputName = argv[optind + 1];
    }
    this->logger = Logger_init(this->jobInputName, this->jobOutputName, v, fileOnly);
    this->roundRobinOnly = RROnly;
    return this;
}

void Settings_destruct(Settings* this) {
    Logger_destruct(this->logger);
    if (!this->optfileProvided)
        free(this->jobOutputName);
    free(this);
}
