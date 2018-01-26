//
// Created by andrea on 12/12/17.
//
#include <ImageMagick-7/MagickWand/MagickWand.h>
#include <ImageMagick-7/MagickCore/MagickCore.h>
#include "functions.h"

int resize_image(char *IMG_PATH, char *image_name, int quality, char *tmp_cache, char *name_cached_image) {

    char image_complete_path[DIM];
    strcpy(image_complete_path, IMG_PATH);
    strcat(image_complete_path, image_name);

    char cached_complete_path[DIM] = "%s/%s";
    strcpy(cached_complete_path, tmp_cache);
    strcat(cached_complete_path, "/");
    strcat(cached_complete_path, name_cached_image);

    MagickWand *wand = NewMagickWand();
    if (!wand) {
        fprintf(stderr, "resize_image: error creating wand\n");
        return -1;
    }

    if (MagickReadImage(wand, image_complete_path) == MagickFalse) {
        fprintf(stderr, "resize_image: error in MagickReadImage function\n");
        return -1;
    }

    size_t original_height = MagickGetImageHeight(wand);
    size_t original_width = MagickGetImageWidth(wand);

    float new_height = (float)original_height * (float)quality/100;
    float new_width = (float)original_width * (float)quality/100;

    if (MagickScaleImage(wand, (size_t)new_width, (size_t)new_height) == MagickFalse) {
        fprintf(stderr, "resize_image: error in MagickScaleImage function\n");
        return -1;
    }

    if (MagickWriteImage(wand, cached_complete_path) == MagickFalse){
        fprintf(stderr, "resize_image: error in MagickWriteImage function");
        return -1;
    }

    if (LOG) {
        char log_message[DIM];
        sprintf(log_message, "resize_image: created new image_struct in cache_struct: %s\n", name_cached_image);
        write_on_stream(log_message, LOG);
    }

    MagickWandTerminus();
    return 0;
}

// Used to get image_struct from file system
char *get_image(char *image_name, size_t image_dim, char *directory) {

    ssize_t left = 0;
    int image_fd;
    char *image_buffer;
    char path[strlen(image_name) + strlen(directory) + 1];
    memset(path, (int) '\0', strlen(image_name) + strlen(directory) + 1);
    sprintf(path, "%s/%s", directory, image_name);
    if (path[strlen(path)] != '\0')
        path[strlen(path)] = '\0';

    errno = 0;
    if ((image_fd = open(path, O_RDONLY)) == -1) {
        switch (errno) {
            case EACCES:
                fprintf(stderr, "get_image: Permission denied\n");
                break;

            case EISDIR:
                fprintf(stderr, "get_image: '%s' is a directory\n", image_name);
                break;

            case ENFILE:
                fprintf(stderr, "get_image: The maximum allowable number of files is currently open in the system\n");
                break;

            case EMFILE:
                fprintf(stderr, "get_image: File descriptors are currently open in the calling process\n");
                break;

            default:
                fprintf(stderr, "Error in get_image\n");
        }
        return NULL;
    }

    errno = 0;
    if (!(image_buffer = malloc(image_dim))) {
        fprintf(stderr, "errno: %d\t\timage_dim: %d\tget_image: Error in malloc\n", errno, (int) image_dim);
        return image_buffer;
    } else {
        memset(image_buffer, (int) '\0', image_dim);
    }

    //if left == 0 exits while
    while ((left = read(image_fd, image_buffer + left, image_dim)))
        image_dim -= left;

    if (close(image_fd)) {
        fprintf(stderr, "get_image: Error closing file\t\tFile Descriptor: %d\n", image_fd);
    }

    return image_buffer;
}