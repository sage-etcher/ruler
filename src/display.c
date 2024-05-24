#include "display.h"

static void graphics_init (void);
static void graphics_quit (void);


void
start_ruler (settings_obj *settings)
{
    /*{{{*/
    runtime_obj *s = SDL_malloc (sizeof (runtime_obj));

    s->runtime = SDL_TRUE;
    
    s->use_bg_image = (settings->image_path == NULL ? SDL_FALSE : SDL_TRUE);
    s->bg_mode      = settings->image_mode;
    s->bg_surface = NULL;
    s->bg_texture = NULL;

    s->resize_flag  = SDL_TRUE;


    SDL_LogSetAllPriority (settings->priority);

    graphics_init ();

    s->win  = create_window (settings->width, settings->height);
    s->rend = create_renderer (s->win);
    configure_window (s->win, &s->resize_flag, settings->opacity);
    set_render_draw_color (s->rend, settings->color);
   
    if (s->use_bg_image)
    {
        SDL_LogVerbose (SDL_LOG_CATEGORY_INPUT, "Using background image: %s\n", settings->image_path);
        load_image (&s->bg_surface, &s->bg_texture, s->rend, settings->image_path);

        if ((s->bg_surface == NULL) || (s->bg_texture == NULL))
        {
            SDL_LogError (SDL_LOG_CATEGORY_INPUT, "Failed to use background image: %s\n", settings->image_path);
            s->use_bg_image = SDL_FALSE;
        }

    }

    /* show the window */
    SDL_ShowWindow (s->win);

    /* main runtime loop */
    while (handle_events (s), s->runtime)
    {
        clear_renderer (s->rend);

        if (s->use_bg_image)
        {
            draw_background_image (s->rend, s->bg_texture, s->bg_mode, s->bg_surface->clip_rect);
        }

        SDL_RenderPresent (s->rend);
    }

    SDL_DestroyTexture (s->bg_texture);
    SDL_FreeSurface(s->bg_surface);
    SDL_DestroyRenderer (s->rend);
    SDL_DestroyWindow (s->win);
    SDL_free (s);
   
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
