#include "windows.h"
#include "../utils.h"

void DrawThresholdImage(SudOCRu* app)
{
    GtkImage* processed_display = GTK_IMAGE(gtk_builder_get_object(app->ui,
                 "ThresholdImage"));
    SDL_Surface* surf = ImageAsSurface(app->processed_image);
    CopySurfaceToGdkImage(surf, processed_display);
    SDL_FreeSurface(surf);
}

struct RethresholdTask {
    SudOCRu* app;
    float threshold;
    int result;
};

gboolean DoneRethreshold(gpointer user_data)
{
    struct RethresholdTask* task = user_data;
    SudOCRu* app = task->app;
    if (!task->result)
    {
        DrawThresholdImage(app);
    }
    else {
        ShowErrorMessage(app, "Runtime error", "Unable to threshold image");
    }
    gtk_widget_hide(GTK_WIDGET(gtk_builder_get_object(app->ui,
                 "ApplyButtonLoading")));
    free(task);
    return G_SOURCE_REMOVE;
}

gpointer ThreadRethresholdImage(gpointer thr_data) {
    struct RethresholdTask* task = thr_data;
    SudOCRu* app = task->app;
    const Image* img = app->processed_image;

    app->thresholded_image =
        LoadRawImage(img->pixels, img->width, img->height, NULL);
    BinarizeImage(app->thresholded_image, app->tmp_buffer, task->threshold
            / 100.0);
    task->result = 0;
    gdk_threads_add_idle(DoneRethreshold, task);
    return NULL;
}

gboolean ApplyThreshold(GtkButton* button, gpointer user_data)
{
    SudOCRu* app = user_data;
    struct RethresholdTask* task = malloc(sizeof(struct RethresholdTask));
    task->app = app;
    task->threshold = gtk_range_get_value(
            GTK_RANGE(gtk_builder_get_object(app->ui, "ThresholdScale")));

    GtkWidget* loader = GTK_WIDGET(gtk_builder_get_object(app->ui,
                 "ApplyButtonLoading"));
    gtk_widget_show(loader);

    g_thread_new("rethreshold", ThreadRethresholdImage, task);
    return TRUE;
}

void ShowThresholding(SudOCRu* app)
{
    GtkWindow* main = GTK_WINDOW(gtk_builder_get_object(app->ui,
                "MainWindow"));
    GtkWindow* win = GTK_WINDOW(gtk_builder_get_object(app->ui,
                "ThresholdWindow"));
    GtkScale* scale = GTK_SCALE(gtk_builder_get_object(app->ui,
                 "ThresholdScale"));
    GtkButton* apply = GTK_BUTTON(gtk_builder_get_object(app->ui,
                 "ApplyButton"));

    GtkWindowGroup* grp = gtk_window_get_group(main);
    gtk_window_group_add_window(grp, win);
    gtk_window_set_screen(main, gdk_screen_get_default());
    gtk_window_set_screen(win, gdk_screen_get_default());

    DrawThresholdImage(app);

    gtk_range_set_range(GTK_RANGE(scale), 0, 20);
    gtk_range_set_round_digits(GTK_RANGE(scale), 2);
    gtk_range_set_value(GTK_RANGE(scale), THRESH_OPTIMAL * 100.0);

    g_signal_connect(G_OBJECT(win),
        "delete-event", G_CALLBACK(hide_window), NULL);
    g_signal_connect(win, "destroy", G_CALLBACK(hide_window), NULL);
    g_signal_connect(apply, "clicked", G_CALLBACK(ApplyThreshold), app);

    gtk_window_set_destroy_with_parent(win, TRUE);
    gtk_window_set_modal(win, TRUE);
    gtk_window_set_keep_above(win, TRUE);
    gtk_widget_show(GTK_WIDGET(win));
}
