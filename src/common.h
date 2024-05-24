#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <ctype.h>

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include "config.h"

/* #define _DEBUG  */
typedef enum 
{
    IMAGE_STRETCH,
    IMAGE_TILE,
    IMAGE_FILL,
    IMAGE_FIT_WIDTH,
    IMAGE_FIT_HEIGHT
} imgmode;


#define WINDOW_MIN_WIDTH 1
#define WINDOW_MIN_HEIGHT 1
extern SDL_LogPriority g_logging_mode;
extern float    g_opacity;
extern unsigned g_width;
extern unsigned g_height;
extern unsigned g_hex;
extern char *   g_image;
extern imgmode  g_image_mode;

#define HEX_R(_x) ((_x & 0xFF0000) >> 16)
#define HEX_G(_x) ((_x & 0x00FF00) >> 8)
#define HEX_B(_x) ((_x & 0x0000FF))

#define ABS(_x) ((_x < 0 ? -_x : _x))
#define LOG_SDL_BOOL(_v) (((_v) == SDL_TRUE) ? "SDL_TRUE" : "SDL_FALSE")

#endif /* COMMON_H */

/*
vim: ts=4 sts=4 sw=4 et fdm=marker
*/
