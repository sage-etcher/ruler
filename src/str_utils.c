#include "str_utils.h"

#include <string.h>
#include "SDL2/SDL.h"


char *
str_dup (const char *s)
{
    char *dst;
    
    if (!s)
        return NULL;

    dst = SDL_malloc ((strlen (s) + 1) * sizeof (char));
    if (!dst)
        return NULL;

    (void)strcpy (dst, s);
    return dst;
}
