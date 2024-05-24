#include "common.h"

#include "arguments.h"
#include "display.h"


int
main (int argc, char **argv)
{
    /*{{{*/
    settings_obj *settings = default_settings ();
    CLOSE_WINDOWS_TERMINAL (); /* cheat to avoid -mwindows */
    parse_arguments (settings, argc, argv);
    start_ruler (settings);

    SDL_free (settings->image_path);
    SDL_free (settings);
    exit (EXIT_SUCCESS);
    /*}}}*/
}


/*
vim: ts=4 sts=4 sw=4 et fdm=marker
*/
/* end of file */
