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

#define HEX_R(_x) ((_x & 0xFF0000) >> 16)
#define HEX_G(_x) ((_x & 0x00FF00) >> 8)
#define HEX_B(_x) ((_x & 0x0000FF))

#define LOG_SDL_BOOL(_v) (((_v) == SDL_TRUE) ? "SDL_TRUE" : "SDL_FALSE")
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
static void draw_background_image (SDL_Renderer *rend, SDL_Texture *bg_texture, imgmode bg_image_mode, SDL_Rect tex_rect);


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
    set_background_color (rend, hex_color);
   
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
    (void)SDL_SetWindowHitTest (win, callback_window_click, &resize_flag);
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
    {
        return SDL_HITTEST_DRAGGABLE;
    }

    /* if the cursor is along the edge of the window, resize */
    if (resize_window (win, area, &result) == SDL_TRUE) 
    {
        return result;
    }
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


static void
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


static SDL_Window *
create_window (int width, int height)
{
    /*{{{*/
    const char *TITLE= "Ruler";
    SDL_Window *win;

    /* create the window */
    win = SDL_CreateWindow (
            TITLE, 
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            width, height,
            SDL_WINDOW_BORDERLESS | SDL_WINDOW_SKIP_TASKBAR | SDL_WINDOW_HIDDEN);

    if (win) 
    {
        return win;
    }

    SDL_LogCritical (SDL_LOG_CATEGORY_ERROR, "SDL_CreateWindow: Failed to create window\n");
    SDL_LogCritical (SDL_LOG_CATEGORY_ERROR, "SDL_CreateWindow: %s!\n", SDL_GetError ());

    exit (EXIT_FAILURE);

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
        SDL_LogError (SDL_LOG_CATEGORY_SYSTEM, "SDL_SetWindowOpacity: No support for window opacity\n");
        SDL_LogError (SDL_LOG_CATEGORY_SYSTEM, "SDL_SetWindowOpacity: %s\n", SDL_GetError ());
    }
    else if (retcode < 0)
    {
        SDL_LogError (SDL_LOG_CATEGORY_ERROR, "SDL_SetWindowOpacity: Failed to set window opacity\n");
        SDL_LogError (SDL_LOG_CATEGORY_ERROR, "SDL_SetWindowOpacity: %s\n", SDL_GetError ());
    }
    
    retcode = SDL_SetWindowHitTest (win, callback_window_click, resize_flag);
    if (retcode != 0)
    {
        SDL_LogError (SDL_LOG_CATEGORY_ERROR, "SDL_SetWindowHitTest: Failed to set window move/resize callback\n");
        SDL_LogError (SDL_LOG_CATEGORY_ERROR, "SDL_SetWindowHitTest: %s\n", SDL_GetError ());
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
    if (!surface)
    {
        SDL_LogError (SDL_LOG_CATEGORY_ERROR, "IMG_Load: Failed to create surface from file\n");
        SDL_LogError (SDL_LOG_CATEGORY_ERROR, "IMG_Load: %s\n", IMG_GetError ());

        *p_texture = NULL;
        *p_surface = NULL;
        return;
    }

    texture = SDL_CreateTextureFromSurface (rend, surface);
    if (!texture)
    {
        SDL_LogError (SDL_LOG_CATEGORY_ERROR, "SDL_CreateTextureFromSurface: Failed to convert image surface to texture\n");
        SDL_LogError (SDL_LOG_CATEGORY_ERROR, "SDL_CreateTextureFromSurface: %s\n", SDL_GetError ());

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

    if (retcode != 0)
    {
        SDL_LogError (SDL_LOG_CATEGORY_ERROR, "SDL_SetRenderDrawColor: Failed to set background color\n");
        SDL_LogError (SDL_LOG_CATEGORY_ERROR, "SDL_SetRenderDrawColor: %s\n", SDL_GetError ());
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


static void
background_image_stretch (SDL_Renderer *rend, SDL_Texture *bg_texture)
{
    /*{{{*/
    render_copy (rend, bg_texture, NULL, NULL);

    return;
    /*}}}*/
}


static void
background_image_tile (SDL_Renderer *rend, SDL_Texture *bg_texture, SDL_Rect *tex_rect, SDL_Rect *rend_rect)
{
    /*{{{*/
    SDL_Rect i, dst, src;

    i.w = tex_rect->w;
    i.h = tex_rect->h;

    for (i.x = 0; i.x < rend_rect->w; i.x += i.w)
    {
        for (i.y = 0; i.y < rend_rect->h; i.y += i.h)
        {
            dst = i;
            dst.w = (((i.x + i.w) >= rend_rect->w) ? (rend_rect->w - i.x) : (dst.w));
            dst.h = (((i.y + i.h) >= rend_rect->h) ? (rend_rect->h - i.y) : (dst.h));

            src = dst;
            src.x = 0;
            src.y = 0;
             
            render_copy (rend, bg_texture, &src, &dst);
        }
    }

    return;
    /*}}}*/
}


static void
background_image_fill (SDL_Renderer *rend, SDL_Texture *bg_texture, SDL_Rect *tex_rect, SDL_Rect *rend_rect)
{
    /*{{{*/
    SDL_Rect crop_rect;
    float rend_ratio, tex_ratio;

    rend_ratio = ((float)rend_rect->h) / ((float)rend_rect->w);
    tex_ratio  = ((float)tex_rect->h)  / ((float)tex_rect->w);

    /* texture is wider than the renderer */
    if (tex_ratio >= rend_ratio)
    {
        crop_rect.w = tex_rect->w;
        crop_rect.h = (int)((float)crop_rect.w * rend_ratio);
    }
    /* texture is taller */
    else /* if (text_ratio < rend_ratio) */
    {
        crop_rect.h = tex_rect->h;
        crop_rect.w = (int)((float)crop_rect.h / rend_ratio);
    }

    crop_rect.x = (tex_rect->w - crop_rect.w) / 2;
    crop_rect.y = (tex_rect->h - crop_rect.h) / 2;
    
    /* do the copy */
    render_copy (rend, bg_texture, &crop_rect, NULL);

    return;
    /*}}}*/
}


static void
background_image_fit_width (SDL_Renderer *rend, SDL_Texture *bg_texture, SDL_Rect *tex_rect, SDL_Rect *rend_rect)
{
    /*{{{*/
    SDL_Rect crop_rect;
    SDL_Rect *srcrect, *dstrect, *cropsrc;
    float rend_ratio, tex_ratio, *cropratio;

    rend_ratio = ((float)rend_rect->h) / ((float)rend_rect->w);
    tex_ratio  = ((float)tex_rect->h)  / ((float)tex_rect->w);

    /* texture is wider than the renderer */
    if (tex_ratio > rend_ratio)
    {
        cropsrc = tex_rect;
        cropratio = &rend_ratio;
        srcrect = &crop_rect;
        dstrect = rend_rect;
    }
    /* texture is taller */
    else /* if (tex_ratio <= rend_ratio) */
    {
        cropsrc = rend_rect;
        cropratio = &tex_ratio;
        srcrect = tex_rect;
        dstrect = &crop_rect;
    }

    /* create a cropped rectangle */
    crop_rect.w = cropsrc->w;
    crop_rect.h = (int)(((float)crop_rect.w) * (*cropratio));

    crop_rect.x = 0;
    crop_rect.y = (cropsrc->h - crop_rect.h) / 2;
        
    /* do the copy */
    render_copy (rend, bg_texture, srcrect, dstrect);
   
    return;
    /*}}}*/
}


static void
background_image_fit_height (SDL_Renderer *rend, SDL_Texture *bg_texture, SDL_Rect *tex_rect, SDL_Rect *rend_rect)
{
    /*{{{*/
    SDL_Rect crop_rect;
    SDL_Rect *srcrect, *dstrect, *cropsrc;
    float rend_ratio, tex_ratio, *cropratio;
    
    rend_ratio = ((float)rend_rect->h) / ((float)rend_rect->w);
    tex_ratio  = ((float)tex_rect->h)  / ((float)tex_rect->w);

    /* texture is wider than the renderer */
    if (tex_ratio > rend_ratio)
    {
        cropsrc = rend_rect;
        cropratio = &tex_ratio;
        srcrect = tex_rect;
        dstrect = &crop_rect;
    }
    /* texture is taller than the renderer */
    else /* if (tex_ratio <= rend_ratio) */
    {
        cropsrc = tex_rect;
        cropratio = &rend_ratio;
        srcrect = &crop_rect;
        dstrect = rend_rect;
    }

    /* create a cropped rectangle */
    crop_rect.h = cropsrc->h;
    crop_rect.w = (int)(((float)crop_rect.h) / (*cropratio));

    crop_rect.y = 0;
    crop_rect.x = (cropsrc->w - crop_rect.w) / 2;
        
    /* do the copy */
    render_copy (rend, bg_texture, srcrect, dstrect);
   
    return;
    /*}}}*/
}


static void 
draw_background_image (SDL_Renderer *rend, SDL_Texture *bg_texture, imgmode bg_image_mode, SDL_Rect tex_rect)
{
    /*{{{*/
    SDL_Rect rend_rect;
    rend_rect.x = 0;
    rend_rect.y = 0;
    (void)SDL_GetRendererOutputSize (rend, &rend_rect.w, &rend_rect.h);

    switch (bg_image_mode)
    {
    case IMAGE_STRETCH:
        background_image_stretch (rend, bg_texture);
        break;
    case IMAGE_TILE:
        background_image_tile (rend, bg_texture, &tex_rect, &rend_rect);
        break;
    case IMAGE_FILL:
        background_image_fill (rend, bg_texture, &tex_rect, &rend_rect);
        break;
    case IMAGE_FIT_WIDTH:
        background_image_fit_width (rend, bg_texture, &tex_rect, &rend_rect);
        break;
    case IMAGE_FIT_HEIGHT:
        background_image_fit_height (rend, bg_texture, &tex_rect, &rend_rect);
        break;
    }

    return;
    /*}}}*/
}


/*
vim: ts=4 sts=4 sw=4 et fdm=marker
*/
