
#include "common.h"
#include <stdlib.h>
#include <math.h>
#include <iostream>

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
int get_image_buffer_pixel(struct _ImageBuffer<T> * image, int x, int y, struct _RGB<T> * rgb) {
    if (image == NULL) {
        return 0;
    }

    if (image->buffer == NULL) {
        return 0;
    }

    if (x < 0 || x >= image->width) {
        return 0;
    }

    if (y < 0 || y >= image->height) {
        return 0;
    }

    struct _RGB<T> * _rgb = & image->buffer[image->width * y + x];
    rgb->red = _rgb->red;
    rgb->green = _rgb->green;
    rgb->blue = _rgb->blue;

    return 1;
}

template<typename T>
int set_image_buffer_pixel(struct _ImageBuffer<T> * image, int x, int y, struct _RGB<T> * rgb) {
    if (image == NULL) {
        return 0;
    }

    if (image->buffer == NULL) {
        return 0;
    }

    if (x < 0 || x >= image->width) {
        return 0;
    }

    if (y < 0 || y >= image->height) {
        return 0;
    }

    struct _RGB<T> * _rgb = & image->buffer[image->width * y + x];
    _rgb->red = rgb->red;
    _rgb->green = rgb->green;
    _rgb->blue = rgb->blue;

    return 1;
}


/**
 * Fills an _ImageBuffer with zeros.
 */
template<typename T>
void zero_image_buffer(struct _ImageBuffer<T> * image_buffer) {
    struct _RGB<T> rgb;
    rgb.red = rgb.green = rgb.blue = 0;

    for (int x = 0; x < image_buffer->width; x++) {
        for (int y = 0; y < image_buffer->height; y++) {
            set_image_buffer_pixel<T>(image_buffer, x, y, &rgb);
        }
    }
}


/**
 * Scales a value from a from min/max to a to min/max.
 */
template<typename T>
T scale_pixel_value(T pixel_value, T from_min, T from_max, T to_min, T to_max) {

    // Ensure value isn't less than the permitted minimum
    pixel_value = (pixel_value > from_min) ? pixel_value : from_min;

    // Ensure the value doesn't exceed the permitted maximum
    pixel_value = (pixel_value < from_max) ? pixel_value : from_max;

    // Scale it.
    pixel_value = ((pixel_value - from_min) / (from_max - from_min)) * (to_max - to_min) + to_min;
    return pixel_value;
}

/**
 * Scales values from a from min/max to a to min/max.
 */
template<typename T>
void scale_image_buffer(struct _ImageBuffer<T> * image_buffer, float from_min, float from_max, float to_min, float to_max) {
    struct _RGB<T> rgb;
    for (int x = 0; x < image_buffer->width; x++) {
        for (int y = 0; y < image_buffer->height; y++) {
            if (get_image_buffer_pixel<T>(image_buffer, x, y, &rgb) == 1) {
                rgb.red = scale_pixel_value<T>(rgb.red, from_min, from_max, to_min, to_max);
                rgb.green = scale_pixel_value<T>(rgb.green, from_min, from_max, to_min, to_max);
                rgb.blue = scale_pixel_value<T>(rgb.blue, from_min, from_max, to_min, to_max);
                set_image_buffer_pixel<T>(image_buffer, x, y, &rgb);
            }
        }
    }
}

/**
 * Normalizes data from min/max to 0.0/1.0. Don't bother with int-based image buffers.
 */
template<typename T>
void normalize_image_buffer(struct _ImageBuffer<T> * image_buffer, float from_min, float from_max) {
    scale_image_buffer<T>(image_buffer, from_min, from_max, 0.0, 1.0);
}


/*
 * This is slow....
 */
template<typename A, typename B>
int copy_image_buffer(_ImageBuffer<A> * src, _ImageBuffer<B> * dest, bool do_round = false) {
    if (src->width != dest->width || src->height != dest->height) {
        return 0;
    }

    for (int i = 0; i < src->width * src->height; i++) {
        _RGB<A> * src_rgb = &src->buffer[i];
        _RGB<B> * dest_rgb = &dest->buffer[i];

        if (do_round == true) {
            dest_rgb->red = round(src_rgb->red);
            dest_rgb->green = round(src_rgb->green);
            dest_rgb->blue = round(src_rgb->blue);
        } else {
            dest_rgb->red = src_rgb->red;
            dest_rgb->green = src_rgb->green;
            dest_rgb->blue = src_rgb->blue;
        }
    }

    return 1;
}

template<typename T>
void print_image_buffer_stats(_ImageBuffer<T> * buffer) { // For debugging. Delete at some point.
    float min = 99999;
    float max = 0;
    RGBf rgb;
    for (int x = 0; x < buffer->width; x++) {
        for (int y = 0; y < buffer->height; y++) {
            get_image_buffer_pixel<T>(buffer, x, y, &rgb);
            min = (rgb.red < min) ? rgb.red : min;
            max = (rgb.red > max) ? rgb.red : max;
        }
    }
    std::cout << "Min: " << min << ", Max: " << max << std::endl;
}

#endif //IMAGESTACKER_IMAGEBUFFER_H
