#include "shortcuts.h"

#include "SDL2/SDL.h"
#include "runtime.h"

#define IS_NULL_SHORTCUT(_s) ( \
    ((_s)->primary   == 0) && \
    ((_s)->modifiers == 0) && \
    ((_s)->callback  == NULL))

#define CHECK_MODIFIER(_m,_s,_k) { \
    if ( (((_s) & (_k)) != 0 ) && \
         (((_m) & (_k)) == 0 )) { \
        return SDL_FALSE; \
    } \
}


static SDL_bool is_shortcut_pressed (const SDL_Keysym *key, kshortcut *shortcut);



SDL_bool
handle_keyboard_shortcuts (runtime_obj *s, const kshortcut *base_shortcuts, const SDL_Keysym *key)
{
    kshortcut *iter = (kshortcut *)base_shortcuts;
    
    for (; !IS_NULL_SHORTCUT(iter); iter++)
    {
        /* if the key press matches the short cut continue
         * otherwise, check next shortcut */
        if (!is_shortcut_pressed (key, iter))
        {
            continue;
        }
    
        /* shortcut is pressed but no call back was given */
        if (iter->callback == NULL)
        {
            return SDL_TRUE;
        }
        
        iter->callback (key, s);
        return SDL_TRUE;
    }

    return SDL_FALSE;
}


static SDL_bool
is_shortcut_pressed (const SDL_Keysym *key, kshortcut *shortcut)
{
    const SDL_Keymod CHECK_MODIFIERS = (SDL_Keymod)(KMOD_CTRL | KMOD_ALT | KMOD_SHIFT | KMOD_GUI);
    SDL_Keycode primary   = (SDL_Keycode)key->sym;
    SDL_Keymod  modifiers = (SDL_Keymod)key->mod;

    /* check primary key */
    if (primary != shortcut->primary)
    {
        return SDL_FALSE;
    }

    /* clean modifiers */
    /* blank all bits that we aren't checking */
    modifiers = (SDL_Keymod)(modifiers & CHECK_MODIFIERS);

    /* check that no extra modifiers are pressed */
    if ((modifiers & ~(shortcut->modifiers)) != 0)
    {
        return SDL_FALSE;
    }

    /* if any of the following, should be pressed, but isn't, return FALSE */
    CHECK_MODIFIER (modifiers, shortcut->modifiers, KMOD_CTRL);
    CHECK_MODIFIER (modifiers, shortcut->modifiers, KMOD_ALT);
    CHECK_MODIFIER (modifiers, shortcut->modifiers, KMOD_SHIFT);
    CHECK_MODIFIER (modifiers, shortcut->modifiers, KMOD_GUI);

    /* if everything checks out, the key is matching */
    return SDL_TRUE; 
}





