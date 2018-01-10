#include <time.h>
#include "structs.h"
#include "functions.h"
//#include <MagickWand>

#define DIM 512
#define DIM2 64

struct image *img;
struct th_sync thds;

//TODO change error msg from type "function: error" to "error in function"

// Log's file pointer
FILE *LOG = NULL;
// Pointer to html files; 1st: root, 2nd: 404, 3rd 400.
char *HTML[3];
int PORT = 8080;
//MINimum THreads
int MINTH = 250;
int MAXCONN = 1500;
int LISTENsd;
//char IMG_PATH[DIM / 2];
char IMG_PATH[DIM];
// Number of cached images
volatile int CACHE_N = -1;
char tmp_resized[DIM2] = "/tmp/RESIZED.XXXXXX";
// tmp files cached
char tmp_cache[DIM2] = "/tmp/CACHE.XXXXXX";

// User's command
char *user_command = "-Enter 'q'/'Q' to close the server, "
        "'s'/'S' to know server's state or "
        "'f'/'F' to force Log file write";

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
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);       */
/* il server accetta richieste su ogni interfaccia di rete *//*
    servaddr.sin_port = htons(SERV_PORT);

    if ((bind(listensd, (struct sockaddr *) &servaddr, sizeof(servaddr))) < 0) {     *//*"assigning a name to a socket",
 * prima di bind la socket esiste ma non ha assegnato alcun indirizzo *//*
        perror("errore in bind\n");
        exit(EXIT_FAILURE);
    }

    if (listen(listensd, BACKLOG) < 0) {       */
/*marco la socket identificata dal fd come passiva*//*
        perror("errore in listen\n");
        exit(EXIT_FAILURE);
    }

    if (fcntl(listensd, F_SETFL, O_NONBLOCK) !=
        0) {        */
/*rendo la socket non bloccante quando in seguito uso accept*/
/*
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


// To close the process on error
void exit_on_error(char *s) {
    fprintf(stderr, "%s\n", s);
    exit(EXIT_FAILURE);
}

char *get_time(void) {
    time_t now = time(NULL);
    char *k = malloc(sizeof(char) * DIM2);
    if (!k)
        error_found("Error in malloc\n");
    /*
     * The call ctime(t) converts the calendar char_time t into a null-terminated string
     * of the form: "Wed Jun 30 21:49:08 1993\n"
     */
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

void usage(const char *p) {
    fprintf(stderr, "Usage: %s [-l log's file path]\n"
            "\t\t\t[-p port]\n"
            "\t\t\t[-i image's path]\n"
            "\t\t\t[-t number_of_initial_threads]\n"
            "\t\t\t[-c maximum connection's number]\n"
            "\t\t\t[-h help]\n", p);
    exit(EXIT_SUCCESS);
}

void get_opt(int argc, char **argv, char **path, int *perc) {
    int i = 1;
    for (; argv[i] != NULL; ++i)
        // option -h is not allowed
        if (strcmp(argv[i], "-h") == 0)
            usage(argv[0]);

    //mod_t means min thr number has been passed from line command,
    //mod_c if max conn has been passed
    int c; char *e; char mod_c = 0, mod_t = 0;
    struct stat statbuf;
    // Parsing the command line arguments
    // -p := port; -l := directory to store Log files;
    // -i := directory of files to send;
    // -t := minimum number of thread's pool;
    // -c := maximum number of connections.
    // -r := percentage of resized images which belong to HTML file
    // -n := maximum cache size
    while ((c = getopt(argc, argv, "p:l:i:t:c:r:n:")) != -1) {
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
                mod_t = 1;
                break;

            //sets the number of maximum connections at the same char_time
            case 'c':
                errno = 0;
                int c_arg = (int) strtol(optarg, &e, 10);
                if (errno != 0 || *e != '\0')
                    error_found("Error in strtol: Invalid number\n");
                if (c_arg < 1)
                    error_found("Error: maximum connections' number must be > 0!");
                MAXCONN = c_arg;
                mod_c = 1;
                break;

            case 'r':
                errno = 0;
                *perc = (int) strtol(optarg, &e, 10);
                if (errno != 0 || *e != '\0')
                    error_found("Argument -r: Error in strtol: Invalid number\n");
                if (*perc < 1 || *perc > 100)
                    error_found("Argument -r: The number must be >=1 and <= 100\n");
                break;

            case 'n':
                errno = 0;
                int cache_size = (int) strtol(optarg, &e, 10);
                if (errno != 0 || *e != '\0')
                    error_found("Argument -n: Error in strtol: Invalid number\n");
                if (cache_size)
                    CACHE_N = cache_size;
                break;

            case '?':
                error_found("Invalid argument\n");

            default:
                error_found("Unknown error in getopt\n");
        }
    }
    if (mod_c && !mod_t && MAXCONN < MINTH)
        MINTH = MAXCONN;
    else if (mod_t && !mod_c && MINTH > MAXCONN)
        MAXCONN = MINTH;
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

    fprintf(stdout, "-Server's socket correctly created with number: %d\n", PORT);
}

