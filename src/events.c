#include "events.h"

#include "SDL2/SDL.h"
#include "tinyfd/tinyfiledialogs.h"

#include "imgmode.h"
#include "runtime.h"
#include "str_utils.h"
#include "log_helper.h"

#include "arguments.h"
#include "render.h"
#include "background.h"
#include "window.h"


static SDL_bool keyboard_shortcut (SDL_Keysym *s, SDL_Keycode key, SDL_Keymod mod);
static imgmode cycle_mode (imgmode mode);
static void select_new_image (runtime_obj *s);
static void select_new_color (runtime_obj *s);
static void relative_move_opacity (runtime_obj *s, float relative_move);
static void runtime_shortcuts_prompt (void);


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

            if ((keyboard_shortcut (key, SDLK_q, KMOD_LCTRL)) ||
                (keyboard_shortcut (key, SDLK_ESCAPE, KMOD_NONE)))
            {
                /* quit */
                s->runtime = SDL_FALSE;
                SDL_LogDebug (SDL_LOG_CATEGORY_APPLICATION, "runtime: %s\n", log_sdlbool (s->runtime));
                return;
            }
            else if (keyboard_shortcut (key, SDLK_l, KMOD_LCTRL))
            {
                /* lock current size */
                s->resize_flag = (s->resize_flag ? SDL_FALSE : SDL_TRUE);
                SDL_SetWindowResizable(s->win, s->resize_flag);
                SDL_LogDebug (SDL_LOG_CATEGORY_APPLICATION, "resize_flag: %s\n", log_sdlbool (s->resize_flag));
            }
            else if (keyboard_shortcut (key, SDLK_f, KMOD_LCTRL))
            {
                /* cycle image mode */
                s->bg_mode = cycle_mode (s->bg_mode);
                SDL_LogDebug (SDL_LOG_CATEGORY_APPLICATION, "bg_mode: %s\n", log_imgmode (s->bg_mode));
            }
            else if (keyboard_shortcut (key, SDLK_o, KMOD_LCTRL))
            {
                /* new background image */
                select_new_image (s);
                SDL_LogDebug (SDL_LOG_CATEGORY_APPLICATION, "bg_image: %s\n", s->bg_image);
            }
            else if (keyboard_shortcut (key, SDLK_u, KMOD_LCTRL)) 
            { 
                /* new background color */
                select_new_color (s);
                SDL_LogDebug (SDL_LOG_CATEGORY_APPLICATION, "bg_color: 0x%06x\n", s->bg_color);
            }
            else if (keyboard_shortcut (key, SDLK_RIGHTBRACKET, KMOD_LCTRL))
            {
                /* opacity up 10% */
                relative_move_opacity (s, 0.10f);
                SDL_LogDebug (SDL_LOG_CATEGORY_APPLICATION, "opacity: %.02f\n", s->opacity);
            }
            else if (keyboard_shortcut (key, SDLK_LEFTBRACKET, KMOD_LCTRL))
            {
                /* opacity down 10% */
                relative_move_opacity (s, -0.10f);
                SDL_LogDebug (SDL_LOG_CATEGORY_APPLICATION, "opacity: %.02f\n", s->opacity);
            }
            else if ((keyboard_shortcut (key, SDLK_F1, KMOD_NONE)) ||
                     (keyboard_shortcut (key, SDLK_SLASH, KMOD_LCTRL)) ||
                     (keyboard_shortcut (key, SDLK_SLASH, (KMOD_LCTRL | KMOD_LSHIFT))))
            {
                /* show runtime shortcuts */
                runtime_shortcuts_prompt ();
                SDL_LogDebug (SDL_LOG_CATEGORY_APPLICATION, "opened shortcut help window"); 
            }
            break;

        case SDL_QUIT:
            s->runtime = SDL_FALSE;
            SDL_LogDebug (SDL_LOG_CATEGORY_APPLICATION, "runtime: %s\n", log_sdlbool (s->runtime));
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

    /* if we are at the last item, cycle back to the first */
    if (mode >= IMAGE_MODE_LAST)
        return IMAGE_MODE_FIRST;

    /* all other times, move to the next item */
    return (mode + 1);
    /*}}}*/
}


static void
select_new_image (runtime_obj *s)
{
    /*{{{*/
    const char *patterns[3] = {
        "*.png",
        "*.jpg", "*.jpeg"
    };
    SDL_Surface *new_surf;
    SDL_Texture *new_tex;
    SDL_bool status;

    /* NOTE: tinyfd's returned string are not to be freed by us, doing so may
     * cause the program to seg fault. the strings are managed from within
     * the library, as file-static. */
   
    /* prompt the use for a file name */
    const char *new_filename = tinyfd_openFileDialog (
        "Ruler Open",
        NULL,
        (sizeof(patterns) / sizeof(patterns[0])),
        patterns,
        "Image Files",
        0);

    if (!new_filename) /* if user hit cancel, cancel */
        return;

    /* try to create a new background image from the inputted file */
    status = create_background_image ((char *)new_filename, s->rend, &new_surf, &new_tex);
    if (status == SDL_FALSE) /* if we can't, keep the current image */
        return;

    /* if an image was in use, free it. otherwise values will be NULL */
    SDL_free (s->bg_image);
    SDL_DestroyTexture (s->bg_texture);
    SDL_FreeSurface (s->bg_surface);

    /* and copy the new data over */
    /* NOTE: we use str_dup for bg_image, because SDL_free is not compatible 
     * with std::malloc. To avoid juggling different mallocs and frees, I've 
     * opted to instead duplicate the string with SDL_malloc to unify 
     * everything. */

    s->use_bg_image = SDL_TRUE;
    s->bg_image     = str_dup (new_filename);    /* calls SDL_malloc */
    s->bg_surface   = new_surf;
    s->bg_texture   = new_tex;

    return; 
    /*}}}*/
}


static void
select_new_color (runtime_obj *s)
{
    /*{{{*/
    unsigned char selected_rgb[3];
    const unsigned char default_rgb[3] = 
    { 
        GET_HEX_R(s->bg_color),
        GET_HEX_G(s->bg_color),
        GET_HEX_B(s->bg_color) 
    };
    const char *color_string = tinyfd_colorChooser (
            "Ruler Color Picker",
            NULL,
            default_rgb,
            selected_rgb);

    /* handle cancle button */
    if (!color_string)
        return;

    /* create new color */
    SET_HEX_R(s->bg_color, selected_rgb[0]);
    SET_HEX_G(s->bg_color, selected_rgb[1]);
    SET_HEX_B(s->bg_color, selected_rgb[2]);

    /* draw renderer with new color */
    set_render_draw_color (s->rend, s->bg_color);

    return;
    /*}}}*/
}


static void
relative_move_opacity (runtime_obj *s, float relative_move)
{
    /*{{{*/
    float opacity = s->opacity;

    /* calculate new opacity */    
    opacity += relative_move;

    /* limit it to be between 0 and 1 */
    if (opacity > 1.0f)
    {
        opacity = 1.0f;
    }
    else if (opacity < 0.0f)
    {
        opacity = 0.0f;
    }

    /* set the window opacity */
    set_window_opacity (s->win, opacity);
    
    /* update the opacity value in the runtime object */
    s->opacity = opacity;
   
    return; 
    /*}}}*/
}


static void
runtime_shortcuts_prompt (void)
{
    /*{{{*/
    (void)tinyfd_messageBox (
        "Ruler Shortcuts",
        get_shortcuts (),
        "ok",
        "info",
        1);

    return;
    /*}}}*/
}


/*
vim: ts=4 sts=4 sw=4 et fdm=marker
*/
