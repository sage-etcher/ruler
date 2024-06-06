#ifndef WINDOW_H
#define WINDOW_H
#include "extern-c-helper.h"
EXTERN_C_START()

#include "SDL2/SDL.h"


SDL_Window *create_window (int width, int height);
void configure_window (SDL_Window *win, SDL_bool *resize_flag, float opacity);
void set_window_opacity (SDL_Window *win, float opacity);


EXTERN_C_END()
#endif /* WINDOW_H */

/*
vim: ts=4 sts=4 sw=4 et fdm=marker
*/
