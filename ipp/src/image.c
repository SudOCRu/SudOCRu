#include "image.h"
#include <err.h>

Image* CreateImage(unsigned int col, size_t w, size_t h,
        ImageStatus* out_status)
{
    size_t len = w * h;
    size_t size = len * sizeof(unsigned int);
    unsigned int* out_pixels = malloc(size);

    if (out_pixels == NULL)
    {
        if (out_status != NULL) *out_status = AllocError;
        return NULL;
    }
    memset(out_pixels, col, size);

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
    return img;
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

SDL_Surface* ImageAsSurface(const Image* src)
{
    if (src == NULL)
        return 0;
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
    return surf;
}

int SaveImageFile(const Image* src, const char* dest)
{
    if (src == NULL)
        return 0;
    SDL_Surface* surf = ImageAsSurface(src);
    if (surf == NULL)
        return 0;

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
    return img;
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
    return img;
}

void RotateImage(Image* img, float angle, float midX, float midY)
{
    if (fabs(angle) < (M_PI/180)) return; // Less than 1°
    size_t w = img->width, h = img->height;
    unsigned int* dst = calloc(w * h, sizeof(unsigned int));
    float sint = sin(-angle), cost = cos(-angle); 

    for (size_t i = 0; i < h; i++){
        float ny = (float)i - midY;
        for (size_t j = 0; j < w; j++){
            float nx = (float)j - midX;
            float x = nx * cost - ny * sint + midX;
            float y = nx * sint + ny * cost + midY;
            if (x >= 0 && x < w && y >= 0 && y < h)
                dst[i * w + j] = img->pixels[(size_t)y * w + (size_t)x];
        }
    }

    memcpy(img->pixels, dst, w * h * sizeof(unsigned int));
    free(dst);
}

Image* CropImage(const Image* src, size_t l, size_t t, size_t r, size_t b)
{
    if (r <= l || b <= t || r >= src->width || b >= src->height)
    {
        printf("CropImage: out of bounds, l=%lu,t=%lu,r=%lu,b=%lu\n", l,t,r,b);
        return NULL;
    }

    size_t w = r - l, h = b - t;
    Image* dst = CreateImage(0, w, h, NULL);
    if (dst == NULL)
    {
        printf("CropImage: Not enough memory, width=%lu, height=%lu\n", w, h);
        return NULL;
    }

    for (size_t i = 0; i < h; i++)
    {
        for (size_t j = 0; j < w; j++)
        {
            dst->pixels[i * w + j] = src->pixels[(i + t) * src->width + j + l];
        }
    }

    return dst;
}

Image* CropRotateImage(const Image* src, float angle, float midX, float midY,
        int l, int t, int r, int b)
{
    if (fabs(angle) < (M_PI/180)) // Less than 1°
    {
        return CropImage(src, l, t, r, b);
    }

    if (r <= l || b <= t || r >= src->width || b >= src->height)
        return NULL;
    size_t nw = r - l, nh = b - t;

    Image* dst = CreateImage(0, nw, nh, NULL);
    if (dst == NULL)
        return NULL;

    size_t w = src->width, h = src->height;
    float sint = sin(-angle), cost = cos(-angle); 

    for (size_t i = 0; i < nh; i++){
        float ny = (float)(i + t) - midY;
        for (size_t j = 0; j < nw; j++){
            float nx = (float)(j + l) - midX;
            float x = nx * cost - ny * sint + midX;
            float y = nx * sint + ny * cost + midY;
            if (x >= 0 && x < w && y >= 0 && y < h)
                dst->pixels[i * nw + j] = src->pixels[(size_t)y*w+(size_t)x];
            else
                dst->pixels[i * nw + j] = 0;
        }
    }

    return dst;
}

void DestroyImage(Image* image)
{
    if (image == NULL)
        return;

    free(image->pixels);
    free(image);
}
