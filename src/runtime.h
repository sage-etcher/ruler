#ifndef STRUCT_WINDOW_H
#define STRUCT_WINDOW_H

#include "SDL2/SDL.h"
#include "settings.h"

typedef struct
{
    SDL_bool runtime;

    SDL_Window   *win;
    SDL_Renderer *rend;

    SDL_bool     use_bg_image;
    imgmode      bg_mode;
    SDL_Surface *bg_surface;
    SDL_Texture *bg_texture;

    SDL_bool resize_flag;

} runtime_obj;

#endif /* STRUCT_WINDOW_H */