void *map_file(char *path, off_t *size) {
    struct stat statbuf;
    int fd;
    char *map;

    memset(&statbuf, 0, sizeof(struct stat));
    //stat func. requires a stat struct ptr
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

void check_and_build(char *s, char **html, size_t *dim) {
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

/*//inserts resized images in cache
void alloc_r_img(struct image **h, char *name) {
    struct image *k = malloc(sizeof(struct image));
    if (!k)
        error_found("Error in malloc\n");

    strcpy(k->name, name);
    //image_c = image_cache
    k->img_c = NULL;

    char p[DIM / 2];
    //p is the path to the image
    sprintf(p, "%s%s", IMG_PATH, name);
    k->img_r = map_file(p, &k->size_r);

    //OLD VERSION

    *//*at the start *h is set to NULL by init(),
    k is the current image to insert,
    h is the head of the list (first element),
    every insert is after the head like 1->i->i-1->i-2->...*//*

    *//*the list implements the cache LRU like*//*
*//*    if (!*h) {
        k -> next_img = *h;
        *h = k;
    } else {
        k -> next_img = (*h) -> next_img;
        (*h) -> next_img = k;
    }
}*//*

    //NEW VERSION

    k->next_img = *h;
    *h = k;
}*/

// Used to get information from a file on the file system
//  check values: 1 for check directory
//                0 for check regular files
void get_info(struct stat *buf, char *path, int check) {
    memset(buf, (int) '\0', sizeof(struct stat));
    /*char mode[] = "0777";
    int permissions;
    permissions = strtol(mode, 0, 8);
    errno = 0;
    if (chmod(path, permissions)) {
        fprintf(stderr, "get_info(chmod): errno is: %s\n", strerror(errno));
        error_found("get_info: failed giving permission to file\n");
    }*/
    errno = 0;
    if (stat(path, buf) != 0) {
        if (errno == ENAMETOOLONG)
            error_found("Path too long\n");
        mode_t bits = buf->st_mode;
        if((bits & S_IRUSR) == 0){
            fprintf(stderr, "User doesn't have read privilages\n");
        }
        fprintf(stderr, "get_info: path is: %s\n", path);
        fprintf(stderr, "get_info: errno is: %s\n", strerror(errno));
        error_found("alloc_r_img: Invalid path\n");
    }
    if (check) {
        if (!S_ISDIR((*buf).st_mode)) {
            error_found("Argument -l: The path is not a directory!\n");
        }
    } else {
        if (!S_ISREG((*buf).st_mode)) {
            error_found("Non-regular files can not be analysed!\n");
        }
    }
}

// Used to fill img dynamic structure
void alloc_r_img(struct image **h, char *path) {

    char new_path[DIM];
    memset(new_path, (int) '\0', DIM);
    struct image *k = malloc(sizeof(struct image));
    if (!k)
        error_found("Error in malloc\n");
    memset(k, (int) '\0', sizeof(struct image));

    char *name = strrchr(path, '/');
    if (!name) {
        if (!strncmp(path, "favicon.ico", 11)) {
            sprintf(new_path, "%s/%s", IMG_PATH, path);
            strcpy(k->name, path);
            path = new_path;
        } else {
            error_found("alloc_r_img: Error analyzing file");
        }
    } else {
        strcpy(k->name, ++name);
    }

    struct stat statbuf;
    //fprintf(stderr, "alloc_r_img: path is: %s\n", path);
    get_info(&statbuf, path, 0);

    k->size_r = (size_t) statbuf.st_size;
    k->img_c = NULL;

    if (!*h) {
        k->next_img = *h;
        *h = k;
    } else {
        k->next_img = (*h)->next_img;
        (*h)->next_img = k;
    }
}

void check_images(int perc) {

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
    char *html = malloc((size_t)dim * DIM);
    if (!html)
        error_found("Error in malloc\n");
    memset(html, (int) '\0', (size_t) dim * DIM * sizeof(char));

    // writes a string that will be the html home page
    // %s page's title; %s header; %s text.
    char *h = "<!DOCTYPE html><html><head><meta charset=\"utf-8\" /><title>%s</title><style type=\"text/css\"></style><script type=\"text/javascript\"></script></head><body background=\"\"><h1>%s</h1><br><br><h3>%s</h3><hr><br>";
    sprintf(html, h, "WebServerProject", "Welcome", "Select an image below");
    /*    // %s image's path; %d resizing percentage
    char *convert = "convert %s -resize %d%% %s;exit";*/
    size_t len_h = strlen(html), new_len_h;

    struct image **i = &img;
    //input is path to source image, output to resized
    char input[DIM], output[DIM];
    memset(input, (int) '\0', DIM); memset(output, (int) '\0', DIM);
    //readdir read the sirectory stream created by opendir as a sequence of dirent structs
    fprintf(stdout, "-Please wait while resizing images...\n");
    while ((ent = readdir(dir)) != NULL) {
        //DT_REG means a regular file
        if (ent -> d_type == DT_REG) {
            /*If a file is appended with a tilde~,
             * it only means that it is a backup created by a text editor
             * or similar program*/
            if ((k = strrchr(ent -> d_name, '~')) != NULL) {
                //tilde is found
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

            if (resize_image(IMG_PATH, ent -> d_name, perc, tmp_resized, ent -> d_name))
                error_found("check_image: Error resizing images\n");

            /*            char command[DIM * 2];
            memset(command, (int) '\0', DIM * 2);
            sprintf(input, "%s/%s", IMG_PATH, ent -> d_name);
            sprintf(output, "%s/%s", tmp_resized, ent -> d_name);
            sprintf(command, convert, input, perc, output);

            if (system(command))
                error_found("check_image: Error resizing images\n");*/

            //in input there in no / because is already in IMG_PATH
            sprintf(input, "%s%s", IMG_PATH, ent -> d_name);
            sprintf(output, "%s/%s", tmp_resized, ent -> d_name);
            alloc_r_img(i, output);
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
        html = realloc(html, (size_t) dim * DIM);
        if (!html)
            error_found("Checking images: Error in realloc\n");
        memset(html + new_len_h, (int) '\0', (size_t) dim * DIM - new_len_h);
    }
    k = html;
    k += strlen(html);
    strcpy(k, h);

    HTML[0] = html;

    if (closedir(dir))
        error_found("Error in closedir\n");

    fprintf(stdout, "-Images correctly resized in: '%s' with percentage: %d%%\n", tmp_resized, perc);
}

// Used to map in memory HTML files which respond with
//  error 400 or error 404
void map_html_error(char *HTML[3]) {
    char *s = "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\"><html><head><title>%s</title></head><body><h1>%s</h1><p>%s</p></body></html>\0";
    size_t len = strlen(s) + 2 * DIM2 * sizeof(char);

    char *mm1 = malloc(len);
    char *mm2 = malloc(len);
    if (!mm1 || !mm2)
        error_found("Error in malloc\n");
    memset(mm1, (int) '\0', len); memset(mm2, (int) '\0', len);
    sprintf(mm1, s, "404 Not Found", "404 Not Found", "The requested URL was not found on this server.");
    sprintf(mm2, s, "400 Bad Request", "Bad Request", "Your browser sent a request that this server could not understand.");
    HTML[1] = mm1;
    HTML[2] = mm2;
}

/*takes 9 arguments: argc, argv, 3 mutex, 3 condition and a pointer toa th_sync struct.
 * */
void init(int argc, char **argv, pthread_mutex_t *m, pthread_mutex_t *m2,
          pthread_mutex_t *m3, pthread_cond_t *c, pthread_cond_t *c2,
          struct th_sync *d) {
    // Default Log's path;
    char LOG_PATH[DIM],
            IMAGES_PATH[DIM];
    //LOG_PATH is the path to log file, IMAGES_PATH the one to images
    //./ is current dir
    //sets the memory with 0s
    memset(LOG_PATH, (int) '\0', DIM);
    memset(IMAGES_PATH, (int) '\0', DIM);
    strcpy(LOG_PATH, ".");
    strcpy(IMAGES_PATH, ".");
    //an array of 2 ptr
    char *PATH[2];
    PATH[0] = LOG_PATH;
    PATH[1] = IMAGES_PATH;
    int perc = 20;

    //get the manual config options by coomand line arg
    get_opt(argc, argv, PATH, &perc);

    //initializes mutexes and conditions
    if (pthread_mutex_init(m, NULL) != 0 ||
        pthread_mutex_init(m2, NULL) != 0 ||
        pthread_mutex_init(m3, NULL) != 0 ||
        pthread_cond_init(c, NULL) != 0 ||
        pthread_cond_init(c2, NULL) != 0)
        error_found("Error in pthread_mutex_init or pthread_cond_init\n");

    //initialize th_sync fields
    d->connections = d->slot_c = d->to_kill = d->th_act = 0;
    d -> mtx_sync_conditions = m;
    d -> mtx_cache_access = m2;
    d -> mtx_thread_conn_number = m3;
    d -> th_start = c2;
    d -> th_act_thr = MINTH;
    img = NULL;

    d -> clients = malloc(sizeof(int) * MAXCONN);
    d -> threads_cond_list = malloc(sizeof(pthread_cond_t) * MAXCONN);
    if (d->clients == NULL || d->threads_cond_list == NULL) {
        //if (!d -> clients || !d -> new_c)
        error_found("Error in malloc\n");
    } else {
        memset(d->clients, (int) '\0', sizeof(int) * MAXCONN);
        memset(d->threads_cond_list, (int) '\0', sizeof(pthread_cond_t) * MAXCONN);
    }
    // -1 := slot with thread initialized; -2 := empty slot.
    int i;
    for (i = 0; i < MAXCONN; ++i) {
        d -> clients[i] = -2;

        pthread_cond_t cond;
        if (pthread_cond_init(&cond, NULL) != 0)
            error_found("Error in pthread_cond_init\n");
        d -> threads_cond_list[i] = cond;
    }

    startServer();
    LOG = open_file(LOG_PATH);
    char start_server[DIM];
    memset(start_server, (int) '\0', DIM);
    char *k = "\t\tServer started at port:";
    sprintf(start_server, "%s %d\n", k, PORT);
    write_log(start_server);

    // Create tmp folder for resized and cached images
    if (!mkdtemp(tmp_resized) || !mkdtemp(tmp_cache))
        error_found("Error in mkdtmp\n");

    errno = 0;

/*    if (chmod(tmp_resized, 777) || chmod(tmp_cache, 777)) {
        //fprintf(stderr, "%d\n", errno);
        error_found("Error giving permission to directory");
    }*/

    if (CACHE_N > 0) {
        fprintf(stdout, "-Cache size: %d images; located in '%s'\n", CACHE_N, tmp_cache);
    } else {
        fprintf(stdout, "-Cache size: Unlimited; located in '%s'\n", tmp_cache);
    }

    strcpy(IMG_PATH, IMAGES_PATH);
    check_images(perc);
    map_html_error(HTML);
}

// Used to remove file from file system
void rm_link(char *path) {
    //removes the name from FS, if it was the last occurrence file is deleted
    if (unlink(path)) {
        errno = 0;
        switch (errno) {
            case EBUSY:
                exit_on_error("File can not be unlinked: It is being use by the system\n");

            case EIO:
                exit_on_error("File can not be unlinked: An I/O error occurred\n");

            case ENAMETOOLONG:
                exit_on_error("File can not be unlinked: Pathname was too long\n");

            case ENOMEM:
                exit_on_error("File can not be unlinked: Insufficient kernel memory was available\n");

            case EPERM:
                exit_on_error("File can not be unlinked: The file system does not allow unlinking of files\n");

            case EROFS:
                exit_on_error("File can not be unlinked: Pathname refers to a file on a read-only file system\n");

            default:
                exit_on_error("File can not be unlinked: Error in unlink\n");
        }
    }
}

// Used to remove directory from file system
void rm_dir(char *directory) {
    DIR *dir;
    struct dirent *ent;

    fprintf(stdout, "-Removing '%s'\n", directory);
    char *verify = strrchr(directory, '/') + 1;
    //NULL if / is not found
    if (!verify)
        exit_on_error("rm_dir: Unexpected error in strrchr\n");
    verify = strrchr(directory, '.') + 1;
    //not a thing we want to remove
    if (!strncmp(verify, "XXXXXX", 7))
        return;

    errno = 0;
    dir = opendir(directory);
    if (!dir) {
        if (errno == EACCES)
            exit_on_error("Permission denied\n");
        exit_on_error("rm_dir: Error in opendir\n");
    }

    while ((ent = readdir(dir)) != NULL) {
        //deletes all files in directory
        if (ent -> d_type == DT_REG) {
            char buf[DIM];
            memset(buf, (int) '\0', DIM);
            sprintf(buf, "%s/%s", directory, ent -> d_name);
            fprintf(stderr, "%s\n", ent ->d_name);
            rm_link(buf);
        }
    }

    if (closedir(dir))
        exit_on_error("Error in closedir\n");

    errno = 0;
    if (rmdir(directory)) {
        switch (errno) {
            case EBUSY:
                exit_on_error("Directory not removed: resource busy\n");

            case ENOMEM:
                exit_on_error("Directory not removed: Insufficient kernel memory\n");

            case EROFS:
                exit_on_error("Directory not removed: Pathname refers to a directory on a read-only file system\n");

            case ENOTEMPTY:
                exit_on_error("Directory not removed: Directory not empty!\n");

            default:
                exit_on_error("Error in rmdir\n");
        }
    }
}

// Used to free memory allocated from malloc/realloc functions
void free_mem() {
    free(HTML[0]);
    free(HTML[1]);
    free(HTML[2]);
    //TODO find malloc of clients and new_c
    free(thds.clients);
    free(thds.threads_cond_list);
    //CACHE_N can't be 0
    if (CACHE_N >= 0 && thds.cache_hit_head && thds.cache_hit_tail) {
        struct cache_hit *to_be_removed;
        //till tail is NULL
        while (thds.cache_hit_tail) {
            to_be_removed = thds.cache_hit_tail;
            thds.cache_hit_tail = thds.cache_hit_tail->next_hit;
            free(to_be_removed);
        }
    }

    rm_dir(tmp_resized);
    rm_dir(tmp_cache);
}

// Thread which control stdin to recognize user's input
void *catch_command(void *arg) {
    struct th_sync *k = (struct th_sync *) arg;

    printf("\n%s\n", user_command);
    while (1) {
        char cmd[2];
        int conn, n_thds;
        memset(cmd, (int) '\0', 2);
        if (fscanf(stdin, "%s", cmd) != 1)
            error_found("Error in fscanf\n");

        if (strlen(cmd) != 1) {
            printf("%s\n", user_command);
        } else {
            if (cmd[0] == 's' || cmd[0] == 'S') {
                lock(thds.mtx_thread_conn_number);
                conn = thds.connections; n_thds = thds.th_act;
                unlock(thds.mtx_thread_conn_number);
                fprintf(stdout, "\nConnections' number: %d\n"
                        "Threads running: %d\n\n", conn, n_thds);
                continue;
            } else if (cmd[0] == 'f' || cmd[0] == 'F') {
                errno = 0;
                if (fflush(LOG)) {
                    if (errno == EBADF)
                        fprintf(stderr, "Error in fflush: Stream is not an open stream, or is not open for writing.\n");
                    fprintf(stderr, "catch_command: Unexpected error in fflush\n");
                }
                fprintf(stdout, "Log file updated\n");
                continue;
            } else if (cmd[0] == 'q' || cmd[0] == 'Q') {
                fprintf(stdout, "-Closing server\n");

                errno = 0;
                // Kernel may still hold some resources for a period (TIME_WAIT)
                if (close(LISTENsd) != 0) {
                    if (errno == EIO)
                        error_found("I/O error occurred\n");
                    error_found("Error in close\n");
                }

                int i = 0;
                for (; i < MAXCONN; ++i) {
                    if (k -> clients[i] >= 0) {
                        //clients are sockets
                        if (close(k -> clients[i]) != 0) {
                            switch (errno) {
                                case EIO:
                                    error_found("I/O error occurred\n");

                                case ENOTCONN:
                                    error_found("The socket is not connected\n");

                                case EBADF:
                                    fprintf(stderr, "Bad file number. Probably client has disconnected\n");
                                    break;

                                default:
                                    error_found("Error in close or shutdown\n");
                            }
                        }
                    }
                }
                write_log("\t\tServer closed.\n\n\n");

                errno = 0;
                if (fflush(LOG)) {
                    if (errno == EBADF)
                        fprintf(stderr, "Error in fflush: Stream is not an open stream, or is not open for writing.\n");
                    exit(EXIT_FAILURE);
                }

                if (fclose(LOG) != 0)
                    error_found("Error in fclose\n");

                free_mem();

                exit(EXIT_SUCCESS);
            }
            printf("%s\n\n", user_command);
        }
    }
}

void create_th(void * (*routine) (void *), void *k) {
    pthread_t tid;
    errno = 0;
    if (pthread_create(&tid, NULL, routine, k) != 0) {
        if (errno == EAGAIN || errno == ENOMEM)
            error_found("Insufficient resources to create another thread\n");
        else
            error_found("Error in pthread_create\n");
    }
}

// Initialize threads
void init_th(int threads_to_create, void *(*routine) (void *), void *arg) {
    struct th_sync *k = (struct th_sync *) arg;

    int i, j;
    lock(k -> mtx_sync_conditions);
    for (i = j = 0; i < threads_to_create && j < MAXCONN; ++j) {
        // -1 := slot with thread initialized; -2 := empty slot.
        if (k -> clients[j] == -2) {
            k -> slot_c = j;
            create_th(routine, arg);

            k -> clients[j] = -3;
            wait_t(k -> th_start, k -> mtx_sync_conditions);
            ++i;
        }
    }
    k -> th_act += threads_to_create;
    unlock(k -> mtx_sync_conditions);
}

/*
 * Used to split HTTP message:
 *  HTTP msg type
 *  Reqested obj
 *  HTTP vers
 *  Conn
 *  User_Agent
 *  Accept type
 *  Cache-Control
 */
void split_str(char *http_req_buf, char **line_req) {
    char *msg_type[4];
    msg_type[0] = "Connection: ";
    msg_type[1] = "User-Agent: ";
    msg_type[2] = "Accept: ";
    msg_type[3] = "Cache-Control: ";
    // HTTP message type
    line_req[0] = strtok(http_req_buf, " ");
    // Requested object
    line_req[1] = strtok(NULL, " ");
    // HTTP version
    line_req[2] = strtok(NULL, "\n");
    //if HTTP version is not NULL
    if (line_req[2]) {
        //replace \r with \0 at line's end
        if (line_req[2][strlen(line_req[2]) - 1] == '\r')
            line_req[2][strlen(line_req[2]) - 1] = '\0';
    }
    char *k;
    while ((k = strtok(NULL, "\n"))) {
        // Connection type
        if (!strncmp(k, msg_type[0], strlen(msg_type[0]))) {
            line_req[3] = k + strlen(msg_type[0]);
            if (line_req[3][strlen(line_req[3]) - 1] == '\r')
                line_req[3][strlen(line_req[3]) - 1] = '\0';
        }
            // User-Agent type
        else if (!strncmp(k, msg_type[1], strlen(msg_type[1]))) {
            line_req[4] = k + strlen(msg_type[1]);
            if (line_req[4][strlen(line_req[4]) - 1] == '\r')
                line_req[4][strlen(line_req[4]) - 1] = '\0';
        }
            // Accept format
        else if (!strncmp(k, msg_type[2], strlen(msg_type[2]))) {
            line_req[5] = k + strlen(msg_type[2]);
            if (line_req[5][strlen(line_req[5]) - 1] == '\r')
                line_req[5][strlen(line_req[5]) - 1] = '\0';
        }
            // Cache-Control
        else if (!strncmp(k, msg_type[3], strlen(msg_type[3]))) {
            line_req[6] = k + strlen(msg_type[3]);
            if (line_req[6][strlen(line_req[6]) - 1] == '\r')
                line_req[6][strlen(line_req[6]) - 1] = '\0';
        }
    }
}

// Used to send HTTP messages to clients
ssize_t send_http_msg(int sock_fd, char *msg_to_send, ssize_t dim) {
    ssize_t sent = 0;
    char *msg = msg_to_send;
    while (sent < dim) {
        /*
         * Don't generate a SIGPIPE signal if the peer on a stream-oriented
         * socket  has  closed  the  connection.
         */
        sent = send(sock_fd, msg, (size_t) dim, MSG_NOSIGNAL);

        if (sent <= 0)
            break;

        msg += sent;
        dim -= sent;
    }

    return sent;
}

// Used to get image from file system
char *get_img(char *name, size_t img_dim, char *directory) {
    ssize_t left = 0;
    int fd;
    char *buf;
    char path[strlen(name) + strlen(directory) + 1];
    memset(path, (int) '\0', strlen(name) + strlen(directory) + 1);
    sprintf(path, "%s/%s", directory, name);
    if (path[strlen(path)] != '\0')
        path[strlen(path)] = '\0';

    errno = 0;
    if ((fd = open(path, O_RDONLY)) == -1) {
        switch (errno) {
            case EACCES:
                fprintf(stderr, "get_img: Permission denied\n");
                break;

            case EISDIR:
                fprintf(stderr, "get_img: '%s' is a directory\n", name);
                break;

            case ENFILE:
                fprintf(stderr, "get_img: The maximum allowable number of files is currently open in the system\n");
                break;

            case EMFILE:
                fprintf(stderr, "get_img: File descriptors are currently open in the calling process\n");
                break;

            default:
                fprintf(stderr, "Error in get_img\n");
        }
        return NULL;
    }

    errno = 0;
    if (!(buf = malloc(img_dim))) {
        fprintf(stderr, "errno: %d\t\timg_dim: %d\tget_img: Error in malloc\n", errno, (int) img_dim);
        return buf;
    } else {
        memset(buf, (int) '\0', img_dim);
    }

    //if left == 0 exits while
    while ((left = read(fd, buf + left, img_dim)))
        img_dim -= left;

    if (close(fd)) {
        fprintf(stderr, "get_img: Error closing file\t\tFile Descriptor: %d\n", fd);
    }

    return buf;
}

/*
 * Find q factor from Accept header
 * Return values: -1 --> error
 *                -2 --> factor quality not specified in the header
 * NOTE: This server DOES NOT consider the extensions of the images,
 * so this function will analyze the resource type and NOT the subtype.
 */
int quality(char *h_accept) {   //TODO never returns -2;    use WURFL instead
    //h_accept is the accept type field from http_req
    double images, others, q;
    images = others = q = -2.0;
    char *chr;
    char *t1 = strtok(h_accept, ",");
    if (!h_accept || !t1)
        return (int) (q *= 100);

    do {
        while (*t1 == ' ')
            ++t1;

        if (!strncmp(t1, "image", strlen("image"))) {
            chr = strrchr(t1, '=');
            // If not specified the 'q' value or if there was
            //  an error in transmission, the default
            //  value of 'q' is 1.0
            if (!chr) {
                images = 1.0;
                break;
            } else {
                errno = 0;
                double tmp = strtod(++chr, NULL);
                if (tmp > images)
                    images = tmp;
                if (errno != 0)
                    return -1;
            }
        } else if (!strncmp(t1, "*", strlen("*"))) {
            chr = strrchr(t1, '=');
            if (!chr) {
                others = 1.0;
            } else {
                errno = 0;
                others = strtod(++chr, NULL);
                if (errno != 0)
                    return -1;
            }
        }
    } while ((t1 = strtok(NULL, ",")));

    if (images > others || (others > images && images != -2.0))
        q = images;
    else if (others > images && images == -2.0)
        q = others;
    else
        fprintf(stderr, "string: %s\t\tquality: Unexpected error\n", h_accept);

    return (int) (q *= 100);
}

/*
 * http_fields refers to documentation in split_str function
 */
int data_to_send(int sock, char **http_fields) {
    char *http_response = malloc(DIM * DIM * 2);
    if (!http_response)
        error_found("Error in malloc\n");
    memset(http_response, (int) '\0',DIM * DIM * 2);

    // %d status code; %s status code; %s date; %s server; %s content type; %d content's length; %s connection type
    char *header = "HTTP/1.1 %d %s\r\nDate: %s\r\nServer: %s\r\nAccept-Ranges: bytes\r\n"
            "Content-Type: %s\r\nContent-Length: %d\r\nConnection: %s\r\n\r\n";
    char *time = get_time();
    char *server_name = "WebServerProject";  //TODO change project names
    char *h;

    if (!http_fields[0] || !http_fields[1] || !http_fields[2] ||
        ((strncmp(http_fields[0], "GET", 3) && strncmp(http_fields[0], "HEAD", 4)) ||
         (strncmp(http_fields[2], "HTTP/1.1", 8) && strncmp(http_fields[2], "HTTP/1.0", 8)))) {
        sprintf(http_response, header, 400, "Bad Request", time, server_name, "text/html", strlen(HTML[2]), "close");
        h = http_response;
        h += strlen(http_response);
        memcpy(h, HTML[2], strlen(HTML[2]));

        if (send_http_msg(sock, http_response, strlen(http_response)) == -1) {
            fprintf(stderr, "Error while sending data to client\n");
            free_time_http(time, http_response);
            return -1;
        }
        return 0;
    }

    //when root is requested
    if (strncmp(http_fields[1], "/", strlen(http_fields[1])) == 0) {
        sprintf(http_response, header, 200, "OK", time, server_name, "text/html", strlen(HTML[0]), "keep-alive");
        if (strncmp(http_fields[0], "HEAD", 4)) {
            h = http_response;
            h += strlen(http_response);
            memcpy(h, HTML[0], strlen(HTML[0]));
        }

        if (send_http_msg(sock, http_response, strlen(http_response)) == -1) {
            fprintf(stderr, "Error while sending data to client\n");
            free_time_http(time, http_response);
            return -1;
        }
    }
    /*
     * where the real functions begin
     */
    else {
        struct image *i = img;
        char *p_name;
        //if / is not found enters the if
        if (!(p_name = strrchr(http_fields[1], '/')))
            i = NULL;
        ++p_name;
        //make p point to /RESIZED.XXXXXX
        char *p = tmp_resized + strlen("/tmp");

        // Finding image in the image structure
        while (i) {
            /*
             * if p_name equals i->name enters the if,
             * otherwise passes to next image
             */
            if (!strncmp(p_name, i->name, strlen(i->name))) {
                ssize_t dim = 0;
                char *img_to_send = NULL;

                int favicon = 1;
                /*
                 *
                 */
                // Looking for resized image or favicon.ico
                if (!strncmp(p, http_fields[1], strlen(p) - strlen(".XXXXXX")) ||
                    !(favicon = strncmp(p_name, "favicon.ico", strlen("favicon.ico")))) {
                    if (strncmp(http_fields[0], "HEAD", 4)) {
                        img_to_send = get_img(p_name, i->size_r, favicon ? tmp_resized : IMG_PATH);
                        if (!img_to_send) {
                            fprintf(stderr, "data_to_send: Error in get_img\n");
                            free_time_http(time, http_response);
                            return -1;
                        }
                    }
                    dim = i->size_r;
                }
                // Looking for image in memory cache
                else {
                    char name_cached_img[DIM / 2];
                    memset(name_cached_img, (int) '\0', sizeof(char) * DIM / 2);
                    struct cache *c;
                    int def_val = 70;
                    int processing_accept = quality(http_fields[5]);
                    if (processing_accept == -1)
                        fprintf(stderr, "data_to_send: Unexpected error in strtod\n");

                    int quality_factor;
                    if (processing_accept < 0)
                        quality_factor = def_val;
                    else
                        quality_factor = processing_accept;

                    /*int quality_factor = processing_accept < 0 ? def_val : processing_accept;*/

                    lock(thds.mtx_cache_access);
                    c = i->img_c;
                    while (c) {
                        if (c->q == quality_factor) {
                            strcpy(name_cached_img, c->img_q);
                            /*
                             * If an image has been accessed, move it on top of the list
                             * in order to keep the image with less hit in the bottom of the list
                             * CACHE_N = -1 means we don't have cache max length;
                             * if name of cache_head == name of img we have to cache
                             *      the cache list is already updated
                             */
                            look_for_cached_img(CACHE_N, name_cached_img);
                            /*if (CACHE_N >= 0 && strncmp(thds.cache_hit_head->cache_name,
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
                            }*/
                            break;
                        }
                        c = c->next_img_c;
                    }

                    /*
                     * If image has not been cached yet
                     */
                    if (!c) {
                        // %s = image's name; %d = factor quality (between 1 and 99)
                        sprintf(name_cached_img, "%s_%d", p_name, quality_factor);
                        char path[DIM / 2];
                        memset(path, (int) '\0', DIM / 2);
                        sprintf(path, "%s/%s", tmp_cache, name_cached_img);

                        if (CACHE_N > 0) {
                            /*
                             * Cache of limited size
                             * If it has not yet reached the maximum cache size
                             * %s/%s = path/name_image; %d = factor quality
                             */
                            if (resize_image(IMG_PATH, p_name, quality_factor, tmp_cache, name_cached_img)) {
                                fprintf(stderr, "Error in function: rezize_image\n");
                                free_time_http(time, http_response);
                                unlock(thds.mtx_cache_access);
                                return -1;
                            }

                            /*char *format = "convert %s/%s -quality %d %s/%s;exit";
                            char command[DIM];
                            memset(command, (int) '\0', DIM);
                            sprintf(command, format, IMG_PATH, p_name, quality_factor, tmp_cache, name_cached_img);
                            if (system(command)) {
                                fprintf(stderr, "data_to_send: Unexpected error while refactoring image\n");
                                free_time_http(char_time, http_response);
                                unlock(thds.mtx_cache_access);
                                return -1;
                            }*/

                            //after the resize
                            if (insert_in_cache(path, quality_factor, name_cached_img, i, c, time, http_response))
                                fprintf(stderr, "Error in function: insert_in_cache\n");
                            /*struct stat buf;
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
                            struct cache_hit *new_hit = malloc(sizeof(struct cache_hit));
                            memset(new_entry, (int) '\0', sizeof(struct cache));
                            memset(new_hit, (int) '\0', sizeof(struct cache_hit));
                            if (!new_entry || !new_hit) {
                                fprintf(stderr, "data_to_send: Error in malloc\n");
                                free_time_http(char_time, http_response);
                                unlock(thds.mtx_cache_access);
                                return -1;
                            }

                            *//*
                             * filling struct cache of the relative image
                             * and inserting the struct cache_hit in the cache list
                             *//*
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
                            }*/
                            --CACHE_N;

                        }

                        else if (!CACHE_N){
                            /*
                             * Cache full. You have to delete an item.
                             * You choose to delete the oldest requested element.
                             */
                            if (delete_image(img_to_send, time, http_response) != 0)
                                fprintf(stderr, "error in function : delete_image\n");
                            /*char name_to_remove[DIM / 2];
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
                            }*/

                            if (resize_image(IMG_PATH, p_name, quality_factor, tmp_cache, name_cached_img)) {
                                fprintf(stderr, "Error in function: rezize_image\n");
                                free_time_http(time, http_response);
                                unlock(thds.mtx_cache_access);
                                return -1;
                            }

                            /*// %s/%s = path/name_image; %d = factor quality
                            char *format = "convert %s/%s -quality %d %s/%s;exit";
                            char command[DIM];
                            memset(command, (int) '\0', DIM);
                            sprintf(command, format, IMG_PATH, p_name, quality_factor, tmp_cache, name_cached_img);
                            if (system(command)) {
                                fprintf(stderr, "data_to_send: Unexpected error while refactoring image\n");
                                free_time_http(char_time, http_response);
                                unlock(thds.mtx_cache_access);
                                return -1;
                            }*/

                            //freeing a cache slot
                            if (free_cache_slot(c, i, time, http_response))
                                fprintf(stderr, "error in function: free_cache_slot\n");
                            if (insert_in_cache(path, quality_factor, name_cached_img, i, c, time, http_response))
                                fprintf(stderr, "error in function: insert_in_cache\n");

                            /*
                            struct stat buf;
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
                            c = i->img_c;

                            struct image *img_ptr = img;
                            struct cache *cache_ptr;
                            struct cache *cache_prev = NULL;

                            char *ext = strrchr(thds.cache_hit_tail->cache_name, '_');
                            //ext is the "_quality" of the resized img
                            size_t dim_fin = strlen(ext);
                            char name_i[DIM / 2];
                            memset(name_i, (int) '\0', DIM / 2);
                            strncpy(name_i, thds.cache_hit_tail->cache_name,
                                    strlen(thds.cache_hit_tail->cache_name) - dim_fin);

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

                            struct cache_hit *new_hit = malloc(sizeof(struct cache_hit));
                            memset(new_hit, (int) '\0', sizeof(struct cache_hit));
                            if (!new_hit) {
                                fprintf(stderr, "data_to_send: Error in malloc\n");
                                free_time_http(char_time, http_response);
                                unlock(thds.mtx_cache_access);
                                return -1;
                            }

                            strncpy(new_hit->cache_name, name_cached_img, strlen(name_cached_img));

                            struct cache_hit *to_be_removed = thds.cache_hit_tail;
                            new_hit->next_hit = thds.cache_hit_head->next_hit;
                            thds.cache_hit_head->next_hit = new_hit;
                            thds.cache_hit_head = thds.cache_hit_head->next_hit;
                            thds.cache_hit_tail = thds.cache_hit_tail->next_hit;
                            free(to_be_removed);
*/

                        } else {
                            /*
                             * Unlimited cache size
                             */
                            if (resize_image(IMG_PATH, p_name, quality_factor, tmp_cache, name_cached_img)) {
                                fprintf(stderr, "Error in function: rezize_image\n");
                                free_time_http(time, http_response);
                                unlock(thds.mtx_cache_access);
                                return -1;
                            }
                            /*char *format = "convert %s/%s -quality %d %s/%s;exit";
                            char command[DIM];
                            memset(command, (int) '\0', DIM);
                            sprintf(command, format, IMG_PATH, p_name, quality_factor, tmp_cache, name_cached_img);
                            if (system(command)) {
                                fprintf(stderr, "data_to_send: Unexpected error while refactoring image\n");
                                free_time_http(char_time, http_response);
                                unlock(thds.mtx_cache_access);
                                return -1;
                            }*/

                            if (insert_in_cache(path, quality_factor, name_cached_img, i, c, time, http_response))
                                fprintf(stderr, "error in function: insert_in_cache\n");

                        /*
                            struct stat buf;
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

                            new_entry->q = quality_factor;
                            strcpy(new_entry->img_q, name_cached_img);
                            new_entry->size_q = (size_t) buf.st_size;
                            new_entry->next_img_c = i->img_c;
                            i->img_c = new_entry;
                            c = i->img_c;
*/
                        }
                    }

                    unlock(thds.mtx_cache_access);

                    if (strncmp(http_fields[0], "HEAD", 4)) {
                    /*                        DIR *dir;
                        struct dirent *ent;
                        errno = 0;
                        dir = opendir(tmp_cache);
                        if (!dir) {
                            if (errno == EACCES) {
                                fprintf(stderr, "data_to_send: Error in opendir: Permission denied\n");
                                free_time_http(char_time, http_response);
                                return -1;
                            }
                            fprintf(stderr, "data_to_send: Error in opendir\n");
                            free_time_http(char_time, http_response);
                            return -1;
                        }

                        //finding the requested image
                        while ((ent = readdir(dir)) != NULL) {
                            if (ent->d_type == DT_REG) {
                                if (!strncmp(ent->d_name, name_cached_img, strlen(name_cached_img))) {
                                    img_to_send = get_img(name_cached_img, c->size_q, tmp_cache);
                                    if (!img_to_send) {
                                        fprintf(stderr, "data_to_send: Error in get_img\n");
                                        free_time_http(char_time, http_response);
                                        return -1;
                                    }
                                    break;
                                }
                            }
                        }

                        if (closedir(dir)) {
                            fprintf(stderr, "data_to_send: Error in closedir\n");
                            free(img_to_send);
                            free_time_http(char_time, http_response);
                            return -1;
                        }*/
                        if (search_file(name_cached_img, img_to_send, c, time, http_response))
                            fprintf(stderr, "error in function: search_file\n");
                    }
                    dim = c->size_q;
                }

                sprintf(http_response, header, 200, "OK", time, server_name, "image/gif", dim, "keep-alive");
                ssize_t dim_tot = (size_t) strlen(http_response);
                if (strncmp(http_fields[0], "HEAD", 4)) {
                    if (dim_tot + dim > DIM * DIM * 2) {
                        http_response = realloc(http_response, (dim_tot + dim) * sizeof(char));
                        if (!http_response) {
                            fprintf(stderr, "data_to_send: Error in realloc\n");
                            free_time_http(time, http_response);
                            free(img_to_send);
                            return -1;
                        }
                        memset(http_response + dim_tot, (int) '\0', (size_t) dim);
                    }
                    h = http_response;
                    //points at the end of the header
                    h += dim_tot;
                    //writes the image in the body of http response
                    memcpy(h, img_to_send, (size_t) dim);
                    dim_tot += dim;
                }
                if (send_http_msg(sock, http_response, dim_tot) == -1) {
                    fprintf(stderr, "data_to_send: Error while sending data to client\n");
                    free_time_http(time, http_response);
                    return -1;
                }

                free(img_to_send);
                break;
            }
            i = i->next_img;
        }

        //if image has not been found
        if (!i) {
            sprintf(http_response, header, 404, "Not Found", time, server_name, "text/html", strlen(HTML[1]), "close");
            if (strncmp(http_fields[0], "HEAD", 4)) {
                h = http_response;
                //h points at the start of the body
                h += strlen(http_response);
                memcpy(h, HTML[1], strlen(HTML[1]));
            }
            if (send_http_msg(sock, http_response, strlen(http_response)) == -1) {
                fprintf(stderr, "Error while sending data to client\n");
                free_time_http(time, http_response);
                return -1;
            }
        }
    }
    free_time_http(time, http_response);
    return 0;
}

/*
 * Every thread execute this function to deal a connection
 * Analyzes HTTP message
 */
void respond(int sock, struct sockaddr_in client) {
    //buffer
    char http_req[DIM * DIM];
    char *line_req[7];
    ssize_t tmp;
    int i;

    struct timeval tv;
    tv.tv_sec = 10; //TODO look timeval struct
    tv.tv_usec = 0;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(struct timeval)) < 0)
        fprintf(stderr, "respond: Error in setsockopt\n");

    do {
        memset(http_req, (int) '\0', 5 * DIM);
        for (i = 0; i < 7; ++i)
            line_req[i] = NULL;

        errno = 0;
        tmp = recv(sock, http_req, 5 * DIM, 0);

        if (tmp == -1) {
            switch (errno) {
                case EFAULT:
                    fprintf(stderr, "The receive  buffer  pointer(s)  point  outside  the  process's address space");
                    break;

                case EBADF:
                    fprintf(stderr, "The argument of recv() is an invalid descriptor: %d\n", sock);
                    break;

                case ECONNREFUSED:
                    fprintf(stderr, "Remote host refused to allow the network connection\n");
                    break;

                case ENOTSOCK:
                    fprintf(stderr, "The argument of recv() does not refer to a socket\n");
                    break;

                case EINVAL:
                    fprintf(stderr, "Invalid argument passed\n");
                    break;

                case EINTR:
                    fprintf(stderr, "Timeout receiving from socket\n");
                    break;

                case EWOULDBLOCK:
                    fprintf(stderr, "Timeout receiving from socket\n");
                    break;

                default:
                    fprintf(stderr, "Error in recv: error while receiving data from client\n");
                    break;
            }
            break;
        } else if (tmp == 0) {
            fprintf(stderr, "Client disconnected\n");
            break;
        } else {
            split_str(http_req, line_req);

            char log_string[DIM / 2];
            memset(log_string, (int) '\0', DIM / 2);
            sprintf(log_string, "\tClient:\t%s\tRequest: '%s %s %s'\n",
                    inet_ntoa(client.sin_addr), line_req[0], line_req[1], line_req[2]);
            write_log(log_string);

            if (data_to_send(sock, line_req))
                break;
        }
    } while (line_req[3] && !strncmp(line_req[3], "keep-alive", 10));
}

/*
 * This is the routine of all threads.
 * This function is used to manage client's connection
 */
void *manage_connection(void *arg) {
    //join not needed
    if (pthread_detach(pthread_self()) != 0)
        error_found("Error in pthread_detach\n");

    struct th_sync *k = (struct th_sync *) arg;
    struct sockaddr_in client;
    int slot_c, sock;

    lock(k -> mtx_sync_conditions);
    slot_c = k -> slot_c;
    signal_t(k -> th_start);
    unlock(k -> mtx_sync_conditions);

    lock(k -> mtx_thread_conn_number);
    if (k -> clients[slot_c] == -3) {
        // Thread ready for incoming connections
        k -> clients[slot_c] = -1;
    } else {
        fprintf(stderr, "Unknown error: slot[%d]: %d\n", slot_c, k -> clients[slot_c]);
        pthread_exit(NULL);
    }
    // Deal connections
    while (1) {
        memset(&client, (int) '\0', sizeof(struct sockaddr_in));
        wait_t(k->threads_cond_list + slot_c, k->mtx_thread_conn_number);
        /*
         * sock values:
         *      -1 -> thread ready for incoming connections
         *      -2 -> thread killed by kill_th function or thread not yet created
         *      -3 -> newly created thread
         *      >0 -> connection oriented socket file descriptor
         */
        sock = k->clients[slot_c];
        //if clients[slot_c] socket's thread has been killed than decrease th_act counter
        if (sock < 0) {
            if (sock != -2) {
                fprintf(stderr, "Unknown error trying to access sock array: %d\n", sock);
                continue;
            }
            --k->th_act;
            unlock(k->mtx_thread_conn_number);
            break;
        }

        /*
         * if sock >= 0:
         * copies the sockaddr_in struct in client
         * and increments the number of connection
         * new connection has been established
         */
        memcpy(&client, &k->client_addr, sizeof(struct sockaddr_in));
        ++k -> connections;
        /*
         * Analyzes number of connections and active threads
         * and creates new threads if necessary
         */
        spawn_th(k);
        unlock(k -> mtx_thread_conn_number);

        respond(sock, client);

        errno = 0;
        if (close(sock) != 0) {
            switch (errno) {
                case EIO:
                    fprintf(stderr, "I/O error occurred\n");
                    break;

                case EBADF:
                    fprintf(stderr, "Bad file number: %d. Probably client has disconnected\n", sock);
                    break;

                default:
                    fprintf(stderr, "Error in close\n");
            }
        }

        lock(k -> mtx_thread_conn_number);
        --k -> connections;
        kill_th(k);
        k -> clients[slot_c] = -1;

        signal_t(k -> full);
    }

    pthread_exit(EXIT_SUCCESS);
}


/*
 * Used to create other threads
 * in the case in which the server load is rising
 */
void spawn_th(struct th_sync *k) {
    /*
     * Threads are created dynamically in need with the number of connections.
     * If the number of connections decreases, the number of active threads
     * is reduced in a phased manner so as to cope with a possible peak of connections.
     */
    if (k -> connections >= k -> th_act_thr * 2 / 3 &&
        k -> th_act <= k -> th_act_thr) {
        int n_th;
        if (k -> th_act_thr + MINTH / 2 <= MAXCONN) {
            n_th = MINTH / 2;
        } else {
            n_th = MAXCONN - k -> th_act_thr;
        }
        if (n_th) {
            k -> th_act_thr += n_th;
            init_th(n_th, manage_connection, k);
        }
    }
}

/*
 * This is the main thread which manage all incoming connections.
 * Once a client send a request to the server, this thread checks if
 * it can process the connection or not.
 * If so, assigns the connection management to a child thread,
 * otherwise it waits on a pthread_cond_t condition,
 * until the system load is not lowered.
 * */
void *manage_threads(void *arg) {
    struct th_sync *k = (struct th_sync *) arg;

    create_th(catch_command, arg);
    init_th(MINTH, manage_connection, arg);

    int connsocketFD, i = 0, j;
    struct sockaddr_in client;
    socklen_t socksize = sizeof(struct sockaddr_in);

    fprintf(stdout, "\n\n\n-Waiting for incoming connection...\n");
    // Accept connections
    while (1) {
        lock(k -> mtx_thread_conn_number);
        if (k -> connections + 1 > MAXCONN) {
            wait_t(k -> full, k -> mtx_thread_conn_number); }
        unlock(k -> mtx_thread_conn_number);

        memset(&client, (int) '\0', socksize);
        errno = 0;
        connsocketFD = accept(LISTENsd, (struct sockaddr *) &client, &socksize);
        memset(&k->client_addr, (int) '\0', socksize);
        memcpy(&k->client_addr, &client, socksize);

        lock(k -> mtx_thread_conn_number);
        if (connsocketFD == -1) {
            switch (errno) {
                case ECONNABORTED:
                    fprintf(stderr, "The connection has been aborted\n");
                    unlock(k -> mtx_thread_conn_number);
                    continue;

                case ENOBUFS:
                    error_found("Not enough free memory\n");

                case ENOMEM:
                    error_found("Not enough free memory\n");

                case EMFILE:
                    fprintf(stderr, "Too many open files!\n");
                    wait_t(k -> full, k -> mtx_thread_conn_number);
                    unlock(k -> mtx_thread_conn_number);
                    continue;

                case EPROTO:
                    fprintf(stderr, "Protocol error\n");
                    unlock(k -> mtx_thread_conn_number);
                    continue;

                case EPERM:
                    fprintf(stderr, "Firewall rules forbid connection\n");
                    unlock(k -> mtx_thread_conn_number);
                    continue;

                case ETIMEDOUT:
                    fprintf(stderr, "Timeout occured\n");
                    unlock(k -> mtx_thread_conn_number);
                    continue;

                case EBADF:
                    fprintf(stderr, "Bad file number\n");
                    unlock(k -> mtx_thread_conn_number);
                    continue;

                default:
                    error_found("Error in accept\n");
            }
        }

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
            unlock(k -> mtx_thread_conn_number);
            continue;
        }
        k -> clients[i] = connsocketFD;
        signal_t(k -> threads_cond_list + i);
        i = (i + 1) % MAXCONN;
        unlock(k -> mtx_thread_conn_number);
    }
}

int main(int argc, char **argv) {
    if (argc > 11) {
        fprintf(stderr, "Too many arguments\n\n");
        usage(*argv);
    }

    pthread_mutex_t mtx_s_c, mtx_c, mtx_t;
    pthread_cond_t event, th_start, full;

    init(argc, argv, &mtx_s_c, &mtx_c, &mtx_t,
         &th_start, &full, &thds);

    // To ignore SIGPIPE
    catch_signal();

    manage_threads(&thds);

    return EXIT_SUCCESS;
}

