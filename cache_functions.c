//
// Created by andrea on 23/11/17.
//

#include "structs.h"
#include "functions.h"

struct image_struct *image_struct;
struct threads_sync_struct thread_struct;

void look_for_cached_img(int CACHE_N, char *name_cached_img) {  //TODO i'm here

    if (CACHE_N >= 0 && strncmp(thread_struct.cached_name_list_head->cache_name,
                                name_cached_img, strlen(name_cached_img))) {
        struct cached_name_list_element *prev_node, *node;
        prev_node = NULL;
        node = thread_struct.cached_name_list_tail;
        while (node) {
            if (!strncmp(node->cache_name, name_cached_img, strlen(name_cached_img))) {
                if (prev_node) {
                    prev_node->next_cached_image_name = node->next_cached_image_name;
                } else {
                    thread_struct.cached_name_list_tail = thread_struct.cached_name_list_tail->next_cached_image_name;
                }
                node->next_cached_image_name = thread_struct.cached_name_list_head->next_cached_image_name;
                thread_struct.cached_name_list_head->next_cached_image_name = node;
                thread_struct.cached_name_list_head = thread_struct.cached_name_list_head->next_cached_image_name;
                break;
            }
            prev_node = node;
            node = node->next_cached_image_name;
        }
    }
}

void free_time_http(char *time, char *http) {   //TODO change

    free(time);
    free(http);
}

int free_cache_slot(struct image_struct *i, char *char_time, char *http_response) {

    /*
     * To find and delete oldest requested
     * element from cache_struct structure
     */
    struct image_struct *img_ptr = i;
    struct cache_struct *cache_ptr;
    struct cache_struct *cache_prev = NULL;

    char *ext = strrchr(thread_struct.cached_name_list_tail->cache_name, '_');

    //ext is the "_quality" of the resized img
    size_t dim_fin = strlen(ext);
    char name_i[DIM / 2];
    memset(name_i, (int) '\0', DIM / 2);
    strncpy(name_i, thread_struct.cached_name_list_tail->cache_name, strlen(thread_struct.cached_name_list_tail->cache_name) - dim_fin);

    /*
     * img_ptr scans the image_struct linked list
     */

    while (img_ptr) {
        if (!strncmp(img_ptr->name, name_i, strlen(name_i))) {
            cache_ptr = img_ptr->cached_image;

            while (cache_ptr) {
                if (!strncmp(cache_ptr->cached_name, thread_struct.cached_name_list_tail->cache_name,
                             strlen(thread_struct.cached_name_list_tail->cache_name))) {
                    if (!cache_prev)
                        img_ptr->cached_image = cache_ptr->next_cached_image;
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
                        "-Cache size automatically set to Unlimited\n\t\tfinding: %s\n", name_i);
                free_time_http(char_time, http_response);
                CACHE_COUNTER = -1;
                unlock(thread_struct.mtx_cache_access);
                return -1;
            }
            break;
        }
        img_ptr = img_ptr->next_image;
    }

    if (!img_ptr) {
        CACHE_COUNTER = -1;
        fprintf(stderr, "free_cache_slot: Unexpected error while looking for image_struct in struct image_struct\n"
                "-Cache size automatically set to Unlimited\n\t\tfinding: %s\n", name_i);
        free_time_http(char_time, http_response);
        unlock(thread_struct.mtx_cache_access);
        return -1;
    }

    /*
     * updating cached_name tail and head
     */

    struct cached_name_list_element *to_be_removed = thread_struct.cached_name_list_tail;
    thread_struct.cached_name_list_tail = thread_struct.cached_name_list_tail->next_cached_image_name;
    free(to_be_removed);
    return 0;
}

