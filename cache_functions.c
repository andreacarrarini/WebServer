//
// Created by andrea on 23/11/17.
//

#include "structs.h"
#include "functions.h"

struct image_struct *image_struct;
struct threads_sync_struct thread_struct;

void look_for_cached_img(int CACHE_COUNTER, char *name_cached_img) {

    if (CACHE_COUNTER >= 0 && strncmp(thread_struct.cached_name_head->cache_name,
                                name_cached_img, strlen(name_cached_img))) {
        struct cached_name_element *prev_element, *element;
        prev_element = NULL;
        element = thread_struct.cached_name_tail;
        while (element) {
            if (!strncmp(element->cache_name, name_cached_img, strlen(name_cached_img))) {
                if (prev_element) {
                    prev_element->next_cached_image_name = element->next_cached_image_name;
                } else {
                    thread_struct.cached_name_tail = thread_struct.cached_name_tail->next_cached_image_name;
                }
                element->next_cached_image_name = thread_struct.cached_name_head->next_cached_image_name;
                thread_struct.cached_name_head->next_cached_image_name = element;
                thread_struct.cached_name_head = thread_struct.cached_name_head->next_cached_image_name;
                break;
            }
            prev_element = element;
            element = element->next_cached_image_name;
        }
    }
}

void free_time_HTTP_response(char *time, char *HTTP_response) {

    free(time);
    free(HTTP_response);
}

int free_cache_slot(struct image_struct *image, char *char_time, char *HTTP_response) {

    /*
     * To find and delete oldest requested
     * element from cache_struct structure
     */
    struct image_struct *image_ptr = image;
    struct cache_struct *cache_ptr;
    struct cache_struct *cache_prev = NULL;

    char *qualiy_ptr = strrchr(thread_struct.cached_name_tail->cache_name, '_');

    //ext is the "_quality" of the resized img
    size_t quality = strlen(qualiy_ptr);
    char image_name[DIM / 2];
    memset(image_name, (int) '\0', DIM / 2);
    strncpy(image_name, thread_struct.cached_name_tail->cache_name, strlen(thread_struct.cached_name_tail->cache_name) - quality);

    /*
     * image_ptr scans the image_struct linked list
     */

    while (image_ptr) {
        if (!strncmp(image_ptr->name, image_name, strlen(image_name))) {
            cache_ptr = image_ptr->cached_image;

            while (cache_ptr) {
                if (!strncmp(cache_ptr->cached_name, thread_struct.cached_name_tail->cache_name,
                             strlen(thread_struct.cached_name_tail->cache_name))) {
                    if (!cache_prev)
                        image_ptr->cached_image = cache_ptr->next_cached_image;
                    else
                        cache_prev->next_cached_image = cache_ptr->next_cached_image;

                    free(cache_ptr);
                    break;
                }
                cache_prev = cache_ptr;
                cache_ptr = cache_ptr->next_cached_image;
            }
            if (!cache_ptr) {

                fprintf(stderr, "free_cache_slot: Error! struct cache_struct compromised\n"
                        "-Cache size automatically set to Unlimited\n\t\tfinding: %s\n", image_name);
                free_time_HTTP_response(char_time, HTTP_response);
                CACHE_COUNTER = -1;
                unlock(thread_struct.mtx_cache_access);
                return -1;
            }
            break;
        }
        image_ptr = image_ptr->next_image;
    }

    if (!image_ptr) {
        CACHE_COUNTER = -1;
        fprintf(stderr, "free_cache_slot: Unexpected error while looking for image_struct in struct image_struct\n"
                "-Cache size automatically set to Unlimited\n\t\tfinding: %s\n", image_name);
        free_time_HTTP_response(char_time, HTTP_response);
        unlock(thread_struct.mtx_cache_access);
        return -1;
    }

    /*
     * updating cached_name tail and head
     */

    struct cached_name_element *to_be_removed = thread_struct.cached_name_tail;
    thread_struct.cached_name_tail = thread_struct.cached_name_tail->next_cached_image_name;
    free(to_be_removed);
    return 0;
}

