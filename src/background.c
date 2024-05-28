#include "background.h"

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include "imgmode.h"
#include "render.h"


static void background_image_stretch (SDL_Renderer *rend, SDL_Texture *bg_texture);
static void background_image_tile (SDL_Renderer *rend, SDL_Texture *bg_texture, SDL_Rect *tex_rect, SDL_Rect *rend_rect);
static void background_image_fill (SDL_Renderer *rend, SDL_Texture *bg_texture, SDL_Rect *tex_rect, SDL_Rect *rend_rect);
static void background_image_fit_width (SDL_Renderer *rend, SDL_Texture *bg_texture, SDL_Rect *tex_rect, SDL_Rect *rend_rect);
static void background_image_fit_height (SDL_Renderer *rend, SDL_Texture *bg_texture, SDL_Rect *tex_rect, SDL_Rect *rend_rect);


void
load_image (SDL_Surface **p_surface, SDL_Texture **p_texture, SDL_Renderer *rend, const char *filename)
{
    /*{{{*/
    SDL_Surface *surface;
    SDL_Texture *texture;

    surface = IMG_Load (filename);
    if (!surface)
    {
        SDL_LogError (SDL_LOG_CATEGORY_ERROR, "IMG_Load: Failed to create surface from file\n");
        SDL_LogError (SDL_LOG_CATEGORY_ERROR, "IMG_Load: %s\n", IMG_GetError ());

        *p_texture = NULL;
        *p_surface = NULL;
        return;
    }

    texture = SDL_CreateTextureFromSurface (rend, surface);
    if (!texture)
    {
        SDL_LogError (SDL_LOG_CATEGORY_ERROR, "SDL_CreateTextureFromSurface: Failed to convert image surface to texture\n");
        SDL_LogError (SDL_LOG_CATEGORY_ERROR, "SDL_CreateTextureFromSurface: %s\n", SDL_GetError ());

        SDL_FreeSurface (surface);

        *p_texture = NULL;
        *p_surface = NULL;
        return;
    }

    *p_surface = surface;
    *p_texture = texture;
    return;
    /*}}}*/
}


SDL_bool 
create_background_image (char *filename, SDL_Renderer *rend, SDL_Surface **p_surf, SDL_Texture **p_tex)
{
    /*{{{*/ 
    SDL_LogVerbose (SDL_LOG_CATEGORY_INPUT, "Using background image: %s\n", filename);
    load_image (p_surf, p_tex, rend, filename);

    if ((*p_surf == NULL) || (*p_tex == NULL))
    {
        SDL_LogError (SDL_LOG_CATEGORY_INPUT, "Failed to use background image: %s\n", filename);
        return SDL_FALSE;
    }

    return SDL_TRUE;
    /*}}}*/
}


void 
draw_background_image (SDL_Renderer *rend, SDL_Texture *bg_texture, imgmode bg_image_mode, SDL_Rect tex_rect)
{
    /*{{{*/
    SDL_Rect rend_rect;
    rend_rect.x = 0;
    rend_rect.y = 0;
    (void)SDL_GetRendererOutputSize (rend, &rend_rect.w, &rend_rect.h);

    switch (bg_image_mode)
    {
    case IMAGE_STRETCH:
        background_image_stretch (rend, bg_texture);
        break;
    case IMAGE_TILE:
        background_image_tile (rend, bg_texture, &tex_rect, &rend_rect);
        break;
    case IMAGE_FILL:
        background_image_fill (rend, bg_texture, &tex_rect, &rend_rect);
        break;
    case IMAGE_FIT_WIDTH:
        background_image_fit_width (rend, bg_texture, &tex_rect, &rend_rect);
        break;
    case IMAGE_FIT_HEIGHT:
        background_image_fit_height (rend, bg_texture, &tex_rect, &rend_rect);
        break;
    }

    return;
    /*}}}*/
}


/* file static helper functions */
static void
background_image_stretch (SDL_Renderer *rend, SDL_Texture *bg_texture)
{
    /*{{{*/
    render_copy (rend, bg_texture, NULL, NULL);

    return;
    /*}}}*/
}


static void
background_image_tile (SDL_Renderer *rend, SDL_Texture *bg_texture, SDL_Rect *tex_rect, SDL_Rect *rend_rect)
{
    /*{{{*/
    SDL_Rect i, dst, src;

    i.w = tex_rect->w;
    i.h = tex_rect->h;

    for (i.x = 0; i.x < rend_rect->w; i.x += i.w)
    {
        for (i.y = 0; i.y < rend_rect->h; i.y += i.h)
        {
            dst = i;
            dst.w = (((i.x + i.w) >= rend_rect->w) ? (rend_rect->w - i.x) : (dst.w));
            dst.h = (((i.y + i.h) >= rend_rect->h) ? (rend_rect->h - i.y) : (dst.h));

            src = dst;
            src.x = 0;
            src.y = 0;
             
            render_copy (rend, bg_texture, &src, &dst);
        }
    }

    return;
    /*}}}*/
}


