#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include "settings.h"
#include "runtime.h"
#include "default.h"

#include "log_helper.h"

#include "config.h"


#define HEX_R(_x) ((_x & 0xFF0000) >> 16)
#define HEX_G(_x) ((_x & 0x00FF00) >> 8)
#define HEX_B(_x) ((_x & 0x0000FF))

#define ABS(_x) ((_x < 0 ? -_x : _x))


#endif /* COMMON_H */

/*
vim: ts=4 sts=4 sw=4 et fdm=marker
*/
