#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <memory.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <dirent.h>
#include <signal.h>

#define DIM 512
#define DIM2 64

// Log's file pointer
FILE *LOG = NULL;
// Pointer to html files; 1st: root, 2nd: 404, 3rd 400.
char *HTML[3];
int PORT = 8080;
//MINimum THreads
int MINTH = 250;
int MAXCONN = 1500;
int LISTENsd;
char IMG_PATH[DIM / 2];


struct cache {
    // Quality factor ///OPPURE float...
    char q[5];
    // Memory mapped of cached image
    char *img_q;
    size_t size_q;
    struct cache *next_img_c;
    /** se si vuole impl. anche cache su disco
          si deve agiungere il campo "char *img_d"
          che rappresenta il nome del file salvato
          su disco (se esiste)
    */
} *old;

struct image {
    // Name of current image
    char name[DIM2 * 2];
    // Memory mapped of resized image
    char *img_r;
    size_t size_r;
    struct cache *img_c;
    struct image *next_img;
} *img;

struct th_sync {
    //clients array
    int *clients;
    volatile int slot_c,
            connections,
            th_act,
            th_act_thr;
    // To manage thread's number and connections
    pthread_mutex_t *mtx_t;
    // To manage cache access
    pthread_mutex_t *mtx_c;
    // To sync pthread_condition variables
    pthread_mutex_t *mtx_s_c;
    // New event: end connection
    pthread_cond_t *term;
    // Array containing condition variables of all threads
    pthread_cond_t *new_c;
    // To initialize threads
    pthread_cond_t *th_start;
    // Number of maximum connection reached
    pthread_cond_t *full;
} thds;

/*int main(int argc, char *argv[]) {

    typedef enum { false, true } bool;      //definisco il tipo bool

    bool thread_index[2];

    thread_index[0] = true;
    thread_index[1] = false;

    int a,b;
    if (thread_index[0] == true)
        printf("thread index 0 e' true\n");
    if (thread_index[1] == false)
        printf("thread index 1 e' false\n");
    return 0;



}*/

/*void spawn_child(void) {
    int count = 0;
    pid_t pid;
    while (count < 5) {
        if ((pid = fork()) != 0)
            if (pid == -1)
                perror("error in fork\n");
            if (pid > 0)    //processo padre
                continue;
        else child_job();   //processi figli
        count++;
    }
    return;
}

struct thread_data {
    pthread_t tid;
    int listensd, connsd;
    struct sockaddr_in *servaddr, *cliaddr;
    int len;
    int free;       //1 se occupato a servire una richiesta, 0 altrimenti
};


void *worker1_job(void *arg) {

}

void *worker2_job(void *arg) {

}

void child_job(void* arg) {

    #define SERV_PORT 5193
    #define BACKLOG 10
    #define MAXLINE 1024

    int listensd, connsd;
    struct sockaddr_in servaddr;
    struct sockaddr_in cliaddr;

    struct thread_data td[2];   //una struct per thread

    td[0].free = 0;
    td[1].free = 0;

    pthread_t worker1;
    pthread_t worker2;

    struct thread_data *td = (struct thread_data *) arg;

    *//*
    int listensd = td->listensd;
    int connsd = td->connsd;
    int len = td->len;

    struct sockaddr_in servaddr = *td->servaddr;
    struct sockaddr_in cliaddr = *td->cliaddr;
    *//*

    if ((listensd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {     //creo la socket
        perror("errore in socket\n");
        exit(EXIT_FAILURE);
    }

    memset((char *) &servaddr, 0, sizeof(servaddr));     //azzero l'area di memoria di servaddr

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);       *//* il server accetta richieste su ogni interfaccia di rete *//*
    servaddr.sin_port = htons(SERV_PORT);

    if ((bind(listensd, (struct sockaddr *) &servaddr, sizeof(servaddr))) < 0) {     *//*"assigning a name to a socket",
 * prima di bind la socket esiste ma non ha assegnato alcun indirizzo *//*
        perror("errore in bind\n");
        exit(EXIT_FAILURE);
    }

    if (listen(listensd, BACKLOG) < 0) {       *//*marco la socket identificata dal fd come passiva*//*
        perror("errore in listen\n");
        exit(EXIT_FAILURE);
    }

    if (fcntl(listensd, F_SETFL, O_NONBLOCK) !=
        0) {        *//*rendo la socket non bloccante quando in seguito uso accept*//*
        perror("error making non-blocking socket\n");
        exit(EXIT_FAILURE);
    }

    for (;;) {

        int len = sizeof(cliaddr);
        socklen_t scklen = (socklen_t) len;

        if (td[0].free == 0) {

            if (connsd == accept(listensd, (struct sockaddr *) &cliaddr, &scklen) < 0) {     *//*estrae la prima richiesta pendente
 * dalla coda di richieste della socket di ascolto, crea una socket di connessione, e ritorna un FD che la identifica,
 * la socket di ascolto non viene toccata *//*
                perror("errore in accept\n");
            }

            td[0].listensd = listensd;
            td[0].connsd = connsd;
            td[0].servaddr = &servaddr;
            td[0].cliaddr = &cliaddr;
            td[0].free = 1;

            if (pthread_create(&worker1, NULL, worker1_job, &td[0]) != 0) {
                perror("error in pthread_create\n");
                exit(EXIT_FAILURE);
            }
        } else if (td[1].free == 0) {

            if (connsd == accept(listensd, (struct sockaddr *) &cliaddr, &scklen) < 0) {     *//*estrae la prima richiesta pendente
 * dalla coda di richieste della socket di ascolto, crea una socket di connessione, e ritorna un FD che la identifica,
 * la socket di ascolto non viene toccata *//*
                perror("errore in accept\n");
            }

            td[1].listensd = listensd;
            td[1].connsd = connsd;
            td[1].servaddr = &servaddr;
            td[1].cliaddr = &cliaddr;
            td[1].free = 1;

            if (pthread_create(&worker2, NULL, worker2_job, &td[1]) != 0) {
                perror("error in pthread_create\n");
                exit(EXIT_FAILURE);
            }

        else continue;  //errore: se sono occupati entrambi che faccio?????
        }
    }


    return;
}*/



