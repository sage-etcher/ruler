#ifndef DEFAULT_H
#define DEFAULT_H
#include "extern-c-helper.h"
EXTERN_C_START

#define DEFAULT_PRIORITY  SDL_LOG_PRIORITY_ERROR

#define WINDOW_MIN_WIDTH  10
#define WINDOW_MIN_HEIGHT 10

#define DEFAULT_WIDTH     500
#define DEFAULT_HEIGHT    50

#define DEFAULT_OPACITY   0.8f

#define DEFAULT_COLOR     0xffaabb
#define DEFAULT_IMAGE     NULL
#define DEFAULT_IMG_MODE  IMAGE_FILL

/* silences some uneeded compiler warnings, at the cost of compatibility */
/* may need to be undefined if compiler does not support it */
#define USE_PRAGMAS

EXTERN_C_END
#endif /* DEFAULT_H */
