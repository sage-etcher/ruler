#ifndef RENDER_H
#define RENDER_H

#include "common.h"

SDL_Renderer *create_renderer (SDL_Window *win);
void set_render_draw_color (SDL_Renderer *rend, unsigned hex_color);
void clear_renderer (SDL_Renderer *rend);
void render_copy (SDL_Renderer *renderer, SDL_Texture *texture, SDL_Rect *srcrect, SDL_Rect *dstrect);

#endif /* RENDER_H */

/*
vim: ts=4 sts=4 sw=4 et fdm=marker
*/
