#include <time.h>
#include "structs.h"
#include "functions.h"

#define DIM 512
#define DIM2 64

struct image_struct *image_struct;
struct threads_sync_struct thread_struct;

// Log's file pointer
FILE *LOG = NULL;
// Pointer to html files; 1st: root, 2nd: 404, 3rd 400.
char *HTML_PAGES[3];
int PORT = 8080;
//MINimum THreads
int MIN_THREAD_TRESHOLD = 250;
int MAX_CONNECTION = 1500;
int LISTEN_SOCKET_DESCRIPTOR;
//char images_path[DIM / 2];
char images_path[DIM];
// Number of cached images
volatile int CACHE_COUNTER = -1;
char resized_tmp_dir[DIM2] = "/tmp/RESIZED.XXXXXX";
// tmp files cached
char cache_tmp_dir[DIM2] = "/tmp/CACHE.XXXXXX";

// User's command
char *user_command = "-Enter 'q' to stop the server, "
        "'s' to know server state or "
        "'f' to force Log file write";


void write_on_stream(char *string, FILE *file) {

    size_t count;
    size_t len = strlen(string);

    while ((count = fwrite(string, sizeof(char), len, file)) < len) {
        if (ferror(file)) {
            error_found("write_on_stream: error in fwrite\n");
        }
        len -= count;
        string += count;
    }

    if (fflush(file)) {
        error_found("write_on_stream: error in fflush\n");
    }
}

char *get_time(void) {

    time_t time_orig = time(NULL);
    char *formatted_time = malloc(sizeof(char) * DIM2);
    if (!formatted_time)
        error_found("get_time: Error in malloc\n");
    /*
     * The call ctime(t) converts the calendar char_time t into a null-terminated string
     * of the form: "Wed Jun 30 21:49:08 1993\n"
     */
    strcpy(formatted_time, ctime(&time_orig));
    if (!formatted_time)
        error_found("get_time: Error in ctime\n");
    if (formatted_time[strlen(formatted_time) - 1] == '\n')
        formatted_time[strlen(formatted_time) - 1] = '\0';
    return formatted_time;
}

void user_usage(const char *c) {

    fprintf(stderr, "Usage: %s [-l logs file path]\n"
            "\t\t[-i images path]\n"
            "\t\t[-p port number]\n"
            "\t\t[-c maximum connections number]\n"
            "\t\t[-t initial threads number]\n"
            "\t\t[-h help]\n", c);
    exit(EXIT_SUCCESS);
}

