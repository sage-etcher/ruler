#include "events.h"
#include "settings.h"

static SDL_bool keyboard_shortcut (SDL_Keysym *s, SDL_Keycode key, SDL_Keymod mod);
static imgmode cycle_mode (imgmode mode);


void
handle_events (runtime_obj *s)
{
    /*{{{*/
    SDL_Event e;
    SDL_Keysym *key; 

    while (SDL_PollEvent (&e))
    {
        switch (e.type)
        {
        case SDL_KEYDOWN:
            key = &(e.key.keysym);

            if (keyboard_shortcut (key, SDLK_q, KMOD_LCTRL))
            {
                s->runtime = SDL_FALSE;
                SDL_LogDebug (SDL_LOG_CATEGORY_APPLICATION, "runtime: %s\n", LOG_SDL_BOOL (s->runtime));
                return;
            }
            else if (keyboard_shortcut (key, SDLK_ESCAPE, KMOD_NONE))
            {
                s->runtime = SDL_FALSE;
                SDL_LogDebug (SDL_LOG_CATEGORY_APPLICATION, "runtime: %s\n", LOG_SDL_BOOL (s->runtime));
                return;
            }
            else if (keyboard_shortcut (key, SDLK_l, KMOD_LCTRL))
            {
                s->resize_flag = (s->resize_flag ? SDL_FALSE : SDL_TRUE);
                SDL_SetWindowResizable(s->win, s->resize_flag);
                SDL_LogDebug (SDL_LOG_CATEGORY_APPLICATION, "resize_flag: %s\n", LOG_SDL_BOOL (s->resize_flag));
            }
            else if (keyboard_shortcut (key, SDLK_f, KMOD_LCTRL))
            {
                s->bg_mode = cycle_mode (s->bg_mode);
                SDL_LogDebug (SDL_LOG_CATEGORY_APPLICATION, "bg_mode: %s\n", LOG_IMG_MODE (s->bg_mode));
            }
            break;

        case SDL_QUIT:
            s->runtime = SDL_FALSE;
            SDL_LogDebug (SDL_LOG_CATEGORY_APPLICATION, "runtime: %s\n", LOG_SDL_BOOL(s->runtime));
            return;
        }
    }

    return;
    /*}}}*/
}


static SDL_bool
keyboard_shortcut (SDL_Keysym *s, SDL_Keycode key, SDL_Keymod mod)
{
    /*{{{*/
    SDL_Keycode pressed_key = s->sym;
    SDL_Keymod  pressed_mod = s->mod;

    /* remove flags we dont care about */
    /* only look at Ctrl, Alt, Shift, Gui/Win keys */
    pressed_mod &= ~(KMOD_SCROLL | KMOD_MODE | KMOD_CAPS | KMOD_NUM | KMOD_RESERVED);
 
    /* check main keypress */
    if (pressed_key != key)
    {
        return SDL_FALSE;
    }

    /* check if anything other than the allowed modifiers are pressed */
    if (((pressed_mod & ~mod) != 0) || ((pressed_mod & mod) != mod))
    {
        return SDL_FALSE;
    }

    /* if both of them match, it is */
    return SDL_TRUE;
    /*}}}*/
}


static imgmode
cycle_mode (imgmode mode)
{
    /*{{{*/
    switch (mode)
    {
    case IMAGE_STRETCH:
        return IMAGE_FILL;
    case IMAGE_FILL:
        return IMAGE_FIT_HEIGHT;
    case IMAGE_FIT_HEIGHT:
        return IMAGE_FIT_WIDTH;
    case IMAGE_FIT_WIDTH:
        return IMAGE_TILE;
    case IMAGE_TILE:
        return IMAGE_STRETCH;
    default:
        return DEFAULT_IMG_MODE;
    }
    /*}}}*/

}


/*
vim: ts=4 sts=4 sw=4 et fdm=marker
*/
