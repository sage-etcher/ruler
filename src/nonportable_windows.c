
/* enclose everything in preproc if, so we can include the file regardless of
 * host system, but only have code available to windows hosts */
#ifdef _WIN32

#include "nonportable_windows.h"

#include <windows.h>
#include "SDL2/SDL.h"


SDL_bool
ran_from_terminal (void)
{
    /*{{{*/
    DWORD proc_id;
    DWORD count = GetConsoleProcessList (&proc_id, 1);   
    
    /* not from terminal window */
    if (count < 2)
    {
        return SDL_FALSE;
    }

    /* invoked from terminal window */
    return SDL_TRUE;
    /*}}}*/
}


void
close_gui_term (void)
{
    /*{{{*/
    /* kill the terminal if ran in gui mode */
    if (!ran_from_terminal ())
    {
        FreeConsole ();
    }

    return;
    /*}}}*/
}


#endif /* _WIN32 */

/*
vim: ts=4 sts=4 sw=4 et
*/
