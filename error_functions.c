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
    free_memory();
    exit(EXIT_FAILURE);
}

/*
 * Used to map in memory HTML_PAGES files
 * which respond with error 400 or error 404
 */
void build_error_pages(char **HTML) {

    char *s = "<!DOCTYPE HTML_PAGES PUBLIC \"-//IETF//DTD HTML_PAGES 2.0//EN\"><html><head><title>%s</title></head><body><h1>%s</h1><p>%s</p></body></html>\0";
    size_t len = strlen(s) + 2 * DIM2 * sizeof(char);

    char *not_found_404 = malloc(len);
    char *bad_request_400 = malloc(len);
    if (!not_found_404 || !bad_request_400)
        error_found("Error in malloc\n");
    memset(not_found_404, (int) '\0', len); memset(bad_request_400, (int) '\0', len);
    sprintf(not_found_404, s, "404 Not Found", "404 Not Found", "The requested URL is not on this server.");
    sprintf(bad_request_400, s, "400 Bad Request", "Bad Request", "Your browser sent a request this server could not understand.");
    HTML[1] = not_found_404;
    HTML[2] = bad_request_400;
}


