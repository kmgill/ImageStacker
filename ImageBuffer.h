
#include "common.h"
#include <stdlib.h>
#include <math.h>

#ifndef IMAGESTACKER_IMAGEBUFFER_H
#define IMAGESTACKER_IMAGEBUFFER_H

/**
 * A basic image buffer with memory for pixels and values for height and width.
 */
template<typename T>
struct _ImageBuffer {
    struct _RGB<T> * buffer;
    int width;
    int height;
} ;

typedef struct _ImageBuffer<color> ImageBuffer;
typedef struct _ImageBuffer<color_f> ImageBufferF;

template<typename T>
struct _ImageBuffer<T> * allocate_image_buffer(int width, int height) {
    struct _ImageBuffer<T> * image_buffer = (_ImageBuffer<T> * ) malloc(sizeof(_ImageBuffer<T>));

    image_buffer->buffer = (_RGB<T> *) malloc(sizeof(_RGB<T>) * width * height);
    image_buffer->width = width;
    image_buffer->height = height;

    zero_image_buffer(image_buffer);

    return image_buffer;
}

template<typename T>
void destroy_image_buffer(struct _ImageBuffer<T> * image_buffer) {
    if (image_buffer->buffer != NULL) {
        free(image_buffer->buffer);
        image_buffer->buffer = NULL;
    }

    free(image_buffer);
}

template<typename T>
void zero_image_buffer(struct _ImageBuffer<T> * image_buffer) {
    for (int x = 0; x < image_buffer->width; x++) {
        for (int y = 0; y < image_buffer->height; y++) {
            struct _RGB<T> * rgb = get_image_buffer_pixel(image_buffer, x, y);
            rgb->red = rgb->green = rgb->blue = 0x0;
        }
    }
}

template<typename T>
struct _RGB<T> * get_image_buffer_pixel(struct _ImageBuffer<T> * image, int x, int y) {
    if (image == NULL) {
        return NULL;
    }

    if (image->buffer == NULL) {
        return NULL;
    }

    if (x < 0 || x >= image->width) {
        return NULL;
    }

    if (y < 0 || y >= image->height) {
        return NULL;
    }

    struct _RGB<T> * rgb = & image->buffer[image->width * y + x];
    return rgb;
}

/*
 * This is slow....
 */
template<typename A, typename B>
int copy_image_buffer(_ImageBuffer<A> * src, _ImageBuffer<B> * dest) {
    if (src->width != dest->width || src->height != dest->height) {
        return 0;
    }

    for (int i = 0; i < src->width * src->height; i++) {
        _RGB<A> * src_rgb = &src->buffer[i];
        _RGB<B> * dest_rgb = &dest->buffer[i];
        dest_rgb->red = src_rgb->red;
        dest_rgb->green = src_rgb->green;
        dest_rgb->blue = src_rgb->blue;
    }

    return 1;
}

#endif //IMAGESTACKER_IMAGEBUFFER_H
