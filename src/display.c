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

static SDL_bool resize_window (SDL_Window *win, const SDL_Point *area, SDL_HitTestResult *p_ret);
static SDL_HitTestResult callback_always_move_window (SDL_Window *win, const SDL_Point *area, void *data);


void
start_ruler (unsigned width, unsigned height, unsigned hex_color, float opacity)
{
    /*{{{*/
    const char *TITLE= "Ruler";
    SDL_Window *win;
    SDL_Renderer *rend;
    SDL_Event e;
    SDL_Keysym *key; 

    SDL_bool resize_flag = SDL_TRUE;

    (void)SDL_Init (SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS);

    (void)SDL_CreateWindowAndRenderer (
            width, height, 
            SDL_WINDOW_BORDERLESS | SDL_WINDOW_SKIP_TASKBAR,
            &win, &rend);
 
    SDL_SetWindowResizable (win, resize_flag);
    SDL_SetWindowTitle (win, TITLE);
    SDL_SetWindowAlwaysOnTop (win, SDL_TRUE);
    SDL_SetWindowOpacity (win, opacity);
    SDL_SetWindowMinimumSize (win, WINDOW_MIN_WIDTH, WINDOW_MIN_HEIGHT);

    SDL_SetWindowHitTest (win, callback_always_move_window, &resize_flag);

    SDL_RenderSetVSync (rend, 1);
    SDL_SetRenderDrawColor (
            rend, 
            HEX_R (hex_color),
            HEX_G (hex_color),
            HEX_B (hex_color),
            SDL_ALPHA_OPAQUE);
   
    while (1)
    {
        while (SDL_PollEvent (&e))
        {
            switch (e.type)
            {
            case SDL_KEYDOWN:
                key = &(e.key.keysym);

                if ((key->sym == SDLK_q) && ((key->mod & KMOD_CTRL) != 0))
                    goto quit;
                else if (key->sym == SDLK_ESCAPE)
                    goto quit;
                else if ((key->sym == SDLK_l) && ((key->mod & KMOD_CTRL) != 0))
                {
                    resize_flag = (resize_flag ? SDL_FALSE : SDL_TRUE);
                    SDL_SetWindowResizable(win, resize_flag);
                }

                break;

            case SDL_QUIT:
                goto quit;
            }
        }
        
        SDL_RenderClear (rend);
        SDL_RenderPresent (rend);
    }

quit:
    SDL_DestroyRenderer (rend);
    SDL_DestroyWindow (win);
    
    SDL_Quit ();

    return;
    /*}}}*/
}


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
callback_always_move_window (SDL_Window *win, const SDL_Point *area, void *data)
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


/*
vim: ts=4 sts=4 sw=4 et fdm=marker
*/
