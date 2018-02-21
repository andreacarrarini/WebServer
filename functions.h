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
#include "structs.h"
#include "functions.h"
#include "structs.h"


// Puntatore al file di LOG
extern FILE *LOG;
// Puntatori alle pagine html: 1:root, 2:404, 3:400
extern char *HTML_PAGES[3];
// Numero di porta
extern int PORT;
// Soglia minima di thread nel pool
extern int MIN_THREAD_TRESHOLD;
// Numero massimo di connessioni simultanee consentite
extern int MAX_CONNECTION;
// Descrittore della socket in ascolto
extern int LISTEN_SOCKET_DESCRIPTOR;
// Posti residui in cache
extern volatile int CACHE_COUNTER;
// Path alla directory delle immagini base
extern char images_path[DIM];
// Path alla directory delle immagini resizate per le miniature
extern char resized_tmp_dir[DIM2];
// Path alla directory delle immagini cachate
extern char cache_tmp_dir[DIM2];
// Help che il server stampa se il carattere inserito dall'utente non e' tra i consentiti
extern char *usage_str;
// Carattere che l'utente puo' inserire a runtime
extern char *user_command;
// Puntatore alla risposta HTTP da inviare al client
extern char *http_response;
// Versione stringa del tempo
extern char *char_time;

extern struct image_struct *image_struct;
extern struct threads_sync_struct thread_struct;

/*
 * Crea una versione resizata dell'originale e la scrive sul file system
 * all'indirizzo della directory delle immagini cachate
 */
int resize_image(char *IMG_PATH, char *image_name, int quality, char *tmp_cache, char *name_cached_image);

/*
 * Gestisce gli errori fatali che richiedono lo shutdown del server:
 * aggiorna il LOG, libera le risorse e chiude il server
 */
void error_found(char *s);

/*
 * Cerca l'immagine nel file system, se la ricerca ha esito positivo la restituisce
 */
char *search_file(struct image_struct *image, char *name_cached_img, char *img_to_send, struct cache_struct *cache_ptr, char *char_time, char *http_response);

/*
 * Libera un posto in cache rilasciando l'area di memoria occupata dall'elemento piu' vecchio
 */
int free_cache_slot(struct image_struct *image, char *char_time, char *HTTP_response);

/*
 * Cancella l'immagine scelta dal file system
 */
int delete_image(char *image_to_send, char *char_time, char *HTTP_response);

/*
 * Crea un nuovo elemento di tipo cache_struct,
 * lo riempie delle informzioni necessarie e lo inserisce nella lista collegata,
 * poi crea un cached_name_element e lo inserisce in testa alla lista collegata e ordinata
 */
int insert_in_cache(char *path, int quality_factor, char *name_cached_img, struct image_struct *image, char *char_time, char *HTTP_repsonse);

/*
 * Se un'immagine in cache e' stata acceduta recentemente viene spostata in testa alla lista ordinata
 * per realizzare un approccio al caching di tipo LRU
 */
void look_for_cached_img(int CACHE_COUNTER, char *name_cached_image);

/*
 * Scrive la stringa passata sul file indicato
 */
void write_on_stream(char *string, FILE *file);

/*
 * Legge l'immagine base richiesta dall'utente dal file system e la restituisce sotto forma di buffer di caratteri
 */
char *get_image(char *image_name, size_t image_dim, char *directory);

/*
 * Chiamata all'avvio del server dal main thread, crea e inizializza un numero di thread pari alla soglia minima indicata
 */
void initialize_thread(int threads_to_create, void *(*routine)(void *), void *arg);

/*
 * Crea dinamicamente altri thread se il carico sul server supera una soglia critica
 */
void spawn_thread(struct threads_sync_struct *threads_sync_struct);

/*
 * Routine di ogni thread worker: va in wait sulla sua condition,
 * quando viene svegliato dal main thread crea un altro thread worker che prende il suo posto
 * mentre lui gestisce personalemte la connessione col client
 */
void *manage_connection(void *arg);

/*
 * Rimuove gli hard link dal file system, quando l'u;timo e'rimosso il file e' cancellato
 */
void remove_link(char *path);

/*
 * Cancella i file contenuti nella directory e infine cancella la directory stessa
 */
void remove_directory(char *directory);

/*
 * Libera le aree di memoria allocate all'avvio del server, cancella le directory temporanee di resize e cache
 */
void free_memory(void);

/*
 * Gestisce gli errori fatali aggiornando il log, rilasciando le risorse e chiudendo il server
 */
void error_found(char *s);

/*
 * Serve per ignorare il SIGPIPE mandato dalla send_http_message
 */
void catch_signal(void);

/*
 * Per prendere un lock
 */
void lock(pthread_mutex_t *mutex);

/*
 * Per rilasciare un lock
 */
void unlock(pthread_mutex_t *mutex);

/*
 * Per andare in wait sula condition passata
 */
