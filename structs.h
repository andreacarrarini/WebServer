//
// Created by andrea on 23/11/17.
//

#ifndef WEBSERVER_STRUCTS_H
#define WEBSERVER_STRUCTS_H

#include <netinet/in.h>

#define DIM 512
#define DIM2 64

// Struct to manage cache hit
/*
 * lista ordinata dei filename di tutte le immagini cachate
 * (vera e propria cache)
 */
struct cache_hit {
    char cache_name[DIM / 2];
    struct cache_hit *next_hit;
};

/*
 * lista collegata di versioni della stessa immagine
 * cachate con fattori di qualita' diversi
 */
struct cache {
    // Quality factor ///OPPURE float...
    /*char q[5];*/
    int q;
    // Name of cached image format name_quality
    char *img_q;
    size_t size_q;
    struct cache *next_img_c;
    /** se si vuole impl. anche cache su disco
          si deve agiungere il campo "char *img_d"
          che rappresenta il nome del file salvato
          su disco (se esiste)
    */
};

struct image {
    // Name of current image
    char name[DIM2 * 2];
    size_t size_r;
    //pointer to cached image
    struct cache *img_c;
    struct image *next_img;
};

// Struct which contains all variables for synchronise threads
struct th_sync {
    struct sockaddr_in client_addr;
    struct cache_hit *cache_hit_tail,
            *cache_hit_head;
    int *clients;
    volatile int slot_c,
            connections,
            // thread vivi
            th_act,
            //thread treshold (soglia minima)
            th_act_thr,
            to_kill;
    // To manage thread's number and connections
    pthread_mutex_t *mtx_thread_conn_number;
    // To manage cache access
    pthread_mutex_t *mtx_cache_access;
    // To sync pthread_condition variables
    pthread_mutex_t *mtx_sync_conditions;
    // Array containing condition
    // variables of all threads
    pthread_cond_t *threads_cond_list;
    // To initialize threads
    pthread_cond_t *th_start;
    // Number of maximum connection reached
    pthread_cond_t *full;
};

#endif //WEBSERVER_STRUCTS_H
