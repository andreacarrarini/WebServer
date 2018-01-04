//
// Created by andrea on 23/11/17.
//

#include "structs.h"
#include "functions.h"

struct image *img;
struct th_sync thds;

void look_for_cached_img(int CACHE_N, char *name_cached_img) {
    if (CACHE_N >= 0 && strncmp(thds.cache_hit_head->cache_name,
                                name_cached_img, strlen(name_cached_img))) {
        struct cache_hit *prev_node, *node;
        prev_node = NULL;
        node = thds.cache_hit_tail;
        while (node) {
            if (!strncmp(node->cache_name, name_cached_img, strlen(name_cached_img))) {
                if (prev_node) {
                    prev_node->next_hit = node->next_hit;
                } else {
                    thds.cache_hit_tail = thds.cache_hit_tail->next_hit;
                }
                node->next_hit = thds.cache_hit_head->next_hit;
                thds.cache_hit_head->next_hit = node;
                thds.cache_hit_head = thds.cache_hit_head->next_hit;
                break;
            }
            prev_node = node;
            node = node->next_hit;
        }
    }
}

void free_time_http(char *time, char *http) {   //TODO change
    free(time);
    free(http);
}

int free_cache_slot(struct cache *c, struct image *i, char *char_time, char *http_response) {
    /*struct stat buf;
    memset(&buf, (int) '\0', sizeof(struct stat));
    errno = 0;
    if (stat(path, &buf) != 0) {
        if (errno == ENAMETOOLONG) {
            fprintf(stderr, "Path too long\n");
            free_time_http(char_time, http_response);
            unlock(thds.mtx_cache_access);
            return -1;
        }
        fprintf(stderr, "data_to_send: Invalid path\n");
        free_time_http(char_time, http_response);
        unlock(thds.mtx_cache_access);
        return -1;
    } else if (!S_ISREG(buf.st_mode)) {
        fprintf(stderr, "Non-regular files can not be analysed!\n");
        free_time_http(char_time, http_response);
        unlock(thds.mtx_cache_access);
        return -1;
    }

    struct cache *new_entry = malloc(sizeof(struct cache));
    memset(new_entry, (int) '\0', sizeof(struct cache));
    if (!new_entry) {
        fprintf(stderr, "data_to_send: Error in malloc\n");
        free_time_http(char_time, http_response);
        unlock(thds.mtx_cache_access);
        return -1;
    }
    //filling cache struct of current image
    new_entry->q = quality_factor;
    strcpy(new_entry->img_q, name_cached_img);
    new_entry->size_q = (size_t) buf.st_size;
    new_entry->next_img_c = i->img_c;
    i->img_c = new_entry;
    c = i->img_c;*/

    /*
     * To find and delete oldest requested
     * element from cache structure
     */

    /*struct image *img_ptr = img;
    struct cache *cache_ptr;*/
    struct image *img_ptr = i;
    struct cache *cache_ptr = c;
    struct cache *cache_prev = NULL;

    char *ext = strrchr(thds.cache_hit_tail->cache_name, '_');
    //ext is the "_quality" of the resized img
    size_t dim_fin = strlen(ext);
    char name_i[DIM / 2];
    memset(name_i, (int) '\0', DIM / 2);
    strncpy(name_i, thds.cache_hit_tail->cache_name,
            strlen(thds.cache_hit_tail->cache_name) - dim_fin);

    /*
     * img_ptr scans the image linked list
     */
    while (img_ptr) {
        if (!strncmp(img_ptr->name, name_i, strlen(name_i))) {
            cache_ptr = img_ptr->img_c;
            while (cache_ptr) {
                if (!strncmp(cache_ptr->img_q, thds.cache_hit_tail->cache_name,
                             strlen(thds.cache_hit_tail->cache_name))) {
                    if (!cache_prev)
                        img_ptr->img_c = cache_ptr->next_img_c;
                    else
                        cache_prev->next_img_c = cache_ptr->next_img_c;

                    free(cache_ptr);
                    break;
                }
                cache_prev = cache_ptr;
                cache_ptr = cache_ptr->next_img_c;
            }
            if (!cache_ptr) {
                fprintf(stderr, "data_to_send: Error! struct cache compromised\n"
                        "-Cache size automatically set to Unlimited\n\t\tfinding: %s\n", name_i);
                free_time_http(char_time, http_response);
                CACHE_N = -1;
                unlock(thds.mtx_cache_access);
                return -1;
            }
            break;
        }
        img_ptr = img_ptr->next_img;
    }
    if (!img_ptr) {
        CACHE_N = -1;
        fprintf(stderr, "data_to_send: Unexpected error while looking for image in struct image\n"
                "-Cache size automatically set to Unlimited\n\t\tfinding: %s\n", name_i);
        free_time_http(char_time, http_response);
        unlock(thds.mtx_cache_access);
        return -1;
    }

    /*struct cache_hit *new_hit = malloc(sizeof(struct cache_hit));
    memset(new_hit, (int) '\0', sizeof(struct cache_hit));
    if (!new_hit) {
        fprintf(stderr, "data_to_send: Error in malloc\n");
        free_time_http(char_time, http_response);
        unlock(thds.mtx_cache_access);
        return -1;
    }

    strncpy(new_hit->cache_name, name_cached_img, strlen(name_cached_img));*/
    /*
     * updating cache_hit tail and head
     */
    struct cache_hit *to_be_removed = thds.cache_hit_tail;

    /*new_hit->next_hit = thds.cache_hit_head->next_hit;
    thds.cache_hit_head->next_hit = new_hit;
    thds.cache_hit_head = thds.cache_hit_head->next_hit;*/


    thds.cache_hit_tail = thds.cache_hit_tail->next_hit;

    free(to_be_removed);

    return 0;
}

