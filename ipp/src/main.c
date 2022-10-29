#include <stdio.h>
#include <SDL2/SDL.h>
#include <err.h>

#include "image.h"
#include "utils.h"
#include "filtering/image_filter.h"
#include "grid_slicer/grid_slicer.h"

int ParseFlags(int argc, char** argv);

int main(int argc, char** argv)
{
    if (argc < 2)
        errx(EXIT_FAILURE, "Usage: ipp <input_image> [-<options...>]");

    // Initializes the SDL.
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    // Load the image
    ImageStatus status = ImageOk;
    Image* img = LoadImageFile(argv[1], &status);
    int flags = ParseFlags(argc, argv);

    if (img != NULL && status == ImageOk)
    {
        // load succeeded
        printf("Processing image... (%lux%lu)\n",img->width,img->height);

        FilterImage(img, flags);

        if ((flags & SC_FLG_DFIL) != 0 && SaveImageFile(img, "filtered.png"))
        {
            printf("Successfully wrote filtered.png\n");
        }

        printf("Detecting edges...\n");
        Image* edges = CannyEdgeDetection(img);

        if ((flags & SC_FLG_DEDG) != 0 && SaveImageFile(edges, "edges.png"))
        {
            printf("Successfully wrote edges.png\n");
        }

        printf("Extracting cells...\n");
        size_t len = 0;
        Image** cells = ExtractSudokuCells(img, edges, &len, -50, flags);

        if ((flags & SC_FLG_DGRD) != 0 && SaveImageFile(edges, "detected.png"))
        {
            printf("Successfully wrote detected.png\n");
        }

        if (len > 0)
        {
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
        }
        else 
        {
            printf("Oops... Looks like something went wrong"
                   ": No cells were detected :(\n");
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

int ParseFlags(int argc, char** argv)
{
    int flags = SC_NO_FLG;
    for (size_t i = 2; i < argc; i++)
    {
        char* arg = argv[i];
        if (*(arg++) != '-')
        {
            errx(EXIT_FAILURE, "Invalid argument `%s`. "
                   "Check usage in the README", arg);
            return 0;
        }
        for (size_t j = 0; arg[j] != '\0'; j++)
        {
            switch(arg[j])
            {
                case 'L':
                    flags |= SC_FLG_ALINES | SC_FLG_DGRD;
                    break;
                case 'l':
                    flags |= SC_FLG_FLINES | SC_FLG_DGRD;
                    break;
                case 'R':
                    flags |= SC_FLG_ARECTS | SC_FLG_DGRD;
                    break;
                case 'r':
                    flags |= SC_FLG_FRECTS | SC_FLG_DGRD;
                    break;
                case 'p':
                    flags |= SC_FLG_PRESTL;
                    break;
                case 'g':
                    flags |= SC_FLG_DGRS;
                    break;
                case 'G':
                    flags |= SC_FLG_DMED;
                    break;
                case 'S':
                    flags |= SC_FLG_DFIL;
                    break;
                case 'C':
                    flags |= SC_FLG_DEDG;
                    break;
                case 'T':
                    flags |= SC_FLG_DROT;
                    break;
                case 'A':
                    flags |= SC_FLG_DACC;
                    break;
                default:
                    break;
            }
        }
    }
    return flags;
}
