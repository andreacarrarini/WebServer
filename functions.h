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
extern char *HTML_PAGES[3];
extern int PORT;
extern int MIN_THREAD_TRESHOLD;
extern int MAX_CONNECTION;
extern int LISTEN_SOCKET_DESCRIPTOR;
extern volatile int CACHE_COUNTER;
//extern char images_path[DIM / 2];
extern char images_path[DIM];
extern char resized_tmp_dir[DIM2];
extern char cache_tmp_dir[DIM2];
extern char *usage_str;
extern char *user_command;
extern char *http_response;
extern char *char_time;

extern struct image_struct *image_struct;
extern struct threads_sync_struct thread_struct;

char *search_file_resized(struct image_struct *image, char *name_resized_image, char *img_to_send, struct cache_struct *cache_ptr, char *char_time, char *http_response);

int resize_image(char *IMG_PATH, char *image_name, int quality, char *tmp_cache, char *name_cached_image);

void error_found(char *s);

char *search_file(struct image_struct *image, char *name_cached_img, char *img_to_send, struct cache_struct *cache_ptr, char *char_time, char *http_response);

int free_cache_slot(struct image_struct *image, char *char_time, char *HTTP_response);

int delete_image(char *image_to_send, char *char_time, char *HTTP_response);

int insert_in_cache(char *path, int quality_factor, char *name_cached_img, struct image_struct *image, char *char_time, char *HTTP_repsonse);

void look_for_cached_img(int CACHE_COUNTER, char *name_cached_image);

void write_on_stream(char *string, FILE *file);

char *get_image(char *image_name, size_t image_dim, char *directory);

void initialize_thread(int threads_to_create, void *(*routine)(void *), void *arg);

void spawn_thread(struct threads_sync_struct *threads_sync_struct);

void *manage_connection(void *arg);

void remove_link(char *path);

void remove_directory(char *directory);

void free_memory(void);

void error_found(char *s);

void catch_signal(void);

void lock(pthread_mutex_t *mutex);

void unlock(pthread_mutex_t *mutex);

void wait_t(pthread_cond_t *cond, pthread_mutex_t *mutex);

void signal_t(pthread_cond_t *cond);

FILE *open_LOG_file(const char *path_to_LOG_dir);

void build_error_pages(char **HTML);

void get_file_info(struct stat *stat_buf, char *path, int check);

char *get_time(void);

void write_log(char *log_message);

ssize_t send_HTTP_message(int socket_fd, char *s, ssize_t msg_dim);

char *get_image(char *image_name, size_t image_dim, char *directory);

void free_time_HTTP_response(char *time, char *HTTP_response);

void split_HTTP_message(char *HTTP_request_buffer, char **line_request);

void create_thread(void *(*routine)(void *), void *arg);

void user_usage(const char *c);

void build_img_struct(struct image_struct **img, char *path);

void get_command_line_options(int argc, char **argv, char **path);

void check_and_build(char *image_name, char **html, size_t *dim);

void check_WebServer_images(int perc);

void kill_thread(struct threads_sync_struct *threads_sync_struct);

void *main_thread_work(void *arg);

void *manage_connection(void *arg);

void respond(int socket_fd, struct sockaddr_in client_address);

int manage_response(int socket_fd, char **HTTP_message_fields);

int get_quality(char *HTTP_header_access_field);

ssize_t send_HTTP_message(int socket_fd, char *msg_to_send, ssize_t msg_dim);

void split_HTTP_message(char *HTTP_request_buffer, char **line_request);

void *catch_user_command(void *arg);

void free_memory();

void init(int argc, char **argv, pthread_mutex_t *mtx_sync_conditions, pthread_mutex_t *mtx_cache_access,
          pthread_mutex_t *mtx_thread_conn_number, pthread_cond_t *th_start, pthread_cond_t *full,
          struct threads_sync_struct *d);

void check_WebServer_images(int perc);

void build_img_struct(struct image_struct **img, char *path);

void check_and_build(char *image_name, char **html, size_t *dim);

void start_WebServer(void);

void get_command_line_options(int argc, char **argv, char **path);

void user_usage(const char *c);

char *get_time(void);

void write_on_stream(char *string, FILE *file);



#endif //WEBSERVER_FUNCTIONS_H