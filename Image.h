
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
class Image {
private:
    _ImageBuffer<color_f> * buffer;
protected:
    
public:
    Image(_ImageBuffer<color_f> * _buffer);
    Image(int width, int height);

    ~Image();

    int width();
    int height();

    RGBf * getPixel(int x, int y);

    static int rgb2Gray(RGBf * rgb);

    Offset * calculateSimpleCenterOffset(float threshold);

    static color_f getInterpolatedChannelValue(color_f c00, color_f c01, color_f c10, color_f c11, float x, float y);

    void getInterpolatedPixelValue(float Ox, float Oy, RGBf * rgb_f);

    void shiftImageCenter(float threshold);

    void shiftImageCenter(Offset * offset);

    void save(const char * path, int quality=100);

    static Image * open(const char * path);
};



#endif //IMAGESTACKER_IMAGE_H
