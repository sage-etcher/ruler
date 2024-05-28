#ifndef WINDOW_H
#define WINDOW_H

#include "SDL2/SDL.h"


SDL_Window *create_window (int width, int height);
void configure_window (SDL_Window *win, SDL_bool *resize_flag, float opacity);

#endif /* WINDOW_H */

/*
vim: ts=4 sts=4 sw=4 et fdm=marker
*/
