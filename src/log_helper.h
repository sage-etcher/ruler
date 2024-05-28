#ifndef LOG_HELPER_H
#define LOG_HELPER_H

#include "SDL2/SDL.h"

#include "imgmode.h"


const char *log_sdlbool (SDL_bool s);
const char *log_imgmode (imgmode s);

#endif /* LOG_HELPER_H */
