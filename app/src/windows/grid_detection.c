#include "windows.h"
#include "../utils.h"

void ShowGridDetection(SudOCRu* app)
{
    GtkWindow* main = GTK_WINDOW(gtk_builder_get_object(app->ui,
                "MainWindow"));
    GtkWindow* win = GTK_WINDOW(gtk_builder_get_object(app->ui,
                "ResizingWindow"));

    GtkWindowGroup* grp = gtk_window_get_group(main);
    gtk_window_group_add_window(grp, win);
    gtk_window_set_screen(win, gdk_screen_get_default());

    GtkImage* img = GTK_IMAGE(gtk_builder_get_object(app->ui,
                "ResizingImage"));
    DrawImage(app->original_image, img);

    g_signal_connect(win, "destroy", G_CALLBACK(hide_window), NULL);

    gtk_window_set_destroy_with_parent(win, TRUE);
    gtk_window_set_modal(win, TRUE);
    gtk_window_set_keep_above(win, TRUE);
    gtk_widget_show(GTK_WIDGET(win));
}
