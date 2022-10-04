#include "image.h"
#include <err.h>

Image* CreateImage(unsigned int col, size_t w, size_t h,
        ImageStatus* out_status)
{
    size_t len = w * h;
    unsigned int* out_pixels = malloc(len * sizeof(unsigned int));

    if (out_pixels == NULL)
    {
        if (out_status != NULL) *out_status = AllocError;
        return NULL;
    }
    for(size_t i = 0; i < len; i++)
    {
        out_pixels[i] = col;
    }

    Image* img = malloc(sizeof(Image));
    if (img == NULL)
    {
        if (out_status != NULL) *out_status = AllocError;
        return NULL;
    }
    img->width = w;
    img->height = h;
    img->pixels = out_pixels;

    if (out_status != NULL) *out_status = ImageOk;
    return NULL;
}

Image* LoadImageFile(const char* path, ImageStatus* out_status)
{
    SDL_Surface* surf = IMG_Load(path);
    if (surf == NULL)
    {
        if (out_status != NULL) *out_status = LoadError;
        return NULL;
    }
    SDL_Surface* f = SDL_ConvertSurfaceFormat(surf, SDL_PIXELFORMAT_RGB888, 0);
    if (f == NULL)
    {
        if (out_status != NULL) *out_status = FormatError;
        return NULL;
    }
    SDL_FreeSurface(surf);

    Uint32* pixels = f->pixels;
    size_t len = f->w * f->h;
    unsigned int* out_pixels = malloc(len * sizeof(unsigned int));

    if (out_pixels == NULL)
    {
        if (out_status != NULL) *out_status = AllocError;
        return NULL;
    }

    Image* img = malloc(sizeof(Image));
    if (img == NULL)
    {
        if (out_status != NULL) *out_status = AllocError;
        return NULL;
    }
    img->width = f->w;
    img->height = f->h;
    img->pixels = out_pixels;

    SDL_LockSurface(f);
    for(size_t i = 0; i < len; i++)
    {
        out_pixels[i] = pixels[i];
    }
    SDL_UnlockSurface(f);

    SDL_FreeSurface(f);

    if (out_status != NULL) *out_status = ImageOk;
    return img;
}

int SaveImageFile(const Image* src, const char* dest)
{
    SDL_Surface* surf = SDL_CreateRGBSurfaceWithFormat(0, 
            src->width,
            src->height,
            32, SDL_PIXELFORMAT_RGB888);
    if (surf == NULL)
        return 0;

    Uint32* pixels = surf->pixels;
    size_t len = src->width * src->height;
    unsigned int* in_pixels = src->pixels;

    SDL_LockSurface(surf);
    for(size_t i = 0; i < len; i++)
    {
        pixels[i] = in_pixels[i];
    }
    SDL_UnlockSurface(surf);

    int result = IMG_SavePNG(surf, dest) == 0;
    SDL_FreeSurface(surf);
    return result;
}

Image* LoadRawImage(unsigned int* rgb, size_t w, size_t h,
        ImageStatus* out_status)
{
    Image* img = malloc(sizeof(Image));
    if (img == NULL)
    {
        if (out_status != NULL) *out_status = AllocError;
        return NULL;
    }
    img->width = w;
    img->height = h;
    img->pixels = rgb;

    if (out_status != NULL) *out_status = ImageOk;
    return NULL;
}

Image* LoadBufImage(const unsigned int* rgb, size_t w, size_t h,
        ImageStatus* out_status)
{
    size_t len = w * h * sizeof(unsigned int);
    unsigned int* out_pixels = malloc(len);

    if (out_pixels == NULL)
    {
        if (out_status != NULL) *out_status = AllocError;
        return NULL;
    }
    memcpy(out_pixels, rgb, len);

    Image* img = malloc(sizeof(Image));
    if (img == NULL)
    {
        if (out_status != NULL) *out_status = AllocError;
        return NULL;
    }
    img->width = w;
    img->height = h;
    img->pixels = out_pixels;

    if (out_status != NULL) *out_status = ImageOk;
    return NULL;
}

void RotateImage(Image* image, double angle, int fill)
{
    // TODO
}

void DestroyImage(Image* image)
{
    if (image == NULL)
        return;

    free(image->pixels);
    free(image);
}
