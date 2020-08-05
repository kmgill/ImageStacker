
#include "common.h"
#include "ImageBuffer.h"
#include "Offset.h"
#include "jpeg.h"
#include <string.h>
#include <type_traits>

#ifndef IMAGESTACKER_IMAGE_H
#define IMAGESTACKER_IMAGE_H

/**
 * A rather simplistic three-channel image representation
 */
template <typename T>
class Image {
private:
    _ImageBuffer<T> * buffer;
protected:



public:
    Image(_ImageBuffer<T> * _buffer) : buffer(_buffer) { };
    Image(int width, int height) {
        this->buffer = allocate_image_buffer<T>(width, height);
    }

    ~Image() {
        if (this->buffer != NULL) {
            destroy_image_buffer<T>(this->buffer);
            this->buffer = NULL;
        }
    }

    int width() {
        return this->buffer->width;
    }

    int height() {
        return this->buffer->height;
    }

    _RGB<T> * getPixel(int x, int y) {
        return get_image_buffer_pixel<T>(this->buffer, x, y);
    }

    int rgb2Gray(_RGB<T> * rgb) {
        float _r = (float) rgb->red;
        float _g = (float) rgb->green;
        float _b = (float) rgb->blue;

        float _gray = 0.2989 * _r + 0.5870 * _g + 0.1140 * _b;

        int gray = round(_gray);
        return gray;
    }

    Offset * calculateSimpleCenterOffset(float threshold) {
        float Ox = 0.0;
        float Oy = 0.0;
        int count = 0;
        Offset * offset = allocate_offset();

        for (int x = 0; x < this->buffer->width; x++) {
            for (int y = 0; y < this->buffer->height; y++) {
                _RGB<T> * rgb = getPixel(x, y);
                float gray = (float) rgb2Gray(rgb);

                if (gray > threshold) {
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

    static color_f getInterpolatedChannelValue(color c00, color c01, color c10, color c11, float x, float y) {
        color_f v00 = (color_f) c00;
        color_f v01 = (color_f) c01;
        color_f v10 = (color_f) c10;
        color_f v11 = (color_f) c11;
        color_f v0 = v10 * y + v00 * (1.0 - y);
        color_f v1 = v11 * y + v01 * (1.0 - y);
        color_f v = v1 * x + v0 * (1.0 - x);
        return v;
    }

    void getInterpolatedPixelValue(float Ox, float Oy, RGBf * rgb_f) {
        int Oxf = floor(Ox);
        int Oxc = Oxf + 1;
        int Oyf = floor(Oy);
        int Oyc = Oyf + 1;

        float x = Ox - floor(Ox);
        float y = Oy - floor(Oy);

        _RGB<T> * v00 = getPixel(Oxf, Oyf);
        _RGB<T> * v01 = getPixel(Oxc, Oyf);
        _RGB<T> * v10 = getPixel(Oxf, Oyc);
        _RGB<T> * v11 = getPixel(Oxc, Oyc);

        if (NOTNULL(v00) && NOTNULL(v01) && NOTNULL(v10) && NOTNULL(v11)) {
            rgb_f->red = getInterpolatedChannelValue(v00->red, v01->red, v10->red, v11->red, x, y);
            rgb_f->green = getInterpolatedChannelValue(v00->green, v01->green, v10->green, v11->green, x, y);
            rgb_f->blue = getInterpolatedChannelValue(v00->blue, v01->blue, v10->blue, v11->blue, x, y);
        }
    }

    void shiftImageCenter(float threshold) {
        Offset * offset = this->calculateSimpleCenterOffset(threshold);
        this->shiftImageCenter(offset);
        destroy_offset(offset);
    }

    void shiftImageCenter(Offset * offset) {
        _ImageBuffer<T> * tmp_image = allocate_image_buffer<T>(this->buffer->width, this->buffer->height);
        RGBf rgbf;

        for (int x = 0; x < this->buffer->width; x++) {
            for (int y = 0; y < this->buffer->height; y++) {
                float Ox = x - offset->horiz;
                float Oy = y - offset->vert;
                if (_BETWEEN(Ox, 0.0, (float)this->buffer->width - 1) && _BETWEEN(Oy, 0.0, (float)this->buffer->height - 1)) {
                    getInterpolatedPixelValue(Ox, Oy, &rgbf);
                    _RGB<T> * tmp_rgb = get_image_buffer_pixel(tmp_image, x, y);
                    tmp_rgb->red = round(rgbf.red);
                    tmp_rgb->green = round(rgbf.green);
                    tmp_rgb->blue = round(rgbf.blue);
                }
            }
        }

        copy_image_buffer<T, T>(tmp_image, this->buffer);
        destroy_image_buffer<T>(tmp_image);
    }

    void save(const char * path, int quality=100) {
        //NEEDS TO CONVERT TO UINT8: write_JPEG_file(this->buffer, path, quality);

        _ImageBuffer<color> * save_buffer = allocate_image_buffer<color>(this->buffer->width, this->buffer->height);
        copy_image_buffer<T, color>(this->buffer, save_buffer);
        write_JPEG_file(save_buffer, path, quality);

        destroy_image_buffer(save_buffer);
    }

    static Image<color> * openUint8(const char * path) {
        _ImageBuffer<color> * image_buffer = read_JPEG_file(path);
        Image<color> * result = new Image<color>(image_buffer);
        return result;
    }

    static Image<color_f> * openFloat(const char * path) {
        _ImageBuffer<color> * image_buffer = read_JPEG_file(path);
        _ImageBuffer<color_f> * image_buffer_t = allocate_image_buffer<color_f>(image_buffer->width, image_buffer->height);
        copy_image_buffer<color, color_f>(image_buffer, image_buffer_t);
        destroy_image_buffer(image_buffer);
        Image<color_f> * result = new Image<color_f>(image_buffer_t);
        return result;
    }
};



#endif //IMAGESTACKER_IMAGE_H
