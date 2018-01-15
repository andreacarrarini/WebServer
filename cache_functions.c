//
// Created by andrea on 23/11/17.
//

#include "structs.h"
#include "functions.h"

struct image *img;
struct th_sync thds;

void look_for_cached_img(int CACHE_N, char *name_cached_img) {

    fprintf(stderr, "look_for_cached_img\n");

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

    fprintf(stderr, "free_time_http\n");

    free(time);
    free(http);
}

int free_cache_slot(struct image *i, char *char_time, char *http_response) {

    fprintf(stderr, "free_cache_slot\n");

    /*
     * To find and delete oldest requested
     * element from cache structure
     */

    /*struct image *img_ptr = img;
    struct cache *cache_ptr;*/
    struct image *img_ptr = i;
    struct cache *cache_ptr;
    struct cache *cache_prev = NULL;

    char *ext = strrchr(thds.cache_hit_tail->cache_name, '_');

    fprintf(stderr, "free_cache_slot: 1\n");

    //ext is the "_quality" of the resized img
    size_t dim_fin = strlen(ext);
    char name_i[DIM / 2];
    memset(name_i, (int) '\0', DIM / 2);
    strncpy(name_i, thds.cache_hit_tail->cache_name, strlen(thds.cache_hit_tail->cache_name) - dim_fin);

    fprintf(stderr, "free_cache_slot: 2\n");
    fprintf(stderr, "free_cache_slot: cache_hit_tail->cache_name is: %s\n", thds.cache_hit_tail->cache_name);

    /*
     * img_ptr scans the image linked list
     */

    fprintf(stderr, "free_cache_slot: 3\n");
    fprintf(stderr, "free_cache_slot: img_ptr is: %s\n", img_ptr->name);
    fprintf(stderr, "free_cache_slot: name_i is: %s\n", name_i);
    fprintf(stderr, "free_cache_slot: ext is: %s\n", ext);

    while (img_ptr) {
        if (!strncmp(img_ptr->name, name_i, strlen(name_i))) {
            cache_ptr = img_ptr->img_c;

            fprintf(stderr, "free_cache_slot: 4\n");
            fprintf(stderr, "free_cache_slot: cache_ptr is: %s\n", cache_ptr->img_q);

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

                fprintf(stderr, "free_cache_slot: 5\n");

                cache_prev = cache_ptr;
                cache_ptr = cache_ptr->next_img_c;
            }
            if (!cache_ptr) {

                fprintf(stderr, "free_cache_slot: 6\n");

                fprintf(stderr, "free_cache_slot: Error! struct cache compromised\n"
                        "-Cache size automatically set to Unlimited\n\t\tfinding: %s\n", name_i);
                free_time_http(char_time, http_response);

                fprintf(stderr, "free_cache_slot: 7\n");

                CACHE_N = -1;
                unlock(thds.mtx_cache_access);
                return -1;
            }
            break;
        }
        img_ptr = img_ptr->next_img;

        fprintf(stderr, "free_cache_slot: 8\n");

    }

    fprintf(stderr, "free_cache_slot: 9\n");

    if (!img_ptr) {
        CACHE_N = -1;
        fprintf(stderr, "free_cache_slot: Unexpected error while looking for image in struct image\n"
                "-Cache size automatically set to Unlimited\n\t\tfinding: %s\n", name_i);
        free_time_http(char_time, http_response);
        unlock(thds.mtx_cache_access);
        return -1;
    }

    fprintf(stderr, "free_cache_slot: 10\n");

    /*
     * updating cache_hit tail and head
     */
    struct cache_hit *to_be_removed = thds.cache_hit_tail;

    /*new_hit->next_hit = thds.cache_hit_head->next_hit;
    thds.cache_hit_head->next_hit = new_hit;
    thds.cache_hit_head = thds.cache_hit_head->next_hit;*/

    fprintf(stderr, "free_cache_slot: 11\n");

    thds.cache_hit_tail = thds.cache_hit_tail->next_hit;

    fprintf(stderr, "free_cache_slot: 12\n");

    free(to_be_removed);

    fprintf(stderr, "free_cache_slot: END\n");

    return 0;
}

