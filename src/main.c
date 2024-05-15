#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <getopt.h>

#include "config.h"

#include <SDL2/SDL.h>

#define HEX_R(x) ((x & 0xFF0000) >> 16)
#define HEX_G(x) ((x & 0x00FF00) >> 8)
#define HEX_B(x) ((x & 0x0000FF))

void parse_arguments (int argc, char **argv);
void start_ruler (unsigned width, unsigned height, unsigned hex_color, float opacity);
void print_help (FILE *pipe, int exit_code);
void print_version (FILE *pipe, int exit_code);

SDL_HitTestResult callback_always_move_window (SDL_Window *win, const SDL_Point *area, void *data);

/* default values */
#define WINDOW_MIN_WIDTH 1
#define WINDOW_MIN_HEIGHT 1
float    g_opacity = 0.8f;
unsigned g_width   = 500;
unsigned g_height  = 50;
unsigned g_hex     = 0x333333;


int
main (int argc, char **argv)
{
    /*{{{*/
    parse_arguments (argc, argv);
    start_ruler (g_width, g_height, g_hex, g_opacity);
    exit (EXIT_SUCCESS);
    /*}}}*/
}


int 
valid_hexstr (const char *str)
{
    /*{{{*/
    char *iter = (char *)str;

    if (iter[0] != '0' || iter[1] != 'x')
        return 0;

    iter += 2;

    while (*iter != '\0')
    {
        if (!isxdigit (*iter))
            return 0;

        iter++;
    }

    return 1;
    /*}}}*/
}


void
parse_arguments (int argc, char **argv)
{
    /*{{{*/
    int ret;
    int c;
    struct option long_options[] =
    {
    /*   NAME       ARGUEMENT          FLAG        SHORTNAME */
        {"color",   required_argument, NULL,       'c'},
        {"colour",  required_argument, NULL,       'c'},
        {"opacity", required_argument, NULL,       'o'},
        {"width",   required_argument, NULL,       'W'},
        {"height",  required_argument, NULL,       'H'},
        {"help",    no_argument,       NULL,       'h'},
        {"version", no_argument,       NULL,       'V'},
        {NULL,      0,                 NULL,       0},
    };
    int option_index = 0;

    while ((c = getopt_long (argc, argv, "c:o:W:H:hV",
                             long_options, &option_index)) != -1)
    {
        switch (c)
        {
        case 'c':
            if (!valid_hexstr (optarg))
                print_help (stderr, EXIT_FAILURE);
            sscanf (optarg, "%x", &g_hex);
            break;
        case 'o':
            sscanf (optarg, "%f", &g_opacity);
            break;
        case 'W':
            sscanf (optarg, "%u", &g_width);
            break;
        case 'H':
            sscanf (optarg, "%u", &g_height);
            break;
        case 'V':
            print_version (stdout, EXIT_SUCCESS);
        case 'h':
            print_help (stdout, EXIT_SUCCESS);
        default:
            print_help (stderr, EXIT_FAILURE);
        }
    }

    return;
    /*}}}*/
}


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


/* only relevent to resize_window() */
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
/*}}}*/


SDL_bool
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


void
print_help (FILE *pipe, int exit_code)
{
    /*{{{*/
    (void)fprintf (pipe,
        "Usage: " PACKAGE_NAME " [OPTIONS]\n"
        "Example: " PACKAGE_NAME " -c 0xFF00FF -W 1024 -H 100    1024x100px Magenta ruler\n"
        "         " PACKAGE_NAME " -color 0x333333               500x50px Grey ruler\n"
        "         " PACKAGE_NAME "                               500x50px Black ruler\n"
        "Draws a basic single color ruler to the screen. If no -coWH flags are given\n"
        "use default values of 500x50px, 0x333333 color, and 80%% opacity.\n"
        "\n"
        "Mandatory arguments to long options are mandatory for short options too.\n"
        "  -c, --color=HEX      in hex format (0x000000) set the ruler's color\n"
        "  -o, --opacity=FLOAT  a float specifying how opaque the window should be\n"
        "                       on a scale of 0.0 to 1.0\n"
        "  -W, --width=PIXEL    in pixels set the ruler's initial width\n"
        "  -H, --height=PIXEL   in pixels set the ruler's initial height\n"
        "\n"
        "  -h, --help           show this screen and exit\n"
        "  -V, --version        output version details and exit\n"
        "\n"
        "the PIXEL argument is a positive integer, any floating point numbers\n"
        "are floored.\n"
        "\n"
        "the HEX argument is a hexadecimal colour in C hex formatting. it accepts\n"
        "values from 0x000000 to 0xFFFFFF.\n"
        "\n"
        "the FLOAT arguement is a 32bit float value between 0.0 and 1.0.\n"
        "\n"
        "Keyboard Shortcuts:\n"
        "  Ctrl+q   quit\n"
        "  Escape   quit\n"
        "  Ctrl+l   lock size\n"
        "\n"
        "Exit status:\n"
        " 0  if OK,\n"
        " 1  if error\n"
        "\n"
        "For bug reporting and help reach out to <" PACKAGE_BUGREPORT ">,\n"
        "or file an issue at <" PACKAGE_URL ">.\n");

    exit (exit_code);
    /*}}}*/
}


void
print_version (FILE *pipe, int exit_code)
{
    /*{{{*/
    (void)fprintf (pipe,
        PACKAGE_NAME " " PACKAGE_VERSION "\n"
        "Copyright (C) 2024 Sage I. Hendricks\n"
        "License MIT: The MIT License, <https://spdx.org/licenses/MIT.html>\n"
        "This is free software: you are free to change and redistribute it.\n"
        "There is NO WARRANTY, to the extent permitted by law.\n");

    exit (exit_code);
    /*}}}*/
}


/*
vim: ts=4 sts=4 sw=4 et fdm=marker
*/
/* end of file */
