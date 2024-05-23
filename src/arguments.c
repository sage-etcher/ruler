#include "common.h"

#include <cargs.h>

static void print_version (FILE *pipe, int exit_code);
static void print_help    (FILE *pipe, int exit_code);

static struct cag_option options[] =
{
/*    VALUE  SHORT  LONG          ARG      DESCRIPTION */
    { 'c',   "c",   "color",      "HEX",   "draw the background color as HEX" },
    { 'C',   NULL,  "no-color",    NULL,   "dont draw a background color" },
    { 'i',   "i",   "image",      "FILE",  "use FILE as ruler background" },
    { 'I',   NULL,  "no-image",    NULL,   "dont draw a background image" },
    { 'S',   NULL,  "bg-scale",    NULL,   "scale background image to fit" },
    { 'T',   NULL,  "bg-stretch",  NULL,   "stretch background image to fit" },
    { 'R',   NULL,  "bg-crop",     NULL,   "crop background image to fit" }, 
    { 'L',   NULL,  "bg-tile",     NULL,   "tile the background image" },
    { 'o',   "o",   "opacity",    "FLOAT", "set the window's transparency" },
    { 'W',   "W",   "width",      "PIXEL", "initial width" },
    { 'H',   "H",   "height",     "PIXEL", "initial height" },
    { 't',   "t",   "terse",       NULL,   "disable verbose logging" },
    { 'v',   "v",   "verbose",     NULL,   "enable verbose logging" },
    { 'd',   NULL,  "debug",       NULL,   "enable even more verbose logging for debugging" },
    { 'h',   "h",   "help",        NULL,   "print this message" },
    { 'V',   "V",   "version",     NULL,   "print the versioning message" },
};


void
parse_arguments (int argc, char **argv)
{
    const char *value = NULL;

    cag_option_context context;
    cag_option_init (&context, options, CAG_ARRAY_SIZE (options), argc, argv);
    while (cag_option_fetch (&context))
    {
        switch (cag_option_get_identifier (&context))
        {
            case 'c': /* -c --color=HEX */
                g_color_enable = SDL_TRUE;
                value = cag_option_get_value (&context);
                sscanf (value, "%x", &g_hex);
                break;
            case 'C': /*    --no-color */
                g_color_enable = SDL_FALSE;
                break;
            case 'i': /* -i --image=FILE */
                g_image_enable = SDL_TRUE;
                value = cag_option_get_value (&context);
                g_image = (char *)value;
                break;
            case 'I': /*    --no-image */
                g_image_enable = SDL_FALSE;
                break;
            case 'S': /*    --bg-scale */
                g_image_mode = IMAGE_SCALE;
                break;
            case 'T': /*    --bg-stretch */
                g_image_mode = IMAGE_STRETCH;
                break;
            case 'R': /*    --bg-crop */
                g_image_mode = IMAGE_CROP;
                break;
            case 'L': /*    --bg-tile */
                g_image_mode = IMAGE_TILE;
                break;
            case 'o': /* -o --opacity=FLOAT */
                value = cag_option_get_value (&context);
                sscanf (value, "%f", &g_opacity);
                break;
            case 'W': /* -W --width=INTEGER */
                value = cag_option_get_value (&context);
                sscanf (value, "%u", &g_width);
                break;
            case 'H': /* -H --height=INTEGER */
                value = cag_option_get_value (&context);
                sscanf (value, "%u", &g_height);
                break;
            case 't': /* -t --terse */
                g_logging_mode = LOG_TERSE;
                break;
            case 'v': /* -v --verbose */
                g_logging_mode = LOG_VERBOSE;
                break;
            case 'd': /*    --debug */
                g_logging_mode = LOG_DEBUG_VERBOSE;
                break;
            case 'h': /* -h --help */
                print_help (stdout, EXIT_SUCCESS);
                break;
            case 'V': /* -V --version */
                print_version (stdout, EXIT_SUCCESS);
                break;
            case '?': /* error handler */
                cag_option_print_error (&context, stderr);
                break;
        }
    }

}


static void
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
        "Mandatory arguments to long options are mandatory for short options too.\n");

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