int delete_image(char *img_to_send, char *char_time, char *http_response) {

    fprintf(stderr, "delete_image\n");

    char name_to_remove[DIM / 2];
    memset(name_to_remove, (int) '\0', DIM / 2);
    sprintf(name_to_remove, "%s/%s", tmp_cache, thds.cache_hit_tail->cache_name);

    fprintf(stderr, "delete_image: 1\n");
    fprintf(stderr, "delete_image: name_to_remove is: %s\n", name_to_remove);

    DIR *dir;
    struct dirent *ent;
    errno = 0;

    dir = opendir(tmp_cache);
    //error control
    if (!dir) {
        if (errno == EACCES) {
            fprintf(stderr, "delete_image: Error in opendir: Permission denied\n");
            free_time_http(char_time, http_response);
            unlock(thds.mtx_cache_access);
            return -1;
        }
        fprintf(stderr, "delete_image: Error in opendir\n");
        free_time_http(char_time, http_response);
        unlock(thds.mtx_cache_access);
        return -1;
    }

    while ((ent = readdir(dir)) != NULL) {
        if (ent->d_type == DT_REG) {

            fprintf(stderr, "delete_image: 2\n");
            fprintf(stderr, "delete_image: ent->d_name is: %s\n", ent->d_name);
            fprintf(stderr, "delete_image: ent->cache_hit_tail->cache_name is: %s\n", thds.cache_hit_tail->cache_name);

            if (!strncmp(ent->d_name, thds.cache_hit_tail->cache_name,
                         strlen(thds.cache_hit_tail->cache_name))) {

                fprintf(stderr, "data_to_send: 3\n");

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
        fprintf(stderr, "delete_image: Error in closedir\n");

        fprintf(stderr, "data_to_send: 4\n");

        free(img_to_send);
        free_time_http(char_time, http_response);

        fprintf(stderr, "delete_image: 5\n");

        unlock(thds.mtx_cache_access);
        return -1;
    }

    fprintf(stderr, "delete_image: END\n");

    return 0;
}

int insert_in_cache(char *path, int quality_factor, char *name_cached_img, struct image *i, char *char_time, char *http_response) {

    fprintf(stderr, "insert_in_cache\n");

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
        fprintf(stderr, "insert_in_cache: Invalid path\n");
        free_time_http(char_time, http_response);
        unlock(thds.mtx_cache_access);
        return -1;
    } else if (!S_ISREG(buf.st_mode)) {
        fprintf(stderr, "Non-regular files can not be analysed!\n");
        free_time_http(char_time, http_response);
        unlock(thds.mtx_cache_access);
        return -1;
    }

    //fprintf(stderr, "insert_in_cache: 1\n");

    struct cache *new_entry = malloc(sizeof(struct cache));

    /*new_entry -> img_q = malloc(sizeof(name_cached_img));
    if (!new_entry -> img_q)
        fprintf(stderr, "insert_in_cache: error in malloc\n");*/

    //strcpy(new_entry -> img_q, "ciao");
    //fprintf(stderr, "%s\n", new_entry -> img_q);

    //fprintf(stderr, "insert_in_cache: 2\n");

    memset(new_entry, (int) '\0', sizeof(struct cache));
    if (!new_entry) {
        fprintf(stderr, "insert_in_cache: Error in malloc\n");
        free_time_http(char_time, http_response);
        unlock(thds.mtx_cache_access);
        return -1;
    }

    //fprintf(stderr, "insert_in_cache: 3\n");

    /*
     * filling struct cache of the relative image
     * and inserting the struct cache_hit in the cache list
     */
    new_entry->q = quality_factor;

    //fprintf(stderr, "insert_in_cache: 4\n");

    strcpy(new_entry->img_q, name_cached_img);

    fprintf(stderr, "insert_in_cache: name_cached_img is: %s\n", name_cached_img);
    fprintf(stderr, "insert_in_cache: new_entry->img_q is: %s\n", new_entry -> img_q);

    //fprintf(stderr, "insert_in_cache: 5\n");

    new_entry->size_q = (size_t) buf.st_size;

    fprintf(stderr, "insert_in_cache: size_q is: %d\n", new_entry -> size_q);
    //fprintf(stderr, "insert in cache: i->img_c is: %s\n", i->img_c->img_q);
    fprintf(stderr, "insert_in_cache: new_entry is: %s\n", new_entry->img_q);

    //fprintf(stderr, "insert_in_cache: 6\n");

    new_entry->next_img_c = i->img_c;

    fprintf(stderr, "insert_in_cache: 7\n");

    i->img_c = new_entry;

    //fprintf(stderr, "insert_in_cache: 8\n");

    //c = i->img_c;

    fprintf(stderr, "insert_in_cache: 9\n");

    if (CACHE_N != -1) {

        struct cache_hit *new_hit = malloc(sizeof(struct cache_hit));
        memset(new_hit, (int) '\0', sizeof(struct cache_hit));
        if (!new_hit) {
            fprintf(stderr, "insert_in_cache: Error in malloc\n");
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

        fprintf(stderr, "free_cache_slot: 10\n");

        struct cache_hit *prova = malloc(sizeof(struct cache_hit));
        memset(prova, (int) '\0', sizeof(struct cache_hit));
        if (!prova) {
            fprintf(stderr, "insert_in_cache: Error in malloc\n");
            free_time_http(char_time, http_response);
            unlock(thds.mtx_cache_access);
            return -1;
        }

        fprintf(stderr, "free_cache_slot: 11\n");

        prova = thds.cache_hit_tail;
        while (prova) {
            fprintf(stderr, "insert_in_cache: thds.cache_hit_tail->cache_name is: %s\n", prova->cache_name);
            prova = prova->next_hit;
        }

    }

    fprintf(stderr, "insert_in_cache: END\n");

    return 0;
}