#ifndef STRUCT_WINDOW_H
#define STRUCT_WINDOW_H

#include "SDL2/SDL.h"

#include "color.h"
#include "imgmode.h"


typedef struct
{
    SDL_bool runtime;

    SDL_Window   *win;
    SDL_Renderer *rend;

    color        bg_color;

    SDL_bool     use_bg_image;
    char *       bg_image;
    imgmode      bg_mode;
    SDL_Surface *bg_surface;
    SDL_Texture *bg_texture;

    SDL_bool resize_flag;

} runtime_obj;

#endif /* STRUCT_WINDOW_H */
