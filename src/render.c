#include "render.h"

#include <stddef.h>
#include "SDL2/SDL.h"

#include "color.h"


SDL_Renderer * 
create_renderer (SDL_Window *win)
{
    /*{{{*/
    SDL_Renderer *rend;

    /* first attempt */
    rend = SDL_CreateRenderer (
            win, 
            -1, 
            SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

    if (rend)
    {
        SDL_LogVerbose (SDL_LOG_CATEGORY_RENDER, "Using primary renderer\n");
        return rend;
    }
    
    SDL_LogError (SDL_LOG_CATEGORY_ERROR, "SDL_CreateRenderer: Failed to create accelerated vsync renderer\n");
    SDL_LogError (SDL_LOG_CATEGORY_ERROR, "SDL_CreateRenderer: %s\n", SDL_GetError ());

    /* fallback attempt */
    rend = SDL_CreateRenderer (
            win, 
            -1, 
            SDL_RENDERER_PRESENTVSYNC); 
   
    if (rend)
    {
        SDL_LogVerbose (SDL_LOG_CATEGORY_RENDER, "Using fallback renderer\n");
        return rend;
    }

    SDL_LogCritical (SDL_LOG_CATEGORY_ERROR, "SDL_CreateRenderer: Failed to create fallback vsync renderer\n");
    SDL_LogCritical (SDL_LOG_CATEGORY_ERROR, "SDL_CreateRenderer: %s\n", SDL_GetError ());

    /* abort if both fail */
    exit (EXIT_FAILURE);
    /*}}}*/
}


void
set_render_draw_color (SDL_Renderer *rend, color hex_color)
{
    /*{{{*/
    int retcode;

    retcode = SDL_SetRenderDrawColor (
            rend, 
            GET_HEX_R (hex_color),
            GET_HEX_G (hex_color),
            GET_HEX_B (hex_color),
            SDL_ALPHA_OPAQUE);

    if (retcode != 0)
    {
        SDL_LogError (SDL_LOG_CATEGORY_ERROR, "SDL_SetRenderDrawColor: Failed to set background color\n");
        SDL_LogError (SDL_LOG_CATEGORY_ERROR, "SDL_SetRenderDrawColor: %s\n", SDL_GetError ());
    }

    return;
    /*}}}*/
}


void 
clear_renderer (SDL_Renderer *rend)
{
    /*{{{*/
    int retcode;

    retcode = SDL_RenderClear (rend);

    if (retcode != 0)
    {
        SDL_LogError (SDL_LOG_CATEGORY_ERROR, "SDL_RenderClear: Failed to clear the renderer\n");
        SDL_LogError (SDL_LOG_CATEGORY_ERROR, "SDL_RenderClear: %s\n", SDL_GetError ());
    }

    return;
    /*}}}*/
}


void
render_copy (SDL_Renderer *renderer, SDL_Texture *texture, SDL_Rect *srcrect, SDL_Rect *dstrect)
{
    /*{{{*/
    int retcode;  

    retcode = SDL_RenderCopy (renderer, texture, srcrect, dstrect);

    if (retcode != 0)
    {
        SDL_LogError (SDL_LOG_CATEGORY_ERROR, "SDL_RenderCopy: Failed to draw texture to renderer\n");
        SDL_LogError (SDL_LOG_CATEGORY_ERROR, "SDL_RenderCopy: %s\n", SDL_GetError ());
    }

    return;
    /*}}}*/
}


/*
vim: ts=4 sts=4 sw=4 et fdm=marker
*/
