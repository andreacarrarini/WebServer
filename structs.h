//
// Created by andrea on 23/11/17.
//

#ifndef WEBSERVER_STRUCTS_H
#define WEBSERVER_STRUCTS_H

#include <netinet/in.h>

#define DIM 512
#define DIM2 64

// Struct to manage cache_struct hit
/*
 * lista ordinata dei filename di tutte le immagini cachate
 * (vera e propria cache_struct)
 */
struct cached_name_list_element {
    char cache_name[DIM / 2];
    struct cached_name_list_element *next_cached_image_name;
};

/*
 * lista collegata di versioni della stessa immagine
 * cachate con fattori di qualita' diversi
 */
struct cache_struct {
    // Quality factor ///OPPURE float...
    /*char q[5];*/
    int quality;
    // Name of cached image_struct format name_quality
    //char *cached_name;
    char cached_name[DIM / 2];
    size_t cached_image_size;
    struct cache_struct *next_cached_image;
    /*
      se si vuole impl. anche cache_struct su disco
      si deve agiungere il campo "char *img_d"
      che rappresenta il nome del file salvato
      su disco (se esiste)
    */
};

struct image_struct {
    // Name of current image_struct
    char name[DIM2 * 2];
    size_t resized_image_size;
    //pointer to cached image_struct
    struct cache_struct *cached_image;
    struct image_struct *next_image;
};

// Struct which contains all variables for synchronise threads
struct threads_sync_struct {
    struct sockaddr_in client_address;
    struct cached_name_list_element *cached_name_list_tail,
            *cached_name_list_head;
    //array of connsocket descriptor
    int *client_socket_list;
    volatile int client_socket_element,
            connections,
    // thread vivi
            active_threads,
    //thread treshold (soglia minima)
            min_active_threads_treshold,
            threads_to_kill;
    // To manage thread's number and connections
    pthread_mutex_t *mtx_thread_conn_number;
    // To manage cache_struct access
    pthread_mutex_t *mtx_cache_access;
    // To sync pthread_condition variables
    pthread_mutex_t *mtx_sync_conditions;
    // Array containing condition
    // variables of all threads
    pthread_cond_t *threads_cond_list;
    // To initialize threads
    pthread_cond_t *th_start;
    // Number of maximum connection reached
    pthread_cond_t *server_full;
};

#endif //WEBSERVER_STRUCTS_H