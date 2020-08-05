
#include <stdlib.h>
#include "common.h"
#include "Offset.h"


Offset * allocate_offset() {
    Offset * offset = (Offset *) malloc(sizeof(Offset));
    offset->vert = 0.0;
    offset->horiz = 0.0;
    return offset;
}

void destroy_offset(Offset * offset) {
    if (offset != NULL) {
        free(offset);
    }
}