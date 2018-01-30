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

/*
 * Used to remove directory from file system
 */
void remove_directory(char *directory) {

    DIR *dir;
    struct dirent *dirent;

    fprintf(stdout, "-Removing '%s'\n", directory);
    char *verify = strrchr(directory, '/') + 1;
    //NULL if / is not found
    if (!verify)
        error_found("remove_directory: Unexpected error in strrchr\n");
    verify = strrchr(directory, '.') + 1;
    //not a thing we want to remove
    if (!strncmp(verify, "XXXXXX", 7))
        return;

    errno = 0;
    dir = opendir(directory);
    if (!dir) {
        if (errno == EACCES)
            error_found("remove_directory: Permission denied\n");
        error_found("remove_directory: Error in opendir\n");
    }

    while ((dirent = readdir(dir)) != NULL) {
        //deletes all files in directory
        if (dirent -> d_type == DT_REG) {
            char buf[DIM];
            memset(buf, (int) '\0', DIM);
            sprintf(buf, "%s/%s", directory, dirent -> d_name);
            fprintf(stderr, "%s\n", dirent ->d_name);
            remove_link(buf);
        }
    }

    if (closedir(dir))
        error_found("remove_directory: Error in closedir\n");

    errno = 0;
    if (rmdir(directory)) {
        switch (errno) {
            case EBUSY:
                error_found("remove_directory: Directory not removed: Resource busy\n");

            case ENOMEM:
                error_found("remove_directory: Directory not removed: Insufficient kernel memory\n");

            case EROFS:
                error_found("remove_directory: Directory not removed: Directory is read-only\n");

            case ENOTEMPTY:
                error_found("remove_directory: Directory not removed: Directory not empty\n");

            default:
                error_found("remove_directory: Error in rmdir\n");
        }
    }
}

