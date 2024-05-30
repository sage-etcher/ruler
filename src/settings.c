#include "settings.h"

#include "SDL2/SDL.h"

#include "default.h"
#include "str_utils.h"


settings_obj *
default_settings (void)
{
    settings_obj *s = SDL_malloc (sizeof (settings_obj));

    s->priority   = DEFAULT_PRIORITY;

    s->width      = DEFAULT_WIDTH;
    s->height     = DEFAULT_HEIGHT;

    s->opacity    = DEFAULT_OPACITY;

    s->color      = DEFAULT_COLOR;
    s->image_path = DEFAULT_IMAGE;
    s->image_mode = DEFAULT_IMG_MODE;

    return s;
}
