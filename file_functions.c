//
// Created by andrea on 30/11/17.
//

#include "structs.h"
#include "functions.h"

char *search_file(struct image *image, char *name_cached_img, char *img_to_send, struct cache *cache_ptr, char *char_time, char *http_response) {

    fprintf(stderr, "search_file\n");

    DIR *dir;
    struct dirent *ent;
    errno = 0;
    dir = opendir(tmp_cache);
    if (!dir) {
        if (errno == EACCES) {
            fprintf(stderr, "data_to_send: Error in opendir: Permission denied\n");
            free_time_http(char_time, http_response);
            return -1;
        }
        fprintf(stderr, "data_to_send: Error in opendir\n");
        free_time_http(char_time, http_response);
        return -1;
    }

    //finding the requested image

    fprintf(stderr, "search_file: 1\n");

    while ((ent = readdir(dir)) != NULL) {
        if (ent->d_type == DT_REG) {

            fprintf(stderr, "search_file: filenameis : %s\n", ent->d_name);

            if (!strncmp(ent->d_name, name_cached_img, strlen(name_cached_img))) {

                fprintf(stderr, "search_file: 2\n");

                cache_ptr = image->img_c;

                while (cache_ptr) {
                    fprintf(stderr, "search_file: 3\n");

                    fprintf(stderr, "search_file: name_cached_img is: %s\n", name_cached_img);
                    fprintf(stderr, "search_file: cache_ptr->img_q is: %s\n", cache_ptr->img_q);

                    if (!strncmp(name_cached_img, cache_ptr->img_q, strlen(name_cached_img))) {

                        fprintf(stderr, "search_file: 4\n");

                        img_to_send = get_img(name_cached_img, cache_ptr->size_q, tmp_cache);

                        fprintf(stderr, "search_file: img_to_send is: %s\n", img_to_send);

                        fprintf(stderr, "search_file: 5\n");

                        if (!img_to_send) {
                            fprintf(stderr, "search_file: Error in get_img\n");
                            free_time_http(char_time, http_response);
                            return -1;
                        }
                        break;
                    }
                    cache_ptr = cache_ptr->next_img_c;

                    fprintf(stderr, "search_file: 6\n");

                }
            }
        }
    }

    if (closedir(dir)) {
        fprintf(stderr, "data_to_send: Error in closedir\n");
        free(img_to_send);
        free_time_http(char_time, http_response);
        return -1;
    }

    fprintf(stderr, "search_file: END\n");

    return img_to_send;
}

