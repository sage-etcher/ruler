#include "common.h"

#include "arguements.h"
#include "display.h"


/* default values */
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


/*
vim: ts=4 sts=4 sw=4 et fdm=marker
*/
/* end of file */
