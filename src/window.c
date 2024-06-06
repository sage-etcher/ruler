#include "window.h"

#include <stddef.h>
#include <math.h>
#include "SDL2/SDL.h"

#include "default.h"


/* file-static preprocessor macros */
/*{{{*/
#define QUICKDIST(_x,_y) ((_x*_x)+(_y*_y))

#define RESIZEPOINT(_c,_x,_y,_r) \
if (_c) { \
    dist = QUICKDIST(abs(_x),abs(_y)); \
    if (!resize_flag || dist <= closest_dist) { \
        resize_flag = 1; \
        closest_dist = dist; \
        closest_result = _r; \
    } \
} \
/*}}}*/


static SDL_bool resize_window (SDL_Window *win, const SDL_Point *area, SDL_HitTestResult *p_ret);
static SDL_HitTestResult callback_window_click (SDL_Window *win, const SDL_Point *area, void *data);


SDL_Window *
create_window (int width, int height)
{
    /*{{{*/
    const char *TITLE= "Ruler";
    const int WINDOW_FLAGS = (SDL_WINDOW_BORDERLESS | SDL_WINDOW_SKIP_TASKBAR | 
                              SDL_WINDOW_HIDDEN | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window *win;

    /* create the window */
    win = SDL_CreateWindow (
            TITLE, 
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            width, height,
            WINDOW_FLAGS);

    if (win) 
    {
        return win;
    }

    SDL_LogCritical (SDL_LOG_CATEGORY_ERROR, "SDL_CreateWindow: Failed to create window\n");
    SDL_LogCritical (SDL_LOG_CATEGORY_ERROR, "SDL_CreateWindow: %s!\n", SDL_GetError ());

    exit (EXIT_FAILURE);

    /*}}}*/
}


void
configure_window (SDL_Window *win, SDL_bool *resize_flag, float opacity)
{
    /*{{{*/
    int retcode; 

    SDL_SetWindowMinimumSize (win, WINDOW_MIN_WIDTH, WINDOW_MIN_HEIGHT);
    SDL_SetWindowResizable (win, *resize_flag);
    SDL_SetWindowAlwaysOnTop (win, SDL_TRUE);
   
    set_window_opacity (win, opacity); 
    
    retcode = SDL_SetWindowHitTest (win, callback_window_click, resize_flag);
    if (retcode != 0)
    {
        SDL_LogError (SDL_LOG_CATEGORY_ERROR, "SDL_SetWindowHitTest: Failed to set window move/resize callback\n");
        SDL_LogError (SDL_LOG_CATEGORY_ERROR, "SDL_SetWindowHitTest: %s\n", SDL_GetError ());
    }

    return;
    /*}}}*/
}


void
set_window_opacity (SDL_Window *win, float opacity)
{
    /*{{{*/
    int retcode; 

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


/*
vim: ts=4 sts=4 sw=4 et fdm=marker
*/
