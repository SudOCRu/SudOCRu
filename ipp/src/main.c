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

        if (SaveImageFile(img, "filtered.png"))
        {
            printf("Successfully wrote filtered.png\n");
        }

        printf("Detecting edges...\n");
        Image* edges = CannyEdgeDetection(img);

        if (SaveImageFile(edges, "edges.png"))
        {
            printf("Successfully wrote edges.png\n");
        }

        printf("Extracting cells...\n");
        size_t len = 0;
        Image** cells = ExtractSudokuCells(img, edges, &len, -50, SC_FLG_FIL);

        if (SaveImageFile(edges, "detected.png"))
        {
            printf("Successfully wrote detected.png\n");
        }

        char name[18];
        printf("Extracting cell 0/%lu", len);
        for(size_t i = 0; i < len; i++)
        {
            printf("\rExtracting cell %lu/%lu", i + 1, len);
            fflush(stdout);

            Image* cell = cells[i];
            sprintf(name, "cells/cell_%lu.png", i);
            if (!SaveImageFile(cell, name))
            {
                printf("\nError: Could not save %s\n", name);
            }
            DestroyImage(cell);
        }
        printf("\n");

        free(cells);
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
