
#include <stdio.h>
#include <stdlib.h>
#include "jpeglib.h"
#include <setjmp.h>

#include "common.h"
#include "ImageBuffer.h"

#ifndef IMAGESTACKER_JPEG_H
#define IMAGESTACKER_JPEG_H

GLOBAL(void)write_JPEG_file (ImageBuffer * image_buffer, const char * filename, int quality);

ImageBuffer * read_JPEG_file (const char * filename);

#endif //IMAGESTACKER_JPEG_H
