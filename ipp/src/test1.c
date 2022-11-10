#include <stdio.h>
#include <SDL2/SDL.h>
#include <err.h>

#include "image.h"
#include "image_filter.h"
#include "grid_slicer/grid_slicer.h"

int main(int argc, char** argv)
{
    if (argc < 2)
        errx(EXIT_FAILURE, "Usage: ipp <image_file_path> [threshold]");

    // Initializes the SDL.
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    // Load the image
    ImageStatus status = ImageOk;
    Image* img = LoadImageFile(argv[1], &status);

    if (img != NULL && status == ImageOk) // load succeeded
    {
        int threshold = argc >= 3 ? strtol(argv[2], NULL, 10) : -50;

        printf("Loaded image successfully (%lux%lu)\n",img->width,img->height);
        size_t count = 0;
        Image** cells = ExtractSudokuCells(img, &count, threshold, SC_FLG_FIL);

        if (SaveImageFile(img, "out.png"))
        {
            printf("[DEBUG] Successfully wrote out.png\n");
        }

        char name[18];
        for(size_t i = 0; i < count; i++)
        {
            Image* cell = cells[i];
            sprintf(name, "cells/cell_%lu.png", i);
            if (SaveImageFile(cell, name))
            {
                printf("Successfully wrote %s\n", name);
            }
            DestroyImage(cell);
        }
    } else {
        // load failed
        DestroyImage(img);
        errx(EXIT_FAILURE, "Could not load image: %d\n", status);
        // unreachable
    }

    DestroyImage(img);
    SDL_Quit();

    return 0;
}
