/*
 * =====================================================================================
 *
 *       Filename:  rotate.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/26/22 10:29:53
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include <stdlib.h>
#include <err.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

SDL_Surface* load_image(const char* path)
{
    SDL_Surface* surface = IMG_Load(path);
    if (surface == NULL) errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Surface* newsurface = SDL_ConvertSurfaceFormat(surface, 
            SDL_PIXELFORMAT_RGB888, 0);
    if(surface == NULL) errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_FreeSurface(surface);

    return newsurface;
}

Uint32 getpixel(SDL_Surface *surface, int x, int y)  {
  int bpp = surface->format->BytesPerPixel;
  /* Here p is the address to the pixel we want to retrieve */
  Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

  switch(bpp) {
  case 1:
     return *p;

  case 2:
     return *(Uint16 *)p;

  case 3:
     if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
     return p[0] << 16 | p[1] << 8 | p[2];
     else
     return p[0] | p[1] << 8 | p[2] << 16;

  case 4:
     return *(Uint32 *)p;

  default:
     return 0; /* shouldn't happen, but avoids warnings */
  }
}


void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)  {
  int bpp = surface->format->BytesPerPixel;
  /* Here p is the address to the pixel we want to set */
  Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

  switch(bpp) {
  case 1:
     *p = pixel;
     break;

  case 2:
     *(Uint16 *)p = pixel;
     break;

  case 3:
     if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
       p[0] = (pixel >> 16) & 0xff;
       p[1] = (pixel >> 8) & 0xff;
       p[2] = pixel & 0xff;
     } else {
       p[0] = pixel & 0xff;
       p[1] = (pixel >> 8) & 0xff;
       p[2] = (pixel >> 16) & 0xff;
    }
   break;

  case 4:
     *(Uint32 *)p = pixel;
     break;
  }
}

/* 
void surface_to_grayscale(SDL_Surface* surface)
{
    if (surface == NULL) errx(EXIT_FAILURE, "%s", SDL_GetError());

    Uint32* pixels = surface->pixels;
    int len = surface->w * surface->h;
    SDL_LockSurface(surface);

    SDL_PixelFormat* format = surface->format;
    if (format == NULL) errx(EXIT_FAILURE, "%s", SDL_GetError());

    for (int i = 0; i < len; i++){
        pixels[i] = pixel_to_grayscale(pixels[i], format);
    }
    SDL_UnlockSurface(surface);
}
*/

void rotate(SDL_Surface* surface, SDL_Surface* rotated, double angle){

    if (SDL_LockSurface(surface) != 0) 
        errx(EXIT_FAILURE, "Rotate: %s", SDL_GetError());

    int middle_width = surface->w / 2;
    int middle_height = surface->h / 2;
    double sinus = sin(angle);
    double cosinus = cos(angle); 

    //angle = angle % 180;

    int x = 0;
    int y = 0;

    for (int i = 0; i < surface->w; i++){
        for (int j = 0; j < surface->h; j++){         
            x = (int) round((i - middle_width) * cosinus - (j - middle_height)
                * sinus + middle_width);
            y = (int) round((i - middle_width) * sinus + (j - middle_height)
                    * cosinus + middle_height);
            if (0 <= x && x < surface->w && 
                    0 <= y && y < surface->h){
                Uint32 le_pixel = getpixel(surface, i, j);
                putpixel(rotated, x, y, le_pixel);
            }
        }
    }
    SDL_UnlockSurface(surface);
}

int main(int argc, char *argv[])
{
    if (argc == 0) printf("caca\n");

    SDL_Surface* lasurface = load_image(argv[1]);
    
    SDL_Surface* rotated_surface = SDL_CreateRGBSurface(0, lasurface->w,
            lasurface->h, 32, 0, 0, 0, 0);

    char* pointer;
    double angle = strtod(argv[2], &pointer) * (M_PI)/180;

    rotate(lasurface, rotated_surface, angle);

    SDL_SaveBMP(rotated_surface, "test_rotation.bmp");
    

    return 0;
}
