#include "windows.h"
#include "../utils.h"

gboolean RunOCR(GtkButton* button, gpointer user_data)
{
    UNUSED(button);
    SudOCRu* app = user_data;

    app->cells = ExtractSudokuCells(app->thresholded_image, app->grid, 0,
            &app->cells_len);

    ShowOCRResults(app);
    gtk_widget_hide(GTK_WIDGET(gtk_builder_get_object(app->ui,
        "ResizingWindow")));
    return TRUE;
}

void ShowGridDetection(SudOCRu* app)
{
    GtkWindow* main = GTK_WINDOW(gtk_builder_get_object(app->ui,
                "MainWindow"));
    GtkWindow* win = GTK_WINDOW(gtk_builder_get_object(app->ui,
                "ResizingWindow"));
    GtkButton* next = GTK_BUTTON(gtk_builder_get_object(app->ui,
                "ResizingNextButton"));

    GtkWindowGroup* grp = gtk_window_get_group(main);
    gtk_window_group_add_window(grp, win);
    gtk_window_set_screen(win, gdk_screen_get_default());

    GtkImage* img = GTK_IMAGE(gtk_builder_get_object(app->ui,
                "ResizingImage"));
    DrawImage(app->original_image, img);

    g_signal_connect(win, "destroy", G_CALLBACK(hide_window), NULL);
    g_signal_connect(next, "clicked", G_CALLBACK(RunOCR), app);

    gtk_window_set_destroy_with_parent(win, TRUE);
    gtk_window_set_modal(win, TRUE);
    gtk_window_set_keep_above(win, TRUE);
    gtk_widget_show(GTK_WIDGET(win));
}
