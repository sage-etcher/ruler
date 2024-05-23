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
SDL_LogPriority g_logging_mode = SDL_LOG_PRIORITY_ERROR;

unsigned g_width   = 500;
unsigned g_height  = 50;
float    g_opacity = 0.8f;
unsigned g_hex     = 0xffaabb;

char *   g_image      = NULL;
imgmode  g_image_mode = IMAGE_STRETCH;


int
main (int argc, char **argv)
{
    /*{{{*/
    CLOSE_WINDOWS_TERMINAL (); /* cheat to avoid -mwindows */
    parse_arguments (argc, argv);
    start_ruler (g_width, g_height, g_hex, g_opacity, g_image, g_image_mode, g_logging_mode);
    exit (EXIT_SUCCESS);
    /*}}}*/
}


/*
vim: ts=4 sts=4 sw=4 et fdm=marker
*/
/* end of file */
