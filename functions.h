//
// Created by andrea on 23/11/17.
//

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <memory.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <signal.h>
#include <arpa/inet.h>
#include <ImageMagick-7/MagickWand/MagickWand.h>
#include <time.h>
/*#include <bits/sigaction.h>*/

#include "structs.h"
#include "functions.h"
#include "structs.h"



extern FILE *LOG;
extern char *HTML[3];
extern int PORT;
extern int MINTH;
extern int MAXCONN;
extern int LISTENsd;
extern volatile int CACHE_N;
//extern char IMG_PATH[DIM / 2];
extern char IMG_PATH[DIM];
extern char tmp_resized[DIM2];
extern char tmp_cache[DIM2];
extern char *usage_str;
extern char *user_command;
extern char *http_response;
extern char *char_time;

extern struct image *img;
extern struct th_sync thds;

int resize_image(char *IMG_PATH, char *p_name, int quality, char *tmp_cache, char *name_cached_image);

void error_found(char *s);

char *search_file(struct image *image, char *name_cached_img, char *img_to_send, struct cache *cache_ptr, char *char_time, char *http_response);

int free_cache_slot(struct cache *c, struct image *i, char *char_time, char *http_response);

int delete_image(char *img_to_send, char *char_time, char *http_response);

int insert_in_cache(char *path, int quality_factor, char *name_cached_img, struct image *i, char *char_time, char *http_response);

void look_for_cached_img(int CACHE, char *name_cached_image);

void write_on_stream(char *s, FILE *file);

void exit_on_error(char *s);

void rm_link(char *path);

void rm_dir(char *directory);

void free_mem(void);

void error_found(char *s);

void catch_signal(void);

void lock(pthread_mutex_t *m);

void unlock(pthread_mutex_t *m);

void wait_t(pthread_cond_t *c, pthread_mutex_t *m);

void signal_t(pthread_cond_t *c);

FILE *open_file(const char *path);

void map_html_error(char *HTML[3]);

void get_info(struct stat *buf, char *path, int check);

char *get_time(void);

void write_log(char *s);

ssize_t send_http_msg(int sd, char *s, ssize_t dim);

char *get_img(char *name, size_t img_dim, char *directory);

void free_time_http(char *time, char *http);

void split_str(char *s, char **d);

void create_th(void * (*routine) (void *), void *k);

void usage(const char *p);

void alloc_r_img(struct image **h, char *path);

void get_opt(int argc, char **argv, char **path, int *perc);

void check_and_build(char *s, char **html, size_t *dim);

void check_images(int perc);

void kill_th(struct th_sync *k);

/*
 * commento di prova
 */

#endif //WEBSERVER_FUNCTIONS_H