//
// Created by andrea on 23/11/17.
//

#ifndef WEBSERVER_STRUCTS_H
#define WEBSERVER_STRUCTS_H

#include <netinet/in.h>

#define DIM 512
#define DIM2 64

/*
 * lista ordinata dei filename di tutte le immagini cachate
 * (vera e propria cache_struct)
 */
struct cached_name_element {
    char cache_name[DIM / 2];
    struct cached_name_element *next_cached_image_name;
};

/*
 * lista collegata di versioni della stessa immagine
 * cachate con fattori di qualita' diversi
 */
struct cache_struct {
    // Fattore di qualita'
    int quality;
    // Nome dell'immagine cachata in formato nome_qualita'
    char cached_name[DIM / 2];
    // Dimensione in bytes dell'immagine cachata
    size_t cached_image_size;
    // Puntatore alla successiva
    struct cache_struct *next_cached_image;
};

struct image_struct {
    // Nome dell'immagine originale
    char name[DIM2 * 2];
    // Dimensione dell'immagine resizata (miniatura)
    size_t resized_image_size;
    // Puntatore al primo elemento della lista di versioni cachate
    struct cache_struct *cached_image;
    // Puntatore alla successiva
    struct image_struct *next_image;
};

// Struct che contiene mutex, variabili e conditions per la sincronizzazione dei thread
struct threads_sync_struct {
    // Indirizzo dell'ultimo client connessosi
    struct sockaddr_in client_address;
    // Puntatori alla testa e alla coda della lista ordinata di immagini cachate
    struct cached_name_element *cached_name_tail,
            *cached_name_head;
    // Array di descrittori di socket
    int *client_socket_list;
    // Iteratori
    volatile int client_socket_element,
            connections,
    // Thread vivi
            active_threads,
    // Thread treshold (soglia minima), numero di threads da uccidere
            min_active_threads_treshold,
            threads_to_kill;
    // Per gestire il numero di threads e di connessioni
    pthread_mutex_t *mtx_thread_conn_number;
    // Per sincronizzare l'accesso alla cache_struct
    pthread_mutex_t *mtx_cache_access;
    // Per sincronizzare le conditions
    pthread_mutex_t *mtx_sync_conditions;
    // Array di tuttte le conditions dei threads
    pthread_cond_t *threads_cond_list;
    // Per inizializzare i thread
    pthread_cond_t *th_start;
    // NUmero massimo di connessioni simultanee raggiunto
    pthread_cond_t *server_full;
};

#endif //WEBSERVER_STRUCTS_H