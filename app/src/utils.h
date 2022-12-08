#pragma once
#include <gtk/gtk.h>
#include <SDL2/SDL.h>

void CopySurfaceToGdkImage(SDL_Surface *surface, GtkImage* image);
