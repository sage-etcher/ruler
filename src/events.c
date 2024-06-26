#include "events.h"

#include "SDL2/SDL.h"
#include "tinyfd/tinyfiledialogs.h"

#include "default.h"
#include "imgmode.h"
#include "runtime.h"
#include "str_utils.h"
#include "log_helper.h"

#include "arguments.h"
#include "render.h"
#include "background.h"
#include "window.h"

#include "shortcuts.h"

static imgmode cycle_mode (imgmode mode);
static void select_new_image (runtime_obj *s);
static void select_new_color (runtime_obj *s);
static void relative_move_opacity (runtime_obj *s, float relative_move);
static void runtime_shortcuts_prompt (void);
static void scroll_window (runtime_obj *s, SDL_MouseWheelEvent *wheel);

static void shortcut_quit (const void *e, void *data);
static void shortcut_resize_lock (const void *e, void *data);
static void shortcut_cycle_mode (const void *e, void *data);
static void shortcut_set_image (const void *e, void *data);
static void shortcut_toggle_image (const void *e, void *data);
static void shortcut_set_color (const void *e, void *data);
static void shortcut_inc_opacity (const void *e, void *data);
static void shortcut_dec_opacity (const void *e, void *data);
static void shortcut_help (const void *e, void *data);

#define KMOD_ALTSHIFT     (SDL_Keymod)(KMOD_ALT  | KMOD_SHIFT)
#define KMOD_CTRLSHIFT    (SDL_Keymod)(KMOD_CTRL | KMOD_SHIFT)
#define KMOD_CTRLALT      (SDL_Keymod)(KMOD_CTRL | KMOD_ALT)
#define KMOD_CTRLALTSHIFT (SDL_KeyMod)(KMOD_CTRL | KMOD_ALT | KMOD_SHIFT)

void
handle_events (runtime_obj *s)
{
    /*{{{*/
    SDL_Event e;
    
    static kshortcut control_shortcuts[] =
    {
    /*    KEY,                MOD,             CALLBACK */
        { SDLK_q,            (KMOD_CTRL),      shortcut_quit         },
        { SDLK_ESCAPE,       (KMOD_NONE),      shortcut_quit         },
        { SDLK_l,            (KMOD_CTRL),      shortcut_resize_lock  },
        { SDLK_f,            (KMOD_CTRL),      shortcut_cycle_mode   },
        { SDLK_o,            (KMOD_CTRL),      shortcut_set_image    },
        { SDLK_o,            (KMOD_CTRLSHIFT), shortcut_toggle_image },
        { SDLK_u,            (KMOD_CTRL),      shortcut_set_color    },
        { SDLK_RIGHTBRACKET, (KMOD_CTRL),      shortcut_inc_opacity  },
        { SDLK_LEFTBRACKET,  (KMOD_CTRL),      shortcut_dec_opacity  },
        { SDLK_F1,           (KMOD_NONE),      shortcut_help         },
        { SDLK_SLASH,        (KMOD_CTRL),      shortcut_help         },
        { SDLK_SLASH,        (KMOD_CTRLSHIFT), shortcut_help         },
        { (SDL_Keycode)0,    (SDL_Keymod)0,    (kshort_callback)NULL }
    };


    while (SDL_PollEvent (&e))
    {
        switch (e.type)
        {
        case SDL_KEYDOWN:
            (void)handle_keyboard_shortcuts (s, control_shortcuts, &e.key.keysym);
            break;

        case SDL_MOUSEWHEEL:
            (void)scroll_window (s, &e.wheel);
            break;

        case SDL_QUIT:
            shortcut_quit (NULL, s);
            return;
        }
    }

    return;
    /*}}}*/
}


/* shortcut functions */
/*{{{*/
/* the shortcut callback functions are required to accept 2 parameters,
 * 1)  const void *e  for the Event that triggered it, normally SDL_Keysym *
 * 2)  void *data     for the data variable passed to handle_keyboard_events()
 *
 * both of these may be ignored or unused within the function, -Wextra will
 * compliain that they are unused, to silence it, we use the following pragmas
 */
#ifdef USE_PRAGMAS
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif /* USE_PRAGMAS */
static void
shortcut_quit (const void *e, void *data)
{
    /*{{{*/
    runtime_obj *s = (runtime_obj *)data;

    /* quit */
    s->runtime = SDL_FALSE;
    SDL_LogDebug (SDL_LOG_CATEGORY_APPLICATION, "runtime: %s\n", log_sdlbool (s->runtime));

    return;
    /*}}}*/
}

