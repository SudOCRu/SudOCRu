#include "utils.h"

void CopySurfaceToGdkImage(SDL_Surface *surface, GtkImage* image)
{
    // select format
    Uint32 src_format = surface->format->format;
    gboolean has_alpha = SDL_ISPIXELFORMAT_ALPHA(src_format);

    Uint32 dst_format = has_alpha ?
        SDL_PIXELFORMAT_RGBA32 : SDL_PIXELFORMAT_RGB24;

    // create pixbuf
    GdkPixbuf *pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, has_alpha, 8,
                             surface->w, surface->h);
    int rowstride = gdk_pixbuf_get_rowstride (pixbuf);
    guchar *pixels = gdk_pixbuf_get_pixels (pixbuf);

    // copy pixels
    SDL_LockSurface(surface);
    SDL_ConvertPixels (surface->w, surface->h, src_format,
               surface->pixels, surface->pitch,
               dst_format, pixels, rowstride);
    SDL_UnlockSurface(surface);

    // create GtkImage from pixbuf
    gtk_image_set_from_pixbuf (image, pixbuf);

    // release our reference to the pixbuf
    g_object_unref (pixbuf);
}

const char IND_LOAD[] = "..";
const char IND_OK[] = "\033[32;1mOK\033[0m";

void PrintProcedure(const char* name)
{
    printf("\xF0\x9F\xA6\x90 \033[31;1m\xC2\xBB\033[0m Running"
            " \033[35;1m%s\033[0m\n", name);
}

void PrintStage(unsigned char id, unsigned char t, const char* stage, int ok)
{
    const char* indicator = ok ? IND_OK : IND_LOAD;
    if (ok == 1)
    {
        printf("\r%hhu/%hhu [%s] %s\n", id, t, indicator, stage);
    } else if (ok == 2)
    {
        printf("\r%hhu/%hhu [%s] %s", id, t, indicator, stage);
        fflush(stdout);
    }
    else
    {
        printf("%hhu/%hhu [%s] %s", id, t, indicator, stage);
        fflush(stdout);
    }
}
