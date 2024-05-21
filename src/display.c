#include "display.h"

/* file-static preprocessor macros */
/*{{{*/
#define ABS(_x) ((_x < 0 ? -_x : _x))

#define QUICKDIST(_x,_y) ((_x*_x)+(_y*_y))

#define RESIZEPOINT(_c,_x,_y,_r) \
if (_c) { \
    dist = QUICKDIST(ABS((int)(_x)),ABS((int)(_y))); \
    if (!resize_flag || dist <= closest_dist) { \
        resize_flag = 1; \
        closest_dist = dist; \
        closest_result = _r; \
    } \
} \

#define HEX_R(x) ((x & 0xFF0000) >> 16)
#define HEX_G(x) ((x & 0x00FF00) >> 8)
#define HEX_B(x) ((x & 0x0000FF))
/*}}}*/


static SDL_bool keyboard_shortcut (SDL_Keysym *s, SDL_Keycode key, SDL_Keymod mod);
static void handle_events (SDL_Window *win, SDL_bool *runtime, SDL_bool *resize_flag);

static SDL_bool resize_window (SDL_Window *win, const SDL_Point *area, SDL_HitTestResult *p_ret);
static SDL_HitTestResult callback_window_click (SDL_Window *win, const SDL_Point *area, void *data);

static void graphics_init (void);
static void graphics_quit (void);
static SDL_Window *create_window (int width, int height);
static SDL_Renderer *create_renderer (SDL_Window *win);
static void configure_window (SDL_Window *win, SDL_bool *resize_flag, float opacity);
static void clear_renderer (SDL_Renderer *rend);
static void set_background_color (SDL_Renderer *rend, unsigned hex_color);
static void load_image (SDL_Surface **p_surface, SDL_Texture **p_texture, SDL_Renderer *rend, const char *filename);
static void draw_background_image (SDL_Renderer *rend, SDL_Texture *bg_texture);


