#include "help.h"

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


/*
vim: ts=4 sts=4 sw=4 et fdm=marker
*/