int delete_image(char *img_to_send, char *char_time, char *http_response) {

    char name_to_remove[DIM / 2];
    memset(name_to_remove, (int) '\0', DIM / 2);
    sprintf(name_to_remove, "%s/%s", cache_tmp_dir, thread_struct.cached_name_list_tail->cache_name);

    DIR *dir;
    struct dirent *ent;
    errno = 0;

    dir = opendir(cache_tmp_dir);
    //error control
    if (!dir) {
        if (errno == EACCES) {
            fprintf(stderr, "delete_image: Error in opendir: Permission denied\n");
            free_time_http(char_time, http_response);
            unlock(thread_struct.mtx_cache_access);
            return -1;
        }
        fprintf(stderr, "delete_image: Error in opendir\n");
        free_time_http(char_time, http_response);
        unlock(thread_struct.mtx_cache_access);
        return -1;
    }

    while ((ent = readdir(dir)) != NULL) {
        if (ent->d_type == DT_REG) {
            if (!strncmp(ent->d_name, thread_struct.cached_name_list_tail->cache_name,
                         strlen(thread_struct.cached_name_list_tail->cache_name))) {

                remove_link(name_to_remove);
                break;
            }
        }
    }
    //if File is not found
    if (!ent) {
        fprintf(stderr, "delete_image: File: '%s' not removed\n", name_to_remove);
    }

    if (closedir(dir)) {
        fprintf(stderr, "delete_image: Error in closedir\n");
        free(img_to_send);
        free_time_http(char_time, http_response);
        unlock(thread_struct.mtx_cache_access);
        return -1;
    }

    return 0;
}

int insert_in_cache(char *path, int quality_factor, char *name_cached_img, struct image_struct *i, char *char_time, char *http_response) {

    struct stat buf;
    memset(&buf, (int) '\0', sizeof(struct stat));
    errno = 0;
    //error control
    if (stat(path, &buf) != 0) {
        if (errno == ENAMETOOLONG) {
            fprintf(stderr, "Path too long\n");
            free_time_http(char_time, http_response);
            unlock(thread_struct.mtx_cache_access);
            return -1;
        }
        fprintf(stderr, "insert_in_cache: Invalid path\n");
        free_time_http(char_time, http_response);
        unlock(thread_struct.mtx_cache_access);
        return -1;
    } else if (!S_ISREG(buf.st_mode)) {
        fprintf(stderr, "Non-regular files can not be analysed!\n");
        free_time_http(char_time, http_response);
        unlock(thread_struct.mtx_cache_access);
        return -1;
    }

    struct cache_struct *new_entry = malloc(sizeof(struct cache_struct));

    memset(new_entry, (int) '\0', sizeof(struct cache_struct));
    if (!new_entry) {
        fprintf(stderr, "insert_in_cache: Error in malloc\n");
        free_time_http(char_time, http_response);
        unlock(thread_struct.mtx_cache_access);
        return -1;
    }

    /*
     * filling struct cache_struct of the relative image_struct
     * and inserting the struct cached_name in the cache_struct list
     */

    new_entry->quality = quality_factor;
    strcpy(new_entry->cached_name, name_cached_img);
    new_entry->cached_image_size = (size_t) buf.st_size;
    new_entry->next_cached_image = i->cached_image;
    i->cached_image = new_entry;

    if (CACHE_COUNTER != -1) {

        struct cached_name_list_element *new_hit = malloc(sizeof(struct cached_name_list_element));
        memset(new_hit, (int) '\0', sizeof(struct cached_name_list_element));
        if (!new_hit) {
            fprintf(stderr, "insert_in_cache: Error in malloc\n");
            free_time_http(char_time, http_response);
            unlock(thread_struct.mtx_cache_access);
            return -1;
        }


        strncpy(new_hit->cache_name, name_cached_img, strlen(name_cached_img));
        if (!thread_struct.cached_name_list_head && !thread_struct.cached_name_list_tail) {
            new_hit->next_cached_image_name = thread_struct.cached_name_list_head;
            thread_struct.cached_name_list_tail = thread_struct.cached_name_list_head = new_hit;
        }
            //inserting new_hit by the head
        else {
            new_hit->next_cached_image_name = thread_struct.cached_name_list_head->next_cached_image_name;
            thread_struct.cached_name_list_head->next_cached_image_name = new_hit;
            thread_struct.cached_name_list_head = thread_struct.cached_name_list_head->next_cached_image_name;
        }

        struct cached_name_list_element *prova = malloc(sizeof(struct cached_name_list_element));
        memset(prova, (int) '\0', sizeof(struct cached_name_list_element));
        if (!prova) {
            fprintf(stderr, "insert_in_cache: Error in malloc\n");
            free_time_http(char_time, http_response);
            unlock(thread_struct.mtx_cache_access);
            return -1;
        }

        prova = thread_struct.cached_name_list_tail;
        while (prova) {
            fprintf(stderr, "insert_in_cache: thread_struct.cached_name_list_tail->cache_name is: %s\n", prova->cache_name);
            prova = prova->next_cached_image_name;
        }

    }

    return 0;
}