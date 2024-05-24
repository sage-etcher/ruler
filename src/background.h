#ifndef BACKGROUND_H
#define BACKGROUND_H

#include "common.h"
#include "render.h"

void load_image (SDL_Surface **p_surface, SDL_Texture **p_texture, SDL_Renderer *rend, const char *filename);
void draw_background_image (SDL_Renderer *rend, SDL_Texture *bg_texture, imgmode bg_image_mode, SDL_Rect tex_rect);


#endif /* BACKGROUND_H */

/*
vim: ts=4 sts=4 sw=4 et fdm=marker
*/
