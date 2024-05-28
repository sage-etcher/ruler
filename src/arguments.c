#include "arguments.h"

#include <stdio.h>
#include <stddef.h>
#include "cargs.h"
#include "SDL2/SDL.h"

#include "config.h"
#include "default.h"
#include "settings.h"
#include "str_utils.h"


static void print_version (FILE *pipe, int exit_code);
static void print_help    (FILE *pipe, int exit_code);

static struct cag_option options[] =
{
/*    VALUE  SHORT  LONG          ARG      DESCRIPTION */
    { 'c',   "c",   "color",      "HEX",   "draw the background color as HEX" },
    { 'i',   "i",   "image",      "FILE",  "use FILE as ruler background" },
    { 'I',   NULL,  "no-image",    NULL,   "dont draw a background image" },
    { 's',   NULL,  "stretch",     NULL,   "stretch background image to fit" },
    { 'T',   NULL,  "tile",        NULL,   "tile the background image" },
    { 'L',   NULL,  "fill",        NULL,   "scale the background to fill the window" },
    { 'f',   NULL,  "fit-width",   NULL,   "fit background to window width" },
    { 'F',   NULL,  "fit-height",  NULL,   "fit background to window height" },
    { 'o',   "o",   "opacity",    "FLOAT", "set the window's transparency" },
    { 'W',   "W",   "width",      "PIXEL", "initial width" },
    { 'H',   "H",   "height",     "PIXEL", "initial height" },
    { 'v',   "v",   "verbose",     NULL,   "enable verbose logging" },
    { 't',   NULL,  "terse",       NULL,   "disable verbose logging" },
    { 'd',   NULL,  "debug",       NULL,   "enable even more verbose logging for debugging" },
    { 'h',   "h",   "help",        NULL,   "print this message" },
    { 'V',   "V",   "version",     NULL,   "print the versioning message" },
};


void
parse_arguments (settings_obj *s, int argc, char **argv)
{
    const char *value = NULL;

    cag_option_context context;
    cag_option_init (&context, options, CAG_ARRAY_SIZE (options), argc, argv);
    while (cag_option_fetch (&context))
    {
        switch (cag_option_get_identifier (&context))
        {
            case 'c': /* -c --color=HEX */
                value = cag_option_get_value (&context);
                sscanf (value, "%08x", &(s->color));
                break;
            case 'i': /* -i --image=FILE */
                value = cag_option_get_value (&context);
                s->image_path = str_dup (value);
                break;
            case 'I': /* --no-image */
                s->image_path = NULL;
                break;
            case 's': /* --stretch*/
                s->image_mode = IMAGE_STRETCH;
                break;
            case 'T': /* --tile */
                s->image_mode = IMAGE_TILE;
                break;
            case 'L': /* --fill */
                s->image_mode = IMAGE_FILL;
                break;
            case 'f': /* --fit-width */
                s->image_mode = IMAGE_FIT_WIDTH;
                break;
            case 'F': /* --fit-height */
                s->image_mode = IMAGE_FIT_HEIGHT;
                break;
            case 'o': /* -o --opacity=FLOAT */
                value = cag_option_get_value (&context);
                sscanf (value, "%f", &(s->opacity));
                break;
            case 'W': /* -W --width=INTEGER */
                value = cag_option_get_value (&context);
                sscanf (value, "%u", &(s->width));
                break;
            case 'H': /* -H --height=INTEGER */
                value = cag_option_get_value (&context);
                sscanf (value, "%u", &(s->height));
                break;
            case 'v': /* -v --verbose */
                s->priority = SDL_LOG_PRIORITY_VERBOSE;
                break;
            case 't': /* --terse */
                s->priority = SDL_LOG_PRIORITY_ERROR;
                break;
            case 'd': /* --debug */
                s->priority = SDL_LOG_PRIORITY_DEBUG;
                break;
            case 'h': /* -h --help */
                print_help (stdout, EXIT_SUCCESS);
                break;
            case 'V': /* -V --version */
                print_version (stdout, EXIT_SUCCESS);
                break;
            case '?': /* error handler */
                cag_option_print_error (&context, stderr);
                print_help (stderr, EXIT_SUCCESS);
                break;
        }
    }

}


const char *
get_shortcuts (void)
{
    /*{{{*/
    return "Keyboard Shortcuts:\n"
           "  Ctrl+l   lock size\n"
           "  Ctrl+f   cycle image viewing modes\n"
           "  Ctrl+o   select background image\n"
           "  Ctrl+u   select background color\n"
           "  Ctrl+[   decrease opacity by -10%\n"
           "  Ctrl+]   increase opacity by +10%\n"
           "  F1       show shortcut help menu\n"
           "  Ctrl+/   show shortcut help menu\n"
           "  Ctrl+q   quit\n"
           "  Escape   quit\n";
    /*}}}*/
}

static void
print_help (FILE *pipe, int exit_code)
{
    /*{{{*/
    (void)fprintf (pipe,
        "Usage: " PACKAGE_NAME " [OPTIONS]\n"
        "Example: " PACKAGE_NAME " -c 0xFF00FF -W 1024 -H 100    1024x100px Magenta ruler\n"
        "         " PACKAGE_NAME " -color 0x333333               %ux%upx Grey ruler\n"
        "         " PACKAGE_NAME " --tile -i $HOME/photo.png     %ux%upx photo.png ruler\n"
        "         " PACKAGE_NAME "                               %ux%upx default ruler\n"
        "Draws a basic single color ruler to the screen. If no -coWH flags are given\n"
        "use default values of %ux%upx, 0x%06X color, and %u%% opacity.\n"
        "\n"
        "Mandatory arguments to long options are mandatory for short options too.\n",
        DEFAULT_WIDTH, DEFAULT_HEIGHT,
        DEFAULT_WIDTH, DEFAULT_HEIGHT,
        DEFAULT_WIDTH, DEFAULT_HEIGHT,
        DEFAULT_WIDTH, DEFAULT_HEIGHT, DEFAULT_COLOR, (unsigned)(DEFAULT_OPACITY * 100));

    cag_option_print (options, CAG_ARRAY_SIZE (options), pipe);

    (void)fprintf (pipe,
        "\n"
        "the PIXEL argument is a positive integer, any floating point numbers\n"
        "are floored.\n"
        "\n"
        "the HEX argument is a hexadecimal colour in C hex formatting. it accepts\n"
        "values from 0x000000 to 0xFFFFFF.\n"
        "\n"
        "the FLOAT arguement is a 32bit float value between 0.0 and 1.0.\n"
        "\n"
        "the FILE arguement is the path to a file\n"
        "\n"
        "%s"
        "\n"
        "Exit status:\n"
        " 0  if OK,\n"
        " 1  if error\n"
        "\n"
        "For bug reporting and help reach out to <" PACKAGE_BUGREPORT ">,\n"
        "or file an issue at <" PACKAGE_URL ">.\n",
        get_shortcuts ());

    exit (exit_code);
    /*}}}*/
}


static void
print_version (FILE *pipe, int exit_code)
{
    /*{{{*/
    (void)fprintf (pipe,
        PACKAGE_NAME " " PACKAGE_VERSION " (" __DATE__ " " __TIME__ ")\n"
        "Copyright (C) 2024 Sage I. Hendricks\n"
        "License MIT: The MIT License, <https://spdx.org/licenses/MIT.html>\n"
        "This is free software: you are free to change and redistribute it.\n"
        "There is NO WARRANTY, to the extent permitted by law.\n");

    exit (exit_code);
    /*}}}*/
}


