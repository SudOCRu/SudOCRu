#include <stdio.h>
#include <SDL2/SDL.h>
#include <err.h>

int main(int argc, char** argv)
{
    // Initializes the SDL.
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    printf("Image Processing Pipeline TEST1\n");

    SDL_Quit();

    return 0;
}
