//
// Created by andrea on 30/11/17.
//

#include "structs.h"
#include "functions.h"

char *search_file(struct image_struct *image, char *name_cached_img, char *img_to_send, struct cache_struct *cache_ptr, char *char_time, char *http_response) {

    DIR *dir;
    struct dirent *ent;
    errno = 0;
    dir = opendir(cache_tmp_dir);
    if (!dir) {
        if (errno == EACCES) {
            fprintf(stderr, "search_file: Error in opendir: Permission denied\n");
            free_time_http(char_time, http_response);
            return NULL;
        }
        fprintf(stderr, "search_file: Error in opendir\n");
        free_time_http(char_time, http_response);
        return NULL;
    }

    //finding the requested image_struct

    while ((ent = readdir(dir)) != NULL) {
        if (ent->d_type == DT_REG) {

            if (!strncmp(ent->d_name, name_cached_img, strlen(name_cached_img))) {
                cache_ptr = image->cached_image;
                while (cache_ptr) {
                    if (!strncmp(name_cached_img, cache_ptr->cached_name, strlen(name_cached_img))) {
                        img_to_send = get_image(name_cached_img, cache_ptr->cached_image_size, cache_tmp_dir);
                        if (!img_to_send) {
                            fprintf(stderr, "search_file: Error in get_image\n");
                            free_time_http(char_time, http_response);
                            return NULL;
                        }
                        break;
                    }
                    cache_ptr = cache_ptr->next_cached_image;
                }
            }
        }
    }

    if (closedir(dir)) {
        fprintf(stderr, "search_file: Error in closedir\n");
        free(img_to_send);
        free_time_http(char_time, http_response);
        return NULL;
    }
    return img_to_send;
}

char *search_file_resized(struct image_struct *image, char *name_resized_image, char *img_to_send, struct cache_struct *cache_ptr, char *char_time, char *http_response) {

    fprintf(stderr, "search_file_resized\n");

    DIR *dir;
    struct dirent *ent;
    errno = 0;
    dir = opendir(resized_tmp_dir);
    if (!dir) {
        if (errno == EACCES) {
            fprintf(stderr, "search_file_resized: Error in opendir: Permission denied\n");
            free_time_http(char_time, http_response);
            return NULL;
        }
        fprintf(stderr, "search_file_resized: Error in opendir\n");
        free_time_http(char_time, http_response);
        return NULL;
    }

    //finding the requested image_struct

    fprintf(stderr, "search_file_resized: 1\n");

    while ((ent = readdir(dir)) != NULL) {
        if (ent->d_type == DT_REG) {

            fprintf(stderr, "search_file_resized: filename is : %s\n", ent->d_name);

            if (!strncmp(ent->d_name, name_resized_image, strlen(name_resized_image))) {

                fprintf(stderr, "search_file_resized: 2\n");

                img_to_send = get_image(name_resized_image, cache_ptr->cached_image_size, cache_tmp_dir);

                fprintf(stderr, "search_file_resized: img_to_send is: %s\n", img_to_send);

                fprintf(stderr, "search_file_resized: 3\n");

                if (!img_to_send) {
                    fprintf(stderr, "search_file_resized: Error in get_image\n");
                    free_time_http(char_time, http_response);
                    return NULL;
                }
                break;
            }
        }
    }

    if (closedir(dir)) {
        fprintf(stderr, "search_file_resized: Error in closedir\n");
        free(img_to_send);
        free_time_http(char_time, http_response);
        return NULL;
    }

    fprintf(stderr, "search_file_resized: END\n");

    return img_to_send;
}

