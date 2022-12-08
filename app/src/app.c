/*
 * =====================================================================================
 *
 *       Filename:  converttest.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/05/22 14:32:58
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <gtk/gtk.h>
#include <SDL2/SDL.h>
#include <image.h>
#include <filtering/image_filter.h>
#include <transform/transform.h>

/* 
GtkWidget * gtk_image_new_from_sdl_surface (SDL_Surface *surface)
{
    Uint32 src_format;
    Uint32 dst_format;

    GdkPixbuf *pixbuf;
    gboolean has_alpha;
    int rowstride;
    guchar *pixels;

    GtkWidget *image;

    // select format
    src_format = surface->format->format;
    has_alpha = SDL_ISPIXELFORMAT_ALPHA(src_format);
    if (has_alpha) {
        dst_format = SDL_PIXELFORMAT_RGBA32;
    }
    else {
        dst_format = SDL_PIXELFORMAT_RGB24;
    }

    // create pixbuf
    pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, has_alpha, 8,
                             surface->w, surface->h);
    rowstride = gdk_pixbuf_get_rowstride (pixbuf);
    pixels = gdk_pixbuf_get_pixels (pixbuf);

    // copy pixels
    SDL_LockSurface(surface);
    SDL_ConvertPixels (surface->w, surface->h, src_format,
               surface->pixels, surface->pitch,
               dst_format, pixels, rowstride);
    SDL_UnlockSurface(surface);

    // create GtkImage from pixbuf
    image = gtk_image_new_from_pixbuf (pixbuf);

    // release our reference to the pixbuf
    g_object_unref (pixbuf);

    return image;
}
*/

void gtk_image_new_from_sdl_surface (SDL_Surface *surface, GtkImage* image)
{
    Uint32 src_format;
    Uint32 dst_format;

    GdkPixbuf *pixbuf;
    gboolean has_alpha;
    int rowstride;
    guchar *pixels;

    //GtkWidget *image;

    // select format
    src_format = surface->format->format;
    has_alpha = SDL_ISPIXELFORMAT_ALPHA(src_format);
    if (has_alpha) {
        dst_format = SDL_PIXELFORMAT_RGBA32;
    }
    else {
        dst_format = SDL_PIXELFORMAT_RGB24;
    }

    // create pixbuf
    pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, has_alpha, 8,
                             surface->w, surface->h);
    rowstride = gdk_pixbuf_get_rowstride (pixbuf);
    pixels = gdk_pixbuf_get_pixels (pixbuf);

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

    //return image;
}

int main(int argc, char* argv[])
{
    gtk_init(NULL, NULL);
    GtkBuilder* builder = gtk_builder_new();

    GError* error = NULL;
    if (gtk_builder_add_from_file(builder, "SudOCRu.glade", &error) == 0)
    {
        g_printerr("Error loading file: %s\n", error->message);
        g_clear_error(&error);
        return 1;
    }

    GtkWindow* window = GTK_WINDOW(gtk_builder_get_object(builder,
                "ThresholdWindow"));
    GtkImage* processed_display = GTK_IMAGE(gtk_builder_get_object(builder,
                "ThresholdImage"));
    Image* img = LoadImageFile("../examples/processing/image_04.jpg", NULL);
    if (img == NULL)
    {
        printf("Could not load image file!\n");
        return 1;
    }

    if (img->height > 800)
    {
        Image* downscaled = DownscaleImage(img, 0, 0, img->width, img->height,
                img->width / 2, img->height / 2, 0);
        DestroyImage(img);
        img = downscaled;
    }

    printf("Processing image... (%lux%lu)\n",img->width,img->height);

    // tmp is necessary for the image processing pipeline
    unsigned int* tmp = calloc(img->width * img->height, sizeof(unsigned int));
    FilterImage(img, tmp, 0);
    // copy the image into processed
    Image* processed = LoadRawImage(img->pixels, img->width, img->height, NULL);
    BinarizeImage(processed, tmp, THRESH_OPTIMAL);

    SDL_Surface* surf = ImageAsSurface(processed);
    gtk_image_new_from_sdl_surface(surf, processed_display);
    //g_signal_connect(larea, "button-press-event", );

    gtk_main();
    free(tmp);
    return 0;
}
