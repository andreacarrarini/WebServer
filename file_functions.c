//
// Created by andrea on 30/11/17.
//

#include "structs.h"
#include "functions.h"

char *search_file(struct image_struct *image, char *name_cached_img, char *img_to_send, struct cache_struct *cache_ptr, char *char_time, char *http_response) {

    DIR *dir;
    struct dirent *dirent;
    errno = 0;
    dir = opendir(cache_tmp_dir);
    if (!dir) {
        if (errno == EACCES) {
            fprintf(stderr, "search_file: Error in opendir: Permission denied\n");
            free_time_HTTP_response(char_time, http_response);
            return NULL;
        }
        fprintf(stderr, "search_file: Error in opendir\n");
        free_time_HTTP_response(char_time, http_response);
        return NULL;
    }

    //finding the requested image_struct

    while ((dirent = readdir(dir)) != NULL) {
        if (dirent->d_type == DT_REG) {

            if (!strncmp(dirent->d_name, name_cached_img, strlen(name_cached_img))) {
                cache_ptr = image->cached_image;
                while (cache_ptr) {
                    if (!strncmp(name_cached_img, cache_ptr->cached_name, strlen(name_cached_img))) {
                        img_to_send = get_image(name_cached_img, cache_ptr->cached_image_size, cache_tmp_dir);
                        if (!img_to_send) {
                            fprintf(stderr, "search_file: Error in get_image\n");
                            free_time_HTTP_response(char_time, http_response);
                            return NULL;
                        }
                        break;
                    }
                    cache_ptr = cache_ptr->next_cached_image;
                }
            }
        }
    }

    if (closedir(dir)) {
        fprintf(stderr, "search_file: Error in closedir\n");
        free(img_to_send);
        free_time_HTTP_response(char_time, http_response);
        return NULL;
    }
    return img_to_send;
}

// Used to get information from a file on the file system
//  check values: 1 for check directory
//                0 for check regular files
void get_file_info(struct stat *stat_buf, char *path, int check) {

    memset(stat_buf, (int) '\0', sizeof(struct stat));
    /*char mode[] = "0777";
    int permissions;
    permissions = strtol(mode, 0, 8);
    errno = 0;
    if (chmod(path, permissions)) {
        fprintf(stderr, "get_info(chmod): errno is: %s\n", strerror(errno));
        error_found("get_file_info: failed giving permission to file\n");
    }*/
    errno = 0;
    if (stat(path, stat_buf) != 0) {
        if (errno == ENAMETOOLONG)
            error_found("get_file_info: Path too long\n");
        mode_t permission_bits = stat_buf->st_mode;
        if((permission_bits & S_IRUSR) == 0){
            fprintf(stderr, "get_file_info: User doesn't have read privilages\n");
        }
        error_found("get_file_info: Invalid path\n");
    }
    if (check) {
        if (!S_ISDIR((*stat_buf).st_mode)) {
            error_found("get_file_info: Argument -l: The path is not a directory!\n");
        }
    } else {
        if (!S_ISREG((*stat_buf).st_mode)) {
            error_found("get_file_info: Non-regular files can not be analysed!\n");
        }
    }
}

void write_log(char *log_message) {

    char *time = get_time();
    write_on_stream(time, LOG);
    write_on_stream(log_message, LOG);
    free(time);
}

FILE *open_LOG_file(const char *path_to_LOG_dir) {

    errno = 0;
    char LOG_path[strlen(path_to_LOG_dir) + 4];
    sprintf(LOG_path, "%sLOG", path_to_LOG_dir);
    if (LOG_path[strlen(LOG_path)] != '\0')
        LOG_path[strlen(LOG_path)] = '\0';
    FILE *f = fopen(LOG_path, "a");
    if (!f) {
        if (errno == EACCES)
            error_found("open_LOG_file: Missing permission\n");
        error_found("open_LOG_file: Error in fopen\n");
    }
    return f;
}

/*
 * Used to remove file from file system
 */
void remove_link(char *path) {

    //removes the name from FS, if it was the last occurrence file is deleted
    if (unlink(path)) {
        errno = 0;
        switch (errno) {
            case EBUSY:
                error_found("remove_link: File cannot be unlinked: It is being used by the system\n");

            case EIO:
                error_found("remove_link: File cannot be unlinked: An I/O error occurred\n");

            case ENAMETOOLONG:
                error_found("remove_link: File cannot be unlinked: Pathname too long\n");

            case ENOMEM:
                error_found("remove_link: File cannot be unlinked: Insufficient kernel memory\n");

            case EPERM:
                error_found("remove_link: File cannot be unlinked: The file system does not allow unlinking of files\n");

            case EROFS:
                error_found("remove_link: File cannot be unlinked: File is read-only\n");

            default:
                error_found("remove_link: File cannot be unlinked: Error unlinking\n");
        }
    }
}


