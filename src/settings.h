#ifndef SETTINGS_H
#define SETTINGS_H

#include "SDL2/SDL.h"

#include "imgmode.h"
#include "color.h"

typedef struct
{
    SDL_LogPriority priority;

    unsigned width;
    unsigned height;

    float    opacity;

    color    color;
    char    *image_path;
    imgmode  image_mode;

} settings_obj;


settings_obj *default_settings (void);

#endif /* SETTINGS_H */
