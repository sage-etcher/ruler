#include "str_utils.h"

char *
str_dup (const char *s)
{
    char *dst;
    
    if (!s)
        return NULL;

    dst = malloc (strlen (s) * sizeof (char));
    if (!dst)
        return NULL;

    (void)strcpy (dst, s);
    return dst;
}
