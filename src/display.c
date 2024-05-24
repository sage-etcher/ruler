#include "display.h"

static void graphics_init (void);
static void graphics_quit (void);


void
start_ruler (unsigned width, unsigned height, unsigned hex_color, float opacity, const char *bg_image, imgmode bg_image_mode, SDL_LogPriority priority)
{
    /*{{{*/
    SDL_bool runtime = SDL_TRUE;
    
    SDL_Window *win    = NULL;
    SDL_Renderer *rend = NULL;

    SDL_bool resize_flag  = SDL_TRUE;
    SDL_bool use_bg_image = (bg_image == NULL ? SDL_FALSE : SDL_TRUE);
    SDL_Surface *bg_surface = NULL;
    SDL_Texture *bg_texture = NULL;

    SDL_LogSetAllPriority (priority);

    graphics_init ();

    win  = create_window (width, height);
    rend = create_renderer (win);
    configure_window (win, &resize_flag, opacity);
    set_render_draw_color (rend, hex_color);
   
    if (use_bg_image)
    {
        SDL_LogVerbose (SDL_LOG_CATEGORY_INPUT, "Using background image: %s\n", bg_image);
        load_image (&bg_surface, &bg_texture, rend, bg_image);

        if ((bg_surface == NULL) || (bg_texture == NULL))
        {
            SDL_LogError (SDL_LOG_CATEGORY_INPUT, "Failed to use background image: %s\n", bg_image);
            use_bg_image = SDL_FALSE;
        }

    }

    /* show the window */
    SDL_ShowWindow (win);

    /* main runtime loop */
    while (handle_events (win, &runtime, &resize_flag), runtime)
    {
        clear_renderer (rend);

        if (use_bg_image)
        {
            draw_background_image (rend, bg_texture, bg_image_mode, bg_surface->clip_rect);
        }

        SDL_RenderPresent (rend);
    }

    SDL_DestroyTexture (bg_texture);
    SDL_FreeSurface(bg_surface);
    SDL_DestroyRenderer (rend);
    SDL_DestroyWindow (win);
   
    graphics_quit ();

    return;
    /*}}}*/
}


/* helper SDL functions */
static void
graphics_init (void)
{
    /*{{{*/
    const int img_flags = IMG_INIT_JPG | IMG_INIT_PNG;

    if (SDL_Init (SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS) != 0)
    {
        SDL_LogCritical (SDL_LOG_CATEGORY_ERROR, "SDL_Init: Failed to init required video, timer, and event support\n");
        SDL_LogCritical (SDL_LOG_CATEGORY_ERROR, "SDL_Init: %s!\n", SDL_GetError ());
        exit (EXIT_FAILURE);
    }

    if (IMG_Init (img_flags) != img_flags)
    {
        SDL_LogWarn (SDL_LOG_CATEGORY_ERROR, "IMG_Init: Failed to init jpg and png support\n");
        SDL_LogWarn (SDL_LOG_CATEGORY_ERROR, "IMG_Init: %s!\n", IMG_GetError ());
    }

    return;
    /*}}}*/
}


static void
graphics_quit (void)
{
    /*{{{*/
    IMG_Quit ();
    SDL_Quit ();

    return;
    /*}}}*/
}


/*
vim: ts=4 sts=4 sw=4 et fdm=marker
*/
