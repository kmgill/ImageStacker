
#include "Image.h"
#include "ImageBuffer.h"
#include "Offset.h"
#include "jpeg.h"
#include <math.h>
#include <string.h>
#include <iostream>


Image::Image(_ImageBuffer<color_f> * _buffer)
    : buffer(_buffer) {

}


Image::Image(int width, int height) {
    this->buffer = allocate_image_buffer<color_f>(width, height);
}


Image::~Image() {
    if (this->buffer != NULL) {
        destroy_image_buffer<color_f>(this->buffer);
        this->buffer = NULL;
    }
}

int Image::width() {
    return this->buffer->width;
}

int Image::height() {
    return this->buffer->height;
}

RGBf * Image::getPixel(int x, int y) {
    return get_image_buffer_pixel<color_f>(this->buffer, x, y);
}

int Image::rgb2Gray(RGBf * rgb) {
    float _r = (float) rgb->red;
    float _g = (float) rgb->green;
    float _b = (float) rgb->blue;

    float _gray = 0.2989 * _r + 0.5870 * _g + 0.1140 * _b;

    int gray = round(_gray);
    return gray;
}

Offset * Image::calculateSimpleCenterOffset(float threshold) {
    float Ox = 0.0;
    float Oy = 0.0;
    int count = 0;
    Offset * offset = allocate_offset();

    for (int x = 0; x < this->buffer->width; x++) {
        for (int y = 0; y < this->buffer->height; y++) {
            RGBf * rgb = getPixel(x, y);
            float gray = (float) rgb2Gray(rgb);

            if (gray >= threshold) {
                Ox += x;
                Oy += y;
                count++;
            }
        }
    }

    if (count > 0) {
        Ox = ((float)this->buffer->width / 2.0) - (Ox / (float) count);
        Oy = ((float)this->buffer->height / 2.0) - (Oy / (float) count);
    }

    offset->horiz = Ox;
    offset->vert = Oy;

    return offset;
}

color_f Image::getInterpolatedChannelValue(color_f c00, color_f c01, color_f c10, color_f c11, float x, float y) {
    color_f v00 = (color_f) c00;
    color_f v01 = (color_f) c01;
    color_f v10 = (color_f) c10;
    color_f v11 = (color_f) c11;
    color_f v0 = v10 * y + v00 * (1.0 - y);
    color_f v1 = v11 * y + v01 * (1.0 - y);
    color_f v = v1 * x + v0 * (1.0 - x);
    return v;
}

void Image::getInterpolatedPixelValue(float Ox, float Oy, RGBf * rgb_f) {
    int Oxf = floor(Ox);
    int Oxc = Oxf + 1;
    int Oyf = floor(Oy);
    int Oyc = Oyf + 1;

    float x = Ox - floor(Ox);
    float y = Oy - floor(Oy);

    RGBf * v00 = getPixel(Oxf, Oyf);
    RGBf * v01 = getPixel(Oxc, Oyf);
    RGBf * v10 = getPixel(Oxf, Oyc);
    RGBf * v11 = getPixel(Oxc, Oyc);

    if (NOTNULL(v00) && NOTNULL(v01) && NOTNULL(v10) && NOTNULL(v11)) {
        rgb_f->red = getInterpolatedChannelValue(v00->red, v01->red, v10->red, v11->red, x, y);
        rgb_f->green = getInterpolatedChannelValue(v00->green, v01->green, v10->green, v11->green, x, y);
        rgb_f->blue = getInterpolatedChannelValue(v00->blue, v01->blue, v10->blue, v11->blue, x, y);
    }
}

void Image::shiftImageCenter(float threshold) {
    Offset * offset = this->calculateSimpleCenterOffset(threshold);
    this->shiftImageCenter(offset);
    destroy_offset(offset);
}

void Image::shiftImageCenter(Offset * offset) {
    _ImageBuffer<color_f> * tmp_image = allocate_image_buffer<color_f>(this->buffer->width, this->buffer->height);
    RGBf rgbf;

    for (int x = 0; x < this->buffer->width; x++) {
        for (int y = 0; y < this->buffer->height; y++) {
            float Ox = (float)x - offset->horiz;
            float Oy = (float)y - offset->vert;
            if (_BETWEEN(Ox, 0.0, (float)this->buffer->width - 1) && _BETWEEN(Oy, 0.0, (float)this->buffer->height - 1)) {
                getInterpolatedPixelValue(Ox, Oy, &rgbf);
                RGBf * tmp_rgb = get_image_buffer_pixel<color_f>(tmp_image, x, y);
                tmp_rgb->red = rgbf.red;
                tmp_rgb->green = rgbf.green;
                tmp_rgb->blue = rgbf.blue;
            }
        }
    }

    // TODO: Could probably get away with a memcpy here...
    copy_image_buffer<color_f, color_f>(tmp_image, this->buffer);

    destroy_image_buffer<color_f>(tmp_image);
}

void Image::save(const char * path, int quality) {
    //NEEDS TO CONVERT TO UINT8: write_JPEG_file(this->buffer, path, quality);
    _ImageBuffer<color_f> * scale_buffer = allocate_image_buffer<color_f>(this->buffer->width, this->buffer->height);
    copy_image_buffer<color_f, color_f>(this->buffer, scale_buffer);
    scale_image_buffer<color_f>(scale_buffer, 0.0, 1.0, 0.0, 255.0);

    _ImageBuffer<color> * save_buffer = allocate_image_buffer<color>(this->buffer->width, this->buffer->height);
    copy_image_buffer<color_f, color>(scale_buffer, save_buffer);
    write_JPEG_file(save_buffer, path, quality);

    destroy_image_buffer(scale_buffer);
    destroy_image_buffer(save_buffer);
}

Image * Image::open(const char * path) {
    // Some of this will move once I add png/etc support
    _ImageBuffer<color> * image_buffer = read_JPEG_file(path);
    _ImageBuffer<color_f> * image_buffer_t = allocate_image_buffer<color_f>(image_buffer->width, image_buffer->height);
    copy_image_buffer<color, color_f>(image_buffer, image_buffer_t);
    destroy_image_buffer<color>(image_buffer);
    normalize_image_buffer<color_f>(image_buffer_t, 0.0, 255.0);

    Image * result = new Image(image_buffer_t);

    return result;
}