static void
shortcut_resize_lock (const void *e, void *data)
{
    /*{{{*/
    runtime_obj *s = (runtime_obj *)data;

    /* lock current size */
    s->resize_flag = (s->resize_flag ? SDL_FALSE : SDL_TRUE);
    SDL_SetWindowResizable(s->win, s->resize_flag);
    SDL_LogDebug (SDL_LOG_CATEGORY_APPLICATION, "resize_flag: %s\n", log_sdlbool (s->resize_flag));

    return;
    /*}}}*/
}

static void
shortcut_cycle_mode (const void *e, void *data) 
{
    /*{{{*/
    runtime_obj *s = (runtime_obj *)data;

    /* cycle image mode */
    s->bg_mode = cycle_mode (s->bg_mode);
    SDL_LogDebug (SDL_LOG_CATEGORY_APPLICATION, "bg_mode: %s\n", log_imgmode (s->bg_mode));

    return;
    /*}}}*/
}

static void
shortcut_set_image (const void *e, void *data)
{
    /*{{{*/
    runtime_obj *s = (runtime_obj *)data;

    /* new background image */
    select_new_image (s);
    SDL_LogDebug (SDL_LOG_CATEGORY_APPLICATION, "bg_image: %s\n", s->bg_image);

    return;
    /*}}}*/
}

static void
shortcut_toggle_image (const void *e, void *data)
{
    /*{{{*/
    runtime_obj *s = (runtime_obj *)data;

    /* toggle background image */
    if (s->use_bg_image == SDL_TRUE)
    {
        s->use_bg_image = SDL_FALSE;
    }
    else if ((s->use_bg_image == SDL_FALSE) &&
             (s->bg_texture != NULL))
    {
        s->use_bg_image = SDL_TRUE;
    }
    SDL_LogDebug (SDL_LOG_CATEGORY_APPLICATION, "use_bg_image: %s\n", log_sdlbool(s->use_bg_image));

    return;
    /*}}}*/
}

static void
shortcut_set_color (const void *e, void *data)
{ 
    /*{{{*/
    runtime_obj *s = (runtime_obj *)data;

    /* new background color */
    select_new_color (s);
    SDL_LogDebug (SDL_LOG_CATEGORY_APPLICATION, "bg_color: 0x%06x\n", s->bg_color);

    return;
    /*}}}*/
}
   
static void
shortcut_inc_opacity (const void *e, void *data)
{
    /*{{{*/
    runtime_obj *s = (runtime_obj *)data;

    /* opacity up 10% */
    relative_move_opacity (s, 0.10f);
    SDL_LogDebug (SDL_LOG_CATEGORY_APPLICATION, "opacity: %.02f\n", s->opacity);

    return;
    /*}}}*/
}

static void
shortcut_dec_opacity (const void *e, void *data)
{
    /*{{{*/
    runtime_obj *s = (runtime_obj *)data;

    /* opacity up 10% */
    relative_move_opacity (s, -0.10f);
    SDL_LogDebug (SDL_LOG_CATEGORY_APPLICATION, "opacity: %.02f\n", s->opacity);

    return;
    /*}}}*/
}

static void
shortcut_help (const void *e, void *data)
{
    /*{{{*/
    /* show runtime shortcuts */
    runtime_shortcuts_prompt ();
    SDL_LogDebug (SDL_LOG_CATEGORY_APPLICATION, "opened shortcut help window"); 

    return;
    /*}}}*/
}

#ifdef USE_PRAGMAS
#pragma GCC diagnostic pop
#endif /* USE_PRAGMAS */
/*}}}*/


/* helper functions */
static imgmode
cycle_mode (imgmode mode)
{
    /*{{{*/

    /* if we are at the last item, cycle back to the first */
    if (mode >= IMAGE_MODE_LAST)
        return IMAGE_MODE_FIRST;

    /* all other times, move to the next item */
    return (imgmode)(mode + 1);
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


static void
scroll_window (runtime_obj *s, SDL_MouseWheelEvent *wheel)
{
    const int MOVE_TICK = 5;   /* how far (in pixels) does 1 scroll tick move the window? */

    int window_x,   window_y;
    int relative_x, relative_y;
    int absolute_x, absolute_y;

    /* guard */
    if ((wheel == NULL) ||
        (s == NULL) ||
        (s->win == NULL))
    {
        return;
    }

    relative_x = -wheel->x; /* flip the direction */
    relative_y = -wheel->y; 

    SDL_GetWindowPosition (s->win, &window_x, &window_y);

    absolute_x = window_x + (relative_x * MOVE_TICK);
    absolute_y = window_y + (relative_y * MOVE_TICK);

    SDL_SetWindowPosition (s->win, absolute_x, absolute_y);

    return;
}

/*
vim: ts=4 sts=4 sw=4 et fdm=marker
*/
