//
// Created by andrea on 30/11/17.
//

#include <dirent.h>
#include <errno.h>
#include <memory.h>
#include <stdlib.h>
#include "structs.h"
#include "functions.h"

int search_file(char *name_cached_img, char *img_to_send) {
    DIR *dir;
    struct dirent *ent;
    errno = 0;
    dir = opendir(tmp_cache);
    if (!dir) {
        if (errno == EACCES) {
            fprintf(stderr, "data_to_send: Error in opendir: Permission denied\n");
            free_time_http(time, http_response);
            return -1;
        }
        fprintf(stderr, "data_to_send: Error in opendir\n");
        free_time_http(time, http_response);
        return -1;
    }

    //finding the requested image
    while ((ent = readdir(dir)) != NULL) {
        if (ent->d_type == DT_REG) {
            if (!strncmp(ent->d_name, name_cached_img, strlen(name_cached_img))) {
                img_to_send = get_img(name_cached_img, c->size_q, tmp_cache);
                if (!img_to_send) {
                    fprintf(stderr, "data_to_send: Error in get_img\n");
                    free_time_http(time, http_response);
                    return -1;
                }
                break;
            }
        }
    }

    if (closedir(dir)) {
        fprintf(stderr, "data_to_send: Error in closedir\n");
        free(img_to_send);
        free_time_http(time, http_response);
        return -1;
    }

    return 0;
}

