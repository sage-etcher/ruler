#include <stddef.h>
#include "SDL2/SDL.h"

#include "settings.h"
#include "arguments.h"
#include "display.h"

/* nonportable windows hack, shush */
/*{{{*/
#ifdef _WIN32
#include "nonportable_windows.h"

#define CLOSE_WINDOWS_TERMINAL() { close_gui_term (); }
#else
#define CLOSE_WINDOWS_TERMINAL() {}
#endif
/*}}}*/


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
