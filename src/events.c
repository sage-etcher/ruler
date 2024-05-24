#include "events.h"

static SDL_bool keyboard_shortcut (SDL_Keysym *s, SDL_Keycode key, SDL_Keymod mod);


void
handle_events (SDL_Window *win, SDL_bool *runtime, SDL_bool *resize_flag)
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
                *runtime = SDL_FALSE;
                SDL_LogDebug (SDL_LOG_CATEGORY_APPLICATION, "*runtime: %s\n", LOG_SDL_BOOL (*runtime));
                return;
            }
            else if (keyboard_shortcut (key, SDLK_ESCAPE, KMOD_NONE))
            {
                *runtime = SDL_FALSE;
                SDL_LogDebug (SDL_LOG_CATEGORY_APPLICATION, "*runtime: %s\n", LOG_SDL_BOOL (*runtime));
                return;
            }
            else if (keyboard_shortcut (key, SDLK_l, KMOD_LCTRL))
            {
                *resize_flag = (*resize_flag ? SDL_FALSE : SDL_TRUE);
                SDL_SetWindowResizable(win, *resize_flag);
                SDL_LogDebug (SDL_LOG_CATEGORY_APPLICATION, "*resize_flag: %s\n", LOG_SDL_BOOL (*resize_flag));
            }
            break;

        case SDL_QUIT:
            *runtime = SDL_FALSE;
            SDL_LogDebug (SDL_LOG_CATEGORY_APPLICATION, "*runtime: %s\n", LOG_SDL_BOOL(*runtime));
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


/*
vim: ts=4 sts=4 sw=4 et fdm=marker
*/
