#include "windows.h"
#include "../utils.h"
#include <filtering/image_filter.h>

struct OCRTask {
    SudOCRu* app;
    int result;
};

gboolean DoneOCR(gpointer user_data)
{
    struct OCRTask* task = user_data;
    SudOCRu* app = task->app;
    gtk_widget_hide(GTK_WIDGET(gtk_builder_get_object(app->ui,
        "ResizingWindow")));
    gtk_widget_hide(GTK_WIDGET(gtk_builder_get_object(app->ui, "OCRPopup")));
    if (!task->result)
    {
        ShowOCRResults(app);
    } else {
        char code[4];
        snprintf(code, sizeof(code), "E%02i", task->result);
        ShowErrorMessage(app, (const char*)&code, "Unable to extract digits");
    }
    free(task);
    return G_SOURCE_REMOVE;
}

void PrepareForOCR(Image* cell, u8* tmp, double* pixels)
{
    Image* img = PrepareCell(cell, tmp);
    for (size_t i = 0; i < 28 * 28; i++)
    {
        double col = img->pixels[i] & 0xFF;
        pixels[i] = col / 255.0;
    }
    DestroyImage(img);
}

gpointer ThreadRunOCR(gpointer thr_data) {
    struct OCRTask* task = thr_data;
    SudOCRu* app = task->app;

    PrintStage(1, 3, "Extracting cells", 0);
    app->cells = ExtractSudokuCells(app->thresholded_image, app->grid, 0,
            &app->cells_len, &app->cropped_grid);
    PrintStage(1, 3, "Extracting cells", 1);

    PrintStage(2, 3, "Preparing Neural Network", 0);
    if (app->nn == NULL)
    {
        app->nn = ReadNetwork("bin/ocr_weights.bin");
        if (app->nn == NULL)
        {
            task->result = 1;
            gdk_threads_add_idle(DoneOCR, task);
            return NULL;
        }
    }
    PrintStage(2, 3, "Preparing Neural Network", 1);

    PrintStage(3, 3, "Reading digits", 0);
    size_t len = app->cells[0]->width * app->cells[0]->height * sizeof(u8);
    u8* tmp = malloc(len);
    for (size_t i = 0; i < app->cells_len; i++)
    {
        SudokuCell* cell = app->cells[i];
        if (CleanCell(cell->data, tmp))
        {
            double* pixels = malloc(28 * 28 * sizeof(double));
            PrepareForOCR(cell->data, tmp, pixels);
            cell->value = ReadDigit(pixels, app->nn);
        } else {
            cell->value = 0;
        }
    }
    PrintStage(3, 3, "Reading digits...", 1);
    free(tmp);
    task->result = 0;
    gdk_threads_add_idle(DoneOCR, task);
    return NULL;
}

gboolean RunOCR(GtkButton* button, gpointer user_data)
{
    UNUSED(button);
    SudOCRu* app = user_data;

    struct OCRTask* task = malloc(sizeof(struct OCRTask));
    task->app = app;

    ShowLoadingDialog(task->app, "OCRPopup");

    PrintProcedure("OCR");
    g_thread_new("run_ocr", ThreadRunOCR, task);
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
    g_signal_connect(G_OBJECT(win),
        "delete-event", G_CALLBACK(hide_window), NULL);
    g_signal_connect(win, "destroy", G_CALLBACK(hide_window), NULL);

    gtk_window_set_destroy_with_parent(win, TRUE);
    gtk_window_set_modal(win, TRUE);
    gtk_window_set_keep_above(win, TRUE);
    gtk_widget_show(GTK_WIDGET(win));
}
