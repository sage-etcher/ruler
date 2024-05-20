#include "nonportable_windows.h"

#ifdef _WIN32

bool
ran_from_terminal (void)
{
    DWORD proc_id;
    DWORD count = GetConsoleProcessList (&proc_id, 1);   
    
    /* not from terminal window */
    if (count < 2)
    {
        return false;
    }

    /* invoked from terminal window */
    return true;
}


void
close_gui_term (void)
{
    /* kill the terminal if ran in gui mode */
    if (!ran_from_terminal ())
    {
        FreeConsole ();
    }

    return;
}

#endif /* _WIN32 */

/*
vim: ts=4 sts=4 sw=4 et
*/