static void
background_image_fill (SDL_Renderer *rend, SDL_Texture *bg_texture, SDL_Rect *tex_rect, SDL_Rect *rend_rect)
{
    /*{{{*/
    SDL_Rect crop_rect;
    float rend_ratio, tex_ratio;

    rend_ratio = ((float)rend_rect->h) / ((float)rend_rect->w);
    tex_ratio  = ((float)tex_rect->h)  / ((float)tex_rect->w);

    /* texture is wider than the renderer */
    if (tex_ratio >= rend_ratio)
    {
        crop_rect.w = tex_rect->w;
        crop_rect.h = (int)((float)crop_rect.w * rend_ratio);
    }
    /* texture is taller */
    else /* if (text_ratio < rend_ratio) */
    {
        crop_rect.h = tex_rect->h;
        crop_rect.w = (int)((float)crop_rect.h / rend_ratio);
    }

    crop_rect.x = (tex_rect->w - crop_rect.w) / 2;
    crop_rect.y = (tex_rect->h - crop_rect.h) / 2;
    
    /* do the copy */
    render_copy (rend, bg_texture, &crop_rect, NULL);

    return;
    /*}}}*/
}


static void
background_image_fit_width (SDL_Renderer *rend, SDL_Texture *bg_texture, SDL_Rect *tex_rect, SDL_Rect *rend_rect)
{
    /*{{{*/
    SDL_Rect crop_rect;
    SDL_Rect *srcrect, *dstrect, *cropsrc;
    float rend_ratio, tex_ratio, *cropratio;

    rend_ratio = ((float)rend_rect->h) / ((float)rend_rect->w);
    tex_ratio  = ((float)tex_rect->h)  / ((float)tex_rect->w);

    /* texture is wider than the renderer */
    if (tex_ratio > rend_ratio)
    {
        cropsrc = tex_rect;
        cropratio = &rend_ratio;
        srcrect = &crop_rect;
        dstrect = rend_rect;
    }
    /* texture is taller */
    else /* if (tex_ratio <= rend_ratio) */
    {
        cropsrc = rend_rect;
        cropratio = &tex_ratio;
        srcrect = tex_rect;
        dstrect = &crop_rect;
    }

    /* create a cropped rectangle */
    crop_rect.w = cropsrc->w;
    crop_rect.h = (int)(((float)crop_rect.w) * (*cropratio));

    crop_rect.x = 0;
    crop_rect.y = (cropsrc->h - crop_rect.h) / 2;
        
    /* do the copy */
    render_copy (rend, bg_texture, srcrect, dstrect);
   
    return;
    /*}}}*/
}


static void
background_image_fit_height (SDL_Renderer *rend, SDL_Texture *bg_texture, SDL_Rect *tex_rect, SDL_Rect *rend_rect)
{
    /*{{{*/
    SDL_Rect crop_rect;
    SDL_Rect *srcrect, *dstrect, *cropsrc;
    float rend_ratio, tex_ratio, *cropratio;
    
    rend_ratio = ((float)rend_rect->h) / ((float)rend_rect->w);
    tex_ratio  = ((float)tex_rect->h)  / ((float)tex_rect->w);

    /* texture is wider than the renderer */
    if (tex_ratio > rend_ratio)
    {
        cropsrc = rend_rect;
        cropratio = &tex_ratio;
        srcrect = tex_rect;
        dstrect = &crop_rect;
    }
    /* texture is taller than the renderer */
    else /* if (tex_ratio <= rend_ratio) */
    {
        cropsrc = tex_rect;
        cropratio = &rend_ratio;
        srcrect = &crop_rect;
        dstrect = rend_rect;
    }

    /* create a cropped rectangle */
    crop_rect.h = cropsrc->h;
    crop_rect.w = (int)(((float)crop_rect.h) / (*cropratio));

    crop_rect.y = 0;
    crop_rect.x = (cropsrc->w - crop_rect.w) / 2;
        
    /* do the copy */
    render_copy (rend, bg_texture, srcrect, dstrect);
   
    return;
    /*}}}*/
}



/*
vim: ts=4 sts=4 sw=4 et fdm=marker
*/
