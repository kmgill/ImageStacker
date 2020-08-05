
#ifndef IMAGESTACKER_COMMON_H
#define IMAGESTACKER_COMMON_H


#ifndef _BETWEEN
#define _BETWEEN(t, a, b) (t >= a && t <= b)
#endif

#ifndef NOTNULL
#define NOTNULL(x) (x != NULL)
#endif

#ifndef NULL
#define NULL 0x0
#endif


typedef unsigned char color;
typedef float color_f;

template <typename T>
struct _RGB {
    T red;
    T green;
    T blue;
};

typedef struct _RGB<color> RGB;
typedef struct _RGB<color_f> RGBf;


#endif //IMAGESTACKER_COMMON_H
