#pragma once
#include <gtk/gtk.h>
#include <SDL2/SDL.h>

void CopySurfaceToGdkImage(SDL_Surface *surface, GtkImage* image);
void PrintProcedure(const char* name);
void PrintStage(unsigned char id, unsigned char t, const char* stage, int ok);
