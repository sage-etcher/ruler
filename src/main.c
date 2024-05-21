#include "common.h"

#include "arguments.h"
#include "display.h"

#ifdef _WIN32
#include "nonportable_windows.h"
#define CLOSE_WINDOWS_TERMINAL() { close_gui_term(); }
#else /* !def(_WIND32) */
#define CLOSE_WINDOWS_TERMINAL() {}
#endif /* _WIN32 */


/* default values */
float    g_opacity = 0.8f;
unsigned g_width   = 500;
unsigned g_height  = 50;
unsigned g_hex     = 0x333333;
char *   g_image   = NULL;

int
main (int argc, char **argv)
{
    /*{{{*/
    CLOSE_WINDOWS_TERMINAL (); /* cheat to avoid -mwindows */
    parse_arguments (argc, argv);
    start_ruler (g_width, g_height, g_hex, g_opacity, g_image);
    exit (EXIT_SUCCESS);
    /*}}}*/
}


/*
vim: ts=4 sts=4 sw=4 et fdm=marker
*/
/* end of file */