void write_on_stream(char *s, FILE *file) {
    size_t el;
    size_t len = strlen(s);

    while ((el = fwrite(s, sizeof(char), len, file)) < len) {
        if (ferror(file)) {
            fprintf(stderr, "Error in fwrite\n");
            exit(EXIT_FAILURE);
        }
        len -= el;
        s += el;
    }

    if (fflush(file)) {
        fprintf(stderr, "Error in fflush\n");
        exit(EXIT_FAILURE);
    }
}

char *get_time(void) {
    time_t now = time(NULL);
    char *k = malloc(sizeof(char) * DIM2);
    if (!k)
        error_found("Error in malloc\n");
    strcpy(k, ctime(&now));
    if (!k)
        error_found("Error in ctime\n");
    if (k[strlen(k) - 1] == '\n')
        k[strlen(k) - 1] = '\0';
    return k;
}

void write_log(char *s) {
    char *t = get_time();
    write_on_stream(t, LOG);
    write_on_stream(s, LOG);
    free(t);
}

FILE *open_file(const char *path) {
    errno = 0;
    char s[strlen(path) + 4];
    sprintf(s, "%sLOG", path);
    if (s[strlen(s)] != '\0')
        s[strlen(s)] = '\0';
    FILE *f = fopen(s, "a");
    if (!f) {
        if (errno == EACCES)
            error_found("Missing permission\n");
        error_found("Error in fopen\n");
    }

    return f;
}

void error_found(char *s) {
    fprintf(stderr, "%s", s);
    if (LOG)
        write_on_stream(s, LOG);
    exit(EXIT_FAILURE);
}

void usage(const char *p) {
    fprintf(stderr, "Usage: %s [-l log's file path]\n"
            "\t\t\t[-p port]\n"
            "\t\t\t[-i image's path]\n"
            "\t\t\t[-t number_of_initial_threads]\n"
            "\t\t\t[-c maximum connection's number]\n"
            "\t\t\t[-h help]\n", p);
    exit(EXIT_SUCCESS);
}

