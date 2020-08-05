
#ifndef IMAGESTACKER_STACKER_H
#define IMAGESTACKER_STACKER_H

#include "common.h"
#include "Image.h"
#include "ImageBuffer.h"

class Stacker {
protected:
    _ImageBuffer<color_f> * buffer;
    int * count;
public:
    Stacker(int width, int height);
    ~Stacker();

    void add(Image<color_f> * image);
    void add(Image<color> * image);

    void finalize();

    void save(const char * path, int quality = 100);
};


#endif //IMAGESTACKER_STACKER_H
