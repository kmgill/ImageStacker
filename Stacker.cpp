
#include "common.h"
#include "ImageBuffer.h"
#include "Image.h"
#include "Stacker.h"
#include <stdlib.h>
#include <iostream>


Stacker::Stacker(int width, int height) {
    this->buffer = allocate_image_buffer<color_f>(width, height);
    this->count = new int[height * width];
    for (int i = 0; i < height * width; i++) {
        this->count[i] = 0;
    }
}


Stacker::~Stacker() {

    destroy_image_buffer<color_f>(this->buffer);
    this->buffer = NULL;

    delete(this->count);
    this->count = NULL;
}



void Stacker::add(Image<color_f> * image) {
    if ((!(this->buffer->width == image->width())) || (!(this->buffer->height == image->height()))) {
        return; // Raise!
    }

    for (int x = 0; x < this->buffer->width; x++) {
        for (int y = 0; y < this->buffer->height; y++) {
            RGBf * thisBufferRGB = get_image_buffer_pixel<color_f>(this->buffer, x, y);
            RGBf * addingBufferRGB = image->getPixel(x, y);
            thisBufferRGB->red += addingBufferRGB->red;
            thisBufferRGB->green += addingBufferRGB->green;
            thisBufferRGB->blue += addingBufferRGB->blue;
            this->count[y * this->buffer->width + x]++;
        }
    }
}


void Stacker::add(Image<color> * image) {
    if ((!(this->buffer->width == image->width())) || (!(this->buffer->height == image->height()))) {
        return; // Raise!
    }

    for (int x = 0; x < this->buffer->width; x++) {
        for (int y = 0; y < this->buffer->height; y++) {
            RGBf * thisBufferRGB = get_image_buffer_pixel<color_f>(this->buffer, x, y);
            RGB * addingBufferRGB = image->getPixel(x, y);
            thisBufferRGB->red += (color_f) addingBufferRGB->red;
            thisBufferRGB->green += (color_f) addingBufferRGB->green;
            thisBufferRGB->blue += (color_f) addingBufferRGB->blue;
            this->count[y * this->buffer->width + x]++;
        }
    }
}


void Stacker::finalize() {
    for (int x = 0; x < this->buffer->width; x++) {
        for (int y = 0; y < this->buffer->height; y++) {
            float pixelCount = (float) this->count[y * this->buffer->width + x];
            if (pixelCount > 0.0) {
                RGBf * thisBufferRGB = get_image_buffer_pixel<color_f>(this->buffer, x, y);

                thisBufferRGB->red /= pixelCount;
                thisBufferRGB->green /= pixelCount;
                thisBufferRGB->blue /= pixelCount;
                //std::cout << (float) thisBufferRGB->red << ", " << pixelCount << std::endl;
                this->count[y * this->buffer->width + x] = 0;
            }
        }
    }
}

void Stacker::save(const char * path, int quality) {
    //NEEDS TO CONVERT TO UINT8: write_JPEG_file(this->buffer, path, quality);

    _ImageBuffer<color> * save_buffer = allocate_image_buffer<color>(this->buffer->width, this->buffer->height);
    copy_image_buffer<color_f, color>(this->buffer, save_buffer);
    write_JPEG_file(save_buffer, path, quality);

    destroy_image_buffer(save_buffer);
}