#pragma once
#include <stddef.h>

typedef struct Image {
    int* pixels;
    size_t width;
    size_t height;
} Image;

/* Loads an image from a file using SDL */
Image* LoadImageFile(char* path);
/* Loads an image from a RGB buffer */
Image* LoadRawImage(int* rgb, size_t width, size_t height);

/* Rotate the given image by `angle` rads and fills the missing pixels by
 * the `fill` color */
void RotateImage(Image* image, double angle, int fill);

/* Destroys (free the memory) the loaded Image */
void DestroyImage(Image* image);
