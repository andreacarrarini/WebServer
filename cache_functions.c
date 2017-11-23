//
// Created by andrea on 23/11/17.
//

#include <memory.h>
#include <structs.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include "structs.h"

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

int insert_in_cache() {
    struct stat buf;
    memset(&buf, (int) '\0', sizeof(struct stat));
    errno = 0;
    //error control
    if (stat(path, &buf) != 0) {
        if (errno == ENAMETOOLONG) {
            fprintf(stderr, "Path too long\n");
            free_time_http(time, http_response);
            unlock(thds.mtx_cache_access);
            return -1;
        }
        fprintf(stderr, "data_to_send: Invalid path\n");
        free_time_http(time, http_response);
        unlock(thds.mtx_cache_access);
        return -1;
    } else if (!S_ISREG(buf.st_mode)) {
        fprintf(stderr, "Non-regular files can not be analysed!\n");
        free_time_http(time, http_response);
        unlock(thds.mtx_cache_access);
        return -1;
    }

    struct cache *new_entry = malloc(sizeof(struct cache));
    struct cache_hit *new_hit = malloc(sizeof(struct cache_hit));
    memset(new_entry, (int) '\0', sizeof(struct cache));
    memset(new_hit, (int) '\0', sizeof(struct cache_hit));
    if (!new_entry || !new_hit) {
        fprintf(stderr, "data_to_send: Error in malloc\n");
        free_time_http(time, http_response);
        unlock(thds.mtx_cache_access);
        return -1;
    }

    /*  TODO break in a function
     * filling struct cache of the relative image
     * and inserting the struct cache_hit in the cache list
     */
    new_entry->q = quality_factor;
    strcpy(new_entry->img_q, name_cached_img);
    new_entry->size_q = (size_t) buf.st_size;
    new_entry->next_img_c = i->img_c;
    i->img_c = new_entry;
    c = i->img_c;

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