void
start_ruler (unsigned width, unsigned height, unsigned hex_color, float opacity, const char *bg_image)
{
    /*{{{*/
    SDL_bool runtime = SDL_TRUE;
    
    SDL_Window *win    = NULL;
    SDL_Renderer *rend = NULL;

    SDL_bool resize_flag  = SDL_TRUE;
    SDL_bool use_bg_image = (bg_image == NULL ? SDL_FALSE : SDL_TRUE);
    SDL_Surface *bg_surface = NULL;
    SDL_Texture *bg_texture = NULL;

#ifdef _DEBUG
    /* log debug messages */
    SDL_LogSetAllPriority (SDL_LOG_PRIORITY_DEBUG);
#endif

    graphics_init ();

    win  = create_window (width, height);
    rend = create_renderer (win);
    configure_window (win, &resize_flag, opacity);
    set_background_color (rend, hex_color);
   
    if (use_bg_image) {
        load_image (&bg_surface, &bg_texture, rend, bg_image);
        use_bg_image = (((bg_surface != NULL) && 
                         (bg_texture != NULL))
                        ? SDL_TRUE : SDL_FALSE);
    }

    /* prepare the window to be shown */
    clear_renderer (rend);
    if (use_bg_image) {
        draw_background_image (rend, bg_texture);
    }
    SDL_RenderPresent (rend);
    SDL_ShowWindow (win);

    /* main runtime loop */
    while (runtime) {
        handle_events (win, &runtime, &resize_flag);
        clear_renderer (rend);

        if (use_bg_image) {
            draw_background_image (rend, bg_texture);
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


/* callback functions */
static SDL_bool
resize_window (SDL_Window *win, const SDL_Point *area, SDL_HitTestResult *p_ret)
{
    /*{{{*/
    const int RESIZE_DIST = 20;
    int resize_flag = 0;
    long closest_dist = 0, dist;
    SDL_HitTestResult closest_result;

    int x = area->x; 
    int y = area->y;

    int margin_top, margin_bottom, margin_left, margin_right;
    int width, height;

    /* get some numbers for sizing */
    SDL_GetWindowSize (win, &width, &height);

    margin_top    = RESIZE_DIST;
    margin_left   = RESIZE_DIST;
    margin_bottom = height - RESIZE_DIST;
    margin_right  = width  - RESIZE_DIST;

    /* check each corner */
    /*{{{*/
    /* top left */
    RESIZEPOINT(((y <= margin_top) && (x <= margin_left)), 
                 x, y, 
                 SDL_HITTEST_RESIZE_TOPLEFT);
    /* top right */
    RESIZEPOINT(((y <= margin_top) && (x >= margin_right)), 
                 (width - x), y, 
                 SDL_HITTEST_RESIZE_TOPRIGHT);
    /* bottom right */
    RESIZEPOINT(((y >= margin_bottom) && (x >= margin_right)), 
                 (width - x), (height - y), 
                 SDL_HITTEST_RESIZE_BOTTOMRIGHT);
    /* bottom left */
    RESIZEPOINT(((y >= margin_bottom) && (x <= margin_left)), 
                 x, (height - y), 
                 SDL_HITTEST_RESIZE_BOTTOMLEFT);
    /*}}}*/

    /* check each side */
    /*{{{*/
    /* top */
    RESIZEPOINT((y <= margin_top),
               ((width/2) - x), y,
               SDL_HITTEST_RESIZE_TOP);
    /* bottom */
    RESIZEPOINT((y >= margin_bottom),
               ((width/2) - x), (height - y), 
               SDL_HITTEST_RESIZE_BOTTOM);
    /* left */
    RESIZEPOINT((x <= margin_left),
               x, ((height/2) - y),
               SDL_HITTEST_RESIZE_LEFT);
    /* right */
    RESIZEPOINT((x >= margin_right),
               (width - x), ((height/2) - y), 
               SDL_HITTEST_RESIZE_RIGHT);
    /*}}}*/

    if (resize_flag)
    {
        *p_ret = closest_result;
        return SDL_TRUE;
    }

    return SDL_FALSE;
    /*}}}*/
}


SDL_HitTestResult
callback_window_click (SDL_Window *win, const SDL_Point *area, void *data)
{
    /*{{{*/
    const SDL_bool resizeable_flag = *(SDL_bool *)data;
    SDL_HitTestResult result;

    /* make the whole winodw draggable if the resize flag is disabled */
    if (resizeable_flag == SDL_FALSE)
        return SDL_HITTEST_DRAGGABLE;

    /* if the cursor is along the edge of the window, resize */
    if (resize_window (win, area, &result) == SDL_TRUE)
        return result;

    /* otherwise move the window */
    return SDL_HITTEST_DRAGGABLE;

    /*}}}*/
}


/* helper event functions */
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
    if (pressed_key != key) {
        return SDL_FALSE;
    }

    /* check if anything other than the allowed modifiers are pressed */
    if (((pressed_mod & ~mod) != 0) || ((pressed_mod & mod) != mod)) {
        return SDL_FALSE;
    }

    /* if both of them match, it is */
    return SDL_TRUE;
    /*}}}*/
}


static void
handle_events (SDL_Window *win, SDL_bool *runtime, SDL_bool *resize_flag)
{
    /*{{{*/
    SDL_Event e;
    SDL_Keysym *key; 

    while (SDL_PollEvent (&e)) {
        switch (e.type) {
        case SDL_KEYDOWN:
            key = &(e.key.keysym);

            if (keyboard_shortcut (key, SDLK_q, KMOD_LCTRL)) {
                *runtime = SDL_FALSE;
            }
            else if (keyboard_shortcut (key, SDLK_ESCAPE, KMOD_NONE)) {
                *runtime = SDL_FALSE;
            }
            else if (keyboard_shortcut (key, SDLK_l, KMOD_LCTRL)) {
                *resize_flag = (*resize_flag ? SDL_FALSE : SDL_TRUE);
                SDL_SetWindowResizable(win, *resize_flag);
            }
            break;

        case SDL_QUIT:
            *runtime = SDL_FALSE;
            break;
        }
    }

    return;
    /*}}}*/
}


/* helper SDL functions */
static void
graphics_init (void)
{
    /*{{{*/
    const int img_flags = IMG_INIT_JPG | IMG_INIT_PNG;

    if (SDL_Init (SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS) != 0) {
        SDL_Log ("SDL_Init: Failed to init required video, timer, and event support\n");
        SDL_Log ("SDL_Init: %s!\n", SDL_GetError ());
        exit (EXIT_FAILURE);
    }

    if (IMG_Init (img_flags) != img_flags) {
        SDL_Log ("IMG_Init: Failed to init jpg and png support\n");
        SDL_Log ("IMG_Init: %s!\n", IMG_GetError ());
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


static SDL_Window *
create_window (int width, int height)
{
    /*{{{*/
    const char *TITLE= "Ruler";
    SDL_Window *win;
    int retcode;

    /* create the window */
    win = SDL_CreateWindow (
            TITLE, 
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            width, height,
            SDL_WINDOW_BORDERLESS | SDL_WINDOW_SKIP_TASKBAR | SDL_WINDOW_HIDDEN);

    if (!win) {
        SDL_Log ("SDL_CreateWindow: Failed to create window\n");
        SDL_Log ("SDL_CreateWindow: %s!\n", SDL_GetError ());
        exit (EXIT_FAILURE);
    }

    return win;
    /*}}}*/
}


static SDL_Renderer * 
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
        return rend;
    }
    
    SDL_Log ("SDL_CreateRenderer: Failed to create accelerated vsync renderer\n");
    SDL_Log ("SDL_CreateRenderer: %s\n", SDL_GetError ());

    /* fallback attempt */
    rend = SDL_CreateRenderer (
            win, 
            -1, 
            SDL_RENDERER_PRESENTVSYNC); 
   
    if (rend)
    {
        return rend;
    }

    SDL_Log ("SDL_CreateRenderer: Failed to create fallback vsync renderer\n");
    SDL_Log ("SDL_CreateRenderer: %s\n", SDL_GetError ());

    /* abort if both fail */
    exit (EXIT_FAILURE);
    /*}}}*/
}


static void
configure_window (SDL_Window *win, SDL_bool *resize_flag, float opacity)
{
    /*{{{*/
    int retcode; 

    SDL_SetWindowMinimumSize (win, WINDOW_MIN_WIDTH, WINDOW_MIN_HEIGHT);
    SDL_SetWindowResizable (win, *resize_flag);
    SDL_SetWindowAlwaysOnTop (win, SDL_TRUE);
    
    retcode = SDL_SetWindowOpacity (win, opacity);
    if (retcode == -1)
    {
        SDL_Log ("SDL_SetWindowOpacity: No support for window opacity\n");
        SDL_Log ("SDL_SetWindowOpacity: %s\n", SDL_GetError ());
    }
    else if (retcode < 0)
    {
        SDL_Log ("SDL_SetWindowOpacity: Failed to set window opacity\n");
        SDL_Log ("SDL_SetWindowOpacity: %s\n", SDL_GetError ());
    }
    
    retcode = SDL_SetWindowHitTest (win, callback_window_click, resize_flag);
    if (retcode != 0)
    {
        SDL_Log ("SDL_SetWindowHitTest: Failed to set window move/resize callback\n");
        SDL_Log ("SDL_SetWindowHitTest: %s\n", SDL_GetError ());
    }

    return;
    /*}}}*/
}


static void
load_image (SDL_Surface **p_surface, SDL_Texture **p_texture, SDL_Renderer *rend, const char *filename)
{
    /*{{{*/
    SDL_Surface *surface;
    SDL_Texture *texture;

    surface = IMG_Load (filename);
    if (!surface) {
        SDL_Log ("IMG_Load: Failed to create surface from file\n");
        SDL_Log ("IMG_Load: %s\n", IMG_GetError ());

        *p_texture = NULL;
        *p_surface = NULL;
        return;
    }

    texture = SDL_CreateTextureFromSurface (rend, surface);
    if (!texture) {
        SDL_Log ("SDL_CreateTextureFromSurface: Failed to convert image surface to texture\n");
        SDL_Log ("SDL_CreateTextureFromSurface: %s\n", SDL_GetError ());

        *p_texture = NULL;
        *p_surface = NULL;
        return;
    }

    *p_surface = surface;
    *p_texture = texture;
    return;
    /*}}}*/
}


static void
set_background_color (SDL_Renderer *rend, unsigned hex_color)
{
    /*{{{*/
    int retcode;

    retcode = SDL_SetRenderDrawColor (
            rend, 
            HEX_R (hex_color),
            HEX_G (hex_color),
            HEX_B (hex_color),
            SDL_ALPHA_OPAQUE);

    if (retcode != 0) {
        SDL_Log ("SDL_SetRenderDrawColor: Failed to set background color\n");
        SDL_Log ("SDL_SetRenderDrawColor: %s\n", SDL_GetError ());
    }

    return;
    /*}}}*/
}


static void 
clear_renderer (SDL_Renderer *rend)
{
    /*{{{*/
    int retcode;

    retcode = SDL_RenderClear (rend);

    if (retcode != 0)
    {
        SDL_Log ("SDL_RenderClear: Failed to clear the renderer\n");
        SDL_Log ("SDL_RenderClear: %s\n", SDL_GetError ());
    }

    return;
    /*}}}*/
}


static void 
draw_background_image (SDL_Renderer *rend, SDL_Texture *bg_texture)
{
    /*{{{*/
    int retcode;  

    retcode = SDL_RenderCopy (rend, bg_texture, NULL, NULL);

    if (retcode != 0)
    {
        SDL_Log ("SDL_RenderCopy: Failed to draw background on the renderer\n");
        SDL_Log ("SDL_RenderCopy: %s\n", SDL_GetError ());
    }

    return;
    /*}}}*/
}


/*
vim: ts=4 sts=4 sw=4 et fdm=marker
*/