void wait_t(pthread_cond_t *cond, pthread_mutex_t *mutex);

/*
 * Per svegliare il thread che e' in wait sulla specifica condition
 */
void signal_t(pthread_cond_t *cond);

/*
 * Per aprire il file di LOG
 */
FILE *open_LOG_file(const char *path_to_LOG_dir);

/*
 * Costruiisce le pagine di errore HTTP (404 Not Found e 400 Bad Request)
 */
void build_error_pages(char **HTML);

/*
 * Salva in una struttura di tipo stat le informazioni fondamentali del file indicato
 */
void get_file_info(struct stat *stat_buf, char *path, int check);

/*
 * Ottiene la data e l'ora corrente e la converte in una stringa della forma:
 * "Wed Jun 30 21:49:08 1993\n", completa di terminatore di stringa.
 */
char *get_time(void);

/*
 * Scrive sul LOG la stringa passata come argomento
 */
void write_log(char *log_message);

/*
 * Legge l'immagine dal file system e la restituisce come buffer di caratteri
 */
char *get_image(char *image_name, size_t image_dim, char *directory);

/*
 * Libera i buffer della data e della risposta HTTP
 */
void free_time_HTTP_response(char *time, char *HTTP_response);

/*
 * Parsa la richiesta HTTP inviata dal client e la splitta in 7 campi:
 *  HTTP msg type
 *  Reqested obj
 *  HTTP vers
 *  Conn
 *  User_Agent
 *  Accept type
 *  Cache-Control
 */
void split_HTTP_message(char *HTTP_request_buffer, char **line_request);

/*
 * Crea il thread passandogli la routine da eseguire
 */
void create_thread(void *(*routine)(void *), void *arg);

/*
 * Stampa a schermo un breve manuale di utilizzo lato server per l'utente
 */
void user_usage(const char *c);

/*
 * Crea e inizializza le image_struct
 */
void build_img_struct(struct image_struct **img, char *path);

/*
 * Riconosce i parametri passati dall'utente all'avvio del server e personalizza il server secondo le sue richieste
 */
void get_command_line_options(int argc, char **argv, char **path);

/*
 * Costruisce la pagina iniziale del server (index) con le immagini disponibili
 */
void check_WebServer_images(int perc);

/*
 * Riduce il pool di thread quando il carico sul server diminuisce
 */
void kill_thread(struct threads_sync_struct *threads_sync_struct);

/*
 * Routine del main thread: inizializza i thread worker, entra in un ciclo infinito,
 * va in accept (bloccante), quando riceve una richiesta da un client sveglia (signal)
 * il thread worker corrispondente e torna in accept
 */
void *main_thread_work(void *arg);

/*
 * Ogni thread worker esegue questa funzione per gestire la connessione:
 * esegue la recv() e processa la richiesta
 */
void respond(int socket_fd, struct sockaddr_in client_address);

/*
 * Processa effettivamente la richiesta:
 * cerca l'immagine richiesta in cache, se non c'e' effettua il resize e la inserisce in cache distinguendo i casi
 * in cui la cache e' limitata (libera o piena) e illimitata.
 * Completa il messaggio di risposta HTTP con l'oggetto richiesto
 */
int manage_response(int socket_fd, char **HTTP_message_fields);

/*
 * Estrae il fattore di qualita' dall'accept header.
 * Restituisce:
 * -1 --> errore
 * -2 --> fattore di qualita' non specificato nell'header
 */
int get_quality(char *HTTP_header_access_field);

/*
 * Invia effettivamente il messaggio di risposta HTTP sulla socket specificata
 */
ssize_t send_HTTP_message(int socket_fd, char *msg_to_send, ssize_t msg_dim);

/*
 * Routine del thread che gestisce l'input dell'utente da riga di comndo dopo l'avvio del server:
 *  -s: stampa a schermo lo stato del server (thread attivi e connessioni)
 *  -f: forza la scrittura sul LOG
 *  -q: forza la chiusura del server
 */
void *catch_user_command(void *arg);

/*
 * Inizializza i path delle directory contenenti immagini base e log;
 * inizializza mutex e conditions;
 * riempie la thread_sync_struct
 * apre il file di log e crea le cartelle temporanee
 */
void init(int argc, char **argv, pthread_mutex_t *mtx_sync_conditions, pthread_mutex_t *mtx_cache_access,
          pthread_mutex_t *mtx_thread_conn_number, pthread_cond_t *th_start, pthread_cond_t *full,
          struct threads_sync_struct *d);

/*
 * Completa l'index con le miniature delle immagini
 */
void check_and_build(char *resized_image_path, char *image_name, char **html, size_t *dim);

/*
 * Crea la socket, setta le opzioni, chiama la bind() che assegna il numero di porta,
 * chiama infine la listen() marcando la socket come in ascolto
 */
void start_WebServer(void);

#endif //WEBSERVER_FUNCTIONS_H