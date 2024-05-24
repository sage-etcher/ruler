#ifndef DISPLAY_H
#define DISPLAY_H

#include "common.h"

#include "window.h"
#include "render.h"
#include "events.h"
#include "background.h"

void start_ruler (unsigned width, unsigned height, unsigned hex_color, float opacity, const char *bg_image, imgmode bg_image_mode, SDL_LogPriority priority);

#endif /* DISPLAY_H */

/*
vim: ts=4 sts=4 sw=4 et fdm=marker
*/
