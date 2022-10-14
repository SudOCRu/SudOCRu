#include <stdio.h>
#include <SDL2/SDL.h>
#include <err.h>

#include "image.h"
#include "image_filter.h"
#include "hough_lines.h"

int main(int argc, char** argv)
{
    if (argc < 2)
        errx(EXIT_FAILURE, "Usage: ipp <image_file_path> [white_edge]");

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

        size_t len = 0;

        int white_edge = argc >= 3 ? argv[2][0] == '1' : WHITE_EDGE;
        Line** lines = HoughLines(img, &len, white_edge,
                THETA_STEPS, -45);
        /*
        for(size_t i = 0; i < len; i++)
        {
            Line* l = lines[i];
            printf("%lu: theta = %f; rho = %f => y = %fx + %f\n",
                    i, l->theta, l->rho, l->a, l->b);
        }
        */
        RenderLines(img, 0x0000FF, lines, len);
        size_t fil_len = 0;
        Line** filtered = AverageLines(lines, len, &fil_len);
        RenderLines(img, 0xFF0000, filtered, fil_len);

        int rect_count = 0;
        Rect** rects = FindRects(img, filtered, fil_len, &rect_count);
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
