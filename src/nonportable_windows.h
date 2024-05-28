#ifndef NONPORTABLE_WINDOWS_H 
#define NONPORTABLE_WINDOWS_H
#ifdef _WIN32

#include "SDL2/SDL.h"

SDL_bool ran_from_terminal (void);
void close_gui_term (void);
char * open_file_prompt (void);

#endif /* _WIN32 */
#endif /* NONPORTABLE_WINDOWS_H */

/*
vim: ts=4 sts=4 sw=4 et fdm=marker
*/
