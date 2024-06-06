#ifndef RENDER_H
#define RENDER_H
#include "extern-c-helper.h"
EXTERN_C_START

#include "SDL2/SDL.h"

#include "color.h"


SDL_Renderer *create_renderer (SDL_Window *win);
void set_render_draw_color (SDL_Renderer *rend, color hex_color);
void clear_renderer (SDL_Renderer *rend);
void render_copy (SDL_Renderer *renderer, SDL_Texture *texture, SDL_Rect *srcrect, SDL_Rect *dstrect);

EXTERN_C_END
#endif /* RENDER_H */

/*
vim: ts=4 sts=4 sw=4 et fdm=marker
*/
