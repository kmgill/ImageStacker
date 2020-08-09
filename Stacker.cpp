
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


/**
 * Adds an image to the stack
 */
void Stacker::add(Image * image) {

    // TODO: This is bad. Should overlay regardless of size.
    if ((!(this->buffer->width == image->width())) || (!(this->buffer->height == image->height()))) {
        return; // Raise!
    }

    RGBf thisBufferRGB;
    RGBf addingBufferRGB;

    for (int x = 0; x < this->buffer->width; x++) {
        for (int y = 0; y < this->buffer->height; y++) {
            int a = get_image_buffer_pixel<color_f>(this->buffer, x, y, &thisBufferRGB);
            int b = image->getPixel(x, y, &addingBufferRGB);
            if (NOTZERO(a) && NOTZERO(b)) {
                thisBufferRGB.red += addingBufferRGB.red;
                thisBufferRGB.green += addingBufferRGB.green;
                thisBufferRGB.blue += addingBufferRGB.blue;
                set_image_buffer_pixel<color_f>(this->buffer, x, y, &thisBufferRGB);
                this->count[y * this->buffer->width + x]++;
            }
        }
    }
}

/**
 * Finalizes the averaging computation
 */
void Stacker::finalize() {
    RGBf thisBufferRGB;

    for (int x = 0; x < this->buffer->width; x++) {
        for (int y = 0; y < this->buffer->height; y++) {
            float pixelCount = (float) this->count[y * this->buffer->width + x];

            if (pixelCount > 0.0) {
                if (NOTZERO(get_image_buffer_pixel<color_f>(this->buffer, x, y, &thisBufferRGB))) {
                    thisBufferRGB.red /= pixelCount;
                    thisBufferRGB.green /= pixelCount;
                    thisBufferRGB.blue /= pixelCount;
                    this->count[y * this->buffer->width + x] = 0;
                    set_image_buffer_pixel<color_f>(this->buffer, x, y, &thisBufferRGB);
                }
            }
        }
    }
}

/**
 * Exports the image.
 */
void Stacker::save(const char * path, int quality) {
    //NEEDS TO CONVERT TO UINT8: write_JPEG_file(this->buffer, path, quality);
    _ImageBuffer<color_f> * scale_buffer = allocate_image_buffer<color_f>(this->buffer->width, this->buffer->height);
    copy_image_buffer<color_f, color_f>(this->buffer, scale_buffer);
    scale_image_buffer<color_f>(scale_buffer, 0.0, 1.0, 0.0, 255.0);

    _ImageBuffer<color> * save_buffer = allocate_image_buffer<color>(this->buffer->width, this->buffer->height);
    copy_image_buffer<color_f, color>(scale_buffer, save_buffer, true);
    write_JPEG_file(save_buffer, path, quality);

    destroy_image_buffer(scale_buffer);
    destroy_image_buffer(save_buffer);
}
