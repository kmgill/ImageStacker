
#ifndef IMAGESTACKER_OFFSET_H
#define IMAGESTACKER_OFFSET_H

typedef struct _Offset {
    float horiz;
    float vert;
} Offset;


Offset * allocate_offset();
void destroy_offset(Offset * offset);


#endif //IMAGESTACKER_OFFSET_H
