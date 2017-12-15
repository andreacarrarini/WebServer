//
// Created by andrea on 12/12/17.
//
#include <ImageMagick-7/MagickWand/MagickWand.h>
#include <ImageMagick-7/MagickCore/MagickCore.h>
#include "functions.h"

int resize_image(char *IMG_PATH, char *p_name, int quality, char *tmp_cache, char *name_cached_image) {

    char *image_complete_path = "%s/%s";
    sprintf(image_complete_path, IMG_PATH, p_name);

    char *cached_complete_path = "%s/%s";
    sprintf(cached_complete_path, tmp_cache, name_cached_image);

    MagickWand *wand = NewMagickWand();
    if (!wand) {
        fprintf(stderr, "error creating wand\n");
        return -1;
    }
    if (!MagickReadImage(wand, image_complete_path)) {
        fprintf(stderr, "error in MagickReadImage function\n");
    }
    size_t original_height = MagickGetImageHeight(wand);
    size_t original_width = MagickGetImageWidth(wand);

    float new_height = (float)original_height * (float)quality/100;
    float new_width = (float)original_width * (float)quality/100;

    if (!MagickScaleImage(wand, (size_t)new_width, (size_t)new_height)) {
        fprintf(stderr, "error in MagickScaleImage function\n");
        return -1;
    }
    if (!MagickWriteImage(wand, cached_complete_path)){
        fprintf(stderr, "error in MagickWriteImage function");
        return -1;
    }

    if (LOG) {
        char *log_msg = "created new image in cache: %s\n";
        sprintf(log_msg, name_cached_image);
        write_on_stream(log_msg, LOG);
    }

    MagickWandTerminus();

    return 0;
}