int delete_image(char *image_to_send, char *char_time, char *HTTP_response) {

    char name_to_remove[DIM / 2];
    memset(name_to_remove, (int) '\0', DIM / 2);
    sprintf(name_to_remove, "%s/%s", cache_tmp_dir, thread_struct.cached_name_tail->cache_name);

    DIR *dir;
    struct dirent *dirent;
    errno = 0;

    dir = opendir(cache_tmp_dir);
    //error control
    if (!dir) {
        if (errno == EACCES) {
            fprintf(stderr, "delete_image: Error in opendir: Permission denied\n");
            free_time_HTTP_response(char_time, HTTP_response);
            unlock(thread_struct.mtx_cache_access);
            return -1;
        }
        fprintf(stderr, "delete_image: Error in opendir\n");
        free_time_HTTP_response(char_time, HTTP_response);
        unlock(thread_struct.mtx_cache_access);
        return -1;
    }

    while ((dirent = readdir(dir)) != NULL) {
        if (dirent->d_type == DT_REG) {
            if (!strncmp(dirent->d_name, thread_struct.cached_name_tail->cache_name,
                         strlen(thread_struct.cached_name_tail->cache_name))) {

                remove_link(name_to_remove);
                break;
            }
        }
    }
    //if File is not found
    if (!dirent) {
        fprintf(stderr, "delete_image: File: '%s' not removed\n", name_to_remove);
    }

    if (closedir(dir)) {
        fprintf(stderr, "delete_image: Error in closedir\n");
        free(image_to_send);
        free_time_HTTP_response(char_time, HTTP_response);
        unlock(thread_struct.mtx_cache_access);
        return -1;
    }

    return 0;
}

int insert_in_cache(char *path, int quality_factor, char *name_cached_img, struct image_struct *image, char *char_time, char *HTTP_repsonse) {

    struct stat buf;
    memset(&buf, (int) '\0', sizeof(struct stat));
    errno = 0;
    //error control
    if (stat(path, &buf) != 0) {
        if (errno == ENAMETOOLONG) {
            fprintf(stderr, "insert_in_cache: Path too long\n");
            free_time_HTTP_response(char_time, HTTP_repsonse);
            unlock(thread_struct.mtx_cache_access);
            return -1;
        }
        fprintf(stderr, "insert_in_cache: Invalid path\n");
        free_time_HTTP_response(char_time, HTTP_repsonse);
        unlock(thread_struct.mtx_cache_access);
        return -1;
    } else if (!S_ISREG(buf.st_mode)) {
        fprintf(stderr, "insert_in_cache: Non-regular files can not be analysed!\n");
        free_time_HTTP_response(char_time, HTTP_repsonse);
        unlock(thread_struct.mtx_cache_access);
        return -1;
    }

    struct cache_struct *new_cache_struct= malloc(sizeof(struct cache_struct));

    memset(new_cache_struct, (int) '\0', sizeof(struct cache_struct));
    if (!new_cache_struct) {
        fprintf(stderr, "insert_in_cache: Error in malloc\n");
        free_time_HTTP_response(char_time, HTTP_repsonse);
        unlock(thread_struct.mtx_cache_access);
        return -1;
    }

    /*
     * filling struct cache_struct of the relative image_struct
     * and inserting the struct cached_name in the cache_struct list
     */

    new_cache_struct->quality = quality_factor;
    strcpy(new_cache_struct->cached_name, name_cached_img);
    new_cache_struct->cached_image_size = (size_t) buf.st_size;
    new_cache_struct->next_cached_image = image->cached_image;
    image->cached_image = new_cache_struct;

    if (CACHE_COUNTER != -1) {

        struct cached_name_element *new_cached_name_element = malloc(sizeof(struct cached_name_element));
        memset(new_cached_name_element, (int) '\0', sizeof(struct cached_name_element));
        if (!new_cached_name_element) {
            fprintf(stderr, "insert_in_cache: Error in malloc\n");
            free_time_HTTP_response(char_time, HTTP_repsonse);
            unlock(thread_struct.mtx_cache_access);
            return -1;
        }


        strncpy(new_cached_name_element->cache_name, name_cached_img, strlen(name_cached_img));
        if (!thread_struct.cached_name_head && !thread_struct.cached_name_tail) {
            new_cached_name_element->next_cached_image_name = thread_struct.cached_name_head;
            thread_struct.cached_name_tail = thread_struct.cached_name_head = new_cached_name_element;
        }
            //inserting new_cached_name_element by the head
        else {
            new_cached_name_element->next_cached_image_name = thread_struct.cached_name_head->next_cached_image_name;
            thread_struct.cached_name_head->next_cached_image_name = new_cached_name_element;
            thread_struct.cached_name_head = thread_struct.cached_name_head->next_cached_image_name;
        }
    }
    return 0;
}