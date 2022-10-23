#include <stdio.h>
#include <SDL2/SDL.h>
#include <err.h>

#include "image.h"
#include "image_filter.h"
#include "grid_slicer/renderer.h"
#include "grid_slicer/hough_lines.h"

int main(int argc, char** argv)
{
    if (argc < 2)
        errx(EXIT_FAILURE, "Usage: ipp <image_file_path> [white_edge] [th]");

    // Initializes the SDL.
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    // Load the image
    ImageStatus status = ImageOk;
    Image* img = LoadImageFile(argv[1], &status);

    if (img != NULL && status == ImageOk)
    {
        // load succeeded
        printf("Loaded image successfully (%lux%lu)\n",img->width,img->height);


        printf("Running edge detection (HT)...\n");
        int white_edge = argc >= 3 ? argv[2][0] == '1' : WHITE_EDGE;
        int threshold = argc >= 4 ? strtol(argv[3], NULL, 10) : -50;

        size_t len = 0;
        Line** lines = HoughLines(img, &len, white_edge,THETA_STEPS, threshold);
        //RenderLines(img, 0x0000FF, lines, len);
        size_t fil_len = 0;
        Line** filtered = AverageLines(lines, len, &fil_len);
        printf("Filtered lines, %lu -> %lu\n", len, fil_len);
        RenderLines(img, 0xFF0000, filtered, fil_len);

        size_t rect_count = 0;
        Rect** rects = FindRects(img, filtered, fil_len, &rect_count);
        //RenderRects(img, rects, rect_count);
        Rect** best = GetBestRects(rects, rect_count, 5);

        printf("Results:\n");
        printf("-------------------------------------------------\n");
        printf("|  i |  color |     area | squareness |   angle |\n");
        printf("-------------------------------------------------\n");
        int colors[5] = {0x00FF00, 0xFFAA00, 0xAA00FF, 0xFF00AA, 0x00FFFF};
        char* names[5] = { "green", "orange", "purple", "pink", "cyan" };
        for(size_t i = 5; i > 0; i--)
        {
            Rect* r = best[i - 1];
            if (r == NULL) continue;
            printf("| %lu. | %6s | %8u | %10f | %3.2f° |\n", i, names[i - 1],
                    r->area, r->squareness, (r->ep1->alpha) * 180 / M_PI);
            printf("-------------------------------------------------\n");
            RenderRect(img, colors[i - 1], r);
        }

        /*
        Rect* candidate = FindSudokuBoard(img, rects, rect_count);
        if (candidate != NULL)
        {
            printf("=> Found rect:\n");
            printf("   > angle = %f°\n", (candidate->ep1->alpha) * 180 / M_PI);
            printf("   > squareness = %f\n", candidate->squareness);
            printf("   > area = %u pix*pix\n", candidate->area);
            printf("   > perim = %u pix\n", candidate->perim);
            RenderRect(img, 0x00FF00, candidate);
        }*/

        FreeRects(rects, rect_count);
        FreeLines(lines, len);

        if (SaveImageFile(img, "out.png"))
        {
            printf("Successfully wrote out.png\n");
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