void get_opt(int argc, char **argv, char **path) {
    int i = 1;
    for (; argv[i] != NULL; ++i)
        // option -h is not allowed
        if (strcmp(argv[i], "-h") == 0)
            usage(argv[0]);

    char c, *e;
    struct stat statbuf;
    // Parsing the command line arguments
    // -p := port; -l := directory to store Log files;
    // -i := directory of files to send;
    // -t := minimum number of thread's pool;
    // -c := maximum number of connections.
    while ((c = getopt(argc, argv, "p:l:i:t:c:")) != -1) {
        switch (c) {
            //sets the PORT number form the command line argument
            case 'p':
                if (strlen(optarg) > 5)
                    error_found("Port's number too high\n");

                errno = 0;
                int p_arg = (int) strtol(optarg, &e, 10);
                if (errno != 0 || *e != '\0')
                    error_found("Error in strtol: Invalid number port\n");
                if (p_arg > 65535)
                    error_found("Port's number too high\n");
                PORT = p_arg;
                break;

            //sets the log file path from the command line arguments
            case 'l':
                errno = 0;
                //saves in struct stat all the info of the log file
                //if an option is followed by a colon it requires an additional argument, optarg points to that argument
                if (stat(optarg, &statbuf) != 0) {
                    if (errno == ENAMETOOLONG)
                        error_found("Path too long\n");
                    error_found("Invalid path for log files\n");
                } else if (!S_ISDIR(statbuf.st_mode)) {
                    error_found("The path is not a directory!\n");
                }

                if (optarg[strlen(optarg) - 1] != '/') {
                    strcpy(path[0], optarg);
                    strcat(path[0], "/");
                } else
                    strcpy(path[0], optarg);

                if (path[0][strlen(path[0])] != '\0')
                    path[0][strlen(path[0])] = '\0';
                break;

            //sets the file_to_send directory from the command line argument
            case 'i':
                errno = 0;
                if (stat(optarg, &statbuf) != 0) {
                    if (errno == ENAMETOOLONG)
                        error_found("Path too long\n");
                    error_found("Invalid path\n");
                } else if (!S_ISDIR(statbuf.st_mode)) {
                    error_found("The path is not a directory!\n");
                }

                if (optarg[strlen(optarg) - 1] != '/') {
                    strcpy(path[1], optarg);
                    strcat(path[1], "/");
                } else
                    strcpy(path[1], optarg);

                if (path[1][strlen(path[1])] != '\0')
                    path[1][strlen(path[1])] = '\0';
                break;

            //sets the minimum threads in thread's pool
            case 't':
                errno = 0;
                int t_arg = (int) strtol(optarg, &e, 10);
                if (errno != 0 || *e != '\0')
                    error_found("Error in strtol: Invalid number\n");
                if (t_arg < 1)
                    error_found("Error: thread's number must be > 0!\n");
                if (t_arg < 2)
                    error_found("Attention: due to performance problem, thread's numbers must be >= 2!\n");
                MINTH = t_arg;
                break;

            //sets the number of maximum connections at the same time
            case 'c':
                errno = 0;
                int c_arg = (int) strtol(optarg, &e, 10);
                if (errno != 0 || *e != '\0')
                    error_found("Error in strtol: Invalid number\n");
                if (c_arg < 1)
                    error_found("Error: maximum connections' number must be > 0!");
                MAXCONN = c_arg;
                break;

            case '?':
                error_found("Invalid argument\n");
        }
    }
    if (MINTH > MAXCONN)
        error_found("Error: number of maximum connections is lower then minimum number of the threads!\n");
}

