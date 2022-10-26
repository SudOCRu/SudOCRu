#include <stdio.h>
#include <SDL2/SDL.h>
#include <err.h>

#include "image.h"
#include "filtering/image_filter.h"
#include "grid_slicer/grid_slicer.h"

int main(int argc, char** argv)
{
    if (argc != 2)
        errx(EXIT_FAILURE, "Usage: ipp <image_file_path>");

    // Initializes the SDL.
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    // Load the image
    ImageStatus status = ImageOk;
    Image* img = LoadImageFile(argv[1], &status);

    if (img != NULL && status == ImageOk)
    {
        // load succeeded
        printf("Processing image... (%lux%lu)\n",img->width,img->height);

        FilterImage(img);

        if (SaveImageFile(img, "out.png"))
        {
            printf("Successfully wrote out.png\n");
        }

        printf("Detecting edges...\n");
        Image* edges = CannyEdgeDetection(img);

        if (SaveImageFile(edges, "edges.png"))
        {
            printf("Successfully wrote edges.png\n");
        }

        printf("Extracting cells...\n");
        size_t len = 0;
        Image** cells = ExtractSudokuCells(edges, &len, -50, SC_FLG_FIL);

        if (SaveImageFile(edges, "grid.png"))
        {
            printf("Successfully wrote grid.png\n");
        }

        char name[18];
        for(size_t i = 0; i < len; i++)
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
