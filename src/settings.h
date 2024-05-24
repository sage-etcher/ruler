#ifndef SETTINGS_H
#define SETTINGS_H

#include "default.h"
#include "SDL2/SDL.h"

typedef enum 
{
    IMAGE_STRETCH,
    IMAGE_TILE,
    IMAGE_FILL,
    IMAGE_FIT_WIDTH,
    IMAGE_FIT_HEIGHT
} imgmode;


typedef struct
{
    SDL_LogPriority priority;

    unsigned width;
    unsigned height;

    float    opacity;

    uint_fast32_t color;
    char    *image_path;
    imgmode image_mode;

} settings_obj;

settings_obj *default_settings (void);


#endif /* SETTINGS_H */