void get_command_line_options(int argc, char **argv, char **path) {

    int i = 1;
    for (; argv[i] != NULL; ++i)
        // option -h is not allowed
        if (strcmp(argv[i], "-h") == 0)
            user_usage(argv[0]);

    //t_mode means min thr number has been passed from line command,
    //c_mode if max conn has been passed
    int z; char *k; char c_mode = 0, t_mode = 0;
    struct stat statbuf;
    // Parsing the command line arguments
    // -p := port;
    // -l := directory to store Log files;
    // -i := directory of files to send;
    // -t := minimum number of thread's pool;
    // -c := maximum number of connections.
    // -n := maximum cache_struct size
    while ((z = getopt(argc, argv, "p:l:i:t:c:r:n:")) != -1) {
        switch (z) {
            //sets the PORT number form the command line argument
            case 'p':
                if (strlen(optarg) > 5)
                    error_found("get_command_line_options: Port number too high\n");

                errno = 0;
                int p_arg = (int) strtol(optarg, &k, 10);
                if (errno != 0 || *k != '\0')
                    error_found("get_command_line_options: Error in strtol: Invalid number port\n");
                if (p_arg > 65535)
                    error_found("get_command_line_options: Port number too high\n");
                PORT = p_arg;
                break;

                //sets the log file path from the command line arguments
            case 'l':
                errno = 0;
                //saves in struct stat all the info of the log file
                //if an option is followed by a colon it requires an additional argument, optarg points to that argument
                if (stat(optarg, &statbuf) != 0) {
                    if (errno == ENAMETOOLONG)
                        error_found("get_command_line_options: Path too long\n");
                    error_found("get_command_line_options: Invalid path for log files\n");
                } else if (!S_ISDIR(statbuf.st_mode)) {
                    error_found("get_command_line_options: The path is not a directory!\n");
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
                        error_found("get_command_line_options: Path too long\n");
                    error_found("get_command_line_options: Invalid path\n");
                } else if (!S_ISDIR(statbuf.st_mode)) {
                    error_found("Tget_command_line_options: the path is not a directory!\n");
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
                int t_arg = (int) strtol(optarg, &k, 10);
                if (errno != 0 || *k != '\0')
                    error_found("get_command_line_options: Error in strtol: Invalid number\n");
                if (t_arg < 1)
                    error_found("get_command_line_options: Error: threads number must be > 0!\n");
                if (t_arg < 2)
                    error_found("get_command_line_options: Attention: due to performance issue, threads number must be >= 2!\n");
                MIN_THREAD_TRESHOLD = t_arg;
                t_mode = 1;
                break;

                //sets the number of maximum connections at the same char_time
            case 'c':
                errno = 0;
                int c_arg = (int) strtol(optarg, &k, 10);
                if (errno != 0 || *k != '\0')
                    error_found("get_command_line_options: Error in strtol: Invalid number\n");
                if (c_arg < 1)
                    error_found("get_command_line_options: Error: maximum connections number must be > 0!");
                MAX_CONNECTION = c_arg;
                c_mode = 1;
                break;

            case 'n':
                errno = 0;
                int cache_size = (int) strtol(optarg, &k, 10);
                if (errno != 0 || *k != '\0')
                    error_found("get_command_line_options: Argument -n: Error in strtol: Invalid number\n");
                if (cache_size)
                    CACHE_COUNTER = cache_size;
                break;

            case '?':
                error_found("get_command_line_options: Invalid argument\n");

            default:
                error_found("get_command_line_options: Unknown error in getopt\n");
        }
    }
    if (c_mode && !t_mode && MAX_CONNECTION < MIN_THREAD_TRESHOLD)
        MIN_THREAD_TRESHOLD = MAX_CONNECTION;
    else if (t_mode && !c_mode && MIN_THREAD_TRESHOLD > MAX_CONNECTION)
        MAX_CONNECTION = MIN_THREAD_TRESHOLD;
    if (MIN_THREAD_TRESHOLD > MAX_CONNECTION)
        error_found("get_command_line_options: maximum connections number is lower then minimum threads number!\n");
}

// Start server
void start_WebServer(void) {

    struct sockaddr_in server_address;

    if ((LISTEN_SOCKET_DESCRIPTOR = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        error_found("start_WebServer: Error in socket\n");

    //sets the socket with all 0s
    memset((void *) &server_address, 0, sizeof(server_address));
    //IPv4
    (server_address).sin_family = AF_INET;
    //addresses must be in Network Byte Order
    (server_address).sin_addr.s_addr = htonl(INADDR_ANY);  // All available interface
    (server_address).sin_port = htons((unsigned short) PORT);

    // To reuse a socket
    int flag = 1;
    if (setsockopt(LISTEN_SOCKET_DESCRIPTOR, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) != 0)
        error_found("start_WebServer: Error in setsockopt\n");

    errno = 0;
    if (bind(LISTEN_SOCKET_DESCRIPTOR, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
        switch (errno) {
            //adress is protected and the user is not superuser
            case EACCES:
                error_found("start_WebServer: Choose another socket\n");

                //the socket is already bound to an address
            case EINVAL:
                error_found("start_WebServer: The socket is already bound to an address\n");

            default:
                error_found("start_WebServer: Error in bind\n");
        }
    }

    // listen for incoming connections
    if (listen(LISTEN_SOCKET_DESCRIPTOR, MAX_CONNECTION) != 0)
        error_found("start_WebServer: Error in listen\n");

    fprintf(stdout, "-Servers socket created with number: %d\n", PORT);
}

void check_and_build(char *resized_image_path, char *image_name, char **html, size_t *dim) {

    char *k = "<b>%s</b><br><br><a href=\"%s\"><img src=\"\\%s\" height=\"130\" weight=\"100\"></a><br><br><br><br>";

    size_t len = strlen(*html);
    if (len + DIM >= *dim * DIM) {
        ++*dim;
        *html = realloc(*html, *dim * DIM);
        if (!*html)
            error_found("check_and_build: Error in realloc\n");
    }

    /*char *w;
    if (!(w = strrchr(resized_image_path, '/')))
        error_found("check_and_build: unexpected error creating HTML root file\n");
    ++w;*/

    char *w = resized_image_path + strlen("/tmp/");

    char *q = *html + len;
    sprintf(q, k, image_name, image_name, w);
}

// Used to fill img dynamic structure
void build_img_struct(struct image_struct **img, char *path) {

    char new_path[DIM];
    memset(new_path, (int) '\0', DIM);
    struct image_struct *z = malloc(sizeof(struct image_struct));
    if (!z)
        error_found("build_img_struct: Error in malloc\n");
    memset(z, (int) '\0', sizeof(struct image_struct));

    char *name = strrchr(path, '/');
    if (!name) {
        if (!strncmp(path, "favicon.ico", 11)) {
            sprintf(new_path, "%s/%s", images_path, path);
            strcpy(z->name, path);
            path = new_path;
        } else {
            error_found("build_img_struct: Error analyzing file");
        }
    } else {
        strcpy(z->name, ++name);
    }

    struct stat statbuf;
    get_file_info(&statbuf, path, 0);
    z->resized_image_size = (size_t) statbuf.st_size;
    z->cached_image = NULL;

    if (!*img) {
        z->next_image = *img;
        *img = z;
    } else {
        z->next_image = (*img)->next_image;
        (*img)->next_image = z;
    }
}

void check_WebServer_images(int perc) {

    DIR *dir;
    struct dirent *dirent;
    char *char_ptr;

    //opens the target directory, returns a ptr to the directory stream
    errno = 0;
    dir = opendir(images_path);
    if (!dir) {
        if (errno == EACCES)
            error_found("check_WebServer_images: Permission denied\n");
        error_found("check_WebServer_images: Error in opendir\n");
    }

    int images_number = 5;
    char *html = malloc((size_t)images_number * DIM);
    if (!html)
        error_found("check_WebServer_images: Error in malloc\n");
    memset(html, (int) '\0', (size_t) images_number * DIM * sizeof(char));

    // writes a string that will be the html home page
    // %s page's title; %s header; %s text.

    char *html_header = "<!DOCTYPE html><html><head><meta charset=\"utf-8\" /><title>%s</title><style type=\"text/css\"></style><script type=\"text/javascript\"></script></head><body background=\"\"><h1>%s</h1><br><br><h3>%s</h3><hr><br>";

    sprintf(html, html_header, "WebServer", "Choose an image", "It will be resized for your own device!");
    /*    // %s image_struct's path; %d resizing percentage
    char *convert = "convert %s -resize %d%% %s;exit";*/
    size_t header_length = strlen(html), new_header_length;

    struct image_struct **image = &image_struct;
    char source_image_path[DIM], resized_image_path[DIM];
    memset(source_image_path, (int) '\0', DIM); memset(resized_image_path, (int) '\0', DIM);
    //readdir read the sirectory stream created by opendir as a sequence of dirent structs
    fprintf(stdout, "-Please wait while resizing images...\n");
    while ((dirent = readdir(dir)) != NULL) {
        //DT_REG means a regular file
        if (dirent -> d_type == DT_REG) {
            /*If a file is appended with a tilde~,
             * it only means that it is a backup created by a text editor
             * or similar program*/
            if ((char_ptr = strrchr(dirent -> d_name, '~')) != NULL) {
                //tilde is found
                fprintf(stderr, "check_WebServer_images: File '%s' was skipped\n", dirent -> d_name);
                continue;
            }

            if ((char_ptr = strrchr(dirent -> d_name, '.')) != NULL) {
                if (strcmp(char_ptr, ".db") == 0) {
                    fprintf(stderr, "check_WebServer_images: File '%s' was skipped\n", dirent -> d_name);
                    continue;
                }
                if (strcmp(char_ptr, ".gif") != 0 && strcmp(char_ptr, ".GIF") != 0 &&
                    strcmp(char_ptr, ".jpg") != 0 && strcmp(char_ptr, ".JPG") != 0 &&
                    strcmp(char_ptr, ".png") != 0 && strcmp(char_ptr, ".PNG") != 0)
                    fprintf(stderr, "check_WebServer_images: Warning: file '%s' may have an unsupported format\n", dirent -> d_name);
            } else {
                fprintf(stderr, "check_WebServer_images: Warning: file '%s' format in not supported\n", dirent -> d_name);
            }

            if (resize_image(images_path, dirent -> d_name, perc, resized_tmp_dir, dirent -> d_name))
                error_found("check_WebServer_images: Error resizing images\n");

            //in source_image_path there in no / because is already in images_path
            sprintf(source_image_path, "%s%s", images_path, dirent -> d_name);
            sprintf(resized_image_path, "%s/%s", resized_tmp_dir, dirent -> d_name);
            build_img_struct(image, resized_image_path);
            image = &(*image) -> next_image;


            //check_and_build(dirent -> d_name, &html, &images_number);
            check_and_build(resized_image_path, dirent->d_name, &html, &images_number);
        }
    }

    new_header_length = strlen(html);
    if (header_length == new_header_length)
        error_found("check_WebServer_images: There are no images in the specified directory\n");

    html_header = "</body></html>";
    if (new_header_length + DIM2 / 4 > images_number * DIM) {
        ++images_number;
        html = realloc(html, (size_t) images_number * DIM);
        if (!html)
            error_found("check_WebServer_images: Checking images: Error in realloc\n");
        memset(html + new_header_length, (int) '\0', (size_t) images_number * DIM - new_header_length);
    }
    char_ptr = html;
    char_ptr += strlen(html);
    strcpy(char_ptr, html_header);

    HTML_PAGES[0] = html;

    if (closedir(dir))
        error_found("check_WebServer_images: Error in closedir\n");

    fprintf(stdout, "-Images resized in: '%s' with percentage: %d%%\n", resized_tmp_dir, perc);
}

/*takes 9 arguments: argc, argv, 3 mutex, 3 condition and a pointer toa threads_sync_struct struct.
 * */
void init(int argc, char **argv, pthread_mutex_t *mtx_sync_conditions, pthread_mutex_t *mtx_cache_access,
          pthread_mutex_t *mtx_thread_conn_number, pthread_cond_t *th_start, pthread_cond_t *full,
          struct threads_sync_struct *d) {

    char LOG_PATH[DIM], IMAGES_PATH[DIM];
    memset(LOG_PATH, (int) '\0', DIM);
    memset(IMAGES_PATH, (int) '\0', DIM);
    strcpy(LOG_PATH, ".");
    strcpy(IMAGES_PATH, ".");
    char *PATHS[2];
    PATHS[0] = LOG_PATH;
    PATHS[1] = IMAGES_PATH;
    int resize_percentage = 20;

    //get the manual config options by command line arg
    get_command_line_options(argc, argv, PATHS);

    //initializes mutexes and conditions
    if (pthread_mutex_init(mtx_sync_conditions, NULL) != 0 ||
        pthread_mutex_init(mtx_cache_access, NULL) != 0 ||
        pthread_mutex_init(mtx_thread_conn_number, NULL) != 0 ||
        pthread_cond_init(th_start, NULL) != 0 ||
        pthread_cond_init(full, NULL) != 0)
        error_found("init: Error in pthread_mutex_init or pthread_cond_init\n");

    //initialize threads_sync_struct fields
    d->connections = d->client_socket_element = d->threads_to_kill = d->active_threads = 0;
    d -> mtx_sync_conditions = mtx_sync_conditions;
    d -> mtx_cache_access = mtx_cache_access;
    d -> mtx_thread_conn_number = mtx_thread_conn_number;
    d -> th_start = th_start;
    d -> min_active_threads_treshold = MIN_THREAD_TRESHOLD;
    d -> server_full = full;
    image_struct = NULL;

    d -> client_socket_list = malloc(sizeof(int) * MAX_CONNECTION);
    d -> threads_cond_list = malloc(sizeof(pthread_cond_t) * MAX_CONNECTION);
    if (d->client_socket_list == NULL || d->threads_cond_list == NULL) {
        //if (!d -> client_socket_list || !d -> new_c)
        error_found("init: Error in malloc\n");
    } else {
        memset(d->client_socket_list, (int) '\0', sizeof(int) * MAX_CONNECTION);
        memset(d->threads_cond_list, (int) '\0', sizeof(pthread_cond_t) * MAX_CONNECTION);
    }
    // -1 := slot with thread initialized; -2 := empty slot.
    int i;
    for (i = 0; i < MAX_CONNECTION; ++i) {
        d -> client_socket_list[i] = -2;

        pthread_cond_t cond;
        if (pthread_cond_init(&cond, NULL) != 0)
            error_found("init: Error in pthread_cond_init\n");
        d -> threads_cond_list[i] = cond;
    }

    start_WebServer();
    LOG = open_LOG_file(LOG_PATH);
    char start_server[DIM];
    memset(start_server, (int) '\0', DIM);
    char *log_message = "\t\tServer started at port:";
    sprintf(start_server, "%s %d\n", log_message, PORT);
    write_log(start_server);

    // Create tmp folder for resized and cached images
    if (!mkdtemp(resized_tmp_dir) || !mkdtemp(cache_tmp_dir))
        error_found("init: Error in mkdtmp\n");

    errno = 0;

    if (CACHE_COUNTER > 0) {
        fprintf(stdout, "-Cache size: %d; located in '%s'\n", CACHE_COUNTER, cache_tmp_dir);
    } else {
        fprintf(stdout, "-Cache size: Unlimited; located in '%s'\n", cache_tmp_dir);
    }

    strcpy(images_path, IMAGES_PATH);
    check_WebServer_images(resize_percentage);
    build_error_pages(HTML_PAGES);
}

// Used to free memory allocated from malloc/realloc functions
void free_memory() {

    free(HTML_PAGES[0]);
    free(HTML_PAGES[1]);
    free(HTML_PAGES[2]);
    free(thread_struct.client_socket_list);
    free(thread_struct.threads_cond_list);
    //CACHE_COUNTER can't be 0
    if (CACHE_COUNTER >= 0 && thread_struct.cached_name_head && thread_struct.cached_name_tail) {
        struct cached_name_element *to_be_removed;
        //till tail is NULL
        while (thread_struct.cached_name_tail) {
            to_be_removed = thread_struct.cached_name_tail;
            thread_struct.cached_name_tail = thread_struct.cached_name_tail->next_cached_image_name;
            free(to_be_removed);
        }
    }

    remove_directory(resized_tmp_dir);
    remove_directory(cache_tmp_dir);
}

// Thread which control stdin to recognize user's input
void *catch_user_command(void *arg) {

    struct threads_sync_struct *threads_sync_struct = (struct threads_sync_struct *) arg;

    printf("\n%s\n", user_command);
    while (1) {
        char command[2];
        int connections_number, threads_number;
        memset(command, (int) '\0', 2);
        if (fscanf(stdin, "%s", command) != 1)
            error_found("catch_user_command: Error in fscanf\n");

        if (strlen(command) != 1) {
            printf("%s\n", user_command);
        } else {
            if (command[0] == 's' || command[0] == 'S') {
                lock(thread_struct.mtx_thread_conn_number);
                connections_number = thread_struct.connections; threads_number = thread_struct.active_threads;
                unlock(thread_struct.mtx_thread_conn_number);
                fprintf(stdout, "\nConnections' number: %d\n"
                        "Threads running: %d\n\n", connections_number, threads_number);
                continue;
            } else if (command[0] == 'f' || command[0] == 'F') {
                errno = 0;
                if (fflush(LOG)) {
                    if (errno == EBADF)
                        fprintf(stderr, "catch_user_command: Error in fflush: Stream is not open, at least not for writing.\n");
                    fprintf(stderr, "catch_user_command: Unexpected error in fflush\n");
                }
                fprintf(stdout, "Log file updated\n");
                continue;
            } else if (command[0] == 'q' || command[0] == 'Q') {
                fprintf(stdout, "-Closing server\n");

                errno = 0;
                // Kernel may still hold some resources for a period (TIME_WAIT)
                if (close(LISTEN_SOCKET_DESCRIPTOR) != 0) {
                    if (errno == EIO)
                        error_found("catch_user_command: I/O error occurred\n");
                    error_found("catch_user_command: Error in close\n");
                }

                int i = 0;
                for (; i < MAX_CONNECTION; ++i) {
                    if (threads_sync_struct -> client_socket_list[i] >= 0) {
                        //client_socket_list are sockets
                        if (close(threads_sync_struct -> client_socket_list[i]) != 0) {
                            switch (errno) {
                                case EIO:
                                    error_found("catch_user_command: I/O error occurred\n");

                                case ENOTCONN:
                                    error_found("catch_user_command: The socket is not connected\n");

                                case EBADF:
                                    fprintf(stderr, "catch_user_command: Bad file number. Maybe client has disconnected\n");
                                    break;

                                default:
                                    error_found("catch_user_command: Error in close\n");
                            }
                        }
                    }
                }
                write_log("\t\tServer closed.\n\n\n");

                errno = 0;
                if (fflush(LOG)) {
                    if (errno == EBADF)
                        error_found("catch_user_command: Error in fflush: Stream is not open, at least not for writing.\n");
                }

                if (fclose(LOG) != 0)
                    error_found("catch_user_command: Error in fclose\n");

                free_memory();
                exit(EXIT_SUCCESS);
            }
            printf("%s\n\n", user_command);
        }
    }
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
void split_HTTP_message(char *HTTP_request_buffer, char **line_request) {

    char *HTTP_header_format[4];
    HTTP_header_format[0] = "Connection: ";
    HTTP_header_format[1] = "User-Agent: ";
    HTTP_header_format[2] = "Accept: ";
    HTTP_header_format[3] = "Cache-Control: ";
    // HTTP message type
    line_request[0] = strtok(HTTP_request_buffer, " ");
    // Requested object
    line_request[1] = strtok(NULL, " ");
    // HTTP version
    line_request[2] = strtok(NULL, "\n");
    //if HTTP version is not NULL
    if (line_request[2]) {
        //replace \r with \0 at line's end
        if (line_request[2][strlen(line_request[2]) - 1] == '\r')
            line_request[2][strlen(line_request[2]) - 1] = '\0';
    }
    char *k;
    while ((k = strtok(NULL, "\n"))) {
        // Connection type
        if (!strncmp(k, HTTP_header_format[0], strlen(HTTP_header_format[0]))) {
            line_request[3] = k + strlen(HTTP_header_format[0]);
            if (line_request[3][strlen(line_request[3]) - 1] == '\r')
                line_request[3][strlen(line_request[3]) - 1] = '\0';
        }
            // User-Agent type
        else if (!strncmp(k, HTTP_header_format[1], strlen(HTTP_header_format[1]))) {
            line_request[4] = k + strlen(HTTP_header_format[1]);
            if (line_request[4][strlen(line_request[4]) - 1] == '\r')
                line_request[4][strlen(line_request[4]) - 1] = '\0';
        }
            // Accept format
        else if (!strncmp(k, HTTP_header_format[2], strlen(HTTP_header_format[2]))) {
            line_request[5] = k + strlen(HTTP_header_format[2]);
            if (line_request[5][strlen(line_request[5]) - 1] == '\r')
                line_request[5][strlen(line_request[5]) - 1] = '\0';
        }
            // Cache-Control
        else if (!strncmp(k, HTTP_header_format[3], strlen(HTTP_header_format[3]))) {
            line_request[6] = k + strlen(HTTP_header_format[3]);
            if (line_request[6][strlen(line_request[6]) - 1] == '\r')
                line_request[6][strlen(line_request[6]) - 1] = '\0';
        }
    }
}

// Used to send HTTP messages to client_socket_list
ssize_t send_HTTP_message(int socket_fd, char *msg_to_send, ssize_t msg_dim) {

    ssize_t sent = 0;
    char *msg = msg_to_send;
    while (sent < msg_dim) {
        /*
         * Don't generate a SIGPIPE signal if the peer on a stream-oriented
         * socket  has  closed  the  connection.
         */
        sent = send(socket_fd, msg, (size_t) msg_dim, MSG_NOSIGNAL);

        if (sent <= 0)
            break;

        msg += sent;

        msg_dim -= sent;

    }
    return sent;
}

/*
 * Find q factor from Accept header
 * Return values: -1 --> error
 *                -2 --> factor quality not specified in the header
 * NOTE: This server DOES NOT consider the extensions of the images,
 * so this function will analyze the resource type and NOT the subtype.
 */
int get_quality(char *HTTP_header_access_field) {

    //HTTP_header_access_field is the accept type field from http_req
    double images, others, quality;
    images = others = quality = -2.0;
    char *chr;
    char *token = strtok(HTTP_header_access_field, ",");
    if (!HTTP_header_access_field || !token)
        return (int) (quality *= 100);

    do {
        while (*token == ' ')
            ++token;

        if (!strncmp(token, "image_struct", strlen("image_struct"))) {
            chr = strrchr(token, '=');
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
        } else if (!strncmp(token, "*", strlen("*"))) {
            chr = strrchr(token, '=');
            if (!chr) {
                others = 1.0;
            } else {
                errno = 0;
                others = strtod(++chr, NULL);
                if (errno != 0)
                    return -1;
            }
        }
    } while ((token = strtok(NULL, ",")));

    if (images > others || (others > images && images != -2.0))
        quality = images;
    else if (others > images && images == -2.0)
        quality = others;
    else
        fprintf(stderr, "get_quality: string: %s\t\tquality: Unexpected error\n", HTTP_header_access_field);

    return (int) (quality *= 100);
}

/*
 * http_fields refers to documentation in split_HTTP_message function
 */
int manage_response(int socket_fd, char **HTTP_message_fields) {

    char *http_response = malloc(DIM * DIM * 2);
    if (!http_response)
        error_found("manage_response: Error in malloc\n");
    memset(http_response, (int) '\0',DIM * DIM * 2);

    // %d status code; %s status code; %s date; %s server; %s content type; %d content's length; %s connection type
    char *HTTP_header = "HTTP/1.1 %d %s\r\nDate: %s\r\nServer: %s\r\nAccept-Ranges: bytes\r\n"
            "Content-Type: %s\r\nContent-Length: %d\r\nConnection: %s\r\n\r\n";
    char *time = get_time();
    char *server_name = "WebServer";
    char *header_ptr;

    if (!HTTP_message_fields[0] || !HTTP_message_fields[1] || !HTTP_message_fields[2] ||
        ((strncmp(HTTP_message_fields[0], "GET", 3) && strncmp(HTTP_message_fields[0], "HEAD", 4)) ||
         (strncmp(HTTP_message_fields[2], "HTTP/1.1", 8) && strncmp(HTTP_message_fields[2], "HTTP/1.0", 8)))) {
        sprintf(http_response, HTTP_header, 400, "Bad Request", time, server_name, "text/html", strlen(HTML_PAGES[2]), "close");
        header_ptr = http_response;
        header_ptr += strlen(http_response);
        memcpy(header_ptr, HTML_PAGES[2], strlen(HTML_PAGES[2]));

        if (send_HTTP_message(socket_fd, http_response, strlen(http_response)) == -1) {
            fprintf(stderr, "Error while sending data to client\n");
            free_time_HTTP_response(time, http_response);
            return -1;
        }
        return 0;
    }

    //when root is requested
    if (strncmp(HTTP_message_fields[1], "/", strlen(HTTP_message_fields[1])) == 0) {
        sprintf(http_response, HTTP_header, 200, "OK", time, server_name, "text/html", strlen(HTML_PAGES[0]), "keep-alive");
        if (strncmp(HTTP_message_fields[0], "HEAD", 4)) {
            header_ptr = http_response;
            header_ptr += strlen(http_response);
            memcpy(header_ptr, HTML_PAGES[0], strlen(HTML_PAGES[0]));
        }

        if (send_HTTP_message(socket_fd, http_response, strlen(http_response)) == -1) {
            fprintf(stderr, "Error while sending data to client\n");
            free_time_HTTP_response(time, http_response);
            return -1;
        }
    }
        /*
         * where the real functions begin
         */
    else {
        struct image_struct *image = image_struct;
        struct image_struct *first_image = image_struct;
        char *image_name;
        //if / is not found enters the if
        if (!(image_name = strrchr(HTTP_message_fields[1], '/')))
            image = NULL;
        ++image_name;
        //make p point to /RESIZED.XXXXXX
        char *p = resized_tmp_dir + strlen("/tmp");

        // Finding image_struct in the image_struct structure
        while (image) {
            /*
             * if image_name equals i->name enters the if,
             * otherwise passes to next image_struct
             */
            if (!strncmp(image_name, image->name, strlen(image->name))) {
                ssize_t image_to_send_size = 0;
                char *image_to_send = NULL;

                int favicon = 1;
                /*
                 *
                 */
                // Looking for resized image_struct or favicon.ico
                if (!strncmp(p, HTTP_message_fields[1], strlen(p) - strlen(".XXXXXX")) ||
                    !(favicon = strncmp(image_name, "favicon.ico", strlen("favicon.ico")))) {
                    if (strncmp(HTTP_message_fields[0], "HEAD", 4)) {
                        if (favicon)
                            image_to_send = get_image(image_name, image->resized_image_size, resized_tmp_dir);
                        else
                            image_to_send = get_image(image_name, image->resized_image_size, images_path);
                        //image_to_send = get_image(image_name, i->resized_image_size, favicon ? resized_tmp_dir : images_path);
                        if (!image_to_send) {
                            fprintf(stderr, "manage_response: Error in get_image\n");
                            free_time_HTTP_response(time, http_response);
                            return -1;
                        }
                    }
                    image_to_send_size = image->resized_image_size;
                }
                    // Looking for image_struct in memory cache_struct
                else {
                    char name_cached_image[DIM / 2];
                    memset(name_cached_image, (int) '\0', sizeof(char) * DIM / 2);
                    //c e' un puntatore d'appoggio
                    struct cache_struct *cache_struct;
                    int default_value = 70;
                    int processing_accept = get_quality(HTTP_message_fields[5]);
                    if (processing_accept == -1)
                        fprintf(stderr, "manage_response: Unexpected error in strtod\n");

                    int quality_factor;
                    if (processing_accept < 0)
                        quality_factor = default_value;
                    else
                        quality_factor = processing_accept;
                    lock(thread_struct.mtx_cache_access);
                    cache_struct = image->cached_image;
                    while (cache_struct) {
                        if (cache_struct->quality == quality_factor) {
                            strcpy(name_cached_image, cache_struct->cached_name);
                            /*
                             * If an image_struct has been accessed, move it on top of the list
                             * in order to keep the image_struct with less hit in the bottom of the list
                             * CACHE_COUNTER = -1 means we don't have cache_struct max length;
                             * if name of cache_head == name of img we have to cache_struct
                             *      the cache_struct list is already updated
                             */
                            look_for_cached_img(CACHE_COUNTER, name_cached_image);
                            break;
                        }
                        cache_struct = cache_struct->next_cached_image;
                    }

                    /*
                     * If image_struct has not been cached yet
                     */
                    if (!cache_struct) {
                        // %s = image_struct's name; %d = factor get_quality (between 1 and 99)
                        sprintf(name_cached_image, "%s_%d", image_name, quality_factor);
                        char path[DIM / 2];
                        memset(path, (int) '\0', DIM / 2);
                        sprintf(path, "%s/%s", cache_tmp_dir, name_cached_image);

                        if (CACHE_COUNTER > 0) {
                            /*
                             * Cache of limited size
                             * If it has not yet reached the maximum cache_struct size
                             * %s/%s = path/name_image; %d = factor get_quality
                             */

                            if (resize_image(images_path, image_name, quality_factor, cache_tmp_dir, name_cached_image)) {
                                fprintf(stderr, "manage_response: error in resize_image\n");
                                free_time_HTTP_response(time, http_response);
                                unlock(thread_struct.mtx_cache_access);
                                return -1;
                            }

                            if (insert_in_cache(path, quality_factor, name_cached_image, image, time, http_response)) {
                                fprintf(stderr, "manage_response: error in insert_in_cache\n");
                                free_time_HTTP_response(time, http_response);
                                unlock(thread_struct.mtx_cache_access);
                                return -1;
                            }
                            /*
                             * filling struct cache_struct of the relative image_struct
                             * and inserting the struct cached_name in the cache_struct list
                             */
                            --CACHE_COUNTER;
                        }

                        else if (!CACHE_COUNTER){
                            /*
                             * Cache server_full. You have to delete an item.
                             * You choose to delete the oldest requested element.
                             */
                            if (delete_image(image_to_send, time, http_response) != 0) {
                                fprintf(stderr, "manage_response: error in delete_image\n");
                                free_time_HTTP_response(time, http_response);
                                unlock(thread_struct.mtx_cache_access);
                                return -1;
                            }

                            if (resize_image(images_path, image_name, quality_factor, cache_tmp_dir, name_cached_image)) {
                                fprintf(stderr, "manage_response: error in resize_image\n");
                                free_time_HTTP_response(time, http_response);
                                unlock(thread_struct.mtx_cache_access);
                                return -1;
                            }
                            //freeing a cache_struct slot
                            if (free_cache_slot(first_image, time, http_response)) {
                                fprintf(stderr, "manage_response: error in free_cache_slot\n");
                                free_time_HTTP_response(time, http_response);
                                unlock(thread_struct.mtx_cache_access);
                                return -1;
                            }

                            if (insert_in_cache(path, quality_factor, name_cached_image, image, time, http_response)) {
                                fprintf(stderr, "manage_response: error in insert_in_cache\n");
                                free_time_HTTP_response(time, http_response);
                                unlock(thread_struct.mtx_cache_access);
                                return -1;
                            }

                        } else {
                            /*
                             * Unlimited cache_struct size
                             */

                            if (resize_image(images_path, image_name, quality_factor, cache_tmp_dir, name_cached_image)) {
                                fprintf(stderr, "manage_response: error in resize_image\n");
                                free_time_HTTP_response(time, http_response);
                                unlock(thread_struct.mtx_cache_access);
                                return -1;
                            }
                            if (insert_in_cache(path, quality_factor, name_cached_image, image, time, http_response)) {
                                fprintf(stderr, "manage_response: error in insert_in_cache\n");
                                free_time_HTTP_response(time, http_response);
                                unlock(thread_struct.mtx_cache_access);
                                return -1;
                            }
                        }
                    }

                    unlock(thread_struct.mtx_cache_access);

                    if (strncmp(HTTP_message_fields[0], "HEAD", 4)) {

                        image_to_send = search_file(image, name_cached_image, image_to_send, cache_struct, time, http_response);
                        if (!image_to_send) {
                            fprintf(stderr, "manage_response: Error in get_image\n");
                            free_time_HTTP_response(time, http_response);
                            return -1;
                        }
                    }
                    image_to_send_size = image->cached_image->cached_image_size;

                } //END if looking in cache_struct

                sprintf(http_response, HTTP_header, 200, "OK", time, server_name, "image/gif", image_to_send_size, "keep-alive");
                ssize_t http_response_header_size = (size_t) strlen(http_response);
                if (strncmp(HTTP_message_fields[0], "HEAD", 4)) {
                    if (http_response_header_size + image_to_send_size > DIM * DIM * 2) {
                        http_response = realloc(http_response, (http_response_header_size + image_to_send_size) * sizeof(char));
                        if (!http_response) {
                            fprintf(stderr, "manage_response: Error in realloc\n");
                            free_time_HTTP_response(time, http_response);
                            free(image_to_send);
                            return -1;
                        }
                        memset(http_response + http_response_header_size, (int) '\0', (size_t) image_to_send_size);
                    }

                    header_ptr = http_response;
                    //points at the end of the HTTP_header
                    header_ptr += http_response_header_size;
                    //writes the image_struct in the body of http response
                    memcpy(header_ptr, image_to_send, (size_t) image_to_send_size);
                    http_response_header_size += image_to_send_size;
                }

                if (send_HTTP_message(socket_fd, http_response, http_response_header_size) == -1) {
                    fprintf(stderr, "manage_response: Error while sending data to client\n");
                    free_time_HTTP_response(time, http_response);
                    return -1;
                }
                free(image_to_send);
                break;
            }
            image = image->next_image;
        }
        //if image_struct has not been found
        if (!image) {
            sprintf(http_response, HTTP_header, 404, "Not Found", time, server_name, "text/html", strlen(HTML_PAGES[1]), "close");
            if (strncmp(HTTP_message_fields[0], "HEAD", 4)) {
                header_ptr = http_response;
                //h points at the start of the body
                header_ptr += strlen(http_response);
                memcpy(header_ptr, HTML_PAGES[1], strlen(HTML_PAGES[1]));
            }

            if (send_HTTP_message(socket_fd, http_response, strlen(http_response)) == -1) {
                fprintf(stderr, "manage_response: Error while sending data to client\n");
                free_time_HTTP_response(time, http_response);
                return -1;
            }
        }
    }
    free_time_HTTP_response(time, http_response);
    return 0;
}

/*
 * Every thread execute this function to deal a connection
 * Analyzes HTTP message
 */
void respond(int socket_fd, struct sockaddr_in client_address) {

    //buffer
    char HTTP_request_buffer[DIM * DIM];
    char *line_request[7];
    ssize_t received;
    struct timeval timeval;
    timeval.tv_sec = 10;
    timeval.tv_usec = 0;

    if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeval, sizeof(struct timeval)) < 0)
        fprintf(stderr, "respond: Error in setsockopt\n");

    int i;

    do {
        memset(HTTP_request_buffer, (int) '\0', 5 * DIM);
        for (i = 0; i < 7; ++i)
            line_request[i] = NULL;

        errno = 0;
        received = recv(socket_fd, HTTP_request_buffer, 5 * DIM, 0);
        if (received == -1) {
            switch (errno) {
                case EFAULT:
                    fprintf(stderr, "The receive  buffer  pointer(s)  point  outside  the  process's address space");
                    break;

                case EINTR:
                    fprintf(stderr, "Timeout receiving from socket\n");
                    break;

                case EBADF:
                    fprintf(stderr, "The argument of recv() is an invalid descriptor: %d\n", socket_fd);
                    break;

                case ENOTSOCK:
                    fprintf(stderr, "The argument of recv() does not refer to a socket\n");
                    break;

                case ECONNREFUSED:
                    fprintf(stderr, "Remote host refused to allow the network connection\n");
                    break;

                case EINVAL:
                    fprintf(stderr, "Invalid argument passed\n");
                    break;

                case EWOULDBLOCK:
                    fprintf(stderr, "Timeout receiving from socket\n");
                    break;

                default:
                    fprintf(stderr, "Error in recv: error while receiving data from client_address\n");
                    break;
            }
            break;
        } else if (received == 0) {
            fprintf(stderr, "Client disconnected\n");
            break;
        } else {
            split_HTTP_message(HTTP_request_buffer, line_request);
            char log_string[DIM / 2];
            memset(log_string, (int) '\0', DIM / 2);
            sprintf(log_string, "\tClient:\t%s\tRequest: '%s %s %s'\n",
                    inet_ntoa(client_address.sin_addr), line_request[0], line_request[1], line_request[2]);
            write_log(log_string);

            if (manage_response(socket_fd, line_request))
                break;
        }
    } while (line_request[3] && !strncmp(line_request[3], "keep-alive", 10));
}

/*
 * This is the routine of all threads.
 * This function is used to manage client's connection
 */
void *manage_connection(void *arg) {

    //join not needed
    if (pthread_detach(pthread_self()) != 0)
        error_found("manage_connection: Error in pthread_detach\n");

    struct threads_sync_struct *threads_sync_struct = (struct threads_sync_struct *) arg;
    struct sockaddr_in client;
    int client_socket_element, socket_fd;

    lock(threads_sync_struct -> mtx_sync_conditions);
    client_socket_element = threads_sync_struct -> client_socket_element;
    signal_t(threads_sync_struct -> th_start);
    unlock(threads_sync_struct -> mtx_sync_conditions);
    lock(threads_sync_struct -> mtx_thread_conn_number);

    if (threads_sync_struct -> client_socket_list[client_socket_element] == -3) {
        // Thread ready for incoming connections
        threads_sync_struct -> client_socket_list[client_socket_element] = -1;
    } else {
        fprintf(stderr, "manage_connection: Unknown error: slot[%d]: %d\n", client_socket_element, threads_sync_struct -> client_socket_list[client_socket_element]);
        pthread_exit(NULL);
    }
    // Deal connections
    while (1) {
        memset(&client, (int) '\0', sizeof(struct sockaddr_in));
        wait_t(threads_sync_struct->threads_cond_list + client_socket_element, threads_sync_struct->mtx_thread_conn_number);
        /*
         * socket_fd values:
         *      -1 -> thread ready for incoming connections
         *      -2 -> thread killed by kill_thread function or thread not yet created
         *      -3 -> newly created thread
         *      >0 -> connection oriented socket file descriptor
         */
        socket_fd = threads_sync_struct->client_socket_list[client_socket_element];
        //if client_socket_list[client_socket_element] socket's thread has been killed than decrease active_threads counter
        if (socket_fd < 0) {
            if (socket_fd != -2) {
                fprintf(stderr, "manage_connection: Unknown error trying to access socket_fd array: %d\n", socket_fd);
                continue;
            }
            --threads_sync_struct->active_threads;
            unlock(threads_sync_struct->mtx_thread_conn_number);
            break;
        }
        /*
         * if socket_fd >= 0:
         * copies the sockaddr_in struct in client
         * and increments the number of connection
         * new connection has been established
         */
        memcpy(&client, &threads_sync_struct->client_address, sizeof(struct sockaddr_in));
        ++threads_sync_struct -> connections;
        /*
         * Analyzes number of connections and active threads
         * and creates new threads if necessary
         */
        spawn_thread(threads_sync_struct);
        unlock(threads_sync_struct -> mtx_thread_conn_number);
        respond(socket_fd, client);

        errno = 0;
        if (close(socket_fd) != 0) {
            switch (errno) {
                case EIO:
                    fprintf(stderr, "manage_connection: I/O error occurred\n");
                    break;

                case EBADF:
                    fprintf(stderr, "manage_connection: Bad file number: %d. Probably client has disconnected\n", socket_fd);
                    break;

                default:
                    fprintf(stderr, "manage_connection: Error in close\n");
            }
        }
        lock(threads_sync_struct -> mtx_thread_conn_number);
        --threads_sync_struct -> connections;
        kill_thread(threads_sync_struct);
        threads_sync_struct -> client_socket_list[client_socket_element] = -1;
        /*
         * signals main thread that server is no more full
         */
        signal_t(threads_sync_struct -> server_full);
    }
    pthread_exit(EXIT_SUCCESS);
}

/*
 * This is the main thread which manage all incoming connections.
 * Once a client send a request to the server, this thread checks if
 * it can process the connection or not.
 * If so, assigns the connection management to a child thread,
 * otherwise it waits on a pthread_cond_t condition,
 * until the system load is not lowered.
 * */
void *main_thread_work(void *arg) {

    struct threads_sync_struct *threads_sync_struct = (struct threads_sync_struct *) arg;
    int conn_socket_fd, i = 0, j;
    struct sockaddr_in client;
    socklen_t socket_size = sizeof(struct sockaddr_in);

    create_thread(catch_user_command, arg);
    initialize_thread(MIN_THREAD_TRESHOLD, manage_connection, arg);

    fprintf(stdout, "\n\n\n-Waiting for incoming connection...\n");
    // Accept connections
    while (1) {
        lock(threads_sync_struct -> mtx_thread_conn_number);
        if (threads_sync_struct -> connections + 1 > MAX_CONNECTION) {
            wait_t(threads_sync_struct -> server_full, threads_sync_struct -> mtx_thread_conn_number); }
        unlock(threads_sync_struct -> mtx_thread_conn_number);
        memset(&client, (int) '\0', socket_size);

        errno = 0;
        conn_socket_fd = accept(LISTEN_SOCKET_DESCRIPTOR, (struct sockaddr *) &client, &socket_size);
        memset(&threads_sync_struct->client_address, (int) '\0', socket_size);
        memcpy(&threads_sync_struct->client_address, &client, socket_size);
        lock(threads_sync_struct -> mtx_thread_conn_number);

        if (conn_socket_fd == -1) {
            switch (errno) {
                case ECONNABORTED:
                    fprintf(stderr, "main_thread_work: The connection has been aborted\n");
                    unlock(threads_sync_struct -> mtx_thread_conn_number);
                    continue;

                case ENOBUFS:
                    error_found("main_thread_work: Not enough free memory\n");

                case ENOMEM:
                    error_found("main_thread_work: Not enough free memory\n");

                case EMFILE:
                    fprintf(stderr, "main_thread_work: Too many open files!\n");
                    wait_t(threads_sync_struct -> server_full, threads_sync_struct -> mtx_thread_conn_number);
                    unlock(threads_sync_struct -> mtx_thread_conn_number);
                    continue;

                case ETIMEDOUT:
                    fprintf(stderr, "main_thread_work: Timeout occured\n");
                    unlock(threads_sync_struct -> mtx_thread_conn_number);
                    continue;

                case EPROTO:
                    fprintf(stderr, "main_thread_work: Protocol error\n");
                    unlock(threads_sync_struct -> mtx_thread_conn_number);
                    continue;

                case EPERM:
                    fprintf(stderr, "main_thread_work: Firewall rules forbid connection\n");
                    unlock(threads_sync_struct -> mtx_thread_conn_number);
                    continue;

                case EBADF:
                    fprintf(stderr, "main_thread_work: Bad file number\n");
                    unlock(threads_sync_struct -> mtx_thread_conn_number);
                    continue;

                default:
                    error_found("main_thread_work: Error in accept\n");
            }
        }

        j = 1;
        while (threads_sync_struct -> client_socket_list[i] != -1) {
            if (j > MAX_CONNECTION) {
                j = -1;
                break;
            }
            i = (i + 1) % MAX_CONNECTION;
            ++j;
        }
        if (j == -1) {
            unlock(threads_sync_struct -> mtx_thread_conn_number);
            continue;
        }
        threads_sync_struct -> client_socket_list[i] = conn_socket_fd;
        signal_t(threads_sync_struct -> threads_cond_list + i);
        i = (i + 1) % MAX_CONNECTION;
        unlock(threads_sync_struct -> mtx_thread_conn_number);
    }
}

int main(int argc, char **argv) {

    if (argc > 11) {
        fprintf(stderr, "main: Too many arguments\n\n");
        user_usage(*argv);
    }

    pthread_mutex_t mtx_s_c, mtx_c, mtx_t;
    pthread_cond_t th_start, full;

    init(argc, argv, &mtx_s_c, &mtx_c, &mtx_t,
         &th_start, &full, &thread_struct);
    // To ignore SIGPIPE
    catch_signal();
    main_thread_work(&thread_struct);
    return EXIT_SUCCESS;
}