int delete_image(char *img_to_send, char *char_time, char *http_response) {
    char name_to_remove[DIM / 2];
    memset(name_to_remove, (int) '\0', DIM / 2);
    sprintf(name_to_remove, "%s/%s", tmp_cache, thds.cache_hit_tail->cache_name);

    DIR *dir;
    struct dirent *ent;
    errno = 0;
    dir = opendir(tmp_cache);
    //error control
    if (!dir) {
        if (errno == EACCES) {
            fprintf(stderr, "data_to_send: Error in opendir: Permission denied\n");
            free_time_http(char_time, http_response);
            unlock(thds.mtx_cache_access);
            return -1;
        }
        fprintf(stderr, "data_to_send: Error in opendir\n");
        free_time_http(char_time, http_response);
        unlock(thds.mtx_cache_access);
        return -1;
    }

    while ((ent = readdir(dir)) != NULL) {
        if (ent->d_type == DT_REG) {
            if (!strncmp(ent->d_name, thds.cache_hit_tail->cache_name,
                         strlen(thds.cache_hit_tail->cache_name))) {
                rm_link(name_to_remove);
                break;
            }
        }
    }
    //if File is not found
    if (!ent) {
        fprintf(stderr, "File: '%s' not removed\n", name_to_remove);
    }

    if (closedir(dir)) {
        fprintf(stderr, "data_to_send: Error in closedir\n");
        free(img_to_send);
        free_time_http(char_time, http_response);
        unlock(thds.mtx_cache_access);
        return -1;
    }
    return 0;
}

int insert_in_cache(char *path, int quality_factor, char *name_cached_img, struct image *i, struct cache *c, char *char_time, char *http_response) {
    struct stat buf;
    memset(&buf, (int) '\0', sizeof(struct stat));
    errno = 0;
    //error control
    if (stat(path, &buf) != 0) {
        if (errno == ENAMETOOLONG) {
            fprintf(stderr, "Path too long\n");
            free_time_http(char_time, http_response);
            unlock(thds.mtx_cache_access);
            return -1;
        }
        fprintf(stderr, "data_to_send: Invalid path\n");
        free_time_http(char_time, http_response);
        unlock(thds.mtx_cache_access);
        return -1;
    } else if (!S_ISREG(buf.st_mode)) {
        fprintf(stderr, "Non-regular files can not be analysed!\n");
        free_time_http(char_time, http_response);
        unlock(thds.mtx_cache_access);
        return -1;
    }

    struct cache *new_entry = malloc(sizeof(struct cache));
    memset(new_entry, (int) '\0', sizeof(struct cache));
    if (!new_entry) {
        fprintf(stderr, "data_to_send: Error in malloc\n");
        free_time_http(char_time, http_response);
        unlock(thds.mtx_cache_access);
        return -1;
    }

    /*
     * filling struct cache of the relative image
     * and inserting the struct cache_hit in the cache list
     */
    new_entry->q = quality_factor;
    strcpy(new_entry->img_q, name_cached_img);
    new_entry->size_q = (size_t) buf.st_size;
    new_entry->next_img_c = i->img_c;
    i->img_c = new_entry;
    c = i->img_c;

    if (CACHE_N > 0) {

        struct cache_hit *new_hit = malloc(sizeof(struct cache_hit));
        memset(new_hit, (int) '\0', sizeof(struct cache_hit));
        if (!new_hit) {
            fprintf(stderr, "data_to_send: Error in malloc\n");
            free_time_http(char_time, http_response);
            unlock(thds.mtx_cache_access);
            return -1;
        }


        strncpy(new_hit->cache_name, name_cached_img, strlen(name_cached_img));
        if (!thds.cache_hit_head && !thds.cache_hit_tail) {
            new_hit->next_hit = thds.cache_hit_head;
            thds.cache_hit_tail = thds.cache_hit_head = new_hit;
        }
            //inserting new_hit by the head
        else {
            new_hit->next_hit = thds.cache_hit_head->next_hit;
            thds.cache_hit_head->next_hit = new_hit;
            thds.cache_hit_head = thds.cache_hit_head->next_hit;
        }
    }
    return 0;
}