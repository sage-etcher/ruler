#ifndef SHORTCUTS_H
#define SHORTCUTS_H
#include "extern-c-helper.h"
EXTERN_C_START()

#include <SDL2/SDL.h>

#include "runtime.h"


typedef void (*kshort_callback)(const void *e, void *data);

typedef struct
{
    SDL_Keycode     primary;
    SDL_Keymod      modifiers;
    kshort_callback callback;
} kshortcut;

#define NULL_SHORTCUT {0, 0, NULL}

SDL_bool handle_keyboard_shortcuts (runtime_obj *s, const kshortcut *base_shortcuts, const SDL_Keysym *key);

EXTERN_C_END()
#endif /* SHORTCUTS_H */
