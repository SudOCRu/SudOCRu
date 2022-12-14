#include <stdio.h>
#include <SDL2/SDL.h>
#include <err.h>

#include "image.h"
#include "utils.h"
#include "filtering/image_filter.h"
#include "filtering/edge_detection.h"
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

        u32* tmp = calloc(img->width * img->height, sizeof(u32));
        FilterImage(img, tmp, flags);
        BinarizeImage(img, tmp, THRESH_OPTIMAL);

        if ((flags & SC_FLG_DFIL) != 0 && SaveImageFile(img, "filtered.png"))
        {
            printf("Successfully wrote filtered.png\n");
        }

        printf("Detecting edges...\n");

        Image* edges = CannyEdgeDetection(img, tmp);
        free(tmp);

        if ((flags & SC_FLG_DEDG) != 0 && SaveImageFile(edges, "edges.png"))
        {
            printf("Successfully wrote edges.png\n");
        }

        printf("Extracting cells...\n");
        size_t len = 0;
        SudokuGrid* grid = ExtractSudoku(img, edges, -50, flags);
        if (grid == NULL)
            errx(EXIT_FAILURE, "Unable to find rects");

        if ((flags & SC_FLG_DGRD) != 0 && SaveImageFile(edges, "detected.png"))
        {
            printf("Successfully wrote detected.png\n");
        }

        Image* crop;
        SudokuCell** cells = ExtractSudokuCells(img, grid, flags, &len, &crop);
        if (SaveImageFile(crop, "sudoku.png"))
        {
            printf("Successfully wrote sudoku.png\n");
        }

        if (len > 0)
        {
            u32* tmp = malloc(cells[0]->width * cells[0]->height * sizeof(u32));
            char name[19];
            printf("Extracting cell 0/%lu", len);
            for(size_t i = 0; i < len; i++)
            {
                printf("\rExtracting cell %lu/%lu", i + 1, len);
                fflush(stdout);

                SudokuCell* cell = cells[i];
                if (CleanCell(cell->data, tmp))
                {
                    Image* cropped = CreateImage(0, 28, 28, NULL);
                    double* values = PrepareCell(cell->data, tmp);
                    for (size_t i = 0; i < 28 * 28; i++)
                    {
                        unsigned int col = values[i] * 0xFF;
                        cropped->pixels[i] = (col << 16) | (col << 8) | col;
                    }
                    DestroyImage(cell->data);
                    free(values);
                    cell->data = cropped;
                }
                snprintf(name, sizeof(name), "cells/cell_%02hhu.png", (u8) i);
                if (!SaveImageFile(cell->data, name))
                {
                    printf("\nError: Could not save %s\n", name);
                }
                FreeSudokuCell(cell);
            }
            printf("\n");

            free(tmp);
        }
        else
        {
            printf("Oops... Looks like something went wrong"
                   ": No cells were detected :(\n");
        }

        free(cells);
        FreeSudokuGrid(grid);
    } else {
        // load failed
        DestroyImage(img);
        errx(EXIT_FAILURE, "Could not load image: %s", SDL_GetError());
        // unreachable
    }

    DestroyImage(img);
    SDL_Quit();

    return 0;
}

int ParseFlags(int argc, char** argv)
{
    int flags = SC_NO_FLG;
    for (int i = 2; i < argc; i++)
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
