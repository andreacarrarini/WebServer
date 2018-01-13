//
// Created by andrea on 12/12/17.
//
#include <ImageMagick-7/MagickWand/MagickWand.h>
#include <ImageMagick-7/MagickCore/MagickCore.h>
#include "functions.h"

int resize_image(char *IMG_PATH, char *p_name, int quality, char *tmp_cache, char *name_cached_image) {

    fprintf(stderr, "resize_image\n");

    char image_complete_path[DIM];
    strcpy(image_complete_path, IMG_PATH);
    strcat(image_complete_path, p_name);

    char cached_complete_path[DIM] = "%s/%s";
    strcpy(cached_complete_path, tmp_cache);
    strcat(cached_complete_path, "/");
    strcat(cached_complete_path, name_cached_image);

    MagickWand *wand = NewMagickWand();
    if (!wand) {
        fprintf(stderr, "error creating wand\n");
        return -1;
    }

    if (MagickReadImage(wand, image_complete_path) == MagickFalse) {
        fprintf(stderr, "error in MagickReadImage function\n");
        return -1;
    }

    size_t original_height = MagickGetImageHeight(wand);
    size_t original_width = MagickGetImageWidth(wand);

    float new_height = (float)original_height * (float)quality/100;
    float new_width = (float)original_width * (float)quality/100;

    if (MagickScaleImage(wand, (size_t)new_width, (size_t)new_height) == MagickFalse) {
        fprintf(stderr, "error in MagickScaleImage function\n");
        return -1;
    }

/*    struct stat buf;
    memset(&buf, (int) '\0', sizeof(struct stat));
    errno = 0;
    if (stat(image_complete_path, &buf) != 0) {
        if (errno == ENAMETOOLONG)
            error_found("Path too long\n");
        fprintf(stderr, "resize_image: stat: errno is: %d\n", errno);
        error_found("resize_image: stat: Invalid path\n");
    }

    mode_t bits = buf.st_mode;
    if((bits & S_IRUSR) == 0){
        fprintf(stderr, "resize_image: User doesn't have read privilages\n");
    }
    if((bits & S_IWUSR) == 0){
        fprintf(stderr, "resize_image: User doesn't have write privilages\n");
    }

    memset(&buf, (int) '\0', sizeof(struct stat));
    errno = 0;
    if (stat(image_complete_path, &buf) != 0) {
        if (errno == ENAMETOOLONG)
            error_found("Path too long\n");
        fprintf(stderr, "resize_image2: stat: errno is: %d\n", errno);
        error_found("resize_image2: stat: Invalid path\n");
    }

    bits = buf.st_mode;
    fprintf(stderr, "%d\n", bits);
    fprintf(stderr, "%s\n", cached_complete_path);
    if((bits & S_IRUSR) == 0){
        fprintf(stderr, "resize_image2: User doesn't have read privilages\n");
    }
    if((bits & S_IWUSR) == 0){
        fprintf(stderr, "resize_image2: User doesn't have write privilages\n");
    }*/

    if (MagickWriteImage(wand, cached_complete_path) == MagickFalse){
        fprintf(stderr, "error in MagickWriteImage function");
        return -1;
    }

    if (LOG) {
        char *log_msg = malloc(DIM);
        //log_msg = "created new image in cache: %s\n";
        //char *log_msg = "created new image in cache: %s\n";
        strcpy(log_msg, name_cached_image);
        write_on_stream(log_msg, LOG);
    }

    fprintf(stderr, "resize_image: name_cached_image is: %s\n", name_cached_image);

    MagickWandTerminus();

    return 0;
}