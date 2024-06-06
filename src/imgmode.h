#ifndef IMGMODE_H
#define IMGMODE_H
#include "extern-c-helper.h"
EXTERN_C_START()

typedef enum 
{
    IMAGE_STRETCH,
    IMAGE_TILE,
    IMAGE_FILL,
    IMAGE_FIT_WIDTH,
    IMAGE_FIT_HEIGHT,
    
    IMAGE_MODE_FIRST = IMAGE_STRETCH,
    IMAGE_MODE_LAST  = IMAGE_FIT_HEIGHT
} imgmode;


EXTERN_C_END()
#endif /* IMGMODE_H */
