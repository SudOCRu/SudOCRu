#pragma once
#include <stddef.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

typedef struct Image {
    unsigned int* pixels;
    size_t width;
    size_t height;
} Image;

typedef enum ImageStatus
{
    ImageOk = 0,
    LoadError = 1,
    FormatError = 2,
    AllocError = 3
} ImageStatus;

/* Creates a new image filled by the specified color. Do not forgot to call
 * DestroyImage to free up the allocated memory.
 * Returns NULL if an error occured.
 *
 * The variable out_status returns if the function succeded (ImageOk) or failed
 * (else), it is safe to pass NULl for no image status information.
 * */
Image* CreateImage(unsigned int col, size_t w, size_t h,
        ImageStatus* out_status);

/* Loads an image from a file using SDL and copies its surface pixels to a new
 * pixel array stored in the Image struct. Do not forgot to call DestroyImage
 * to free up the allocated memory. The path string must be a valid string.
 * Returns NULL if an error occured.
 *
 * The variable out_status returns if the function succeded (ImageOk) or failed
 * (else), it is safe to pass NULl for no image status information.
 * */
Image* LoadImageFile(const char* path, ImageStatus* out_status);

/* Creates an SDL Surface from the `src` image. If the operation fails this
 * function returns the NULl pointer.
 */
SDL_Surface* ImageAsSurface(const Image* src);

/* Saves the `src` image to the `dest` file using SDL. The dest string must be
 * a valid string. If the operation fails this function returns 0 (false), 1
 * otherwise (true). This function uses the ImageAsSurface function.
 */
int SaveImageFile(const Image* src, const char* dest);

/* Loads an image from a RGB buffer without copying the pixels in a new
 * buffer. Do not forgot to call DestroyImage to free up the allocated memory.
 * Returns NULL if an error occured.
 *
 * The variable out_status returns if the function succeded (ImageOk) or failed
 * (else), it is safe to pass NULl for no image status information.
 * */
Image* LoadRawImage(unsigned int* rgb, size_t w, size_t h,
        ImageStatus* out_status);

/* Loads an image from a RGB buffer and copies the pixels in a new
 * buffer. Do not forgot to call DestroyImage to free up the allocated memory.
 * Returns NULL if an error occured.
 *
 * The variable out_status returns if the function succeded (ImageOk) or failed
 * (else), it is safe to pass NULl for no image status information.
 * */
Image* LoadBufImage(const unsigned int* rgb, size_t w, size_t h,
        ImageStatus* out_status);

/* Destroys (free the memory) the loaded Image.
 * It is safe to pass NULL to this function. */
void DestroyImage(Image* image);
