#include "windows.h"
#include <filtering/edge_detection.h>

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
        GtkImage* img = GTK_IMAGE(gtk_builder_get_object(app->ui,
                    "ThresholdImage"));
        DrawImage(app->thresholded_image, img);
    }
    else
    {
        ShowErrorMessage(app, "Runtime error", "Unable to threshold image");
    }
    GtkButton* apply =
        GTK_BUTTON(gtk_builder_get_object(app->ui, "ApplyButton"));

    GtkContainer* container = GTK_CONTAINER(apply);
    GList *children, *iter;

    children = gtk_container_get_children(container);
    for(iter = children; iter != NULL; iter = g_list_next(iter))
    {
        gtk_container_remove(container, GTK_WIDGET(iter->data));
    }
    g_list_free(children);

    GtkWidget* label = gtk_label_new("Apply");
    gtk_widget_set_halign(label, GTK_ALIGN_FILL);
    gtk_widget_set_valign(label, GTK_ALIGN_FILL);
    gtk_widget_set_hexpand(label, TRUE);
    gtk_widget_set_vexpand(label, TRUE);
    gtk_container_add(container, label);

    gtk_widget_set_sensitive(GTK_WIDGET(apply), TRUE);
    gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(app->ui,
                 "NextButton")), TRUE);
    free(task);
    return G_SOURCE_REMOVE;
}

gpointer ThreadRethresholdImage(gpointer thr_data) {
    struct RethresholdTask* task = thr_data;
    SudOCRu* app = task->app;
    const Image* img = app->processed_image;

    memcpy(app->thresholded_image->pixels, img->pixels,
            img->width * img->height * sizeof(u32));
    BinarizeImage(app->thresholded_image, app->tmp_buffer, task->threshold
            / 100.0);
    task->result = 0;
    gdk_threads_add_idle(DoneRethreshold, task);
    return NULL;
}

gboolean ApplyThreshold(GtkButton* button, gpointer user_data)
{
    UNUSED(button);
    SudOCRu* app = user_data;
    struct RethresholdTask* task = malloc(sizeof(struct RethresholdTask));
    task->app = app;
    task->threshold = gtk_range_get_value(
            GTK_RANGE(gtk_builder_get_object(app->ui, "ThresholdScale")));

    /*
    GtkWidget* loader = GTK_WIDGET(gtk_builder_get_object(app->ui,
                 "ApplyButtonLoading"));
    gtk_widget_show(loader);
    gtk_spinner_start(GTK_SPINNER(loader));
    */

    GtkButton* apply =
        GTK_BUTTON(gtk_builder_get_object(app->ui, "ApplyButton"));

    GtkContainer* container = GTK_CONTAINER(apply);
    GList *children, *iter;

    children = gtk_container_get_children(container);
    for (iter = children; iter != NULL; iter = g_list_next(iter))
    {
        gtk_container_remove(container, GTK_WIDGET(iter->data));
    }
    g_list_free(children);

    GtkWidget* spinner = gtk_spinner_new();
    gtk_widget_set_halign(spinner, GTK_ALIGN_FILL);
    gtk_widget_set_valign(spinner, GTK_ALIGN_FILL);
    gtk_widget_set_hexpand(spinner, TRUE);
    gtk_widget_set_vexpand(spinner, TRUE);
    gtk_container_add(container, spinner);

    gtk_widget_set_sensitive(GTK_WIDGET(apply), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(app->ui,
                 "NextButton")), FALSE);

    g_thread_new("rethreshold", ThreadRethresholdImage, task);
    return TRUE;
}

struct DetectGridTask {
    SudOCRu* app;
    int result;
};

gboolean DoneGridDetection(gpointer user_data)
{
    struct DetectGridTask* task = user_data;
    SudOCRu* app = task->app;
    if (!task->result)
    {
        gtk_widget_hide(GTK_WIDGET(gtk_builder_get_object(app->ui,
            "ThresholdWindow")));
        ShowGridDetection(app);
    }
    else {
        ShowErrorMessage(app, "Runtime error", "Unable to detect grid");
    }
    gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(app->ui,
                 "ApplyButton")), TRUE);
    gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(app->ui,
                 "NextButton")), TRUE);
    free(task);
    return G_SOURCE_REMOVE;
}

gpointer ThreadDetectGrid(gpointer thr_data) {
    struct DetectGridTask* task = thr_data;
    SudOCRu* app = task->app;
    const Image* img = app->processed_image;

    Image* edges = CannyEdgeDetection(app->thresholded_image, app->tmp_buffer);
    if (edges == NULL)
    {
        task->result = 1;
        gdk_threads_add_idle(DoneGridDetection, task);
        return NULL;
    }

    SudokuGrid* grid = ExtractSudoku(img, edges, -50, 0);
    if (grid == NULL)
    {
        task->result = 2;
        gdk_threads_add_idle(DoneGridDetection, task);
        return NULL;
    }
    app->grid = grid;
    task->result = 0;
    gdk_threads_add_idle(DoneGridDetection, task);
    printf("Grid found!\n");
    return NULL;
}

gboolean RunGridDetection(GtkButton* button, gpointer user_data)
{
    UNUSED(button);
    SudOCRu* app = user_data;
    struct DetectGridTask* task = malloc(sizeof(struct DetectGridTask));
    task->app = app;

    gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(app->ui,
                 "ApplyButton")), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(app->ui,
                 "NextButton")), FALSE);

    g_thread_new("detect_grid", ThreadDetectGrid, task);
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
    GtkButton* next = GTK_BUTTON(gtk_builder_get_object(app->ui,
                 "NextButton"));

    GtkWindowGroup* grp = gtk_window_get_group(main);
    gtk_window_group_add_window(grp, win);
    gtk_window_set_screen(win, gdk_screen_get_default());

    GtkImage* img = GTK_IMAGE(gtk_builder_get_object(app->ui,
                "ThresholdImage"));
    DrawImage(app->thresholded_image, img);

    gtk_range_set_range(GTK_RANGE(scale), 0.5, 30);
    gtk_range_set_round_digits(GTK_RANGE(scale), 2);
    gtk_range_set_value(GTK_RANGE(scale), THRESH_OPTIMAL * 100.0);

    g_signal_connect(G_OBJECT(win),
        "delete-event", G_CALLBACK(hide_window), NULL);
    g_signal_connect(win, "destroy", G_CALLBACK(hide_window), NULL);
    g_signal_connect(apply, "clicked", G_CALLBACK(ApplyThreshold), app);
    g_signal_connect(next, "clicked", G_CALLBACK(RunGridDetection), app);

    gtk_window_set_destroy_with_parent(win, TRUE);
    gtk_window_set_modal(win, TRUE);
    gtk_window_set_keep_above(win, TRUE);
    gtk_widget_show(GTK_WIDGET(win));
}
