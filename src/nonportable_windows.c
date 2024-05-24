#include "nonportable_windows.h"

#include <stdio.h>

#ifdef _WIN32

SDL_bool
ran_from_terminal (void)
{
    DWORD proc_id;
    DWORD count = GetConsoleProcessList (&proc_id, 1);   
    
    /* not from terminal window */
    if (count < 2)
    {
        return SDL_FALSE;
    }

    /* invoked from terminal window */
    return SDL_TRUE;
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


char *
open_file_prompt (void)
{
    OPENFILENAME ofn;
    size_t file_len = 260;
    char *file_buf;

    file_buf = SDL_malloc (file_len * sizeof (char));
    if (!file_buf)
        return NULL;

    /* initialize OPENFILENAME */
    ZeroMemory (&ofn, sizeof (ofn));
    ofn.lStructSize = sizeof (ofn);
    ofn.hwndOwner = GetActiveWindow ();
    ofn.lpstrFile = file_buf;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = file_len;
    ofn.lpstrFilter = "All\0"   "*.*\0"
                      "Image\0" "*.PNG;*.JPG;*.JPEG\0"
                      "PNG\0"   "*.PNG\0"
                      "JPEG\0"  "*.JPG;*.JPEG\0";
    ofn.nFilterIndex = 2;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle= 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    /* display the open dialog box. */
    if (GetOpenFileName (&ofn) == TRUE)
    {
        return ofn.lpstrFile;
    }    

    SDL_free (file_buf);
    return NULL;
}


#endif /* _WIN32 */

/*
vim: ts=4 sts=4 sw=4 et
*/
