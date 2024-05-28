#include "log_helper.h"

#include "SDL2/SDL.h"

#include "imgmode.h"


const char *
log_sdlbool (SDL_bool s)
{
    const char *STR_TRUE  = "SDL_TRUE";
    const char *STR_FALSE = "SDL_FALSE";

    /* return strings associated w/ each enum */
    switch (s)
    {
    case SDL_TRUE:
        return STR_TRUE;
    case SDL_FALSE:
        return STR_FALSE;
    }

    /* if value not listed, return an error */
    return NULL;
}


const char *
log_imgmode (imgmode s)
{
    const char *IMGMODE_ARRAY[] = {
        [IMAGE_STRETCH]    = "IMAGE_STRETCH",
        [IMAGE_TILE]       = "IMAGE_TILE",
        [IMAGE_FILL]       = "IMAGE_FILL",
        [IMAGE_FIT_WIDTH]  = "IMAGE_FIT_WIDTH",
        [IMAGE_FIT_HEIGHT] = "IMAGE_FIT_HEIGHT"
    };

    /* check bounds */
    if ((s < IMAGE_MODE_FIRST) ||
        (s > IMAGE_MODE_LAST))
    {
        return NULL;
    }

    /* return string */
    return IMGMODE_ARRAY[s];
}


