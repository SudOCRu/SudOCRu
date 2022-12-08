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