// Start server
void startServer(void) {
    struct sockaddr_in server_addr;

    if ((LISTENsd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        error_found("Error in socket\n");

    //sets the socket with all 0s
    memset((void *) &server_addr, 0, sizeof(server_addr));
    //IPv4
    (server_addr).sin_family = AF_INET;
    //addresses must be in Network Byte Order
    (server_addr).sin_addr.s_addr = htonl(INADDR_ANY);  // All available interface
    (server_addr).sin_port = htons((unsigned short) PORT);

    /*// To reuse a socket  //TODO watch this
    int flag = 1;
    if (setsockopt(LISTENsd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) != 0)
        error_found("Error in setsockopt\n");*/

    errno = 0;
    if (bind(LISTENsd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        switch (errno) {
            //adress is protected and the user is not superuser
            case EACCES:
                error_found("Choose another socket\n");

            //the given address is already in use
            case EADDRINUSE:
                // NON PIU NECESSARIO GRAZIE A SO_REUSEADDR (vedi sopra)
                error_found("Address in use\n");

            //the socket is already bound to an address
            case EINVAL:
                error_found("The socket is already bound to an address\n");

            default:
                error_found("Error in bind\n");
        }
    }

    // listen for incoming connections
    if (listen(LISTENsd, MAXCONN) != 0)
        error_found("Error in listen\n");
}

void *map_file(char *path, off_t *size) {
    struct stat statbuf;
    int fd;
    char *map;

    memset(&statbuf, 0, sizeof(struct stat));
    if (stat(path, &statbuf) != 0) {
        if (errno == ENAMETOOLONG)
            error_found("Path too long\n");
        error_found("Invalid path\n");
    } else if (!S_ISREG(statbuf.st_mode))
        error_found("Non-regular files can not be mapped!\n");

    fd = open(path, O_RDONLY);
    if (fd == -1)
        error_found("Error opening html file\n");

    *size = statbuf.st_size;
    map = mmap(NULL, *size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (map == (void *) -1)
        error_found("Error in mmap\n");

    return map;
}

void check_and_build(char *s, char **html, int *dim) {
    char *k = "<b>%s</b><br><br><a href=\"%s\"><img src=\"RESIZED\\%s\" height=\"130\" weight=\"100\"></a><br><br><br><br>";;

    size_t len = strlen(*html);
    if (len + DIM >= *dim * DIM) {
        ++*dim;
        *html = realloc(*html, *dim * DIM);
        if (!*html)
            error_found("Error in realloc\n");
    }

    char *q = *html + len;
    sprintf(q, k, s, s, s);
}

void alloc_r_img(struct image **h, char *name) {
    struct image *k = malloc(sizeof(struct image));
    if (!k)
        error_found("Error in malloc\n");

    strcpy(k -> name, name);
    k -> img_c = NULL;

    char p[DIM / 2];
    sprintf(p, "%s%s", IMG_PATH, name);
    k -> img_r = map_file(p, &k -> size_r);

    if (!*h) {
        k -> next_img = *h;
        *h = k;
    } else {
        k -> next_img = (*h) -> next_img;
        (*h) -> next_img = k;
    }
}

void check_images(void) {
    DIR *dir;
    struct dirent *ent;
    char *k;

    //opens the target directory, returns a ptr to the directory stream
    errno = 0;
    dir = opendir(IMG_PATH);
    if (!dir) {
        if (errno == EACCES)
            error_found("Permission denied\n");
        error_found("Error in opendir\n");
    }

    int dim = 4;
    char *html = malloc(dim * DIM);
    if (!html)
        error_found("Error in malloc\n");

    // writes a string that will be the html home page
    // %s page's title; %s header; %s text.
    char *h = "<!DOCTYPE html><html><head><meta charset=\"utf-8\" /><title>%s</title><style type=\"text/css\"></style><script type=\"text/javascript\"></script></head><body background=\"\"><h1>%s</h1><br><br><h3>%s</h3><hr><br>";
    sprintf(html, h, "Madonna", "Benvenuto", "Seleziona un'immagine qui sotto");
    size_t len_h = strlen(html), new_len_h;

    struct image **i = &img;
    //readdir read the sirectory stream created by opendir as a sequence of dirent structs
    while ((ent = readdir(dir)) != NULL) {
        if (ent -> d_type == DT_REG) {
            /*If a file is appended with a tilde~,
             * it only means that it is a backup created by a text editor
             * or similar program*/
            if ((k = strrchr(ent -> d_name, '~')) != NULL) {
                fprintf(stderr, "File '%s' was skipped\n", ent -> d_name);
                continue;
            }

            if ((k = strrchr(ent -> d_name, '.')) != NULL) {
                if (strcmp(k, ".db") == 0) {
                    fprintf(stderr, "File '%s' was skipped\n", ent -> d_name);
                    continue;
                }
                if (strcmp(k, ".gif") != 0 && strcmp(k, ".GIF") != 0 &&
                    strcmp(k, ".jpg") != 0 && strcmp(k, ".JPG") != 0 &&
                    strcmp(k, ".png") != 0 && strcmp(k, ".PNG") != 0)
                    fprintf(stderr, "Warning: file '%s' may have an unsupported format\n", ent -> d_name);
            } else {
                fprintf(stderr, "Warning: file '%s' may have an unsupported format\n", ent -> d_name);
            }

            /** metti la RESIZE QUI */      //TODO resize
            // probabilmente una volta messa la RESIZE
            // non servira il 3o parametro nella alloc_r_img()
            alloc_r_img(i, ent -> d_name);  //TODO i'm here 2
            i = &(*i) -> next_img;
            check_and_build(ent -> d_name, &html, &dim);
        }
    }

    new_len_h = strlen(html);
    if (len_h == new_len_h)
        error_found("There are no images in the specified directory\n");

    h = "</body></html>";
    if (new_len_h + DIM2 / 4 > dim * DIM) {
        ++dim;
        html = realloc(html, dim * DIM);
        if (!html)
            error_found("Error in realloc\n");
    }
    k = html;
    k += strlen(html);
    strcpy(k, h);

    HTML[0] = html;

    if (closedir(dir))
        error_found("Error in closedir\n");
}

void map_html_error(void) {
    char *s = "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\"><html><head><title>%s</title></head><body><h1>%s</h1><p>%s</p></body></html>\0";
    char *mm1 = malloc(strlen(s) + 2 * DIM2);
    char *mm2 = malloc(strlen(s) + 2 * DIM2);
    if (!mm1 || !mm2)
        error_found("Error in malloc\n");
    sprintf(mm1, s, "404 Not Found", "404 Not Found", "The requested URL was not found on this server.");
    sprintf(mm2, s, "400 Bad Request", "Bad Request", "Your browser sent a request that this server could not understand.");
    HTML[1] = mm1;
    HTML[2] = mm2;
}

/*takes 9 arguments: argc, argv, 3 mutex, 3 condition and a pointer toa th_sync struct.
 * */
void init(int argc, char **argv, pthread_mutex_t *m, pthread_mutex_t *m2,
          pthread_mutex_t *m3, pthread_cond_t *c, pthread_cond_t *c2,
          pthread_cond_t *c3, struct th_sync *d) {
    // Default Log's path;
    char LOG_PATH[DIM],
            IMAGES_PATH[DIM];
    strcpy(LOG_PATH, "./");
    strcpy(IMAGES_PATH, "./");
    char *PATH[2];
    PATH[0] = LOG_PATH;
    PATH[1] = IMAGES_PATH;

    //get the manual config options by coomand line arg
    get_opt(argc, argv, PATH);

    //initializes mutexes and conditions
    if (pthread_mutex_init(m, NULL) != 0 ||
        pthread_mutex_init(m2, NULL) != 0 ||
        pthread_mutex_init(m3, NULL) != 0 ||
        pthread_cond_init(c, NULL) != 0 ||
        pthread_cond_init(c2, NULL) != 0 ||
        pthread_cond_init(c3, NULL) != 0)
        error_found("Error in pthread_mutex_init or pthread_cond_init\n");

    //initialize th_sync fields
    d -> connections = d -> slot_c = 0;
    d -> mtx_s_c = m;
    d -> mtx_c = m2;
    d -> mtx_t = m3;
    d -> term = c;
    d -> th_start = c2;
    d -> full = c3;
    d -> th_act_thr = MINTH;
    old = NULL;
    img = NULL;

    d -> clients = malloc(sizeof(int) * MAXCONN);
    d -> new_c = malloc(sizeof(pthread_cond_t) * MAXCONN);
    if (d->clients == NULL || d->new_c == NULL)
    //if (!d -> clients || !d -> new_c)
        error_found("Error in malloc\n");
    // -1 := slot with thread initialized; -2 := empty slot.
    int i;
    for (i = 0; i < MAXCONN; ++i) {
        d -> clients[i] = -2;

        pthread_cond_t cond;
        if (pthread_cond_init(&cond, NULL) != 0)
            error_found("Error in pthread_cond_init\n");
        d -> new_c[i] = cond;
    }

    startServer();
    LOG = open_file(LOG_PATH);
    char start_server[DIM];
    char *k = "\t\tServer started at port:";
    sprintf(start_server, "%s %d\n", k, PORT);
    write_log(start_server);

    strcpy(IMG_PATH, IMAGES_PATH);  //TODO why?
    check_images(); // TODO i'm here 1

    map_html_error();
}

void catch_signal(void) {
    struct sigaction sa;

    sigemptyset(&sa.sa_mask);
    sa.sa_handler = SIG_IGN;
    if (sigaction(SIGPIPE, &sa, NULL) == -1)
        error_found("Error in sigaction\n");
}

void *manage_threads(void *arg) {
    struct th_sync *k = (struct th_sync *) arg;

    create_th(catch_command, arg);
    create_th(kill_th, arg);
    init_th(MINTH, manage_connection, arg);

    int connsocket, i = 0, j;
    socklen_t socksize = sizeof(struct sockaddr_in);
    struct sockaddr_in client_addr;
    // Accept connections
    while (1) {
        lock(k -> mtx_t);
        if (k -> connections + 1 > MAXCONN)
            wait_t(k -> full, k -> mtx_t);
        unlock(k -> mtx_t);

        memset(&client_addr, (int) '\0', socksize);
        errno = 0;
        connsocket = accept(LISTENsd, (struct sockaddr *) &client_addr, &socksize);

        lock(k -> mtx_t);
        if (connsocket == -1) {
            switch (errno) {
                case ECONNABORTED:
                    fprintf(stderr, "The connection has been aborted\n");
                    unlock(k -> mtx_t);
                    continue;

                case ENOBUFS:
                    error_found("Not enough free memory\n");

                case ENOMEM:
                    error_found("Not enough free memory\n");

                case EMFILE:
                    fprintf(stderr, "Too many open files!\n");
                    wait_t(k -> full, k -> mtx_t);
                    unlock(k -> mtx_t);
                    continue;

                case EPROTO:
                    fprintf(stderr, "Protocol error\n");
                    unlock(k -> mtx_t);
                    continue;

                case EPERM:
                    fprintf(stderr, "Firewall rules forbid connection\n");
                    unlock(k -> mtx_t);
                    continue;

                case ETIMEDOUT:
                    fprintf(stderr, "Timeout occured\n");
                    unlock(k -> mtx_t);
                    continue;

                default:
                    error_found("Error in accept\n");
            }
        }

        ++k -> connections;
        //printf("\nNUM CONN: %d\t\tTH_ACT: %d\t\tTH_THR: %d\n\n", k -> connections, k -> th_act, k -> th_act_thr);
        j = 1;
        while (k -> clients[i] != -1) {
            if (j > MAXCONN) {
                j = -1;
                break;
            }
            i = (i + 1) % MAXCONN;
            ++j;
        }
        if (j == -1) {
            unlock(k -> mtx_t);
            continue;
        }
        k -> clients[i] = connsocket;
        signal_t(k -> new_c + i);
        i = (i + 1) % MAXCONN;
        // Threads are created dynamically in need with the number of connections.
        // If the number of connections decreases, the number of active threads
        // 	is reduced in a phased manner so as to cope with a possible peak of connections.
        if (k -> connections >= k -> th_act_thr * 2 / 3 &&
            k -> th_act <= k -> th_act_thr) {
            int n_th;
            if (k -> th_act_thr + MINTH / 2 <= MAXCONN) {
                n_th = MINTH / 2;
                k -> th_act_thr += n_th;
            } else {
                n_th = MAXCONN - k -> th_act_thr;
                k -> th_act_thr += n_th;
            }
            init_th(n_th, manage_connection, k);
        }
        unlock(k -> mtx_t);

        char m[DIM2];
        sprintf(m, "%s  %s\n", "\tClient:", inet_ntoa(client_addr.sin_addr));
        write_log(m);
    }

    pthread_exit(EXIT_SUCCESS);
}

int main(int argc, char **argv) {
    if (argc > 11) {
        fprintf(stderr, "Too many arguments\n\n");
        usage(*argv);
    }

    pthread_mutex_t mtx_s_c, mtx_c, mtx_t;
    pthread_cond_t event, th_start, full;

    init(argc, argv, &mtx_s_c, &mtx_c, &mtx_t,
         &event, &th_start, &full, &thds);

    // To ignore SIGPIPE
    catch_signal();

    manage_threads(&thds);

    return EXIT_SUCCESS;
}

