//
// Created by andrea on 23/11/17.
//
#include <stdio.h>
#include <stdlib.h>
#include "functions.h"

// To close the process on error
void error_found(char *s) {
    fprintf(stderr, "%s", s);
    if (LOG)
        write_on_stream(s, LOG);

    free_mem();

    exit(EXIT_FAILURE);
}

